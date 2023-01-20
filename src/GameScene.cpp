#include "GameScene.h"

#include "ConsoleTools.h"
#include "UpdateArgs.h"
#include "StringTools.h"
#include <Random.h>
#include <diag/Log.h>
#include "entity/ComponentData.h"
#include "lang/Lang.h"
#include "level/Network.h"
#include "level/NetworkGenerator.h"
#include "level/NetworkTools.h"

static const int ScanWarnTime = 3;
static const int DownloadSteps = 3;
static const int HackSteps = 3;
static const int BarPositionY = 2;

void GameScene::update_and_render(const SceneArgs& args)
{
    input = *args.input;

    if (!initialized)
    {
        initialized = true;
        init(*args.randomizer);
        scene_stack->push_scene(&help_scene);
    }
    else if (is_player_dead())
    {
        death_scene.set_results(world.level, world.score);
        scene_stack->push_scene(&death_scene);
        init(*args.randomizer);
    }
    else if (input.is_pressed(InputAction::RestartLevel))
    {
        init(*args.randomizer);
    }
    else if (next_level)
    {
        go_to_next_level();
        next_level = false;
    }

    update(args.update_args);
    render_world(args.console);
    render_hud(args.console);
}

void GameScene::update(const UpdateArgs* args)
{
    if (input.is_pressed(InputAction::ShowHelp))
    {
        scene_stack->push_scene(&help_scene);
        return;
    }

    if (!animator.is_blocking_animation())
    {
        if (phase == Phase::StatusUpdates)
        {
            update_status(args);
        }
        else if (phase == Phase::PlayerActions)
        {
            update_player(args);
            if (player_turn_taken)
            {
                next_phase();
            }
        }
        else if (phase == Phase::EnemyActions)
        {
            update_enemies(args);
        }
        else if (phase == Phase::AlarmIncrease)
        {
            update_alarm(args);
        }

        if (world_map_dirty)
        {
            world_map_dirty = false;
            cache_world_map();
        }
    }

    // Update animations
    animator.update(args->delta_time);
}

void GameScene::next_phase()
{
    int phase_int = static_cast<int>(phase);
    int max_phase = static_cast<int>(Phase::_Count);
    phase_int = (phase_int + 1) % max_phase;
    phase = static_cast<Phase>(phase_int);
}

bool GameScene::is_player_dead() const
{
    auto player = world.entities.find_first<Player>();
    return player.get_component<Player>()->attacked;
}

bool GameScene::can_player_download() const
{
    auto player = world.entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;
    auto* tile = world.network.get_tile_safe(player_pos);
    return tile->type == TileType::Node && tile->node().type == NodeType::DataStore;
}

bool GameScene::can_player_hack() const
{
    if (world.can_leave())
    {
        return false;
    }

    auto player = world.entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;
    return world.network.exit == player_pos;
}

bool GameScene::can_player_leave() const
{
    if (!world.can_leave())
    {
        return false;
    }

    auto player = world.entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;
    return world.network.exit == player_pos;
}

void GameScene::push_bar_explosion(const StringView& message, const palette::ID& color)
{
    ProgressBarExplosion explosion;
    explosion.color = color;
    explosion.message = message.to_string();
    explosion.bar_width = progress_bar.get_width();
    explosion.bar_y = BarPositionY;
    animator.bar_explosions.push_back(explosion);
}

void GameScene::update_status(const UpdateArgs* args)
{
    disabled_system.update();
    next_phase();
}

