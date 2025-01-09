
enum ABILITY
{
    CRATER,
    PLACE_OBSTACLE,
    PLACE_BOUNCER,
    RETRY,
    HEAVY_WIND,
    PROTECTION,

    ABILITY_COUNT,

    MOVE_HOLE,
    SHANK,
    NONE,
};

const i32 MAX_ABILITIES = 16;

void OnAbilitySelected(i32 ability);
void OnAbilityUse(i32 ability);
void AbilityCodeForWhenTurnStarts();
