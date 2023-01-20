#include "HelpScene.h"

#include "ConsoleTools.h"
#include "Palette.h"
#include "StringTools.h"
#include "UpdateArgs.h"
#include <ds/Color.h>
#include "input/Input.h"
#include "lang/Lang.h"
#include <text/Box.h>
#include <text/Console.h>

void HelpScene::update_and_render(const SceneArgs& args)
{
    if (args.input->is_pressed(InputAction::ShowHelp) || args.input->is_pressed(InputAction::NextScene))
    {
        scene_stack->pop_scene();
    }

    auto text_color = palette::get(palette::ID::Normal);
    auto header_color = palette::get(palette::ID::Bold);
    args.console->clear(palette::get(palette::ID::Background));

    Recti panel {
        0, 0,
        args.console->size.width,
        args.console->size.height
    };
    panel.explode(-1);

    Box::blit_single_line(*args.console, panel, palette::get(palette::ID::Normal));

    math::Vec2i explanation_cursor;
    explanation_cursor.y = panel.top + 2;
    auto goal_str = Lang::get_view(LangID::Goal);
    explanation_cursor.x = panel.center_x() - static_cast<int>(goal_str.get_size()) / 2;
    args.console->blit(explanation_cursor, goal_str, header_color);

    explanation_cursor.y += 2;
    explanation_cursor.x = panel.left + 2;
    auto goal_text = Lang::get_view(LangID::ExplainGoal);
    auto goal_lines = stringtools::wrap_text(goal_text, panel.width() - 4);
    for (auto& line : goal_lines)
    {
        args.console->blit(explanation_cursor, line, text_color);
        explanation_cursor.y += 1;
    }

    // Render controls section
    struct ControlPrompt
    {
        std::string key;
        LangID description;
    };
    explanation_cursor.y += 2;
    auto controls_str = Lang::get_view(LangID::Controls);
    explanation_cursor.x = panel.center_x() - static_cast<int>(controls_str.get_size()) / 2;
    args.console->blit(explanation_cursor, controls_str, header_color);
    explanation_cursor.y += 2;

    std::vector<ControlPrompt> controls;
    std::size_t max_prompt_size;
    math::Vec2i controls_start = explanation_cursor;

    controls = {
        { "ARROWS", LangID::MoveControl },
        { args.input->get_key_string(InputAction::PeekNodes), LangID::PeekControl },
        { args.input->get_key_string(InputAction::Interact), LangID::InteractControl },
    };

    max_prompt_size = 0;
    for (auto& prompt : controls)
    {
        max_prompt_size = math::max(max_prompt_size, consoletools::get_prompt_size(prompt.key));
    }

    controls_start.x = panel.left + 2;
    explanation_cursor = controls_start;
    for (auto& prompt : controls)
    {
        explanation_cursor.x = controls_start.x;
        consoletools::render_prompt(args.console, explanation_cursor, prompt.key);
        explanation_cursor.x += static_cast<int>(max_prompt_size) + 1;
        args.console->blit(explanation_cursor, Lang::get_view(prompt.description), text_color);
        explanation_cursor.y += 2;
    }

    controls = {
        { args.input->get_key_string(InputAction::RestartLevel), LangID::RestartGame },
        { args.input->get_key_string(InputAction::ShowHelp), LangID::HelpControl },
    };

    max_prompt_size = 0;
    for (auto& prompt : controls)
    {
        max_prompt_size = math::max(max_prompt_size, consoletools::get_prompt_size(prompt.key));
    }

    controls_start.x = panel.center_x() + 2;
    explanation_cursor = controls_start;
    for (auto& prompt : controls)
    {
        explanation_cursor.x = controls_start.x;
        consoletools::render_prompt(args.console, explanation_cursor, prompt.key);
        explanation_cursor.x += static_cast<int>(max_prompt_size) + 1;
        args.console->blit(explanation_cursor, Lang::get_view(prompt.description), text_color);
        explanation_cursor.y += 2;
    }


    // Render legend section
    struct GlyphExplanation
    {
        Console::CharCodeType glyph;
        palette::ID fg_color;
        palette::ID bg_color;
        LangID description;
    };

    std::vector<palette::ID> subnet_colors = palette::get_subnet_color_ids();
    subnet_color_timer += args.update_args->delta_time;
    std::size_t subnet_color_index = static_cast<std::size_t>(subnet_color_timer);
    if (subnet_color_index >= subnet_colors.size())
    {
        subnet_color_timer = 0.0f;
        subnet_color_index = 0;
    }
    auto subnet_id = subnet_colors[subnet_color_index];

    std::vector<GlyphExplanation> explanations = {
        {'@', palette::ID::Player, palette::ID::Background, LangID::ExplainPlayer},
        {'A', palette::ID::Enemy, palette::ID::Background, LangID::ExplainAdmin},
        {'+', subnet_id, palette::ID::Background, LangID::ExplainNode},
        {'?', palette::ID::Map_UnknownMarker, palette::ID::Background, LangID::ExplainUnknown},
        {'>', palette::ID::Background, subnet_id, LangID::ExplainExit},
        {'#', palette::ID::Background, subnet_id, LangID::ExplainProtectedExit},
        {'$', palette::ID::Background, subnet_id, LangID::ExplainDataStore},
    };

    explanation_cursor.y += 3;
    auto legend_str = Lang::get_view(LangID::Legend);
    explanation_cursor.x = panel.center_x() - static_cast<int>(legend_str.get_size()) / 2;
    args.console->blit(explanation_cursor, legend_str, header_color);
    explanation_cursor.y += 2;
    for (auto& explanation : explanations)
    {
        explanation_cursor.x = panel.left + 2;
        args.console->blit(explanation_cursor, explanation.glyph, palette::get(explanation.fg_color), palette::get(explanation.bg_color));
        explanation_cursor.x += 3;
        auto description_str = Lang::get_view(explanation.description);
        args.console->blit(explanation_cursor, description_str, text_color);
        explanation_cursor.y += 2;
    }

    // Dismiss prompt
    auto dismiss_key = args.input->get_key_string(InputAction::NextScene);
    auto dismiss_text = Lang::get(LangID::DismissHelp);
    explanation_cursor.x = panel.center_x();
    explanation_cursor.y += 1;
    consoletools::render_prompt_centered(args.console, explanation_cursor, dismiss_key, dismiss_text);
}
