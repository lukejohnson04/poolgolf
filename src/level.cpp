
internal
bool IsBallOnSpawnTile(LevelState *level, v2 pos)
{
    // check if ball is in spawn position (balls can't collide in the spawn zone)
    v2i ballTile = {
        (i32)pos.x / 64.f,
        (i32)pos.y / 64.f};
    if (level->validSpawn[ballTile.x][ballTile.y])
    {
        return true;
    }
    return false;
}

void LoadLevelForFirstTime(LevelState *level)
{    
    i32 players = 2;
    for (int i=0; i<players; i++)
    {
        game_state->playerCount++;
        level->ballCount++;
        BallInit(&level->balls[i]);
        game_state->players[i].ball = &level->balls[i];
        game_state->players[i].id = i;
        ChangeStrokes(&game_state->players[i], 0);
    }
    
    // load map
    SDL_Surface *mp_surf = IMG_Load("res/levels.png");

    i32 levelNumber = 2;
    
    for (int i=0; i<MAP_SIZE; i++)
    {
        for (int j=0; j<MAP_SIZE; j++)
        {
            Color pixel = getPixel(mp_surf, levelNumber * MAP_SIZE + i, j);
            level->tiles[i][j] = TILE_TYPE::GRASS;
            level->validSpawn[i][j] = false;
            if (pixel == COLOR_BLACK)
            {
                level->tiles[i][j] = TILE_TYPE::WALL;
            } else if (pixel == COLOR_YELLOW)
            {
                level->holePos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_WHITE)
            {
                level->ballSpawnPosition = {i*64.f, j*64.f};
                for (i32 n=0; n<level->ballCount; n++)
                {
                    level->balls[n].pos = level->ballSpawnPosition;
                }
                level->validSpawn[i][j] = true;
            } else if (pixel == COLOR_BLUE)
            {
                level->tiles[i][j] = TILE_TYPE::WATER;
            } else if (pixel == COLOR_RED)
            {
                // Item
                level->itemDropSpawns[level->itemDropSpawnCount++] = {i*64.f + 32.f, j*64.f + 32.f};
            } else if (pixel == 0xff9600ff)
            {
                level->tiles[i][j] = TILE_TYPE::GRASS;
                level->bouncers[level->bouncerCount++] = {i*64.f + 32.f, j*64.f + 32.f};
            } else if (pixel == COLOR_GREEN)
            {
                level->tiles[i][j] = TILE_TYPE::GRASS;
            } else if (pixel == 0x648c1eff)
            {
                level->validSpawn[i][j] = true;
                
            } else
            {
                level->tiles[i][j] = GetDownhillTileFromPixel(pixel);
            }
        }
    }
    SDL_FreeSurface(mp_surf);
}

void InitializeNewRound(LevelState *level)
{
    level->itemDropCount = 0;
    for (int i=0; i<level->itemDropSpawnCount; i++)
    {
        level->itemDrops[level->itemDropCount++] = level->itemDropSpawns[i];
    }
    game_state->round++;

    if (game_state->round == 1)
    {
        game_state->currentPlayer = 0;
        for (int i=0; i<game_state->playerCount; i++)
        {
            game_state->players[i].unprocessedItems = 1;
        }
    } else
    {
        i32 mostScore = 0;
        i32 playerWithMostScore = 0;
        for (int i=0; i<game_state->playerCount; i++)
        {
            PlayerData *player = &game_state->players[i];
            BallInit(player->ball);
            player->ball->pos = level->ballSpawnPosition;
            player->madeBallOnCurrentRound = false;
            // Give each player items corresponding to how many strokes under par they were
            i32 gainedItems = MAX(1, level->par - player->strokeCount + 1);
            player->unprocessedItems += gainedItems;
            printf("Player %d gained %d items\n", player->id, gainedItems);
            
            player->score += player->strokeCount;
            ChangeStrokes(player, 0);
            
            if (playerWithMostScore == 0 ||
                player->score > mostScore)
            {
                playerWithMostScore = i;
                mostScore = player->score;
            }
        }
        game_state->currentPlayer = playerWithMostScore;
    }

    ProcessCollectedItems();

    // give all players debug ability
    for (int i=0; i<game_state->playerCount; i++)
    {
        // Debug initialize test ability
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::HEAVY_WIND;
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::PLACE_OBSTACLE;
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::SHANK;
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::CRATER;
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::PLACE_BOUNCER;
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::PROTECTION;
    }
}

