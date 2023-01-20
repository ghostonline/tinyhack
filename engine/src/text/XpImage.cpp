#include "XpImage.h"

#include <diag/Assert.h>
#include <io/BinaryStream.h>
#include <cstdint>
#include <miniz.h>

static const std::uint16_t ExpectedMagicNumber = 0x8b1f;

struct CompressionMethod
{
    enum Type : std::uint8_t
    {
        Store = 0,
        Compress = 1,
        Pack = 2,
        Lzh = 3,
        // 4..7: reserved
        Deflate = 8,
    };
};

struct OSType
{
    enum Type : std::uint8_t
    {
        FAT = 0x00,				// FAT filesystem (MS-DOS, OS/2, NT/Win32)
        Amiga = 0x01,			// Amiga
        VMS = 0x02,				// VMS (or OpenVMS)
        Unix = 0x03,			// Unix
        VM_CMS = 0x04,			// VM/CMS
        Atari_TOS = 0x05,		// Atari TOS
        HPFS = 0x06,			// HPFS filesystem (OS/2, NT)
        Macintosh = 0x07,		// Macintosh
        Z_System = 0x08,		// Z-System
        CP_M = 0x09,			// CP/M
        TOPS_20 = 0x0a,			// TOPS-20
        NTFS = 0x0b,			// NTFS filesystem (NT)
        QDOS = 0x0c,			// QDOS
        Acorn_RISCOS = 0x0d,	// Acorn RISCOS
        Unknown  = 0xff,
    };
};

struct GzipHeader
{
    std::uint16_t magic_number;
    std::uint8_t compression_method;
    std::uint8_t flags;
    std::uint32_t modification_time;
    std::uint8_t compression_flags;
    std::uint8_t os_type;
};
static const unsigned GzipHeaderSize = 10; // Cannot use sizeof due to packing issues

struct GzipFooter
{
    std::uint32_t crc32_hash;
    std::uint32_t uncompressed_size;
};
static const unsigned GzipFooterSize = sizeof(GzipFooter);

static const int XpImageVersion = -1;
static const unsigned XpImageCelSize = sizeof(std::uint32_t) + 2 * (3 * sizeof(std::uint8_t));

