
const int HOLE_SIZE = 56;

void UpdateLevel(float delta)
{
    bool stopped=true;
    for (int i = 0; i < game_state->ballCount; i++)
    {
        Ball *ball = &game_state->balls[i];
        // see if it hits an item
        for(int j=0; j<game_state->itemDropCount; j++)
        {
            v2 dropPos = game_state->itemDrops[j];
            fRect dropRect = {dropPos.x - 24.f, dropPos.y - 24.f, 48.f, 48.f};
            if (dropRect.contains(ball->pos))
            {
                game_state->players[i].unprocessedItems++;
                game_state->itemDrops[j] = game_state->itemDrops[game_state->itemDropCount-1];
                game_state->itemDropCount--;
                j--;
            }
        }
        
        if (ball->active == false)
        {
            continue;
        }

        if (Length(ball->vel) >= 0.001f)
        {
            stopped = false;
        }

        
        UpdateBall(ball, game_state->tiles, delta);
        if (ball->falling)
        {
            if (ball->radius <= 0)
            {
                ball->active = false;
            }

            if (ball->fallingInHole)
            {
                ball->pos = Lerp(
                    ball->pos,
                    game_state->holePos,
                    (1.f - (ball->radius / 16.f))*0.5f);
                continue;
                
            } else if (ball->active == false)
            {
                // Ball fell off, must reset it
                BallInit(ball);
                ball->pos = game_state->ballStartPosition;
            }
        }

        if (IsBallOnSpawnTile(ball->pos))
        {
            continue;
        }
        
        if (ball->vel.x != 0 || ball->vel.y != 0) {
            for (int n = 0; n < game_state->ballCount; n++) {
                if (n == i) {
                    continue;
                }
                BallHandleCollision(ball, &game_state->balls[n]);
            }
        }
        if (DistanceBetween(ball->pos, game_state->holePos) < HOLE_SIZE/2) {
            ball->falling = true;
            ball->fallingInHole = true;
            game_state->players[i].madeBallOnCurrentRound = true;
        }
    }

    // Check if the shooting segment is over
    if (game_state->roundState == GameState::BALL_MOVING)
    {
        if (stopped == false)
        {
            game_state->ballsStoppedTimer = 0.f;
        } else
        {
            game_state->ballsStoppedTimer += delta;
            if (game_state->ballsStoppedTimer > 1.f)
            {
                game_state->ballsStoppedTimer = 0.f;
                OnBallsStopMoving();
            }
        }
    }
}

