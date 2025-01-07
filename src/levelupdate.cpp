
const int HOLE_SIZE = 56;

void UpdateLevel(LevelState *level, float delta)
{
    bool stopped=true;
    for (int i = 0; i < level->ballCount; i++)
    {
        Ball *ball = &level->balls[i];
        // see if it hits an item
        for(int j=0; j<level->itemDropCount; j++)
        {
            v2 dropPos = level->itemDrops[j];
            fRect dropRect = {dropPos.x - 24.f, dropPos.y - 24.f, 48.f, 48.f};
            if (dropRect.contains(ball->pos))
            {
                game_state->players[i].unprocessedItems++;
                level->itemDrops[j] = level->itemDrops[level->itemDropCount-1];
                level->itemDropCount--;
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

        
        UpdateBall(ball, level, delta);
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
                    level->holePos,
                    (1.f - (ball->radius / 16.f))*0.5f);
                continue;
                
            } else if (ball->active == false)
            {
                // Ball fell off, must reset it
                BallInit(ball);
                ball->pos = level->ballStartPosition;
            }
        }

        if (IsBallOnSpawnTile(level, ball->pos))
        {
            continue;
        }
        
        if (ball->vel.x != 0 || ball->vel.y != 0) {
            for (int n = 0; n < level->ballCount; n++) {
                if (n == i) {
                    continue;
                }
                BallHandleCollision(ball, &level->balls[n]);
            }
        }
        if (DistanceBetween(ball->pos, level->holePos) < HOLE_SIZE/2) {
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
            level->ballsStoppedTimer = 0.f;
        } else
        {
            level->ballsStoppedTimer += delta;
            if (level->ballsStoppedTimer > 1.f)
            {
                level->ballsStoppedTimer = 0.f;
                OnBallsStopMoving();
            }
        }
    }
}

