
struct Button
{
    generic_drawable text;
    v2 pos;
};

internal
Button createButton(v2 pos, TTF_Font *font, std::string str, Color col={255,255,255,255})
{
    Button b;
    b.text = GenerateTextObj(font, str, col);
    b.text.scale = {2.f,2.f};
    b.pos = pos;
    return b;
}

