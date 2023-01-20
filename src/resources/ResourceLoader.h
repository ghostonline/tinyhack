#pragma once

#include <diag/Assert.h>
#include <ds/ByteArrayView.h>
#include <gfx/GfxRef.h>
#include <os/Path.h>

#include <vector>

class Renderer;
class XpImage;

#define RESOURCE_MANIFEST \
    RESOURCE(Font, "terminal16x16_gs_ro.png", ResourceType::Texture) \
    RESOURCE(Title, "title.xp", ResourceType::XpImage) \
    // End of manifest

enum class ResourceID
{
#define RESOURCE(name, file, type) name,
    RESOURCE_MANIFEST
#undef RESOURCE

    _Count,
};

enum class LoadState
{
    Unavailable,
    Loading,
    Ready,
    Error,
};

enum class ResourceType
{
    XpImage,
    Texture,
};

class ResourceLoader
{
    struct Resource
    {
        union TypedData
        {
            TypedData() {}

            TextureRef texture;
            XpImage* xp_image;
        };

        Resource(Path static_path, ResourceType type) : filename(static_path), type(type) {}

        const Path filename;

        LoadState state = LoadState::Unavailable;
        unsigned load_count = 0;
        std::vector<byte> raw_data;
        TypedData typed_data;
        ResourceType type;
    };

public:
    ResourceLoader();
    ~ResourceLoader();

    void load(ResourceID id);
    LoadState get_state(ResourceID id) const { return get_resource(id).state; }
    void release(ResourceID id);

    ConstByteArrayView get_raw_data(ResourceID id) const { return get_resource(id).raw_data; }
    TextureRef get_texture(ResourceID id) const;
    const XpImage* get_xp_image(ResourceID id) const;

    void update(Renderer& renderer);

private:
    void get_typed_data(ResourceID id, Resource::TypedData* data) const;

    const Resource& get_resource(ResourceID id) const { return resources[static_cast<int>(id)]; }
    Resource& get_resource(ResourceID id) { return resources[static_cast<int>(id)]; }

    std::vector<Resource> resources;
};

inline void ResourceLoader::load(ResourceID id)
{
    auto& resource = get_resource(id);
    resource.load_count += 1;
}

inline void ResourceLoader::release(ResourceID id)
{
    auto& resource = get_resource(id);
    T3D_IF_ELSE_ASSERT(resource.load_count > 0)
    {
        --resource.load_count;
    }
}

inline TextureRef ResourceLoader::get_texture(ResourceID id) const
{
    Resource::TypedData data; data.texture = TextureRef::Null;
    get_typed_data(id, &data);
    return data.texture;
}

inline const XpImage* ResourceLoader::get_xp_image(ResourceID id) const
{
    Resource::TypedData data; data.xp_image = nullptr;
    get_typed_data(id, &data);
    return data.xp_image;
}
