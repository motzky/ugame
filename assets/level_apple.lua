PLAYING = 0
COMPLETE = 1
LOST = 2

local barrels = {}

local initial_player_position = Vector3(0.0, 0.0, 0.0)
local last_player_position = Vector3(0.0, 0.0, 0.0)

function Level_init_level(player_position)
    Level_restart_level()

    last_player_position = player_position
    initial_player_position = player_position
end

function Level_update_level(player_position)
    if barrels[2].visibility then
        barrels[2].position = barrels[2].position + (player_position - last_player_position)
    end
    last_player_position = player_position
end

function Level_restart_level()
    barrels[1] = {
        position = Vector3(0.0, -0.2, 0.0),
        visibility = true,
        color = Vector3(1.0, 0.0, 0.0),
        tint = 5.0,
        collision_layer = 1,
        collision_mask = 0,
    }
    barrels[2] = {
        position = Vector3(5.0, -0.2, 0.0),
        visibility = true,
        color = Vector3(1.0, 0.0, 0.0),
        tint = 0.3,
        collision_layer = 2,
        collision_mask = 2,
    }

    last_player_position = initial_player_position
end

function Level_state()
    if distance(barrels[1].position, barrels[2].position) < 1.0 then
        return COMPLETE
    else
        return PLAYING
    end
end

function Level_name()
    return 'apple'
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

end
