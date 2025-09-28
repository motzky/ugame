local barrels = {}

local last_player_position = Vector3(0.0, 0.0, 0.0)

function Level_init_level(player_position)
    Level_restart_level()

    last_player_position = player_position
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
        visibility = true
    }
    barrels[2] = {
        position = Vector3(5.0, -0.2, 0.0),
        visibility = true
    }
end

function Level_is_complete()
    return distance(barrels[1].position, barrels[2].position) < 1.0
end

function Level_name()
    return 'apple'
end

function Level_entity_count()
    return #barrels
end

function Level_entity_position(index)
    return barrels[index].position
end

function Level_set_entity_visibility(index, visibility)
    barrels[index].visibility = visibility
end