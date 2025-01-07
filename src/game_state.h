
struct GameState {
    game_assets assets;

    i32 round=0;

    PlayerData players[2];
    i32 playerCount = 0;
    i32 currentPlayer = 0;

    LevelState level;
    LevelState levelBuffer;

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
        struct
        {
            i32 rotation = 0;
        } obstacleAbility;

        struct
        {
            i32 selectedBall = 0;
        } craterAbility;

        struct
        {            
            float needleRotation;
            float selectionTime;
            bool activated;
        } windAbility;
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
