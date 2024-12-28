
const int HOLE_SIZE = 56;

void UpdateLevel(float delta)
{
    for (int i = 0; i < game_state->ballCount; i++) {
        if (game_state->balls[i].active == false)
        {
            continue;
        }
        UpdateBall(&game_state->balls[i], game_state->tiles, delta);
        if (game_state->balls[i].falling)
        {
            game_state->balls[i].pos = Lerp(
                game_state->balls[i].pos,
                game_state->holePos,
                (1.f - (game_state->balls[i].radius / 16.f))*0.5f);
            continue;
        }
        if (game_state->balls[i].vel.x != 0 || game_state->balls[i].vel.y != 0) {
            for (int n = 0; n < game_state->ballCount; n++) {
                if (n == i) {
                    continue;
                }
                BallHandleCollision(&game_state->balls[i], &game_state->balls[n]);
            }
        }
        if (DistanceBetween(game_state->balls[i].pos, game_state->holePos) < HOLE_SIZE/2) {
            game_state->balls[i].falling = true;
        }
    }
}
