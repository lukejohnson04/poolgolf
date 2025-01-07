
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
    
    for (int i=0; i<MAP_SIZE; i++)
    {
        for (int j=0; j<MAP_SIZE; j++)
        {
            Color pixel = getPixel(mp_surf, i, j);
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
    } else
    {
        i32 mostStrokes = 0;
        i32 playerWithMostStrokes = 0;
        for (int i=0; i<game_state->playerCount; i++)
        {
            BallInit(game_state->players[i].ball);
            game_state->players[i].madeBallOnCurrentRound = false;
            game_state->players[i].ball->pos = level->ballSpawnPosition;
            if (playerWithMostStrokes == 0 ||
                game_state->players[i].strokeCount > mostStrokes)
            {
                playerWithMostStrokes = i;
                mostStrokes = game_state->players[i].strokeCount;
            }
        }
        game_state->currentPlayer = playerWithMostStrokes;
    }

    // give all players debug ability
    for (int i=0; i<game_state->playerCount; i++)
    {
        // Debug initialize test ability
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::HEAVY_WIND;
    }
}

void StartTurn()
{
    LevelState *level = &game_state->level;
    game_state->roundState = GameState::AIMING;
    level->cuePower = 0.f;
    level->shotAlready = false;
    AbilityCodeForWhenTurnStarts();
}

void FinishTurn();

void OnBallsStopMoving()
{
    LevelState *level = &game_state->level;
    i32 unfinishedPlayers = 0;
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

        if (player->madeBallOnCurrentRound == false)
        {
            unfinishedPlayers++;
        }
    }
    
    if (unfinishedPlayers == 0 || GetCurrentPlayer()->abilityCount == 0)
    {
        FinishTurn();
        return;
    }
    
    // Ability stage
    game_state->roundState = GameState::POST_SHOT;
    level->shotAlready = true;
}

void FinishTurn()
{
    LevelState *level = &game_state->level;

    float greatestDistance=0.f;
    i32 greatestDistPlayer=0;
    i32 unfinishedPlayers=0;
    for (int i=0; i<game_state->playerCount; i++)
    {
        PlayerData *player = &game_state->players[i];
        
        if (player->madeBallOnCurrentRound)
        {
            continue;
        }

        unfinishedPlayers++;
        float distToHole = DistanceBetween(
            player->ball->pos,
            level->holePos);
        if (i==0 || distToHole > greatestDistance)
        {
            greatestDistPlayer = i;
            greatestDistance = distToHole;
        }
    }

    if (unfinishedPlayers == 0)
    {
        InitializeNewRound(level);
        StartTurn();
    } else
    {
        game_state->currentPlayer = greatestDistPlayer;
        StartTurn();
    }
}

