
{
    Button left, right;
    left.bounds = {WINDOW_WIDTH/2 - 300 - 64, WINDOW_HEIGHT/2, 64, 128};
    left.inactive = {144,272,32,32};
    left.hover = {176,272,32,32};
    left.click = {208,272,32,32};
    right = left;
    right.bounds = {WINDOW_WIDTH/2 + 300, WINDOW_HEIGHT/2, 64, 128};

    if (DoButton(&left))
    {
        do
        {
            selectedBall--;
            if (selectedBall < 0)
            {
                selectedBall = level->ballCount-1;
            }
        } while (level->balls[selectedBall].active == false);
    }
    if (DoButton(&right))
    {
        do
        {
            selectedBall++;
            if (selectedBall > level->ballCount-1)
            {
                selectedBall = 0;
            }
        } while (level->balls[selectedBall].active == false);
    }

}
