
struct PlayerData
{
    i32 abilities[MAX_ABILITIES] = {ABILITY::NONE};
    i32 abilityCount = 0;
    i32 selectedAbility = 0;
    i32 strokeCount = 0;
    i32 score = 0;
    i32 unprocessedItems = 0;
    i32 id = 0;
    bool madeBallOnCurrentRound=false;
    Ball *ball;
    generic_drawable strokeText;
};

void ChangeStrokes(PlayerData *player, i32 count);
void ConsumeAbility(PlayerData *player);
