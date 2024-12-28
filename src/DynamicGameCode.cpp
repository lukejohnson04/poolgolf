
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

#include "shapes.cpp"
#include "ball.cpp"

struct RenderState {
    Shader sh_texture;
    Shader sh_color;
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
        BALL_MOVING
    } roundState = AIMING;
    v2 ballStartPosition = {0.f,0.f};

    float shootingMotionTimer=0.f;

    float cueRotation = 0.f;
    float cuePower = 1.0f;
    float cueRotSpeed = 0.5f;
    int tiles[64][64];

    i32 strokeCount = 0;

    v2 holePos;

    TTF_Font *m5x7 = nullptr;

    generic_drawable strokeText;
};

global_variable GameState *game_state=nullptr;

const glm::mat4 projection = glm::ortho(0.0f,
        static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);

void LoadRenderState(RenderState *st) {
    st->sh_texture = CreateShader("texture.vert","texture.frag");
    st->sh_color = CreateShader("color.vert","color.frag");
    
    glm::mat4 model = glm::mat4(1.0f);
    
    UseShader(&game_state->rst.sh_color);
    st->sh_color.UniformM4fv("projection",projection);
    st->sh_color.UniformM4fv("model",model);
    UseShader(&game_state->rst.sh_texture);
    st->sh_texture.UniformM4fv("projection",projection);
    st->sh_texture.UniformColor("colorMod",{255,255,255,255});
    st->sh_texture.UniformM4fv("model",model);
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


void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    LoadRenderState(&game_state->rst);

    srand((u32)time(0));

    game_state->m5x7 = TTF_OpenFont("res/m5x7.ttf",16);

    game_state->cueBall = &game_state->balls[0];

    game_state->ballCount++;
    game_state->cueBall->pos = {300.f,100.f};
    game_state->cueBall->radius = 16.f;
    game_state->cueBall->mass = PIf * game_state->cueBall->radius * game_state->cueBall->radius;
    game_state->cueBall->vel = {0.f,0.f};
    game_state->cueBall->active = true;
    
    game_state->ballCount++;
    game_state->balls[1].pos = {500.f, 100.f};
    game_state->balls[1].radius = 16.f;
    game_state->balls[1].mass = PIf * game_state->balls[1].radius * game_state->balls[1].radius;
    game_state->balls[1].vel = {0.f,0.f};
    game_state->balls[1].active = true;

    // load map
    SDL_Surface *mp_surf = IMG_Load("res/levels.png");
    
    for (int i=0; i<20; i++)
    {
        for (int j=0; j<12; j++)
        {
            Color pixel = getPixel(mp_surf, i, j);
            game_state->tiles[i][j] = 0;
            if (pixel == COLOR_BLACK)
            {
                game_state->tiles[i][j] = 1;
            } else if (pixel == COLOR_YELLOW)
            {
                game_state->holePos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_WHITE)
            {
                game_state->cueBall->pos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_RED)
            {
                game_state->balls[1].pos = {i*64.f, j*64.f};
            }
        }
    }
    SDL_FreeSurface(mp_surf);

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
    ControlCueStick(delta);

    // Update balls
    UpdateLevel(delta);
    
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

    UseShader(&game_state->rst.sh_texture);
    glm::mat4 model = glm::mat4(1.0);
    game_state->rst.sh_texture.UniformM4fv("model",model);
    game_state->rst.sh_texture.Uniform1i("_texture",GetTexture("res/sprites.png"));
    
    model = glm::mat4(1.0);
    game_state->rst.sh_texture.UniformM4fv("model",model);

    // background tiles
    for (int i=0; i<20; i++)
    {
        for (int j=0; j<12; j++)
        {
            v2 pos = GetTileWorldPos(i,j);
            iRect src = {game_state->tiles[i][j] != 0 ? 32 : 0,32,32,32};
            GL_DrawTexture(src,{i*64,j*64,64,64});
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

    // Render stroke text
    UseShader(&game_state->rst.sh_texture);
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
