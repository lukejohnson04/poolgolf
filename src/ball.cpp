
internal
void BallInit(Ball *a)
{
    *a = {};
    a->radius = 12.f;
    a->mass = PIf * a->radius * a->radius;
    a->vel = {0.f,0.f};
    a->active = true;
}

internal
v2 BallGetCollisionPoint(Ball *a, Ball *b, v2 dir)
{
    v2 closest = GetClosestPointOnLine(a->pos, a->pos + dir, b->pos);
    float dist_to_closest = DistanceBetween(closest, b->pos);
    if ((a->pos.x == b->pos.x && a->pos.y == b->pos.y) ||
        dist_to_closest >= a->radius + b->radius) {
        return v2(-1, -1);
    }

    float offset;
    if (dist_to_closest == 0) {
        offset = b->radius+a->radius;
    } else {
        offset = (float)sqrt(pow((double)(a->radius + b->radius), 2) - (double)(dist_to_closest * dist_to_closest));
    }
    closest = closest - (Normalize(dir) * offset);
    
    return closest;
}

internal
void BallHandleCollision(Ball *a, Ball *b)
{
    v2 closest = BallGetCollisionPoint(a, b, a->vel);
    if (closest.x == -1 && closest.y == -1)
    {
        return;
    }
    
    float n_dist = DistanceBetween(a->pos, closest);
    if (n_dist > Length(a->vel))
    {
        return;
    }

    // correct a's position to exactly where they first collide
    float offset = DistanceBetween(a->pos, closest);
    a->pos = closest;

    v2 r = b->pos - a->pos;
    v2 collisionNormal = Normalize(r);

    v2 relativeVelocity = a->vel - b->vel;
    float impulse;
    float massRatio = ((a->mass * b->mass) / (a->mass + b->mass));
    impulse = (1.f + RESTITUTION) * massRatio * V2DotProduct(relativeVelocity, collisionNormal);

    a->vel = a->vel - (collisionNormal * (impulse / a->mass));
    b->vel = b->vel + (collisionNormal * (impulse / b->mass));

    a->pos = a->pos + (Normalize(a->vel) * offset);
}

internal
void BallPredictCollisionResolve(Ball *a, Ball *b, v2 impact_dir, v2 collision_point, v2 *a_dir, v2 *b_dir)
{
    // just calculate simply for now
    float mass1 = a->mass;
    float mass2 = b->mass;

    v2 dir = b->pos - collision_point;
    dir = Normalize(dir);

    float vval_1 = V2DotProduct(impact_dir, dir);
    float vval_2 = 0;

    // is this code wrong?
    float newVVAL_1 = (mass1 * vval_1 + mass2 * vval_2 - mass2 * (vval_1 - vval_2) * RESTITUTION) / (mass1 + mass2);
    float newVVAL_2 = (mass1 * vval_1 + mass2 * vval_2 - mass1 * (vval_2 - vval_1) * RESTITUTION) / (mass1 + mass2);

    *a_dir = impact_dir + (dir * (newVVAL_1 - vval_1));
    *b_dir = dir * (newVVAL_2 - vval_2);
}

internal
void UpdateBall(Ball *ball, LevelState *level, float delta)
{
    if (!ball->active)
    {
        return;
    }
    
    if (ball->falling)
    {
        float fallRate = 16.f;
        ball->radius -= fallRate * delta;
        
        if (ball->fallingInHole == false)
        {
            ball->vel *= 0.9f;
            ball->pos += ball->vel;
        }
        return;
    }
    
    // friction
    float len = Length(ball->vel);
    len -= FRICTION * delta;

    v2 vel = ball->vel;
    
    if (len < 0) {
        ball->vel = v2(0, 0);
        return;
    }
    vel = Normalize(vel) * len;

    ball->vel = vel;
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int n = 0; n < MAP_SIZE; n++) {
            i32 tile = level->tiles[i][n];
            if (tile != TILE_TYPE::WALL &&
                tile != TILE_TYPE::WATER &&
                IsTileDownhill(tile) == false
                ) {
                continue;
            }
            
            v2 tile_pos = GetTileWorldPos(i, n);

            fRect tile_rect(tile_pos.x, tile_pos.y, 64, 64);
            fRect col_rect(ball->left() + ball->vel.x, ball->top(), ball->radius * 2, ball->radius * 2);

            if (tile == TILE_TYPE::WALL)
            {
                if (DoRectsCollide(col_rect, tile_rect)) {
                    ball->vel.x = -ball->vel.x;
                }
                col_rect.x = ball->left();
                col_rect.y = ball->top() + ball->vel.y;
                if (DoRectsCollide(col_rect, tile_rect)) {
                    ball->vel.y = -ball->vel.y;
                }                

            } else
            {
                if (tile_rect.contains(ball->pos))
                {
                    if (IsTileDownhill(tile))
                    {
                        v2 slopeDirection = v2(1.0f, 0.0f);
                        float degrees = (tile - TILE_TYPE::DOWNHILL_RIGHT) / 8.f;
                        degrees *= PIf * 2.f;
                        slopeDirection = V2Rotate(slopeDirection, degrees);
                        // Angle of the slope
                        float g = GRAVITY * (float)sin(deg_2_rad(45));
                        
                        ball->vel += slopeDirection * (g * delta);
                    
                    // Tile is water
                    } else
                    {
                        // Leniency for water tiles
                        if (DistanceBetween(ball->pos, v2(tile_rect.x+tile_rect.w/2,tile_rect.y+tile_rect.h/2)) <
                            tile_rect.w/2 - 2)
                        {
                            ball->falling = true;
                        }
                    }
                }
            }
        }
    }

    float unprocessedVelocity = Length(ball->vel);
    // obstacle collision
    for (i32 i=0; i<level->obstacleCount; i++)
    {
        v2 futurePos = ball->pos + (Normalize(ball->vel) * unprocessedVelocity);
        Obstacle obstacle = level->obstacles[i];
        float obstacleRot = obstacle.rot;
        // Translate the ball to the obstacles coordinate space
        v2 c = futurePos - obstacle.pos;
        c = V2Rotate(c, -obstacleRot);
        float oWidth = 128;
        float oHeight = 36;
        v2 clamped;
        clamped.x = CLAMP(-oWidth/2, oWidth/2, c.x);
        clamped.y = CLAMP(-oHeight/2, oHeight/2, c.y);

        float dx = c.x - clamped.x;
        float dy = c.y - clamped.y;
        float distSq = dx * dx + dy * dy;

        float r = ball->radius;
        if (distSq <= (r * r))
        {
            // collision
            printf("Collision\n");
            v2 clampedPointWorld = V2Rotate(clamped, obstacleRot);
            clampedPointWorld += obstacle.pos;

            v2 collisionNormalUnnormalized = futurePos - clampedPointWorld;
            v2 collisionNormal = Normalize(collisionNormalUnnormalized);

            // Push ball out
            float d = Length(collisionNormalUnnormalized);
            ball->pos += collisionNormal * (r - d);
            unprocessedVelocity -= (r - d);

            // Reflect velocity
            ball->vel -= (collisionNormal * (2.f * V2DotProduct(ball->vel, collisionNormal)));
        }
    }

    ball->pos += Normalize(ball->vel) * unprocessedVelocity;
}
