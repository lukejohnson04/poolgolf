
void OnAbilitySelected(i32 ability)
{
    if (ability == ABILITY::CRATER)
    {
        game_state->abilityState.craterAbility.selectedBall = GetCurrentPlayer()->id;

    } else if (ability == ABILITY::PLACE_OBSTACLE)
    {
        printf("Obstacle place selected\n");
        
    } else if (ability == ABILITY::RETRY)
    {
        game_state->level = game_state->levelBuffer;
        ConsumeAbility(GetCurrentPlayer());
        SetRoundState(GameState::AIMING);
        ChangeStrokes(GetCurrentPlayer(), GetCurrentPlayer()->strokeCount-1);

    } else if (ability == ABILITY::HEAVY_WIND)
    {
        game_state->abilityState.windAbility.needleRotation = 0.f;
        game_state->abilityState.windAbility.selectionTime = 0.f;
        game_state->abilityState.windAbility.activated = false;

    } else if (ability == ABILITY::SHANK)
    {
    } else if (ability == ABILITY::PROTECTION)
    {
        GetCurrentPlayer()->ball->shielded = true;
        ConsumeAbility(GetCurrentPlayer());
        SetRoundState(GetRoundState(1));
    }
}

void OnAbilityUse(i32 ability)
{
    LevelState *level = &game_state->level;
    if (ability == ABILITY::CRATER)
    {
        // Place a crater around the selected ball
        v2 ballPos = level->balls[game_state->abilityState.craterAbility.selectedBall].pos;
        v2i ballTile = (v2i)(ballPos * (1.f/64.f));
        for (int i=0; i<7; i++)
        {
            for (int j=0; j<7; j++)
            {
                TILE_TYPE craterValue = craterPattern[i][j];
                v2i tilePos = {ballTile.x - 3 + i, ballTile.y - 3 + j};
                if (tilePos.x < 0 || tilePos.y < 0 ||
                    tilePos.x >= MAP_SIZE || tilePos.y >= MAP_SIZE)
                {
                    continue;
                }
                
                i32 tileType = level->tiles[tilePos.x][tilePos.y];
                if (tileType != TILE_TYPE::GRASS &&
                    IsTileDownhill(tileType) == false)
                {
                    continue;
                }
                    
                if (craterValue != TILE_TYPE::TT_NONE)
                {
                    level->tiles[tilePos.x][tilePos.y] = craterValue;
                }
            }
        }            
    } else if (ability == ABILITY::HEAVY_WIND)
    {
        float windForce = 5.f;
        v2 windDir = -ConvertAngleToVec(game_state->abilityState.windAbility.needleRotation);
        windDir *= windForce;
        for (i32 i=0; i<level->ballCount; i++)
        {
            Ball *ball = &level->balls[i];
            if (ball->active == false || ball->falling)
            {
                continue;
            }
            ball->vel = windDir;
        }
        SetRoundState(GameState::BALL_MOVING);
        
    } else if (ability == ABILITY::RETRY)
    {
        printf("Used retry! 2x\n");
    }
}

void RestoreAfterAbilityUse()
{
    LevelState *level = &game_state->level;
    game_state->ability = ABILITY::NONE;
    if (level->shotAlready)
    {
        OnBallsStopMoving();
        
    } else
    {
        SetRoundState(GameState::AIMING);
    }
}

void AbilityCodeForWhenTurnStarts()
{
    PlayerData *player = GetCurrentPlayer();
    for (i32 i=0; i<player->abilityCount; i++)
    {
        i32 ability = player->abilities[i];
        if (ability == ABILITY::RETRY)
        {
            // Create save of the world
            game_state->levelBuffer = game_state->level;
        }
    }
}

