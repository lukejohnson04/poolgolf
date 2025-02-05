
const float RESTITUTION = 0.95f;
const float FRICTION = 3.25f;

struct Ball
{
    v2 pos;
    v2 vel;
    float radius;
    float mass;
    
    bool active=false;
    bool falling=false;
    float fallTimer=0.f;

    float left() {
        return pos.x - radius;
    }

    float right() {
        return pos.x + radius;
    }

    float top() {
        return pos.y - radius;
    }

    float bottom() {
        return pos.y + radius;
    }
};


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

void UpdateBall(Ball *ball, int tiles[][64], float delta)
{
    if (!ball->active)
    {
        return;
    }
    
    if (ball->falling)
    {
        float fallRate = 16.f;
        ball->radius -= fallRate * delta;
        if (ball->radius <= 0)
        {
            ball->active = false;
        }
        return;
    }
    
    // friction
    float len = Length(ball->vel);
    len -= FRICTION * (1.f/60.f);

    v2 vel = ball->vel;
    
    if (len < 0) {
        ball->vel = v2(0, 0);
        return;
    }
    vel = Normalize(vel) * len;

    ball->vel = vel;
    for (int i = 0; i < 64; i++) {
        for (int n = 0; n < 64; n++) {
            if (tiles[i][n] != 0) {
                continue;
            }
            v2 tile_pos = GetTileWorldPos(i, n);

            fRect tile_rect(tile_pos.x, tile_pos.y, 64, 64);
            fRect col_rect(ball->left() + ball->vel.x, ball->top(), ball->radius * 2, ball->radius * 2);

            if (DoRectsCollide(col_rect, tile_rect)) {
                ball->vel.x = -ball->vel.x;
            }
            col_rect.x = ball->left();
            col_rect.y = ball->top() + ball->vel.y;
            if (DoRectsCollide(col_rect, tile_rect)) {
                ball->vel.y = -ball->vel.y;
            }
        }
    }

    ball->pos += ball->vel;
}
