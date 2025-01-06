
#include "core/engine.cpp"

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

inline
v2 GetTileWorldPos(int posx, int posy) {
    return v2(posx * 64.f, posy * 64.f);
}

enum TILE_TYPE
{
    TT_NONE,
    WATER,
    WALL,
    GRASS,
    DOWNHILL_RIGHT,
    DOWNHILL_DOWN_RIGHT,
    DOWNHILL_DOWN,
    DOWNHILL_DOWN_LEFT,
    DOWNHILL_LEFT,
    DOWNHILL_UP_LEFT,
    DOWNHILL_UP,
    DOWNHILL_UP_RIGHT,
    HOLE,
};

bool IsTileDownhill(i32 tile)
{
    return tile >= DOWNHILL_RIGHT && tile < DOWNHILL_RIGHT+8;
}

const int MAP_SIZE = 256;

int craterPattern[7][7];

#include "shapes.cpp"
#include "ball.cpp"
#include "camera.cpp"

struct RenderState {
    Shader sh_texture;
    Shader sh_color;
};

enum ABILITY
{
    NONE,
    MOVE_HOLE,
    PLACE_OBSTACLE,
    CRATER
};

struct GameState {
    game_assets assets;
    RenderState rst;

    Ball balls[16];
    Ball *cueBall;
    i32 ballCount = 0;

    enum : int {
        AIMING,
        SHOOTING_MOTION,
        BALL_MOVING,
        USE_ABILITY
    } roundState = AIMING;

    i32 ability = ABILITY::NONE;
    union {
        union
        {
            i32 rotation = 0;
        } obstacleAbility;

        union
        {
            
        } craterAbility;
    } abilityState;
    
    v2 ballStartPosition = {0.f,0.f};

    float shootingMotionTimer=0.f;

    float cueRotation = 0.f;
    float cuePower = 1.0f;
    float cueRotSpeed = 0.5f;
    int tiles[MAP_SIZE][MAP_SIZE];

    v2 itemDrops[64];
    i32 itemDropCount = 0;

    i32 strokeCount = 0;

    v2 holePos;

    TTF_Font *m5x7 = nullptr;
    Camera camera;
    bool autocam = true;

    generic_drawable strokeText;

    float globalTimer=0.f;
};

global_variable GameState *game_state=nullptr;

const glm::mat4 projection = glm::ortho(0.0f,
        static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);

void LoadRenderState(RenderState *st) {
    st->sh_texture = CreateShader("texture.vert","texture.frag");
    st->sh_color = CreateShader("color.vert","color.frag");
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = CalculateCameraViewMatrix(&game_state->camera);
    
    UseShader(&game_state->rst.sh_color);
    st->sh_color.UniformM4fv("projection",projection);
    st->sh_color.UniformM4fv("model",model);
    st->sh_color.UniformM4fv("view",view);
    UseShader(&game_state->rst.sh_texture);
    st->sh_texture.UniformM4fv("projection",projection);
    st->sh_texture.UniformColor("colorMod",{255,255,255,255});
    st->sh_texture.UniformM4fv("model",model);
    st->sh_texture.UniformM4fv("view",view);
}

void ChangeStrokes(i32 count)
{
    game_state->strokeCount = count;
    game_state->strokeText = GenerateTextObj(
        game_state->m5x7, std::to_string(game_state->strokeCount),
        COLOR_WHITE, game_state->strokeText.gl_texture);
    game_state->strokeText.position = {28,20};
    game_state->strokeText.scale = {2.f, 2.f};
}

#include "cuestick.cpp"
#include "levelupdate.cpp"
#include "renderpreviewline.cpp"

TILE_TYPE GetDownhillTileFromPixel(Color pixel)
{
    if (pixel == Color(0,100,0,255))
    {
        return TILE_TYPE::DOWNHILL_RIGHT;
    } else if (pixel == 0x250064ff)
    {
        return TILE_TYPE::DOWNHILL_LEFT;
    } else if (pixel == 0x640062ff)
    {
        return TILE_TYPE::DOWNHILL_UP;
    } else if (pixel == 0x646200ff)
    {
        return TILE_TYPE::DOWNHILL_DOWN;
    } else if (pixel == 0x641b00ff)
    {
        return TILE_TYPE::DOWNHILL_DOWN_RIGHT;
    } else if (pixel == 0xa5b50eff)
    {
        return TILE_TYPE::DOWNHILL_DOWN_LEFT;
    } else if (pixel == 0x8145beff)
    {
        return TILE_TYPE::DOWNHILL_UP_LEFT;
    } else if (pixel == 0x008f45ff)
    {
        return TILE_TYPE::DOWNHILL_UP_RIGHT;
    }
    return TILE_TYPE::GRASS;
}

