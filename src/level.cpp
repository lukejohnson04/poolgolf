

internal
bool IsBallOnSpawnTile(v2 pos)
{
    // check if ball is in spawn position (balls can't collide in the spawn zone)
    v2i ballTile = {
        (i32)pos.x / 64.f,
        (i32)pos.y / 64.f};
    if (game_state->validSpawn[ballTile.x][ballTile.y])
    {
        return true;
    }
    return false;
}
 
void InitializeNewRound()
{
    if (game_state->round != 0)
    {
        printf("Round %d finished. Resetting game...", game_state->round);
    }
    game_state->itemDropCount = 0;
    for (int i=0; i<game_state->itemDropSpawnCount; i++)
    {
        game_state->itemDrops[game_state->itemDropCount++] = game_state->itemDropSpawns[i];
    }
    game_state->roundState = GameState::AIMING;
    game_state->round++;
    game_state->cuePower = 0.f;

    i32 mostStrokes = 0;
    i32 playerWithMostStrokes=0;
    for (int i=0; i<game_state->playerCount; i++)
    {
        BallInit(game_state->players[i].ball);
        game_state->players[i].madeBallOnCurrentRound = false;
        game_state->players[i].ball->pos = game_state->ballSpawnPosition;
        if (playerWithMostStrokes == 0 ||
            game_state->players[i].strokeCount > mostStrokes)
        {
            playerWithMostStrokes = i;
            mostStrokes = game_state->players[i].strokeCount;
        }
        // Debug initialize test ability
        game_state->players[i].abilities[game_state->players[i].abilityCount++] = ABILITY::PLACE_OBSTACLE;
    }
    game_state->currentPlayer = playerWithMostStrokes;
}

void FinishTurn();

void OnBallsStopMoving()
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

    if (GetCurrentPlayer()->abilityCount == 0)
    {
        FinishTurn();
        return;
    }
    
    // Ability stage
    game_state->roundState = GameState::POST_SHOT;
    game_state->shotAlready = true;
}

void FinishTurn()
{
    game_state->roundState = GameState::AIMING;
    game_state->cuePower = 0.f;
    game_state->shotAlready = false;

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
            game_state->holePos);
        if (i==0 || distToHole > greatestDistance)
        {
            greatestDistPlayer = i;
            greatestDistance = distToHole;
        }
    }

    if (unfinishedPlayers == 0)
    {
        InitializeNewRound();
    } else
    {
        game_state->currentPlayer = greatestDistPlayer;
    }
}
