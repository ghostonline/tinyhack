#include "Palette.h"

#include <diag/Assert.h>

#include <array>

namespace palette
{
    using Colors = std::array<Color, static_cast<int>(ID::_Count)>;
    static Colors colors;

    inline void set_color(ID id, const Color& color)
    {
        colors[static_cast<int>(id)] = color;
    }

    inline void set_color(ID id, Color::byte r, Color::byte g, Color::byte b)
    {
        set_color(id, Color::from_rgb_bytes(r, g, b));
    }

    inline void set_color(ID id, ID color_id)
    {
        set_color(id, get(color_id));
    }

    const Color& get(ID id)
    {
        T3D_ASSERT(id != ID::_Count);
        return colors[static_cast<int>(id)];
    }

    void init()
    {
        // Text
        set_color(ID::Background, Color::Black);
        set_color(ID::Normal, Color::Grey);
        set_color(ID::Bold, Color::White);
        set_color(ID::Accent, Color::Yellow);
        set_color(ID::Error, Color::Red);

        // Progress bar
        set_color(ID::Bar_Brackets, Color::White);
        set_color(ID::Bar_Progress, Color::Yellow);
        set_color(ID::Bar_Progress_TU, Color::Red);
        set_color(ID::Bar_Multiplier, Color::White);

        // Animations
        set_color(ID::Animation_Focus, Color::Red);
        set_color(ID::Animation_Flash, Color::Red);

        // Prompt
        set_color(ID::Prompt_Text, ID::Bold);
        set_color(ID::Prompt_Key, ID::Accent);

        // Entities
        set_color(ID::Enemy, Color::Red);
        set_color(ID::Enemy_Disabled, Color::Grey);
        set_color(ID::Player, Color::White);

        // Map
        set_color(ID::Map_Current, Color::White);
        set_color(ID::Map_Known, Color::Grey);
        set_color(ID::Map_SpecialNode, 57,106,177); // More agreeable blue
        set_color(ID::Map_UnknownMarker, Color::Grey);
        set_color(ID::Map_PlayerMarker, Color::Yellow);

        // Subnets
        set_color(ID::Subnet01, 57,106,177);
        set_color(ID::Subnet02, 218,124,48);
        set_color(ID::Subnet03, 62,150,81);
        set_color(ID::Subnet04, 204,37,41);
        set_color(ID::Subnet05, 107,76,154);
        set_color(ID::Subnet06, 146,36,40);
        set_color(ID::Subnet07, 148,139,61);
    }

    std::vector<ID> get_subnet_color_ids()
    {
        std::size_t num_subnets = static_cast<std::size_t>(ID::LastSubnet) - static_cast<std::size_t>(ID::FirstSubnet) + 1;
        std::vector<ID> ids(num_subnets);
        for (std::size_t idx = 0; idx < num_subnets; ++idx)
        {
            ids[idx] = static_cast<ID>(static_cast<std::size_t>(ID::FirstSubnet) + idx);
        }
        return ids;
    }

    std::vector<Color> get_subnet_colors()
    {
        auto ids = get_subnet_color_ids();
        std::vector<Color> subnet_colors(ids.size());
        for (std::size_t idx = 0; idx < ids.size(); ++idx)
        {
            ID subnet_id = ids[idx];
            subnet_colors[idx] = get(subnet_id);
        }
        return subnet_colors;
    }
}
