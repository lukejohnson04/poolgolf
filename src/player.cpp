
void ChangeStrokes(PlayerData *player, i32 count)
{
    player->strokeCount = count;
    player->strokeText = GenerateTextObj(
        GetFont("res/m5x7.ttf"), "P" + std::to_string(player->id+1) +
        ": " + std::to_string(count) + "/" + std::to_string(game_state->level.par),
        COLOR_WHITE, player->strokeText.gl_texture);
    player->strokeText.pos = {28,20 + 56 * player->id};
    player->strokeText.scale = {4.f, 4.f};
}

void ConsumeAbility(PlayerData *player)
{
    player->abilities[player->selectedAbility] =
        player->abilities[--player->abilityCount];
}

