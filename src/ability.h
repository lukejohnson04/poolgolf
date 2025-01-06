
enum ABILITY
{
    CRATER,
    PLACE_OBSTACLE,
    RETRY,
    HEAVY_WIND,
    MOVE_HOLE,
    SHOOT_THEIR_BALL,
    ABILITY_COUNT,
    NONE,

    PLACE_BOUNCER,
};

void OnAbilitySelected(i32 ability);
void OnAbilityUse(i32 ability);
