
void LoadRenderState() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = CalculateCameraViewMatrix(&game_state->camera);

    Shader *sh_texture = GetShader("texture");
    Shader *sh_color = GetShader("color");
    UseShader(sh_color);
    sh_color->UniformM4fv("projection",projection);
    sh_color->UniformM4fv("model",model);
    sh_color->UniformM4fv("view",view);
    UseShader(sh_texture);
    sh_texture->UniformM4fv("projection",projection);
    sh_texture->UniformColor("colorMod",{255,255,255,255});
    sh_texture->UniformM4fv("model",model);
    sh_texture->UniformM4fv("view",view);
}

internal
PlayerData *GetCurrentPlayer()
{
    return &game_state->players[game_state->currentPlayer];
}

internal
void InitializeGameMemory(GameMemory *memory)
{
    *game_state = {};

    LoadRenderState();

    srand((u32)time(0));

    i32 players = 2;
    for (int i=0; i<players; i++)
    {
        game_state->playerCount++;
        game_state->ballCount++;
        BallInit(&game_state->balls[i]);
        game_state->players[i].ball = &game_state->balls[i];
        game_state->players[i].id = i;
        ChangeStrokes(&game_state->players[i], 0);
    }
    
    // load map
    SDL_Surface *mp_surf = IMG_Load("res/levels.png");
    
    for (int i=0; i<MAP_SIZE; i++)
    {
        for (int j=0; j<MAP_SIZE; j++)
        {
            Color pixel = getPixel(mp_surf, i, j);
            game_state->tiles[i][j] = TILE_TYPE::GRASS;
            game_state->validSpawn[i][j] = false;
            if (pixel == COLOR_BLACK)
            {
                game_state->tiles[i][j] = TILE_TYPE::WALL;
            } else if (pixel == COLOR_YELLOW)
            {
                game_state->holePos = {i*64.f, j*64.f};
            } else if (pixel == COLOR_WHITE)
            {
                game_state->ballSpawnPosition = {i*64.f, j*64.f};
                for (i32 n=0; n<game_state->playerCount; n++)
                {
                    game_state->players[n].ball->pos = game_state->ballSpawnPosition;
                }
                game_state->validSpawn[i][j] = true;
            } else if (pixel == COLOR_BLUE)
            {
                game_state->tiles[i][j] = TILE_TYPE::WATER;
            } else if (pixel == COLOR_RED)
            {
                // Item
                game_state->itemDropSpawns[game_state->itemDropSpawnCount++] = {i*64.f + 32.f, j*64.f + 32.f};
            } else if (pixel == COLOR_GREEN)
            {
                game_state->tiles[i][j] = TILE_TYPE::GRASS;
            } else if (pixel == 0x648c1eff)
            {
                game_state->validSpawn[i][j] = true;
                
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

    InitializeNewRound();

    memory->is_initialized = true;
    std::cout << "Initialized game memory\n";
}
