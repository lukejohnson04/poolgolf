
const float RESTITUTION = 0.95f;
const float FRICTION = 3.25f;
const float GRAVITY = 11.f;

struct Ball
{
    v2 pos;
    v2 vel;
    float radius;
    float mass;
    
    bool active=false;
    bool falling=false;
    bool fallingInHole=false;
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

struct LevelState;

internal void BallInit(Ball *a);
internal v2 BallGetCollisionPoint(Ball *a, Ball *b, v2 dir);
internal void BallHandleCollision(Ball *a, Ball *b);
internal void BallPredictCollisionResolve(Ball *a, Ball *b,
                                          v2 impact_dir, v2 collision_point,
                                          v2 *a_dir, v2 *b_dir);
internal void UpdateBall(Ball *ball, LevelState *level, float delta);


