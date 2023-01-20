#pragma once

#include <ds/Color.h>

#include <vector>

namespace palette
{
    enum class ID
    {
        // Text
        Background,
        Normal,
        Bold,
        Accent,
        Error,

        // Progress bar
        Bar_Brackets,
        Bar_Progress,
        Bar_Progress_TU,
        Bar_Multiplier,

        // Animations
        Animation_Focus,
        Animation_Flash,

        // Prompt
        Prompt_Text,
        Prompt_Key,

        // Entities
        Enemy,
        Enemy_Disabled,
        Player,

        // Map
        Map_Current,
        Map_Known,
        Map_SpecialNode,
        Map_UnknownMarker,
        Map_PlayerMarker,

        // Subnets
        Subnet01,
        Subnet02,
        Subnet03,
        Subnet04,
        Subnet05,
        Subnet06,
        Subnet07,

        _Count,

        FirstSubnet = Subnet01,
        LastSubnet = Subnet07,
    };

    void init();
    const Color& get(ID id);

    std::vector<ID> get_subnet_color_ids();
    std::vector<Color> get_subnet_colors();
}
