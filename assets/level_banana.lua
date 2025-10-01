local barrels = {}

local initial_player_position = Vector3(0.0, 0.0, 0.0)
local last_player_position = Vector3(0.0, 0.0, 0.0)

function Level_init_level(player_position)
    Level_restart_level()

    last_player_position = player_position
    initial_player_position = player_position
end

function Level_update_level(player_position)
    if not barrels[2].visibility then
        barrels[2].position = barrels[2].position + Vector3(0.1, 0.0, 0.0)
    else
        local delta = player_position - last_player_position
        if delta.x > 0.0 then
            delta.x = 0.0
        end
        barrels[2].position = barrels[2].position + delta
    end
    last_player_position = player_position
end

function Level_restart_level()
    barrels[1] = {
        position = Vector3(5.0, -0.2, 1.0),
        visibility = true,
        color = Vector3(1.0, 1.0, 0.0),
        tint = 1.0,
        collision_layer = 1,
        collision_mask = 0,
    }
    barrels[2] = {
        position = Vector3(-5.0, -0.2, -1.0),
        visibility = true,
        color = Vector3(1.0, 1.0, 0.0),
        tint = 0.5,
        collision_layer = 2,
        collision_mask = 2,
    }

    last_player_position = initial_player_position
end

function Level_is_complete()
    return distance(barrels[1].position, barrels[2].position) < 1.0
end

function Level_name()
    return 'banana'
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
