#include "TitleScene.h"
#include "Palette.h"
#include "resources/ResourceLoader.h"
#include <text/XpImage.h>
#include "ConsoleTools.h"
#include "lang/Lang.h"
#include "input/Input.h"

void TitleScene::update_and_render(const SceneArgs& args)
{
    switch (state)
    {
    case State::Initial:
        args.resource_loader->load(ResourceID::Title);
        state = State::Loading;
        break;
    case State::Loading:
        update_loading(args.resource_loader);
        break;
    case State::Rendering:
        update_render(args);
        break;
    }
}

void TitleScene::update_loading(ResourceLoader* loader)
{
    auto load_state = loader->get_state(ResourceID::Title);
    if (load_state == LoadState::Ready)
    {
        auto* title_image = loader->get_xp_image(ResourceID::Title);
        auto size = title_image->get_layer_size();
        title_screen.resize(size);
        title_screen.clear(palette::get(palette::ID::Background));
        for (int x = 0; x < size.width; ++x)
        {
            for (int y = 0; y < size.height; ++y)
            {
                auto cell = title_image->get_cell(x, y);
                T3D_ASSERT(cell.glyph < 0xFF); // We only support up to 255 characters per font for now
                title_screen.blit_character({x, y}, static_cast<Console::CharCodeType>(cell.glyph), palette::get(palette::ID::Bold));
            }
        }
        loader->release(ResourceID::Title);
        state = State::Rendering;
    }
    else if (load_state == LoadState::Error)
    {
        // Unable to render anything, just exit title screen
        loader->release(ResourceID::Title);
        start_game();
    }
}

void TitleScene::update_render(const SceneArgs& args)
{
    if (args.input->is_pressed(InputAction::NextScene))
    {
        start_game();
    }

    args.console->clear(palette::get(palette::ID::Background));

    auto console_size = args.console->size;
    auto title_size = title_screen.size;
    math::Vec2i pos{
        (console_size.width - title_size.width) / 2,
        (console_size.height - title_size.height) / 2,
    };
    args.console->blit(pos, title_screen);

    StringView subtitle = "Digital heist RL";
    pos.x += title_size.width - static_cast<int>(subtitle.get_size());
    pos.y += title_size.height - 1;
    args.console->blit(pos, subtitle, palette::get(palette::ID::Normal));

    pos.x = console_size.width / 2;
    pos.y += 5;
    auto input_str = args.input->get_key_string(InputAction::NextScene);
    consoletools::render_prompt_centered(args.console, pos, input_str, Lang::get_view(LangID::StartGamePrompt));

    args.console->blit({1, args.console->size.height - 3}, "Bart Veldstra", palette::get(palette::ID::Normal));
    args.console->blit({1, args.console->size.height - 2}, "XenonGhost Software", palette::get(palette::ID::Normal));
}

void TitleScene::start_game()
{
    scene_stack->pop_scene();
    state = State::Initial;
}
