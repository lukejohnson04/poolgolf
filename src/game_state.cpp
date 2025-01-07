
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

    LoadLevelForFirstTime(&game_state->level);

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

    InitializeNewRound(&game_state->level);
    StartTurn();

    memory->is_initialized = true;
    std::cout << "Initialized game memory\n";
}