void GameScene::update_player(const UpdateArgs* args)
{
    // Unless specifically told, player will consume their turn after completing this method
    player_turn_taken = true;

    if (input.is_pressed(InputAction::PeekNodes))
    {
        auto player = world.entities.find_first<Player>();
        auto player_pos = player.get_component<Position>()->pos;

        auto neighbours = networktools::get_node_neighbours(world.network, player_pos);
        bool has_peeked = false;
        for (auto& neighbour : neighbours)
        {
            if (world.get_visibility(neighbour) == Visibility::Detected) // Are we next to peekable tiles?
            {
                auto* tile = world.network.get_tile(neighbour);
                world.known_subnets[tile->node().subnet_id] = true;
                has_peeked = true;
            }
        }

        if (has_peeked)
        {
            world_map_dirty = true;
            return;
        }
    }

    if (input.is_pressed(InputAction::WaitTurn))
    {
        return;
    }

    math::Vec2i direction;
    if (input.is_pressed(InputAction::MoveLeft))
    {
        direction.x -= 1;
    }
    if (input.is_pressed(InputAction::MoveRight))
    {
        direction.x += 1;
    }
    if (input.is_pressed(InputAction::MoveDown))
    {
        direction.y += 1;
    }
    if (input.is_pressed(InputAction::MoveUp))
    {
        direction.y -= 1;
    }

    if ((direction.x != 0 && direction.y == 0) || (direction.x == 0 && direction.y != 0))
    {
        // Only allow a single direction
        auto player = world.entities.find_first<Player>();
        Position* position = player.get_component<Position>();
        auto connector_tile = world.network.get_tile(position->pos + direction);
        ConnectorType expected_type = direction.x ? ConnectorType::Horizontal : ConnectorType::Vertical;
        if (connector_tile->type == TileType::Connector && connector_tile->connector().type == expected_type)
        {
            auto current_tile = world.network.get_tile(position->pos);
            T3D_ASSERT(current_tile->type == TileType::Node);
            position->pos += direction * 2;
            auto new_tile = world.network.get_tile(position->pos);
            T3D_ASSERT(new_tile->type == TileType::Node);

            if (new_tile->node().subnet_id != current_tile->node().subnet_id)
            {
                world.known_subnets[new_tile->node().subnet_id] = true;
                world_map_dirty = true;
            }

            if (world.download_progress)
            {
                world.download_progress = 0; // Reset download progress
                push_bar_explosion(Lang::get(LangID::DownloadLost), palette::ID::Error);
            }

            if (world.exit_progress)
            {
                world.exit_progress = 0; // Reset hack progress
                push_bar_explosion(Lang::get(LangID::HackLost), palette::ID::Error);
            }

            if (new_tile->node().type == NodeType::DataStore)
            {
                progress_bar.set_level(0);
                progress_bar.set_progress(0, DownloadSteps);
            }
            else if (new_tile->node().type == NodeType::Exit && can_player_hack())
            {
                progress_bar.set_level(world.exit_strength);
                progress_bar.set_progress(world.exit_progress, HackSteps);
            }

            return;
        }
    }

    if (input.is_pressed(InputAction::Interact))
    {
        auto player = world.entities.find_first<Player>();
        Position* position = player.get_component<Position>();
        Tile* tile = world.network.get_tile(position->pos);
        if (tile->node().type == NodeType::Exit)
        {
            if (world.can_leave())
            {
                next_level = true;
                player_turn_taken = false;
            }
            else
            {
                world.exit_progress += 1;
                if (world.exit_progress >= HackSteps)
                {
                    world.exit_progress = 0;
                    world.exit_strength -= 1;

                    if (world.can_leave())
                    {
                        push_bar_explosion(Lang::get_view(LangID::HackComplete));
                        world_map_dirty = true;
                    }
                    else
                    {
                        push_bar_explosion(); // New level
                    }
                }

                if (!world.can_leave())
                {
                    // Update progress bar
                    progress_bar.set_level(world.exit_strength);
                    progress_bar.set_progress(world.exit_progress, HackSteps);
                }
            }
            return;
        }
        else if (tile->node().type == NodeType::DataStore)
        {
            world.download_progress += 1;
            progress_bar.set_progress(world.download_progress, DownloadSteps);
            if (world.download_progress >= DownloadSteps)
            {
                world.score += 1;
                world.download_progress = 0;
                // TODO: Really want this to be its own entity instead
                tile->node().type = NodeType::Normal;
                world_map_dirty = true;
                push_bar_explosion(Lang::get(LangID::DownloadComplete));
            }
            return;
        }
    }

    player_turn_taken = false;
}

