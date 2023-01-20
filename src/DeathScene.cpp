#include "DeathScene.h"

#include "ConsoleTools.h"
#include "Palette.h"
#include "StringTools.h"
#include "input/Input.h"
#include "lang/Lang.h"
#include <text/Console.h>
#include <text/Box.h>

void DeathScene::update_and_render(const SceneArgs& args)
{
    if (args.input->is_pressed(InputAction::RestartLevel))
    {
        scene_stack->pop_scene();
    }

    args.console->clear(palette::get(palette::ID::Background));

    Recti death_popup{
        0, 0,
        30, 10
    };

    auto text_color = palette::get(palette::ID::Bold);

    math::Vec2i pos;
    pos.x = (args.console->size.width - death_popup.width()) / 2;
    pos.y = (args.console->size.height - death_popup.height()) / 2;
    death_popup.offset(pos.x, pos.y);
    Box::blit_single_line(*args.console, death_popup, palette::get(palette::ID::Normal));

    auto gameover_str = Lang::get_view(LangID::GameOver);
    pos.x = death_popup.center_x() - static_cast<int>(gameover_str.get_size()) / 2;
    pos.y = death_popup.top + 2;
    args.console->blit(pos, gameover_str, text_color);

    auto level_str = Lang::get_view(LangID::NetworkLevel);
    auto score_str = Lang::get_view(LangID::Score);
    int label_width = static_cast<int>(math::max(level_str.get_size(), score_str.get_size()));
    std::string level_num = stringtools::sprintf("%d", level);
    std::string score_num = stringtools::sprintf("%d", score);
    int value_width = static_cast<int>(math::max(level_num.size(), score_num.size()));
    int total_width = label_width + value_width + 4;
    pos.y += 2;
    pos.x = death_popup.center_x() - total_width / 2;
    args.console->blit(pos, level_str, text_color);
    pos.x = death_popup.center_x() + total_width / 2 - static_cast<int>(level_num.size());
    args.console->blit(pos, level_num, text_color);
    pos.y += 1;
    pos.x = death_popup.center_x() - total_width / 2;
    args.console->blit(pos, score_str, text_color);
    pos.x = death_popup.center_x() + total_width / 2 - static_cast<int>(score_num.size());
    args.console->blit(pos, score_num, text_color);

    pos.x = death_popup.center_x();
    pos.y = death_popup.bottom - 3;
    auto continue_key = args.input->get_key_string(InputAction::RestartLevel);
    auto continue_prompt = Lang::get_view(LangID::RestartGame);
    consoletools::render_prompt_centered(args.console, pos, continue_key, continue_prompt);
}
