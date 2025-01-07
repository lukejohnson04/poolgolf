
void ControlCueStick(float delta)
{
    LevelState *level = &game_state->level;
    if (game_state->roundState == GameState::AIMING)
    {
        float cueRotSpeed = level->cueRotSpeed;
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
            level->cueRotation -= cueRotSpeed*PIf * delta;
        }

        if (input->is_pressed[SDL_SCANCODE_D])
        {
            level->cueRotation += cueRotSpeed*PIf * delta;
        }

        level->cueRotSpeed = cueRotSpeed;

        if (input->is_pressed[SDL_SCANCODE_S]) {
            level->cuePower += 100.f * delta;
        }

        if (input->is_pressed[SDL_SCANCODE_W]) {
            level->cuePower -= 100.f * delta;
        }

        level->cuePower = CLAMP(1.f, 100.f, level->cuePower);

        // hit ball
        if (input->is_pressed[SDL_SCANCODE_SPACE])
        {
            game_state->roundState = GameState::SHOOTING_MOTION;
            level->shootingMotionTimer = 0.f;
        }
    } else if (game_state->roundState == GameState::SHOOTING_MOTION)
    {
        level->shootingMotionTimer += delta;
        if (level->shootingMotionTimer >= 1.f)
        {
            game_state->roundState = GameState::BALL_MOVING;
            level->ballStartPosition = GetCurrentPlayer()->ball->pos;
            GetCurrentPlayer()->ball->vel = ConvertAngleToVec(level->cueRotation + PIf) * level->cuePower*0.25f;
            GetCurrentPlayer()->strokeCount++;
            ChangeStrokes(GetCurrentPlayer(), GetCurrentPlayer()->strokeCount);
        }
    }
}
