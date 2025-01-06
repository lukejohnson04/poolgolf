
const i32 MAX_ABILITIES = 16;

struct PlayerData
{
    i32 abilities[MAX_ABILITIES] = {ABILITY::NONE};
    i32 abilityCount = 0;
    i32 selectedAbility = 0;
    i32 strokeCount = 0;
    i32 unprocessedItems = 0;
    i32 id = 0;
    bool madeBallOnCurrentRound=false;
    Ball *ball;
    generic_drawable strokeText;
};

void ChangeStrokes(PlayerData *player, i32 count)
{
    player->strokeCount = count;
    player->strokeText = GenerateTextObj(
        GetFont("res/m5x7.ttf"), "P" + std::to_string(player->id+1) + ": " + std::to_string(count),
        COLOR_WHITE, player->strokeText.gl_texture);
    player->strokeText.pos = {28,20 + 56 * player->id};
    player->strokeText.scale = {4.f, 4.f};
}

void ConsumeAbility(PlayerData *player)
{
    player->abilities[player->selectedAbility] =
        player->abilities[--player->abilityCount];
}

