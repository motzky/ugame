PLAYING = 0
COMPLETE = 1
LOST = 2

local barrels = {}

local initial_player_position = Vector3(0.0, 0.0, 0.0)
local last_player_position = Vector3(0.0, 0.0, 0.0)
local current_level_state = PLAYING

function Level_init_level(player_position)
    Level_restart_level()

    last_player_position = player_position
    initial_player_position = player_position
end

function Level_update_level(player_position)
    if current_level_state ~= PLAYING then
        return
    end
    barrels[1].position = barrels[1].position + ((player_position - last_player_position) * 0.5)
    barrels[2].position = barrels[2].position + (last_player_position - player_position)
    last_player_position = player_position

    if distance(barrels[1].position, barrels[2].position) < 1.0 then
        current_level_state = COMPLETE
    end
end

function Level_restart_level()
    barrels[1] = {
        position = Vector3(-5.0, -0.2, 0.0),
        visibility = true,
        color = Vector3(0.8196, 0.8863, 0.1922),
        tint = 1.0,
        collision_layer = 1,
        collision_mask = 4,
    }
    barrels[2] = {
        position = Vector3(5.0, -0.2, 0.0),
        visibility = true,
        color = Vector3(0.8196, 0.8863, 0.1922),
        tint = 0.5,
        collision_layer = 2,
        collision_mask = 6,
    }
    barrels[3] = {
        position = Vector3(0.0, -0.2, 0.0),
        visibility = true,
        color = Vector3(1.0, 1.0, 1.0),
        tint = 0.0,
        collision_layer = 4,
        collision_mask = 3,
    }
    barrels[4] = {
        position = Vector3(0.0, -0.2, -5.0),
        visibility = true,
        color = Vector3(0.1, 0.1, 0.1),
        tint = 0.7,
        collision_layer = 4,
        collision_mask = 3,
    }

    last_player_position = initial_player_position
    current_level_state = PLAYING
end

function Level_state()
    return current_level_state
end

function Level_name()
    return 'pear'
end

function Level_entity_count()
    return #barrels
end

function Level_entity_info(index)
    return barrels[index].position, barrels[index].color, barrels[index].tint, barrels[index].collision_layer,
        barrels[index].collision_mask
end

function Level_entity_visibility(index)
    return barrels[index].visibility
end

function Level_set_entity_visibility(index, visibility)
    barrels[index].visibility = visibility
end

function Level_set_entity_position(index, position)
    barrels[index].position = position
end

function Level_handle_entity_intersect(index_a, index_b)
    if index_a == 4 or index_b == 4 then
        current_level_state = LOST
    end
end
