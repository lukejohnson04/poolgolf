
#include "core/engine.cpp"
#include "button.cpp"

/*
 * How to write code in this code base:
 * Primitive structs are snake_case
 * Variables are snake_case
 *
 * Other structs are PascalCase
 * Functions are PascalCase
 *
 * Member functions are camelCase
 */

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#include "tile.cpp"
#include "ability.h"


const int MAP_SIZE = 128;

TILE_TYPE craterPattern[7][7];

#include "shapes.cpp"
#include "ball.h"
#include "camera.cpp"

#include "level.h"
#include "player.h"
#include "game_state.h"
#include "player.cpp"
#include "level.cpp"
#include "game_state.cpp"
#include "ball.cpp"

#include "cuestick.cpp"
#include "levelupdate.cpp"
#include "renderpreviewline.cpp"
#include "ability.cpp"

#if defined __cplusplus
extern "C"
#endif
GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    game_state = (GameState*)game_memory->permanentStorage;
    input = n_input;
    private_global_assets = &game_state->assets;
    if (!input) {
        return;
    }
    
    if (game_memory->is_initialized == false) {
        InitializeGameMemory(game_memory);
    }
    
    // <----------------------->
    //       UPDATE BEGIN
    // <----------------------->
    game_state->globalTimer += delta;
    
    ControlCueStick(delta);

    // Update balls
    LevelState *level = &game_state->level;
    UpdateLevel(level, delta);

    if (input->just_pressed[SDL_SCANCODE_GRAVE])
    {
        game_state->autocam = !game_state->autocam;
    }

    if (game_state->roundState != GameState::AIMING)
    {
        game_state->autocam = true;
    }

    if (game_state->roundState == GameState::USE_ABILITY)
    {
        if (game_state->ability == ABILITY::CRATER || game_state->ability == ABILITY::SHANK)
        {
            v2 cameraDest = level->balls[game_state->abilityState.craterAbility.selectedBall].pos - v2(WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f);
            game_state->camera.pos = Lerp(game_state->camera.pos, cameraDest, 0.05f);
        }
    } else if (game_state->autocam)
    {
        game_state->camera.pos = Lerp(game_state->camera.pos, GetCurrentPlayer()->ball->pos - v2(WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f), 0.05f);
    } else
    {
        // move camera around
        if (input->is_pressed[SDL_SCANCODE_J])
        {
            game_state->camera.pos.x -= 512.f * (1.0f / game_state->camera.zoom) * delta;
        }
        if (input->is_pressed[SDL_SCANCODE_L])
        {
            game_state->camera.pos.x += 512.f * (1.0f / game_state->camera.zoom) * delta;
        }
        if (input->is_pressed[SDL_SCANCODE_I])
        {
            game_state->camera.pos.y -= 512.f * (1.0f / game_state->camera.zoom) * delta;
        }
        if (input->is_pressed[SDL_SCANCODE_K])
        {
            game_state->camera.pos.y += 512.f * (1.0f / game_state->camera.zoom) * delta;
        }

        if (input->is_pressed[SDL_SCANCODE_1])
        {
            game_state->camera.zoom -= 1.f*delta;
        }
        if (input->is_pressed[SDL_SCANCODE_2])
        {
            game_state->camera.zoom += 1.f*delta;
        }
    }

    if (game_state->roundState == GameState::POSITIONING_BALL)
    {
        v2 mPos = GetMouseWorldPos(&game_state->camera);
        v2i tilePos = v2i((i32)(mPos.x / 64.f), (i32)(mPos.y / 64.f));
        tilePos.x = CLAMP(0, MAP_SIZE, tilePos.x);
        tilePos.y = CLAMP(0, MAP_SIZE, tilePos.y);

        if (level->validSpawn[tilePos.x][tilePos.y])
        {
            GetCurrentPlayer()->ball->pos = mPos;
        }

        if (input->mouse_just_pressed)
        {
            SetRoundState(GameState::AIMING);
        }
    }

    // Abilities
    if (game_state->roundState == GameState::AIMING)
    {
        if (input->just_pressed[SDL_SCANCODE_F])
        {
            if (GetCurrentPlayer()->strokeCount == 0)
            {
                SetRoundState(GameState::POSITIONING_BALL);
            }
        } else if (input->just_pressed[SDL_SCANCODE_5])
        {
            game_state->ability = ABILITY::PLACE_OBSTACLE;
            SetRoundState(GameState::USE_ABILITY);
        }
    }
    
    // <----------------------->
    //       RENDER BEGIN
    // <----------------------->
    
    // start by checking for resource updates
    static float res_timer=0.f;
    res_timer += delta;
    if (res_timer >= 0.1f) {
        res_timer = 0.f;
        CheckForResourceUpdates(&game_state->assets);
    }

    UseShader(GetShader("color"));
    GetShader("color")->UniformM4fv("projection",projection);
    UseShader(GetShader("texture"));
    GetShader("texture")->UniformM4fv("projection",projection);
    GetShader("texture")->Uniform4f("colorMod",1.f,1.f,1.f,1.f);
    
    glClearColor(0.2f, 0.2f, 0.21f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 cameraView = CalculateCameraViewMatrix(&game_state->camera);

    Shader *sh_texture = GetShader("texture");
    Shader *sh_color = GetShader("color");
    
    UseShader(sh_texture);
    sh_texture->UniformM4fv("model",model);
    sh_texture->UniformM4fv("view",cameraView);
    sh_texture->Uniform1i("_texture",GetTexture("res/sprites.png"));
    
    UseShader(sh_color);
    sh_color->UniformM4fv("model",model);
    sh_color->UniformM4fv("view",cameraView);

    // background tiles
    UseShader(sh_texture);
    int startx, starty, endx, endy;
    fRect camBounds = game_state->camera.GetBounds();
    startx = (int)(camBounds.x / 64.f) - 2;
    starty = (int)(camBounds.y / 64.f) - 2;
    endx = (int)((camBounds.x + camBounds.w*2.f) / 64.f) + 2;
    endy = (int)((camBounds.y + camBounds.h*2.f) / 64.f) + 2;

    startx = MAX(startx, 0);
    starty = MAX(starty, 0);
    endx = MIN(endx, MAP_SIZE);
    endy = MIN(endy, MAP_SIZE);

    for (int i=startx; i<endx; i++)
    {
        for (int j=starty; j<endy; j++)
        {
            v2 pos = GetTileWorldPos(i,j);
            i32 tile = level->tiles[i][j];
            iRect src = {0,32,32,32};
            iRect dest = {i*64,j*64,64,64};
            if (tile == TILE_TYPE::GRASS)
            {
                if (level->validSpawn[i][j])
                {
                    src.x = 96;
                    src.y = 0;
                    if (game_state->roundState == GameState::POSITIONING_BALL)
                    {
                        src.y = 32;
                    }
                } else
                {                    
                    src.x = 0;
                }
            } else if (tile == TILE_TYPE::WALL)
            {
                src.x = 32;
            } else if (tile == TILE_TYPE::WATER)
            {
                src.x = 64;
            } else if (IsTileDownhill(tile))
            {
                src.x = 96;
                GL_DrawTexture({128,32,32,32}, dest);
            }
            GL_DrawTexture(src, dest);

            // Draw arrow pointing in direction of slope
            if (IsTileDownhill(tile))
            {
                glm::mat4 rotMatrix = rotate_model_matrix(((tile - TILE_TYPE::DOWNHILL_RIGHT) * PIf*2.f) / 8.f, dest, {dest.w/2.f,dest.h/2.f});
                sh_texture->UniformM4fv("model", rotMatrix);
                GL_DrawTexture({128,32,32,32}, dest);
                sh_texture->UniformM4fv("model", glm::mat4(1.0));
            }
        }
    }

    // Draw bouncers over tiles
    for (i32 i=0; i<level->bouncerCount; i++)
    {
        v2 bouncerPos = level->bouncers[i];
        iRect bouncerSrc = {64, 80, 64, 64};
        fRect bouncerDest;
        bouncerDest.w = 128;
        bouncerDest.h = 128;
        bouncerDest.x = bouncerPos.x - bouncerDest.w / 2;
        bouncerDest.y = bouncerPos.y - bouncerDest.h / 2;
        GL_DrawTexture(bouncerSrc, bouncerDest);
    }

    // Draw obstacles
    for (i32 i=0; i<level->obstacleCount; i++)
    {
        v2 obstaclePos = level->obstacles[i].pos;
        iRect obstacleSrc = {192,32,64,18};
        fRect obstacleDest;
        obstacleDest.w = 128;
        obstacleDest.h = 36;
        obstacleDest.x = obstaclePos.x - obstacleDest.w / 2;
        obstacleDest.y = obstaclePos.y - obstacleDest.h / 2;

        glm::mat4 rotMat = rotate_model_matrix(level->obstacles[i].rot, obstacleDest, {obstacleDest.w/2,obstacleDest.h/2});
        sh_texture->UniformM4fv("model", rotMat);
        GL_DrawTexture(obstacleSrc, obstacleDest);
    }
    sh_texture->UniformM4fv("model", glm::mat4(1.0));


    // Draw preview tiles for obstacle placement ability
    if (game_state->roundState == GameState::USE_ABILITY)
    {
        if (game_state->ability == ABILITY::PLACE_OBSTACLE)
        {
            
            float rot = game_state->abilityState.obstacleAbility.rotation;
            float rotSpeed = 1.f * delta;
            if (input->is_pressed[SDL_SCANCODE_Q])
            {
                rot -= rotSpeed;
            }
            if (input->is_pressed[SDL_SCANCODE_E])
            {
                rot += rotSpeed;
            }

            game_state->abilityState.obstacleAbility.rotation = rot;

            v2i placement = GetMouseWorldPos(&game_state->camera);
            
            iRect obstacleSrc = {192,32,64,18};
            fRect obstacleDest;
            obstacleDest.w = 128;
            obstacleDest.h = 36;
            obstacleDest.x = placement.x - obstacleDest.w / 2;
            obstacleDest.y = placement.y - obstacleDest.h / 2;

            sh_texture->Uniform4f("colorMod",1.f,1.f,1.f,0.5f);
            glm::mat4 rotMat = rotate_model_matrix(rot, obstacleDest, {obstacleDest.w/2,obstacleDest.h/2});
            sh_texture->UniformM4fv("model", rotMat);
            GL_DrawTexture(obstacleSrc, obstacleDest);
            sh_texture->UniformM4fv("model", glm::mat4(1.0));
            sh_texture->Uniform4f("colorMod",1.f,1.f,1.f,1.f);
            
            //GL_DrawTexture({32,32,32,32}, {placement.x * 64, placement.y * 64, 64, 64});
            //GL_DrawTexture({32,32,32,32}, {secondPlacement.x * 64, secondPlacement.y * 64, 64, 64});
            
            if (input->mouse_just_pressed)
            {
                // This is much easier to do inline rather than putting it in
                // OnAbilityUse because the placements of the tiles needed for
                // the placement are already calculated for previewing
                Obstacle obstacle;
                obstacle.pos = placement;
                obstacle.rot = rot;
                level->obstacles[level->obstacleCount++] = obstacle;
                ConsumeAbility(GetCurrentPlayer());
                RestoreAfterAbilityUse();
            }
        } else if (game_state->ability == ABILITY::PLACE_BOUNCER)
        {
            v2i bouncerPos = GetMouseWorldPos(&game_state->camera);

            iRect bouncerSrc = {64, 80, 64, 64};
            fRect bouncerDest;
            bouncerDest.w = 128;
            bouncerDest.h = 128;
            bouncerDest.x = bouncerPos.x - bouncerDest.w / 2;
            bouncerDest.y = bouncerPos.y - bouncerDest.h / 2;

            sh_texture->Uniform4f("colorMod",1.f,1.f,1.f,0.5f);
            GL_DrawTexture(bouncerSrc, bouncerDest);
            sh_texture->Uniform4f("colorMod",1.f,1.f,1.f,1.f);
            
            if (input->mouse_just_pressed)
            {
                level->bouncers[level->bouncerCount++] = bouncerPos;
                ConsumeAbility(GetCurrentPlayer());
                RestoreAfterAbilityUse();
            }
        }
    }

    // Draw hole
    fRect holeDest = {level->holePos.x - HOLE_SIZE/2,
        level->holePos.y - HOLE_SIZE/2,
        HOLE_SIZE, HOLE_SIZE};
    GL_DrawTexture({0,80,64,64}, holeDest);

    // Balls
    for (int i=0; i<level->ballCount; i++)
    {
        Ball *ball = &level->balls[i];
        if (ball->active == false)
        {
            continue;
        }
        
        iRect src = {0,0,32,32};
        iRect dest = {
                (i32)ball->left(),
                (i32)ball->top(),
                (i32)(ball->radius*2.f),
                (i32)(ball->radius*2.f)};
        if (ball->shielded)
        {
            sh_texture->Uniform4f("colorMod",0.5f,0.6f,1.f,1.f);
            GL_DrawTexture(src,dest);
            sh_texture->Uniform4f("colorMod",1.f,1.f,1.f,1.f);
        }
        else
        {
            GL_DrawTexture(src,dest);
        }
    }

    // Draw item drops
    for (int i=0; i<level->itemDropCount; i++)
    {
        v2 pos = level->itemDrops[i];
        iRect dest = iRect((i32)(pos.x - 32.f), (i32)(pos.y - 32.f), 64, 64);
        dest.y += (i32)(sin(game_state->globalTimer * 3.0) * 6.0);
        GL_DrawTexture({160,32,32,32}, dest);
    }

    // Cue stick
    if (game_state->roundState == GameState::SHOOTING_MOTION ||
        game_state->roundState == GameState::AIMING ||
        game_state->roundState == GameState::BALL_MOVING)
    {
        float cueVisualDistance = level->cuePower + 16.f;
        v2 cueBallPos = GetCurrentPlayer()->ball->pos;
        if (game_state->roundState == GameState::SHOOTING_MOTION)
        {
            float maxCueDistance = cueVisualDistance + 0.75f * cueVisualDistance;
            if (level->shootingMotionTimer < 0.75f)
            {
                cueVisualDistance += level->shootingMotionTimer * cueVisualDistance;
            } else
            {
                float t = (level->shootingMotionTimer - 0.75f) / 0.25f;
                cueVisualDistance = Lerp(maxCueDistance, 0.f, t);
            }
        } else if (game_state->roundState == GameState::BALL_MOVING)
        {
            cueVisualDistance = 0.f;
            cueBallPos = level->ballStartPosition;
        }

        fRect dest = {cueBallPos.x - 16.f - 192.f - cueVisualDistance, cueBallPos.y - 16.f + 8.f, 192.f, 16.f};
        v2 cueStickOrigin = {192.f + GetCurrentPlayer()->ball->radius + cueVisualDistance, 8.f};
        glm::mat4 stickModelMatrix = rotate_model_matrix(level->cueRotation, dest, cueStickOrigin);
        sh_texture->UniformM4fv("model", stickModelMatrix);
        GL_DrawTexture({0,64,192,16},(iRect)dest);
        sh_texture->UniformM4fv("model", model);        
    }

    // preview line
    if (game_state->roundState == GameState::AIMING ||
        game_state->roundState == GameState::SHOOTING_MOTION)
    {
        RenderPreviewLine();        
    }

    UseShader(sh_texture);

    // GUI
    UseShader(sh_texture);
    sh_texture->UniformM4fv("view", glm::mat4(1.0));
    for (int i=0; i<game_state->playerCount; i++)
    {
        // Render stroke text
        sh_texture->Uniform1i("_texture", game_state->players[i].strokeText.gl_texture);
        GL_DrawTexture(game_state->players[i].strokeText.bound, game_state->players[i].strokeText.getDrawRect());
    }

    // Render ability icons for current player
    if (GetCurrentPlayer()->abilityCount != 0)
    {
        sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));

        if (game_state->roundState == GameState::POST_SHOT)
        {
            // Display button to prompt player to end turn without using ability
            local_persist bool generatedEndTurnButton = false;
            local_persist generic_drawable endTurnText;
            if (!generatedEndTurnButton)
            {
                endTurnText = GenerateTextObj(GetFont("res/m5x7.ttf"), "End Turn", COLOR_BLACK);
                endTurnText.scale = {4.f, 4.f};
                sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
            }
            iRect textBounds = endTurnText.getDrawRect();
            iRect buttonDest = {WINDOW_WIDTH/2 - 128, WINDOW_HEIGHT/2 + 200, 256, 80};
            iRect textDest = {buttonDest.x + buttonDest.w/2 - textBounds.w/2,
                buttonDest.y + buttonDest.h/2 - textBounds.h/2 - 8,
                textBounds.w,
                textBounds.h};

            iRect buttonSrc = {0, 272, 48, 32};
            v2i mPos = GetMousePosition();
            if (buttonDest.contains(mPos))
            {
                buttonSrc.x = 48;
                if (input->mouse_pressed)
                {
                    buttonSrc.x = 96;
                    textDest.y += 12;
                    if (input->mouse_just_pressed)
                    {
                        FinishTurn();
                    }
                }
            }

            sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
            sh_texture->UniformM4fv("model", glm::mat4(1.0));
            GL_DrawTexture(buttonSrc, buttonDest);
            sh_texture->Uniform1i("_texture", endTurnText.gl_texture);
            GL_DrawTexture(endTurnText.bound, textDest);
            sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
        
        } else if (game_state->roundState == GameState::USE_ABILITY)
        {
            if (game_state->ability == ABILITY::HEAVY_WIND)
            {
                auto *windAbility = &game_state->abilityState.windAbility;
                if (windAbility->activated == false)
                {
                    iRect compassSrc = {0, 320, 48, 48};
                    iRect needleSrc = {48, 320, 48, 48};

                    float scale = 4.f;
                    fRect compassDest;
                    compassDest.w = compassSrc.w * scale;
                    compassDest.h = compassSrc.h * scale;
                    compassDest.x = WINDOW_WIDTH/2 - compassDest.w/2;
                    compassDest.y = WINDOW_HEIGHT/2 - compassDest.h/2;
                
                    fRect needleDest = {
                        WINDOW_WIDTH/2 - (needleSrc.w*scale)/2.f,
                        WINDOW_HEIGHT/2 - (needleSrc.h*scale)/2.f,
                        needleSrc.w * scale,
                        needleSrc.h * scale};

                    float needleRotationSpeed = 2.5f * PIf;
                    float needleRadians;
                    if (windAbility->selectionTime != 0.f)
                    {
                        needleRadians = windAbility->needleRotation;
                        float timeSinceSelection = game_state->globalTimer - windAbility->selectionTime;
                        if (timeSinceSelection > 1.5f)
                        {
                            windAbility->activated = true;
                            OnAbilityUse(game_state->ability);
                            ConsumeAbility(GetCurrentPlayer());
                            game_state->ability = ABILITY::NONE;
                        }
                    } else
                    {
                        needleRadians = needleRotationSpeed * game_state->globalTimer;
                        if (input->just_pressed[SDL_SCANCODE_SPACE])
                        {
                            windAbility->needleRotation = needleRadians;
                            windAbility->selectionTime = game_state->globalTimer;
                        }
                    }

                    glm::mat4 needleRotationMatrix = rotate_model_matrix(
                        needleRadians, needleDest, {compassDest.w/2,compassDest.h/2});

                    GL_DrawTexture(compassSrc, compassDest);
                    sh_texture->UniformM4fv("model", needleRotationMatrix);
                    GL_DrawTexture(needleSrc, needleDest);
                    sh_texture->UniformM4fv("model", glm::mat4(1.0));
                }
                
            } else if (game_state->ability == ABILITY::SHANK)
            {
                Button left, right;
                left.bounds = {WINDOW_WIDTH/2 - 300 - 64, WINDOW_HEIGHT/2, 64, 128};
                left.inactive = {144,272,32,32};
                left.hover = {176,272,32,32};
                left.click = {208,272,32,32};
                right = left;
                right.bounds = {WINDOW_WIDTH/2 + 300, WINDOW_HEIGHT/2, 64, 128};
                i32 selectedBall = game_state->abilityState.craterAbility.selectedBall;

                if (DoButton(&left))
                {
                    do
                    {
                        selectedBall--;
                        if (selectedBall < 0)
                        {
                            selectedBall = level->ballCount-1;
                        }
                    } while (level->balls[selectedBall].active == false);
                }
                if (DoButton(&right))
                {
                    do
                    {
                        selectedBall++;
                        if (selectedBall > level->ballCount-1)
                        {
                            selectedBall = 0;
                        }
                    } while (level->balls[selectedBall].active == false);
                }
                
                game_state->abilityState.craterAbility.selectedBall = selectedBall;

                local_persist bool generatedActivateButton = false;
                local_persist generic_drawable activateText;
                if (!generatedActivateButton)
                {
                    generatedActivateButton = true;
                    activateText = GenerateTextObj(GetFont("res/m5x7.ttf"), "Activate", COLOR_BLACK);
                    activateText.scale = {4.f, 4.f};
                    sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
                }

                Button activate;
                activate.bounds = {WINDOW_WIDTH/2 - 128, WINDOW_HEIGHT/2 + 200, 256, 80};
                activate.inactive = {0, 272, 48, 32};
                activate.hover = {48, 272, 48, 32};
                activate.click = {96, 272, 48, 32};
                if (DoButton(&activate))
                {
                    OnAbilityUse(game_state->ability);
                    ConsumeAbility(GetCurrentPlayer());
                    RestoreAfterAbilityUse();
                }

                iRect textBounds = activateText.getDrawRect();
                iRect textDest = {activate.bounds.x + activate.bounds.w/2 - textBounds.w/2,
                    activate.bounds.y + activate.bounds.h/2 - textBounds.h/2 - 8,
                    textBounds.w,
                    textBounds.h};
                if (activate.clicking)
                {
                    textDest.y += 12;
                }

                sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
                model = rotate_model_matrix(PIf, left.bounds, {left.bounds.w/2.f, left.bounds.h/2.f});
                sh_texture->UniformM4fv("model", model);
                GL_DrawTexture(left.src, left.bounds);
                sh_texture->UniformM4fv("model", glm::mat4(1.0));
                GL_DrawTexture(right.src, right.bounds);

                GL_DrawTexture(activate.src, activate.bounds);
                sh_texture->Uniform1i("_texture", activateText.gl_texture);
                GL_DrawTexture(activateText.bound, textDest);
                
            } else if (game_state->ability == ABILITY::CRATER)
            {
                Button left, right;
                left.bounds = {WINDOW_WIDTH/2 - 300 - 64, WINDOW_HEIGHT/2, 64, 128};
                left.inactive = {144,272,32,32};
                left.hover = {176,272,32,32};
                left.click = {208,272,32,32};
                right = left;
                right.bounds = {WINDOW_WIDTH/2 + 300, WINDOW_HEIGHT/2, 64, 128};
                i32 selectedBall = game_state->abilityState.craterAbility.selectedBall;

                if (DoButton(&left))
                {
                    do
                    {
                        selectedBall--;
                        if (selectedBall < 0)
                        {
                            selectedBall = level->ballCount-1;
                        }
                    } while (level->balls[selectedBall].active == false);
                }
                if (DoButton(&right))
                {
                    do
                    {
                        selectedBall++;
                        if (selectedBall > level->ballCount-1)
                        {
                            selectedBall = 0;
                        }
                    } while (level->balls[selectedBall].active == false);
                }
                
                game_state->abilityState.craterAbility.selectedBall = selectedBall;

                local_persist bool generatedActivateButton = false;
                local_persist generic_drawable activateText;
                if (!generatedActivateButton)
                {
                    generatedActivateButton = true;
                    activateText = GenerateTextObj(GetFont("res/m5x7.ttf"), "Activate", COLOR_BLACK);
                    activateText.scale = {4.f, 4.f};
                    sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
                }

                Button activate;
                activate.bounds = {WINDOW_WIDTH/2 - 128, WINDOW_HEIGHT/2 + 200, 256, 80};
                activate.inactive = {0, 272, 48, 32};
                activate.hover = {48, 272, 48, 32};
                activate.click = {96, 272, 48, 32};
                if (DoButton(&activate))
                {
                    OnAbilityUse(game_state->ability);
                    ConsumeAbility(GetCurrentPlayer());
                    RestoreAfterAbilityUse();
                }

                iRect textBounds = activateText.getDrawRect();
                iRect textDest = {activate.bounds.x + activate.bounds.w/2 - textBounds.w/2,
                    activate.bounds.y + activate.bounds.h/2 - textBounds.h/2 - 8,
                    textBounds.w,
                    textBounds.h};
                if (activate.clicking)
                {
                    textDest.y += 12;
                }


                sh_texture->Uniform1i("_texture", GetTexture("res/sprites.png"));
                model = rotate_model_matrix(PIf, left.bounds, {left.bounds.w/2.f, left.bounds.h/2.f});
                sh_texture->UniformM4fv("model", model);
                GL_DrawTexture(left.src, left.bounds);
                sh_texture->UniformM4fv("model", glm::mat4(1.0));
                GL_DrawTexture(right.src, right.bounds);

                GL_DrawTexture(activate.src, activate.bounds);
                sh_texture->Uniform1i("_texture", activateText.gl_texture);
                GL_DrawTexture(activateText.bound, textDest);                

            }
        }

        // The reason the code for selecting abilities is after
        // the ability code itself is so that there is a guaranteed
        // one frame buffer, i.e. so the click that selects
        // the ability doesn't also trigger the ability itself
        for (int i=0; i<GetCurrentPlayer()->abilityCount; i++)
        {
            i32 ability = GetCurrentPlayer()->abilities[i];
            iRect src = {0, 160, 48, 48};
            if (ability == ABILITY::CRATER)
            {
                src.x = 0;
            } else if (ability == ABILITY::PLACE_OBSTACLE)
            {
                src.x = 48;
            } else if (ability == ABILITY::RETRY)
            {
                src.x = 144;
            } else if (ability == ABILITY::HEAVY_WIND)
            {
                src.x = 192;
            } else if (ability == ABILITY::SHANK)
            {
                src.x = 0;
                src.y = 208;
            } else if (ability == ABILITY::PLACE_BOUNCER)
            {
                src.x = 48;
                src.y = 208;
            } else if (ability == ABILITY::PROTECTION)
            {
                src.x = 96;
                src.y = 208;
            }
            i32 size = 96;
            iRect dest = {WINDOW_WIDTH - 24 - (size + 24) *(i+1), 32, size, size};
            
            v2i mPos = GetMousePosition();
            if (dest.contains(mPos))
            {
                if (input->mouse_just_pressed &&
                    (game_state->roundState == GameState::AIMING ||
                    game_state->roundState == GameState::POST_SHOT))
                {
                    SetRoundState(GameState::USE_ABILITY);
                    game_state->ability = ability;
                    GetCurrentPlayer()->selectedAbility = i;
                    OnAbilitySelected(game_state->ability);
                }
                sh_texture->Uniform4f("colorAdd", 0.2f, 0.2f, 0.2f, 1.0f);
                GL_DrawTexture(src, dest);
                sh_texture->Uniform4f("colorAdd", 0.f, 0.f, 0.f, 0.f);
            } else
            {
                GL_DrawTexture(src, dest);
            }            
        }
    }

    local_persist bool generatedDebugText=false;
    local_persist generic_drawable debugText;
    if (!generatedDebugText)
    {
        std::string stateText = "State: ";
        if (game_state->roundState == GameState::AIMING)
        {
            stateText += "Aiming";
        } else if (game_state->roundState == GameState::SHOOTING_MOTION)
        {
            stateText += "Shooting";
        } else if (game_state->roundState == GameState::BALL_MOVING)
        {
            stateText += "Balls moving";
        } else if (game_state->roundState == GameState::POST_SHOT)
        {
            stateText += "Post shot";
        } else if (game_state->roundState == GameState::USE_ABILITY)
        {
            stateText += "Ability";
        }
        debugText = GenerateTextObj(GetFont("res/m5x7.ttf"), stateText, COLOR_WHITE);
        debugText.scale = {4.f, 4.f};
        debugText.pos = {16, WINDOW_HEIGHT - 16 - debugText.getDrawRect().h};
    }

    sh_texture->Uniform1i("_texture",debugText.gl_texture);
    GL_DrawTexture(debugText.bound, debugText.getDrawRect());
    sh_texture->Uniform1i("_texture",GetTexture("res/sprites.png"));
    
    SDL_GL_SwapWindow(window);
}

#if defined __cplusplus
extern "C"
#endif
GAME_HANDLE_EVENT(GameHandleEvent) {
    if (e.sdl_event.type == SDL_QUIT) {
        *running = false;
        e.handled = true;
        return;
    }
    if (game_state == nullptr) {
        return;
    }
}

#ifdef GAMEDISABLED
#include <SDL.h>
#if defined __cplusplus
extern "C"
#endif
extern GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
}

#if defined __cplusplus
extern "C"
#endif
extern GAME_HANDLE_EVENT(GameHandleEvent) {
}
#endif
