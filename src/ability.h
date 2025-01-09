
enum ABILITY
{
    CRATER,
    PLACE_OBSTACLE,
    PLACE_BOUNCER,
    RETRY,
    HEAVY_WIND,
    MOVE_HOLE,
    SHANK,

    ABILITY_COUNT,
    NONE,
};

void OnAbilitySelected(i32 ability);
void OnAbilityUse(i32 ability);
void AbilityCodeForWhenTurnStarts();