void StartTurn()
{
    LevelState *level = &game_state->level;
    SetRoundState(GameState::AIMING);
    level->cuePower = 0.f;
    level->shotAlready = false;
    AbilityCodeForWhenTurnStarts();
}

void ProcessCollectedItems()
{
    for (int i=0; i<game_state->playerCount; i++)
    {
        PlayerData *player = &game_state->players[i];
        if (player->unprocessedItems > 0)
        {
            player->unprocessedItems = CLAMP(0, MAX_ABILITIES - player->abilityCount, player->unprocessedItems);
            for (int n=0; n<player->unprocessedItems; n++)
            {
                i32 randAbility = rand() % ABILITY::ABILITY_COUNT;
                player->abilities[player->abilityCount++] = randAbility;
                printf("Player %d got ability %d\n", player->id, randAbility);
            }
            player->unprocessedItems = 0;
        }
    }    
}

i32 GetUnfinishedPlayers()
{    
    i32 unfinishedPlayers = 0;
    for (int i=0; i<game_state->playerCount; i++)
    {        
        if (game_state->players[i].madeBallOnCurrentRound == false)
        {
            unfinishedPlayers++;
        }
    }
    return unfinishedPlayers;
}

void OnBallsStopMoving()
{
    for (i32 i=0; i<game_state->level.ballCount; i++)
    {
        Ball *ball = &game_state->level.balls[i];
        if (ball->shieldBrokenOnThisShot)
        {
            ball->shielded = false;
            ball->shieldBrokenOnThisShot = false;
        }
    }

    ProcessCollectedItems();
    i32 stillPlaying = GetUnfinishedPlayers();

        
    if (stillPlaying == 0)
    {
        InitializeNewRound(&game_state->level);
        StartTurn();
        return;
    }


    if (GetPreviousRoundState() == GameState::USE_ABILITY)
    {
        i32 returnState = GetRoundState(2);
        // Reset to the round state before the ability was used.
        // The exception is if you're going to
        // the post shot state and there are no abilities to be used,
        // just fall through and switch turns as usual.
        if (returnState == GameState::POST_SHOT &&
            GetCurrentPlayer()->abilityCount != 0)
        {
            SetRoundState(returnState);
            return;
        // The other exception is if the prestate was aiming,
        // and the ball is now in the hole, skip to the post state
        // or end the turn depending on if the player has abilities
        } else if (returnState == GameState::AIMING)
        {
            if (GetCurrentPlayer()->madeBallOnCurrentRound == true)
            {
                if (GetCurrentPlayer()->abilityCount != 0)
                {
                    SetRoundState(GameState::POST_SHOT);
                } else
                {
                    FinishTurn();
                }
            } else
            {
                SetRoundState(returnState);
            }
            return;            
        }
    }


    if (GetCurrentPlayer()->abilityCount == 0)
    {
        FinishTurn();
        return;
    }

    SetRoundState(GameState::POST_SHOT);
    game_state->level.shotAlready = true;
}

void FinishTurn()
{
    LevelState *level = &game_state->level;

    float greatestDistance=0.f;
    i32 greatestDistPlayer=0;

    for (int i=0; i<game_state->playerCount; i++)
    {
        PlayerData *player = &game_state->players[i];
        
        if (player->madeBallOnCurrentRound)
        {
            continue;
        }

        float distToHole = DistanceBetween(
            player->ball->pos,
            level->holePos);
        if (i==0 || distToHole > greatestDistance)
        {
            greatestDistPlayer = i;
            greatestDistance = distToHole;
        }
    }

    game_state->currentPlayer = greatestDistPlayer;
    StartTurn();
}
