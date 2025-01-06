
struct GameState {
    game_assets assets;

    i32 round=0;

    float ballsStoppedTimer = 0.f;
    v2 ballSpawnPosition;
    v2 ballStartPosition = {0.f,0.f};
    PlayerData players[2];
    i32 playerCount = 0;
    i32 currentPlayer = 0;
    
    Ball balls[16];
    i32 ballCount = 0;

    float shootingMotionTimer=0.f;

    float cueRotation = 0.f;
    float cuePower = 1.0f;
    float cueRotSpeed = 0.5f;
    int tiles[MAP_SIZE][MAP_SIZE];
    int validSpawn[MAP_SIZE][MAP_SIZE];

    v2 itemDrops[64];
    i32 itemDropCount = 0;
    v2 itemDropSpawns[64];
    i32 itemDropSpawnCount = 0;

    v2 holePos;

    bool shotAlready=false;
    enum : int {
        POSITIONING_BALL,
        AIMING,
        SHOOTING_MOTION,
        BALL_MOVING,
        POST_SHOT,
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
            i32 selectedBall = 0;
        } craterAbility;
    } abilityState;
    

    Camera camera;
    bool autocam = true;

    float globalTimer=0.f;
};

global_variable GameState *game_state=nullptr;

const glm::mat4 projection = glm::ortho(0.0f,
        static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);

internal PlayerData *GetCurrentPlayer();
internal void InitializeGameMemory(GameMemory *memory);
