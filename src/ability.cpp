
void OnAbilitySelected(i32 ability)
{
    if (ability == ABILITY::CRATER)
    {
        game_state->abilityState.craterAbility.selectedBall = GetCurrentPlayer()->id;

    } else if (ability == ABILITY::PLACE_OBSTACLE)
    {
        printf("Obstacle place selected\n");
    }
}

void OnAbilityUse(i32 ability)
{
    if (ability == ABILITY::CRATER)
    {
        // Place a crater around the selected ball
        v2 ballPos = game_state->balls[game_state->abilityState.craterAbility.selectedBall].pos;
        v2i ballTile = (v2i)(ballPos * (1.f/64.f));
        for (int i=0; i<7; i++)
        {
            for (int j=0; j<7; j++)
            {
                i32 craterValue = craterPattern[i][j];
                v2i tilePos = {ballTile.x - 3 + i, ballTile.y - 3 + j};
                if (tilePos.x < 0 || tilePos.y < 0 ||
                    tilePos.x >= MAP_SIZE || tilePos.y >= MAP_SIZE)
                {
                    continue;
                }
                
                i32 tileType = game_state->tiles[tilePos.x][tilePos.y];
                if (tileType != TILE_TYPE::GRASS &&
                    IsTileDownhill(tileType) == false)
                {
                    continue;
                }
                    
                if (craterValue != TILE_TYPE::TT_NONE)
                {
                    game_state->tiles[tilePos.x][tilePos.y] = craterValue;
                }
            }
        }            
    }
}

void RestoreAfterAbilityUse()
{
    game_state->ability = ABILITY::NONE;
    if (game_state->shotAlready)
    {
        OnBallsStopMoving();
        
    } else
    {
        game_state->roundState = GameState::AIMING;
    }
}
