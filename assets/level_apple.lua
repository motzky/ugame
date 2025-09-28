local barrels = {}

local last_player_position = Vector3(0.0, 0.0, 0.0)

function Level_init_level(player_position)
    barrels[1] = Vector3(0.0, -0.2, 0.0)
    barrels[2] = Vector3(5.0, -0.2, 0.0)

    last_player_position = player_position
end

function Level_update_level(player_position)

    barrels[2] = barrels[2] + (player_position - last_player_position)
    last_player_position = player_position
end

function Level_restart_level()
end

function Level_is_complete()
    return distance(barrels[1], barrels[2]) < 1.0
end

function Level_name()
    return 'apple'
end

function barrel_count()
    return #barrels
end

function barrel_position(index)
    return barrels[index]
end