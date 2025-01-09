
struct Button
{
    v2 pos;
    iRect bounds;
    iRect inactive;
    iRect hover;
    iRect click;
    
    iRect src;

    bool hovering=false;
    bool clicking=false;
    bool justClicked=false;
};

bool DoButton(Button *a)
{
    v2i mPos = GetMousePosition();
    a->justClicked = false;
    if (a->bounds.contains(mPos))
    {
        a->hovering = true;
        if (input->mouse_pressed)
        {
            a->clicking = true;
            a->src = a->click;
            if (input->mouse_just_pressed)
            {
                a->justClicked = true;
                return true;
            }
        } else
        {
            a->hovering = false;
            a->src = a->hover;
        }
    } else
    {
        a->hovering = false;
        a->clicking = false;
        a->src = a->inactive;
    }
    return false;
}
