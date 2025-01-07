
inline
v2 GetTileWorldPos(int posx, int posy) {
    return v2(posx * 64.f, posy * 64.f);
}

enum TILE_TYPE : u8
{
    TT_NONE,
    WATER,
    WALL,
    GRASS,
    DOWNHILL_RIGHT,
    DOWNHILL_DOWN_RIGHT,
    DOWNHILL_DOWN,
    DOWNHILL_DOWN_LEFT,
    DOWNHILL_LEFT,
    DOWNHILL_UP_LEFT,
    DOWNHILL_UP,
    DOWNHILL_UP_RIGHT,
    HOLE,
};


TILE_TYPE GetDownhillTileFromPixel(Color pixel)
{
    if (pixel == Color(0,100,0,255))
    {
        return TILE_TYPE::DOWNHILL_RIGHT;
    } else if (pixel == 0x250064ff)
    {
        return TILE_TYPE::DOWNHILL_LEFT;
    } else if (pixel == 0x640062ff)
    {
        return TILE_TYPE::DOWNHILL_UP;
    } else if (pixel == 0x646200ff)
    {
        return TILE_TYPE::DOWNHILL_DOWN;
    } else if (pixel == 0x641b00ff)
    {
        return TILE_TYPE::DOWNHILL_DOWN_RIGHT;
    } else if (pixel == 0xa5b50eff)
    {
        return TILE_TYPE::DOWNHILL_DOWN_LEFT;
    } else if (pixel == 0x8145beff)
    {
        return TILE_TYPE::DOWNHILL_UP_LEFT;
    } else if (pixel == 0x008f45ff)
    {
        return TILE_TYPE::DOWNHILL_UP_RIGHT;
    }
    return TILE_TYPE::GRASS;
}


bool IsTileDownhill(i32 tile)
{
    return tile >= DOWNHILL_RIGHT && tile < DOWNHILL_RIGHT+8;
}