void GameScene::update_enemies(const UpdateArgs* args)
{
    attack_system.update(); // Pre-movement damage
    admin_ai_system.update();
    monitor_ai_system.update();
    attack_system.update(); // Post-movement damage
    next_phase();
}

void GameScene::update_alarm(const UpdateArgs* args)
{
    world.current_alarm += 1;
    if (world.current_alarm == world.max_alarm)
    {
        auto player_pos = world.entities.find_first<Player>().get_component<Position>()->pos;
        if (player_pos != world.network.entrance && world.current_alarm_level < world.max_alarm_level)
        {
            ++world.current_alarm_level;
            admin_ai_system.create(world.network.entrance);
        }

        // Create scan animation
        {
            ScanAnimation scan_animation;
            networktools::visit_nodes(world.network, player_pos, [&scan_animation](const networktools::VisitData& data)
            {
                scan_animation.points.push_back({});
                scan_animation.points.back().pos = data.pos;
                scan_animation.points.back().time = static_cast<float>(data.distance_from_start);
                return networktools::CallbackResult::Continue;
            });
            float latest_time = scan_animation.points.back().time;
            for (auto& point : scan_animation.points)
            {
                point.time = (latest_time - point.time) / latest_time;
            }
            animator.scan_animations.push_back(scan_animation);
        }

        admin_ai_system.alert_nearest();

        world.current_alarm = 0;
    }
    next_phase();
}

void GameScene::cache_world_map()
{
    world.update_visibility_map();

    static const std::vector<char> node_glyphs =
    {
        '+',
        '<',
        '>',
        '$',
    };

    auto player = world.entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;
    auto player_subnet = world.network.get_tile(player_pos)->node().subnet_id;
    auto subnet_colors = palette::get_subnet_color_ids();
    auto player_subnet_color = subnet_colors[player_subnet % subnet_colors.size()];

    world_map.resize(world.network.size);
    world_map.clear(palette::get(palette::ID::Background));
    for (int y = 0; y < world.network.size.height; ++y)
    {
        for (int x = 0; x < world.network.size.width; ++x)
        {
            math::Vec2i pos{x, y};
            auto visibility = world.get_visibility(pos);

            auto* tile = world.network.get_tile(pos);
            if (tile->type == TileType::Node)
            {
                if (visibility == Visibility::Visible)
                {
                    auto subnet = tile->node().subnet_id;
                    auto subnet_color = palette::get(subnet == player_subnet ? player_subnet_color : palette::ID::Map_Known);
                    auto fg_color = subnet_color;
                    auto bg_color = palette::get(palette::ID::Background);
                    auto node_type = tile->node().type;
                    if (node_type != NodeType::Normal)
                    {
                        std::swap(fg_color, bg_color);
                    }

                    char glyph = node_glyphs[static_cast<int>(node_type)];
                    if (node_type == NodeType::Exit && !world.can_leave())
                    {
                        glyph = '#';
                    }

                    world_map.blit(pos, glyph, fg_color, bg_color);
                }
                else if (visibility == Visibility::Detected)
                {
                    world_map.blit_character(pos, '?', palette::get(palette::ID::Map_UnknownMarker));
                }
            }
            else if (tile->type == TileType::Connector && visibility != Visibility::Hidden)
            {
                Console::CharCodeType glyph = 0;
                bool subnet_separator = world.is_subnet_separator(pos);
                palette::ID color_id = palette::ID::Map_Known;
                if (subnet_separator)
                {
                    glyph = 250;
                }
                else
                {
                    glyph = tile->connector().type == ConnectorType::Horizontal ? '-' : '|';
                }

                if (world.is_connected_to_subnet(pos, player_subnet))
                {
                    color_id = palette::ID::Map_Current;
                }

                world_map.blit_character(pos, glyph, palette::get(color_id));
            }
        }
    }
}

