
void RenderPreviewLine() {
    LevelState *level = &game_state->level;
    UseShader(GetShader("color"));
    GetShader("color")->UniformColor("color", COLOR_WHITE);
    v2 shotVec = ConvertAngleToVec(level->cueRotation + PIf);

    float previewLength;
    previewLength = level->cuePower * 8.f;
    
    previewLength = CLAMP(16.f, 800.f, previewLength);
    
    int closest=-1;
    v2 lineEnd = GetCurrentPlayer()->ball->pos + (shotVec * previewLength);
    v2 a_dir(-1,-1);
    v2 b_dir(-1,-1);
    
    for (int i = 0; i < level->ballCount; i++) {
        if (game_state->currentPlayer == i)
        {
            continue;
        }

        if (level->balls[i].active == false ||
            level->balls[i].falling)
        {
            continue;
        }

        // Check if ball is over spawn tile
        if (IsBallOnSpawnTile(level,level->balls[i].pos))
        {
            continue;
        }
        
        // Check for collision along the ray
        v2 col_point = BallGetCollisionPoint(GetCurrentPlayer()->ball, &level->balls[i], shotVec);
        if (col_point.x == -1 && col_point.y == -1)
        {
            continue;
        }

        // Ensure the ray is cast in the correct direction
        float dist = DistanceBetween(level->balls[i].pos, GetCurrentPlayer()->ball->pos);
        float dist2 = DistanceBetween(level->balls[i].pos, GetCurrentPlayer()->ball->pos + shotVec);
        if (dist <= dist2)
        {
            continue;
        }

        // Ensure the distance to collision is within the length of the line
        // Note this is distance from the edge, not from the center of one ball to another
        float outterDistance = dist - GetCurrentPlayer()->ball->radius - level->balls[i].radius;
        if (outterDistance > previewLength)
        {
            continue;
        }

        BallPredictCollisionResolve(GetCurrentPlayer()->ball, &level->balls[i], shotVec, col_point, &a_dir, &b_dir);
        
        lineEnd = col_point;
        closest = i;
    }

    
    if (closest != -1 && DistanceBetween(lineEnd, GetCurrentPlayer()->ball->pos) >= previewLength)
    {
        lineEnd = Normalize(lineEnd - GetCurrentPlayer()->ball->pos) * previewLength;
        closest = 0;
    }

    v2 firstLineEnd = lineEnd;
    if (closest != -1)
    {
        firstLineEnd += Normalize(GetCurrentPlayer()->ball->pos - lineEnd) * level->balls[closest].radius;
    }
    
    GL_DrawLine(GetCurrentPlayer()->ball->pos, firstLineEnd);
    GL_DrawLine(GetCurrentPlayer()->ball->pos+v2(1.f,0.f), firstLineEnd+v2(1.f,0.f));
    GL_DrawLine(GetCurrentPlayer()->ball->pos+v2(0.f,1.f), firstLineEnd+v2(0.f,1.f));

    // Draw prediction of ball-on-ball collision
    if (closest != -1)
    {
        v2 aLineStart = lineEnd + (Normalize(a_dir) * level->balls[closest].radius);
        v2 bLineStart = lineEnd + (Normalize(b_dir) * level->balls[closest].radius);

        float ballPreviewLength;
        ballPreviewLength = previewLength -
            DistanceBetween(GetCurrentPlayer()->ball->pos, firstLineEnd);
        a_dir = lineEnd + (Normalize(a_dir) * ballPreviewLength);
        b_dir = lineEnd + (Normalize(b_dir) * ballPreviewLength);
        
        // poor mans line width lmao
        GL_DrawLine(aLineStart, a_dir);
        GL_DrawLine(aLineStart - v2(1.f,0.f), a_dir - v2(1.f,0.f));
        GL_DrawLine(aLineStart + v2(1.f,0.f), a_dir + v2(1.f,0.f));
        GL_DrawLine(aLineStart - v2(0.f,1.f), a_dir - v2(0.f,1.f));
        GL_DrawLine(aLineStart + v2(0.f,1.f), a_dir + v2(0.f,1.f));

        GL_DrawLine(bLineStart - v2(1.f,0.f), b_dir - v2(1.f,0.f));
        GL_DrawLine(bLineStart + v2(1.f,0.f), b_dir + v2(1.f,0.f));
        GL_DrawLine(bLineStart - v2(0.f,1.f), b_dir - v2(0.f,1.f));
        GL_DrawLine(bLineStart + v2(0.f,1.f), b_dir + v2(0.f,1.f));

        UseShader(GetShader("texture"));
        iRect outline_rect = { 64, 0, 32, 32 };
        iRect outline_dest = { (i32)lineEnd.x-16, (i32)lineEnd.y-16, 32, 32 };
        GL_DrawTexture(outline_rect, outline_dest);
    }
}

