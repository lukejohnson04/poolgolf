
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
#include "camera.cpp"

struct RenderState {
    Shader sh_texture;
    Shader sh_color;
    Shader sh_light;
};

const int MAP_SIZE = 64;

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
    int tiles[MAP_SIZE][MAP_SIZE];

    i32 strokeCount = 0;

    v2 holePos;

    TTF_Font *m5x7 = nullptr;
    Camera3D camera;

    generic_drawable strokeText;
};

global_variable GameState *game_state=nullptr;

const glm::mat4 projection = glm::ortho(0.0f,
                                        (float)WINDOW_WIDTH,
                                        (float)WINDOW_HEIGHT,
                                        0.0f,
                                        -1.0f, 1.0f);

void LoadRenderState(RenderState *st) {
    st->sh_texture = CreateShader("texture.vert","texture.frag");
    st->sh_color = CreateShader("color.vert","color.frag");
    st->sh_light = CreateShader("light.vert","light.frag");
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view = CalculateCameraViewMatrix(&game_state->camera);
    
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


void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    glEnable(GL_DEPTH_TEST);

    LoadRenderState(&game_state->rst);
    
    srand((u32)time(0));

    game_state->m5x7 = TTF_OpenFont("res/m5x7.ttf",16);

    game_state->cueBall = &game_state->balls[0];

    game_state->ballCount++;
    game_state->cueBall->radius = 16.f;
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
            game_state->tiles[i][j] = 1;
            if (pixel == COLOR_BLACK)
            {
                game_state->tiles[i][j] = 0;
            } else if (pixel == COLOR_YELLOW)
            {
                game_state->holePos = {i, j};
            } else if (pixel == COLOR_RED)
            {
                game_state->cueBall->pos = {i, j};
            } else
            {
                game_state->tiles[i][j] = 1;
            }
        }
    }
    SDL_FreeSurface(mp_surf);

    ChangeStrokes(0);

    game_state->camera.pos = glm::vec3(0.0f, 1.5f, 3.0f);
    game_state->camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

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
        SDL_SetWindowGrab(window, SDL_TRUE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        InitializeGameMemory(game_memory);
    }

    // Render test 3D
    static float res_timer=0.f;
    res_timer += delta;
    if (res_timer >= 0.1f) {
        res_timer = 0.f;
        CheckForResourceUpdates(&game_state->assets);
    }
    
    glClearColor(0.3f, 0.8f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    UseShader(&game_state->rst.sh_texture);
    game_state->rst.sh_texture.Uniform1i("_texture", GetTexture("res/sprites.png"));

    glm::mat4 proj = glm::perspective(glm::radians(80.0f), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 100.0f);
    game_state->rst.sh_texture.UniformM4fv("projection", proj);    
    glm::mat4 model = glm::mat4(1.0f);
    float rot = (float)SDL_GetTicks() / 50.f;
    model = glm::rotate(model, glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
    game_state->rst.sh_texture.UniformM4fv("model", model);

    glm::mat4 view = glm::mat4(1.0f);

    // mouse
    float mouseSensitivity = 1.0f;
    float xOffset = input->mouseXMotion * mouseSensitivity;
    float yOffset = input->mouseYMotion * mouseSensitivity;
    
    game_state->camera.yaw += xOffset;
    game_state->camera.pitch -= yOffset;

    if (game_state->camera.pitch > 89.0f)
    {
        game_state->camera.pitch = 89.0f;
    }
    if (game_state->camera.pitch < -89.0f)
    {
        game_state->camera.pitch = -89.0f;
    }
    glm::vec3 direction;
    direction.x = cos(glm::radians(game_state->camera.yaw)) * cos(glm::radians(game_state->camera.pitch));
    direction.y = sin(glm::radians(game_state->camera.pitch));
    direction.z = sin(glm::radians(game_state->camera.yaw)) * cos(glm::radians(game_state->camera.pitch));
    glm::vec3 cameraFront = glm::vec3(glm::normalize(direction));

    float cameraSpeed = 0.05f;
    if (input->is_pressed[SDL_SCANCODE_W])
    {
        game_state->camera.pos += cameraSpeed * cameraFront;
    }
    if (input->is_pressed[SDL_SCANCODE_S])
    {
        game_state->camera.pos -= cameraSpeed * cameraFront;
        
    }
    if (input->is_pressed[SDL_SCANCODE_A])
    {
        game_state->camera.pos -= glm::normalize(glm::cross(cameraFront, game_state->camera.up)) * cameraSpeed;
    }
    if (input->is_pressed[SDL_SCANCODE_D])
    {
        game_state->camera.pos += glm::normalize(glm::cross(cameraFront, game_state->camera.up)) * cameraSpeed;
    }

    
    view = glm::lookAt(game_state->camera.pos, game_state->camera.pos + cameraFront, game_state->camera.up);

    game_state->rst.sh_texture.UniformM4fv("view", view);
    
    local_persist GLuint VAO, VBO, lightVAO, lightVBO;
    local_persist bool generate=false;
    if (generate == false)
    {
        generate = true;
        glGenVertexArrays(1, &VAO);
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &lightVBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    for(int x = 0; x < MAP_SIZE; x++)
    {
        for (int y = 0; y < MAP_SIZE; y++)
        {
            glm::vec3 position = glm::vec3((float)x, 0.0f, (float)y);
            
            v2 uv_offset, uv_scale;
            iRect source = {0,32,32,32};
            if (game_state->tiles[x][y] == 0)
            {
                source = {32,32,32,32};
                position.y += 1.0f;
            }

            GetUvCoordinates(source, &uv_offset, &uv_scale);

            game_state->rst.sh_texture.Uniform2f("u_uvOffset",uv_offset);
            game_state->rst.sh_texture.Uniform2f("u_uvScale",uv_scale);
            model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            game_state->rst.sh_texture.UniformM4fv("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
            
        }
    }
    
    // Draw ball (as a cube lol)
    glm::vec3 position = glm::vec3(game_state->cueBall->pos.x, 1.f, game_state->cueBall->pos.y);
    glm::vec3 lightPosition = glm::vec3(position.x+1.5f,position.y+1.5f,position.z+2.f);

    v2 uv_offset, uv_scale;
    GetUvCoordinates({0,0,32,32},&uv_offset,&uv_scale);
    game_state->rst.sh_texture.Uniform2f("u_uvOffset",uv_offset);
    game_state->rst.sh_texture.Uniform2f("u_uvScale",uv_scale);
    model = glm::mat4(1.0f);
    
    model = glm::translate(model, position);
    float ballScale = (game_state->cueBall->radius*2.f) / 64.f;
    model = glm::scale(model, glm::vec3(ballScale, ballScale, ballScale));
    game_state->rst.sh_texture.UniformM4fv("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw a solid colored cube
    UseShader(&game_state->rst.sh_color);
    game_state->rst.sh_color.UniformM4fv("view",view);
    game_state->rst.sh_color.UniformM4fv("projection",proj);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x,position.y+1.f,position.z));
    game_state->rst.sh_color.UniformM4fv("model", model);
    game_state->rst.sh_color.Uniform3f("objectColor", v3(1.0f,0.5f,0.31f));
    game_state->rst.sh_color.Uniform3f("lightColor", v3(1.0f,1.0f,1.0f));
    game_state->rst.sh_color.Uniform3f("lightPos", lightPosition);
    game_state->rst.sh_color.Uniform3f("viewPos", game_state->camera.pos);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES_NORMALS), CUBE_VERTICES_NORMALS, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw light source
    UseShader(&game_state->rst.sh_light);
    game_state->rst.sh_light.UniformM4fv("view",view);
    game_state->rst.sh_light.UniformM4fv("projection",proj);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
    game_state->rst.sh_light.UniformM4fv("model",model);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    SDL_GL_SwapWindow(window);
}
/*

    
    // <----------------------->
    //       UPDATE BEGIN
    // <----------------------->
    ControlCueStick(delta);

    // Update balls
    UpdateLevel(delta);

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
    for (int i=0; i<MAP_SIZE; i++)
    {
        for (int j=0; j<MAP_SIZE; j++)
        {
            v2 pos = GetTileWorldPos(i,j);
            iRect src = {32,32,32,32};
            if (game_state->tiles[i][j] != 0)
            {
                src.x = 0;
            }
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

    // Let's draw a cube
    UseShader(&game_state->rst.sh_texture);
    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 100.0f);
    game_state->rst.sh_texture.UniformM4fv("projection",proj2);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.f,0.f,-3.0f));
    game_state->rst.sh_texture.UniformM4fv("view",view);
    
    GL_DrawTexture({64,32,32,32},{256,256,64,64});
    game_state->rst.sh_texture.UniformM4fv("projection",projection);

    // Render stroke text
    UseShader(&game_state->rst.sh_texture);
    game_state->rst.sh_texture.UniformM4fv("view", glm::mat4(1.0));
    game_state->rst.sh_texture.Uniform1i("_texture", game_state->strokeText.gl_texture);
    GL_DrawTexture(game_state->strokeText.bound, game_state->strokeText.getDrawRect());

    
    SDL_GL_SwapWindow(window);
}
*/
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