void GameScene::render_world(Console* console) const
{
    console->clear(palette::get(palette::ID::Background));
    math::Vec2i map_offset;
    map_offset.x = (console->size.width - world_map.size.width) >> 1;
    map_offset.y = (console->size.height - world_map.size.height) >> 1;
    console->blit(map_offset, world_map);

    Recti camera_frustum{
        -map_offset.x,
        -map_offset.y,
        console->size.width,
        console->size.height
    };

    Array2<Sprite::Layer> zbuffer(console->size.width, console->size.height, Sprite::Layer::None);
    std::vector<math::Vec2i> alert_targets;
    for (auto& drawable : world.entities.find_all<Position, Sprite>())
    {
        auto* position = drawable.get_component<Position>();
        if (camera_frustum.contains(position->pos.x, position->pos.y))
        {
            auto console_pos = map_offset + position->pos;
            auto* sprite = drawable.get_component<Sprite>();
            auto sprite_layer = sprite->layer;
            auto sprite_glyph = sprite->glyph;
            auto sprite_color = sprite->color;

            auto visible_state = drawable.get_component<VisibleState>();
            if (visible_state)
            {
                if (visible_state->alerted)
                {
                    if (animator.is_alert_visible())
                    {
                        // Always show alert pip, even when owner is not visible
                        alert_targets.push_back(visible_state->alert_target);
                        sprite_glyph = '!';
                    }
                }
                if (visible_state->disabled)
                {
                    sprite_layer = Sprite::Layer::Disabled;
                    sprite_color = palette::ID::Enemy_Disabled;
                    if (animator.is_alert_visible())
                    {
                        sprite_glyph = '/';
                    }
                }
            }

            if (zbuffer.at(console_pos.x, console_pos.y) < sprite_layer && world.get_visibility(position->pos) != Visibility::Hidden)
            {
                zbuffer.at(console_pos.x, console_pos.y) = sprite_layer;
                console->blit_character(map_offset + position->pos, sprite_glyph, palette::get(sprite_color));
            }
        }
    }

    for (auto& target : alert_targets)
    {
        if (camera_frustum.contains(target.x, target.y))
        {
            auto console_pos = map_offset + target;
            if (zbuffer.at(console_pos.x, console_pos.y) < Sprite::Layer::Marker)
            {
                zbuffer.at(console_pos.x, console_pos.y) = Sprite::Layer::Marker;
                console->blit_character(console_pos, '!', palette::get(palette::ID::Map_PlayerMarker));
            }
        }
    }

    animator.draw_world(*console, camera_frustum, world);
}

