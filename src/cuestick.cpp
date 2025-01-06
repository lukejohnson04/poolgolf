
void ControlCueStick(float delta)
{
    if (game_state->roundState == GameState::AIMING)
    {
        float cueRotSpeed = game_state->cueRotSpeed;
        if (input->just_pressed[SDL_SCANCODE_Q])
        {
            if (cueRotSpeed == 2.f)
            {
                cueRotSpeed = 0.5f;
            } else if (cueRotSpeed == 0.5f)
            {
                cueRotSpeed = 0.1f;
            } else if (cueRotSpeed == 0.1f)
            {
                cueRotSpeed = 0.01f;
            }        
        }

        if (input->just_pressed[SDL_SCANCODE_E])
        {
            if (cueRotSpeed == 0.01f)
            {
                cueRotSpeed = 0.1f;
            } else if (cueRotSpeed == 0.1f)
            {
                cueRotSpeed = 0.5f;
            } else if (cueRotSpeed == 0.5f)
            {
                cueRotSpeed = 2.0f;
            }
        }

        if (input->is_pressed[SDL_SCANCODE_A])
        {
            game_state->cueRotation -= cueRotSpeed*PIf * delta;
        }

        if (input->is_pressed[SDL_SCANCODE_D])
        {
            game_state->cueRotation += cueRotSpeed*PIf * delta;
        }

        game_state->cueRotSpeed = cueRotSpeed;

        if (input->is_pressed[SDL_SCANCODE_S]) {
            game_state->cuePower += 100.f * delta;
        }

        if (input->is_pressed[SDL_SCANCODE_W]) {
            game_state->cuePower -= 100.f * delta;
        }

        game_state->cuePower = CLAMP(1.f, 100.f, game_state->cuePower);

        // hit ball
        if (input->is_pressed[SDL_SCANCODE_SPACE])
        {
            game_state->roundState = GameState::SHOOTING_MOTION;
            game_state->shootingMotionTimer = 0.f;
        }
    } else if (game_state->roundState == GameState::SHOOTING_MOTION)
    {
        game_state->shootingMotionTimer += delta;
        if (game_state->shootingMotionTimer >= 1.f)
        {
            game_state->roundState = GameState::BALL_MOVING;
            game_state->ballStartPosition = GetCurrentPlayer()->ball->pos;
            GetCurrentPlayer()->ball->vel = ConvertAngleToVec(game_state->cueRotation + PIf) * game_state->cuePower*0.25f;
            GetCurrentPlayer()->strokeCount++;
            ChangeStrokes(GetCurrentPlayer(), GetCurrentPlayer()->strokeCount);
        }
    }
}
