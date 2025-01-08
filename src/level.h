
struct Obstacle
{
    v2 pos;
    float rot;
};

// All data for the level at a given moment
struct LevelState
{
    u8 tiles[MAP_SIZE][MAP_SIZE];
    u8 validSpawn[MAP_SIZE][MAP_SIZE];

    v2 itemDrops[64];
    v2 itemDropSpawns[64];
    v2 bouncers[32];
    Obstacle obstacles[32];
    Ball balls[16];

    i32 itemDropCount = 0;
    i32 itemDropSpawnCount = 0;
    i32 bouncerCount = 0;
    i32 ballCount = 0;
    i32 obstacleCount = 0;
    
    float ballsStoppedTimer = 0.f;
    v2 ballSpawnPosition;
    v2 ballStartPosition = {0.f,0.f};
    
    float shootingMotionTimer=0.f;

    float cueRotation = 0.f;
    float cuePower = 1.0f;
    float cueRotSpeed = 0.5f;

    v2 holePos;
    bool shotAlready=false;
};

void LoadLevelForFirstTime(LevelState *level);