void GameScene::render_hud(Console* console) const
{
    auto text_color = palette::get(palette::ID::Bold);

    int cursor_x = 0;
    int bottom_y = console->size.height - 1;
    auto level_str = Lang::get_view(LangID::NetworkLevel);
    console->blit({cursor_x, bottom_y}, level_str, text_color);
    cursor_x += static_cast<int>(level_str.get_size());
    auto level_val = stringtools::sprintf(": %d", world.level);
    console->blit({cursor_x, bottom_y}, level_val, text_color);
    cursor_x += static_cast<int>(level_val.get_size()) + 1;

    auto alarm_str = Lang::get_view(LangID::TuCount);
    console->blit({cursor_x, bottom_y}, alarm_str, text_color);
    cursor_x += static_cast<int>(alarm_str.get_size()) + 1;
    console->blit({cursor_x, bottom_y}, "[", text_color);
    cursor_x += 1;

    int max_alarm_size = world.max_alarm - 1;
    std::vector<char> tu_bar(max_alarm_size, '_');
    int max_alarm = math::min(world.current_alarm, max_alarm_size);
    for (int index = 0; index < max_alarm; ++index)
    {
        tu_bar[index] = 'X';
    }

    if (animator.is_final_alarm_visible() && world.current_alarm == max_alarm_size)
    {
        // Blink final char
        tu_bar.back() = ' ';
    }

    console->blit({cursor_x, bottom_y}, {tu_bar.data(), tu_bar.size()}, palette::get(palette::ID::Bar_Progress_TU));
    cursor_x += static_cast<int>(tu_bar.size());
    console->blit({cursor_x, bottom_y}, "]", text_color);

    cursor_x += 3;
    auto score_str = Lang::get_view(LangID::Score);
    console->blit({cursor_x, bottom_y}, score_str, text_color);
    cursor_x += static_cast<int>(score_str.get_size());
    auto score_val = stringtools::sprintf(": %d", world.score);
    console->blit({cursor_x, bottom_y}, score_val, text_color);

    auto help_key = input.get_key_string(InputAction::ShowHelp);
    auto help_prompt_size = consoletools::get_prompt_size(help_key);
    cursor_x = console->size.width - static_cast<int>(help_prompt_size);
    consoletools::render_prompt(console, {cursor_x, bottom_y}, help_key);

    if (can_player_download())
    {
        auto interact_key = input.get_key_string(InputAction::Interact);
        auto interact_str = Lang::get_view(LangID::DownloadHint);
        consoletools::render_prompt_centered(console, {console->size.width / 2, BarPositionY + 1}, interact_key, interact_str);
        progress_bar.draw(*console, BarPositionY);
    }

    if (can_player_hack())
    {
        auto interact_key = input.get_key_string(InputAction::Interact);
        auto interact_str = Lang::get_view(LangID::HackHint);
        consoletools::render_prompt_centered(console, {console->size.width / 2, BarPositionY + 1}, interact_key, interact_str);
        progress_bar.draw(*console, BarPositionY);
    }

    if (can_player_leave())
    {
        auto interact_key = input.get_key_string(InputAction::Interact);
        auto interact_str = Lang::get_view(LangID::LeaveHint);
        consoletools::render_prompt_centered(console, {console->size.width / 2, BarPositionY + 1}, interact_key, interact_str);
    }

    if (world.current_alarm >= world.max_alarm - ScanWarnTime)
    {
        // Alarm imminent
        for (int x = 0; x < console->size.width; ++x)
        {
            console->blit_character({x, 0}, 0xB0, palette::get(palette::ID::Error));
        }

        auto alarm_text_fmt = Lang::get(LangID::IncomingScan);
        int turns_left = world.max_alarm - world.current_alarm;
        std::string alarm_text = stringtools::sprintf(alarm_text_fmt, turns_left);
        auto alarm_header = stringtools::sprintf(" ! %s ! ", alarm_text.c_str());
        int x = (console->size.width - static_cast<int>(alarm_header.get_size())) / 2;
        console->blit({x, 0}, alarm_header, palette::get(palette::ID::Bold));
    }
    animator.draw_hud(*console);
}

void GameScene::init(Random& seed_rng)
{
    position_system.world = &world;
    disabled_system.world = &world;
    admin_ai_system.world = &world;
    admin_ai_system.animator = &animator;
    monitor_ai_system.world = &world;
    monitor_ai_system.animator = &animator;
    attack_system.world = &world;
    phase = Phase::PlayerActions;

    int world_seed = seed_rng.next();
    Log::print("World seed: {0}", world_seed);
    world.reset();
    world.seed = world_seed;
    world.gameplay_rng.set_seed(world_seed - 1);
    world.level = 1;
    init_level();
}

void GameScene::go_to_next_level()
{
    world.level += 1;
    init_level();
}

void GameScene::init_level()
{
    phase = Phase::PlayerActions;
    animator = Animator();

    world.reset_level();
    world.max_alarm = 15;
    world.exit_strength = world.level;
    world.max_alarm_level = world.level;
    networkgenerator::generate(world.seed + world.level, &world.network);
    world.known_subnets.resize(world.network.subnet_count);
    world.visibility_map.resize(world.network.size.width, world.network.size.height, Visibility::Hidden);

    {
        auto player = world.entities.create_entity();
        player.add_component<Player>();
        player.add_component<Position>()->pos = world.network.entrance;
        auto* sprite = player.add_component<Sprite>();
        sprite->glyph = '@';
        sprite->layer = Sprite::Layer::Player;
        sprite->color = palette::ID::Player;

        world.known_subnets[world.network.get_tile(world.network.entrance)->node().subnet_id] = true;
    }

    world_map_dirty = true;
}