void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    LoadRenderState(&game_state->rst);

    srand((u32)time(0));

    game_state->m5x7 = TTF_OpenFont("res/m5x7.ttf",16);

    game_state->cueBall = &game_state->balls[0];

    game_state->ballCount++;
    game_state->cueBall->pos = {300.f,100.f};
    game_state->cueBall->radius = 8.f;
    game_state->cueBall->mass = PIf * game_state->cueBall->radius * game_state->cueBall->radius;
    game_state->cueBall->vel = {0.f,0.f};
    game_state->cueBall->active = true;

    // load map
    SDL_Surface *mp_surf = IMG_Load("res/levels.png");
    
    for (int i=0; i<MAP_SIZE; i++)
    {
        for (int j=0; j<MAP_SIZE; j++)
        {
            Color pixel = getPixel(mp_surf, i, j);
            game_state->tiles[i][j] = TILE_TYPE::GRASS;
            if (pixel == COLOR_BLACK)
            {
                game_state->tiles[i][j] = TILE_TYPE::WALL;
            } else if (pixel == COLOR_YELLOW)
            {
                game_state->holePos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_WHITE)
            {
                game_state->cueBall->pos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_BLUE)
            {
                game_state->tiles[i][j] = TILE_TYPE::WATER;
            } else if (pixel == COLOR_RED)
            {
                // Item
                game_state->itemDrops[game_state->itemDropCount++] = {i*64.f + 32.f, j*64.f + 32.f};
            } else if (pixel == COLOR_GREEN)
            {
                game_state->tiles[i][j] = TILE_TYPE::GRASS;
            } else
            {
                game_state->tiles[i][j] = GetDownhillTileFromPixel(pixel);
            }
        }
    }
    SDL_FreeSurface(mp_surf);

    SDL_Surface *craterSurf = IMG_Load("res/craterPattern.png");
    for (int i=0; i<7; i++)
    {
        for (int j=0; j<7; j++)
        {
            Color pixel = getPixel(craterSurf, i, j);
            if (pixel == COLOR_TRANSPARENT)
            {
                craterPattern[i][j] = TILE_TYPE::TT_NONE;
            } else
            {                
                craterPattern[i][j] = GetDownhillTileFromPixel(pixel);
            }
        }
    }
    SDL_FreeSurface(craterSurf);

    ChangeStrokes(0);
    
    memory->is_initialized = true;
    std::cout << "Initialized game memory\n";
}

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
    UpdateLevel(delta);

    if (input->just_pressed[SDL_SCANCODE_GRAVE])
    {
        game_state->autocam = !game_state->autocam;
    }

    if (game_state->roundState != GameState::AIMING)
    {
        game_state->autocam = true;
    }

    if (game_state->autocam)
    {
        game_state->camera.pos = Lerp(game_state->camera.pos, game_state->cueBall->pos - v2(WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f), 0.05f);
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

    // Abilities
    if (game_state->roundState == GameState::AIMING)
    {
        if (input->just_pressed[SDL_SCANCODE_5])
        {
            game_state->ability = ABILITY::PLACE_OBSTACLE;
            game_state->roundState = GameState::USE_ABILITY;

        } else if (input->just_pressed[SDL_SCANCODE_6])
        {
            // Place a crater around the cue ball
            v2i ballTile = (v2i)(game_state->cueBall->pos * (1.f/64.f));
            for (int i=0; i<7; i++)
            {
                for (int j=0; j<7; j++)
                {
                    i32 craterValue = craterPattern[i][j];
                    v2i tilePos = {ballTile.x - 3 + i, ballTile.y - 3 + j};
                    i32 tileType = game_state->tiles[tilePos.x][tilePos.y];
                    if (tileType != TILE_TYPE::GRASS &&
                        IsTileDownhill(tileType) == false)
                    {
                        continue;
                    }
                    
                    if (craterValue != TILE_TYPE::TT_NONE)
                    {
                        game_state->tiles[tilePos.x][tilePos.y] = craterValue;
                    }
                }
            }
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
    
    glClearColor(0.2f, 0.2f, 0.21f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 cameraView = CalculateCameraViewMatrix(&game_state->camera);

    UseShader(&game_state->rst.sh_texture);
    game_state->rst.sh_texture.UniformM4fv("model",model);
    game_state->rst.sh_texture.UniformM4fv("view",cameraView);
    game_state->rst.sh_texture.Uniform1i("_texture",GetTexture("res/sprites.png"));

    UseShader(&game_state->rst.sh_color);
    game_state->rst.sh_color.UniformM4fv("model",model);
    game_state->rst.sh_color.UniformM4fv("view",cameraView);

    UseShader(&game_state->rst.sh_texture);
    // background tiles
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
            i32 tile = game_state->tiles[i][j];
            iRect src = {0,32,32,32};
            iRect dest = {i*64,j*64,64,64};
            if (tile == TILE_TYPE::GRASS)
            {
                src.x = 0;
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
                game_state->rst.sh_texture.UniformM4fv("model", rotMatrix);
                GL_DrawTexture({128,32,32,32}, dest);
                game_state->rst.sh_texture.UniformM4fv("model", glm::mat4(1.0));
            }
        }
    }

    
    // Draw hole
    fRect holeDest = {game_state->holePos.x - HOLE_SIZE/2,
        game_state->holePos.y - HOLE_SIZE/2,
        HOLE_SIZE, HOLE_SIZE};
    GL_DrawTexture({0,80,64,64}, holeDest);

    // Balls
    for (int i=0; i<game_state->ballCount; i++)
    {
        if (game_state->balls[i].active == false)
        {
            continue;
        }
        
        iRect src = {0,0,32,32};
        if (i != 0)
        {
            src.x = 32;
        }
        
        GL_DrawTexture(src,{(i32)game_state->balls[i].left(),(i32)game_state->balls[i].top(),(i32)(game_state->balls[i].radius*2.f),(i32)(game_state->balls[i].radius*2.f)});
    }

    // Draw item drops
    for (int i=0; i<game_state->itemDropCount; i++)
    {
        v2 pos = game_state->itemDrops[i];
        iRect dest = iRect((i32)(pos.x - 32.f), (i32)(pos.y - 32.f), 64, 64);
        dest.y += (i32)(sin(game_state->globalTimer * 3.0) * 6.0);
        GL_DrawTexture({160,32,32,32}, dest);
    }

    // Cue stick
    float cueVisualDistance = game_state->cuePower + 16.f;
    v2 cueBallPos = game_state->cueBall->pos;
    if (game_state->roundState == GameState::SHOOTING_MOTION)
    {
        float maxCueDistance = cueVisualDistance + 0.75f * cueVisualDistance;
        if (game_state->shootingMotionTimer < 0.75f)
        {
            cueVisualDistance += game_state->shootingMotionTimer * cueVisualDistance;
        } else
        {
            float t = (game_state->shootingMotionTimer - 0.75f) / 0.25f;
            cueVisualDistance = Lerp(maxCueDistance, 0.f, t);
        }
    } else if (game_state->roundState == GameState::BALL_MOVING)
    {
        cueVisualDistance = 0.f;
        cueBallPos = game_state->ballStartPosition;
    }

    // Draw cue stick
    fRect dest = {cueBallPos.x - 16.f - 192.f - cueVisualDistance, cueBallPos.y - 16.f + 8.f, 192.f, 16.f};
    v2 cueStickOrigin = {192.f + game_state->cueBall->radius + cueVisualDistance, 8.f};
    glm::mat4 stickModelMatrix = rotate_model_matrix(game_state->cueRotation, dest, cueStickOrigin);
    game_state->rst.sh_texture.UniformM4fv("model", stickModelMatrix);
    GL_DrawTexture({0,64,192,16},(iRect)dest);
    game_state->rst.sh_texture.UniformM4fv("model", model);

    // preview line
    if (game_state->roundState == GameState::AIMING ||
        game_state->roundState == GameState::SHOOTING_MOTION)
    {
        RenderPreviewLine();        
    }

    UseShader(&game_state->rst.sh_texture);

    // Ability rendering
    if (game_state->roundState == GameState::USE_ABILITY)
    {
        if (game_state->ability == ABILITY::PLACE_OBSTACLE)
        {
            int rot = game_state->abilityState.obstacleAbility.rotation;
            if (input->just_pressed[SDL_SCANCODE_LEFT])
            {
                rot -= 1;
            }
            if (input->just_pressed[SDL_SCANCODE_RIGHT])
            {
                rot += 1;
            }

            if (rot < 0)
            {
                rot = 3;
            }
            if (rot >= 4)
            {
                rot = 0;
            }

            game_state->abilityState.obstacleAbility.rotation = rot;

            v2i placement = GetMouseWorldPos(&game_state->camera);
            placement.x /= 64;
            placement.y /= 64;
            v2i secondPlacement = placement;
            if (rot == 0)
            {
                secondPlacement.x++;
            } else if (rot == 1)
            {
                secondPlacement.y++;
            } else if (rot == 2)
            {
                secondPlacement.x--;
            } else if (rot == 3)
            {
                secondPlacement.y--;
            }

            game_state->rst.sh_texture.Uniform4f("colorMod",0.f,0.f,0.f,0.5f);
            GL_DrawTexture({32,32,32,32}, {placement.x * 64, placement.y * 64, 64, 64});
            GL_DrawTexture({32,32,32,32}, {secondPlacement.x * 64, secondPlacement.y * 64, 64, 64});
            game_state->rst.sh_texture.Uniform4f("colorMod",1.f,1.f,1.f,1.f);
            
            if (input->mouse_just_pressed)
            {
                
                game_state->tiles[placement.x][placement.y] = TILE_TYPE::WALL;
                game_state->tiles[secondPlacement.x][secondPlacement.y] = TILE_TYPE::WALL;

                game_state->ability = ABILITY::NONE;
                game_state->roundState = GameState::AIMING;
            }
        }
    }


    // Render stroke text
    UseShader(&game_state->rst.sh_texture);
    game_state->rst.sh_texture.UniformM4fv("view", glm::mat4(1.0));
    game_state->rst.sh_texture.Uniform1i("_texture", game_state->strokeText.gl_texture);
    GL_DrawTexture(game_state->strokeText.bound, game_state->strokeText.getDrawRect());

    
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