bool XpImage::load_from_buffer(const ConstByteArrayView& buffer)
{
#define T3D_IF_NOT_FAIL_AND_RETURN(expr, msg) if (!(expr)) { T3D_FAIL(msg); return false; }

    T3D_IF_NOT_FAIL_AND_RETURN(buffer.get_size() >= GzipHeaderSize + GzipFooterSize, "Not enough bytes for gzip header and footer");

    auto* header = reinterpret_cast<const GzipHeader*>(buffer.get_ptr());

    T3D_IF_NOT_FAIL_AND_RETURN(header->magic_number == ExpectedMagicNumber, "Invalid gzip header");

    T3D_IF_NOT_FAIL_AND_RETURN(header->compression_method == CompressionMethod::Deflate, "Unhandled compression method");
    T3D_IF_NOT_FAIL_AND_RETURN(header->flags == 0, "Unknown flags set");
    T3D_IF_NOT_FAIL_AND_RETURN(header->compression_flags == 0, "Unknown compression flags set");

    auto* footer = reinterpret_cast<const GzipFooter*>(buffer.get_ptr() + buffer.get_size() - sizeof(GzipFooter));

    auto* deflated_data = buffer.get_ptr() + GzipHeaderSize;
    auto deflated_size = buffer.get_size() - GzipHeaderSize - GzipFooterSize;

    std::vector<byte> dest_buffer(footer->uncompressed_size);

    // Decompress payload
    {
        mz_stream stream;
        int status;
        memset(&stream, 0, sizeof(stream));

        stream.next_in = deflated_data;
        stream.avail_in = static_cast<unsigned int>(deflated_size);
        stream.next_out = dest_buffer.data();
        stream.avail_out = static_cast<unsigned int>(footer->uncompressed_size);

        status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
        T3D_IF_NOT_FAIL_AND_RETURN(status == MZ_OK, "Unable to initialize decompression stream");

        status = mz_inflate(&stream, MZ_FINISH);
        if (status != MZ_STREAM_END)
        {
            T3D_FAIL("Unable to decompress payload");
            mz_inflateEnd(&stream);
            return false;
        }

        if (stream.total_out != footer->uncompressed_size)
        {
            T3D_FAIL("Unexpected compression size");
            mz_inflateEnd(&stream);
            return false;
        }

        status = mz_inflateEnd(&stream);
        T3D_IF_NOT_FAIL_AND_RETURN(status == MZ_OK, "Unable to deinitialize decompression stream");
    }

    std::int32_t xp_version = 0;
    std::uint32_t new_layer_count = 0;
    BinaryStream stream(dest_buffer);
    T3D_IF_NOT_FAIL_AND_RETURN(stream.try_read(xp_version) && stream.try_read(new_layer_count), "Unexpected xp header size");
    T3D_IF_NOT_FAIL_AND_RETURN(xp_version == XpImageVersion, "Unexpected xp version number");
    T3D_IF_NOT_FAIL_AND_RETURN(new_layer_count > 0, "No layers present in image");

    std::vector<std::size_t> new_layer_offsets;
    std::uint32_t layer_width = 0;
    std::uint32_t layer_height = 0;
    unsigned layer_byte_length = 0;
    for (unsigned layer_index = 0; layer_index < new_layer_count; ++layer_index)
    {
        std::uint32_t current_layer_width = 0;
        std::uint32_t current_layer_height = 0;
        T3D_IF_NOT_FAIL_AND_RETURN(stream.try_read(current_layer_width) && stream.try_read(current_layer_height), "Unexpected end of stream while reading layer header");
        if (layer_index == 0)
        {
            layer_width = current_layer_width;
            layer_height = current_layer_height;
            layer_byte_length = layer_width * layer_height * XpImageCelSize;
        }
        else
        {
            T3D_IF_NOT_FAIL_AND_RETURN(layer_width == current_layer_width && layer_height == current_layer_height, "Individual layers have different sizes");
        }

        T3D_IF_NOT_FAIL_AND_RETURN(stream.get_remaining() >= layer_byte_length, "Unexpected end of stream while reading layer data");
        new_layer_offsets.push_back(stream.get_ptr());
        stream.skip(layer_byte_length);
    }

    T3D_IF_NOT_FAIL_AND_RETURN(stream.get_remaining() == 0, "Data remaining after image is fully read");

#undef T3D_IF_NOT_FAIL_AND_RETURN

    layer_size.width = layer_width;
    layer_size.height = layer_height;
    std::swap(new_layer_offsets, this->layer_offsets);
    std::swap(dest_buffer, this->data);

    return true;
}

const XpImage::Cell& XpImage::get_cell_in_layer(int layer, int x, int y) const
{
    unsigned layer_index = static_cast<unsigned>(layer);
    T3D_ASSERT(layer_index < layer_offsets.size() && layer_size.contains(x, y));
    const byte* layer_start = data.data() + layer_offsets[layer_index];
    auto layer_height = static_cast<unsigned>(layer_size.height);
    std::size_t cell_offset = (x * layer_height + y) * XpImageCelSize;
    return *reinterpret_cast<const Cell*>(layer_start + cell_offset);
}

XpImage::Cell XpImage::get_cell(int x, int y) const
{
    T3D_ASSERT(layer_size.contains(x, y));

    auto layer_height = static_cast<unsigned>(layer_size.height);
    std::size_t cell_offset = (x * layer_height + y) * XpImageCelSize;
    Cell cell;
    for (std::size_t layer_index = layer_offsets.size(); layer_index > 0; --layer_index)
    {
        const byte* layer_start = data.data() + layer_offsets[layer_index - 1];
        cell = *reinterpret_cast<const Cell*>(layer_start + cell_offset);
        if (!cell.is_transparent())
        {
            break;
        }
    }

    return cell;
}

