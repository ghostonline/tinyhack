#include "ResourceLoader.h"

#include <diag/Log.h>
#include <gfx/Renderer.h>
#include <math/Math_misc.h>
#include <os/Path.h>
#include <os/FileSystem.h>
#include <text/XpImage.h>

TextureRef create_texture(ResourceID resource, ConstByteArrayView image_data, Renderer& renderer);
template<typename ResourceType>
inline void safe_delete(ResourceType*& res)
{
    delete res;
    res = nullptr;
}

ResourceLoader::ResourceLoader()
{
    resources.reserve(static_cast<int>(ResourceID::_Count));
#define RESOURCE(name, file, type) resources.emplace_back(filesystem::get_resource_path(Path(file)), type);
    RESOURCE_MANIFEST
#undef RESOURCE
}

ResourceLoader::~ResourceLoader()
{
#if ASSERTS_ENABLED
    for (std::size_t index = 0; index < resources.size(); ++index)
    {
        T3D_ASSERT(resources[index].load_count == 0);
    }
#endif
}

void ResourceLoader::update(Renderer& renderer)
{
    for (std::size_t index = 0; index < resources.size(); ++index)
    {
        ResourceID id = static_cast<ResourceID>(index);
        auto& resource = resources[index];
        if (resource.load_count == 0 && resource.state == LoadState::Ready)
        {
            resource.raw_data.clear();
            resource.raw_data.shrink_to_fit();
            switch (resource.type)
            {
            default:
                T3D_FAIL("Unhandled resource type");
            case ResourceType::XpImage:
                safe_delete(resource.typed_data.xp_image);
                break;
            case ResourceType::Texture:
                renderer.free_texture(resource.typed_data.texture);
                break;
            }
            resource.state = LoadState::Unavailable;
        }
        else if (resource.load_count > 0 && resource.state == LoadState::Unavailable)
        {
            resource.raw_data = filesystem::load_binary_file(Path(resource.filename));
            bool keep_raw = false;
            switch (resource.type)
            {
            default:
                T3D_FAIL("Unhandled resource type");
                keep_raw = true;
                break;
            case ResourceType::XpImage:
                resource.typed_data.xp_image = new XpImage();
                T3D_VERIFY(resource.typed_data.xp_image->load_from_buffer(resource.raw_data), "Expecting properly formed xp file");
                break;
            case ResourceType::Texture:
                resource.typed_data.texture = create_texture(id, resource.raw_data, renderer);
                break;
            }

            if (!keep_raw)
            {
                resource.raw_data.clear();
                resource.raw_data.shrink_to_fit();
            }

            resource.state = LoadState::Ready;
        }
    }
}

void ResourceLoader::get_typed_data(ResourceID id, Resource::TypedData* data) const
{
    auto& resource = get_resource(id);
    T3D_IF_ELSE_ASSERT(resource.state == LoadState::Ready)
    {
        *data = resource.typed_data;
    }
}

TextureRef create_texture(ResourceID resource, ConstByteArrayView image_data, Renderer& renderer)
{
    if (image_data.get_size() == 0)
    {
        Log::error("Unable to load texture resource: {0}", static_cast<int>(resource));
        return TextureRef::Null;
    }

    Image image;
    if (!image.load_from_buffer(image_data))
    {
        Log::error("Unable to create image from resource: {0}", static_cast<int>(resource));
        return TextureRef::Null;
    }

    // Make sure it scales up to a PO2
    int width_po2 = math::nearest_po2(image.get_size().width);
    int height_po2 = math::nearest_po2(image.get_size().height);
    Image image_po2({width_po2, height_po2}, image.get_format());
    image_po2.blit(image, 0, 0);

    return renderer.create_texture(image_po2);
}
