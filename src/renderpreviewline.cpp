
void RenderPreviewLine() {
    UseShader(&game_state->rst.sh_color);
    game_state->rst.sh_color.UniformColor("color", COLOR_WHITE);
    v2 shotVec = ConvertAngleToVec(game_state->cueRotation + PIf);

    float previewLength;
    previewLength = game_state->cuePower * 8.f;
    
    previewLength = CLAMP(16.f, 800.f, previewLength);
    
    int closest=-1;
    v2 lineEnd = game_state->cueBall->pos + (shotVec * previewLength);
    v2 a_dir(-1,-1);
    v2 b_dir(-1,-1);
    
    for (int i = 1; i < game_state->ballCount; i++) {
        // Check for collision along the ray
        v2 col_point = BallGetCollisionPoint(&game_state->balls[0], &game_state->balls[i], shotVec);
        if (col_point.x == -1 && col_point.y == -1)
        {
            continue;
        }

        // Ensure the ray is cast in the correct direction
        float dist = DistanceBetween(game_state->balls[i].pos, game_state->cueBall->pos);
        float dist2 = DistanceBetween(game_state->balls[i].pos, game_state->cueBall->pos + shotVec);
        if (dist <= dist2)
        {
            continue;
        }

        // Ensure the distance to collision is within the length of the line
        // Note this is distance from the edge, not from the center of one ball to another
        float outterDistance = dist - game_state->cueBall->radius - game_state->balls[i].radius;
        if (outterDistance > previewLength)
        {
            continue;
        }

        BallPredictCollisionResolve(&game_state->balls[0], &game_state->balls[i], shotVec, col_point, &a_dir, &b_dir);
        
        lineEnd = col_point;
        closest = i;
    }

    
    if (closest != -1 && DistanceBetween(lineEnd, game_state->cueBall->pos) >= previewLength)
    {
        lineEnd = Normalize(lineEnd - game_state->cueBall->pos) * previewLength;
        closest = 0;
    }

    v2 firstLineEnd = lineEnd;
    if (closest != -1)
    {
        firstLineEnd += Normalize(game_state->cueBall->pos - lineEnd) * game_state->balls[closest].radius;
    }
    
    GL_DrawLine(game_state->cueBall->pos, firstLineEnd);
    GL_DrawLine(game_state->cueBall->pos+v2(1.f,0.f), firstLineEnd+v2(1.f,0.f));
    GL_DrawLine(game_state->cueBall->pos+v2(0.f,1.f), firstLineEnd+v2(0.f,1.f));

    if (closest != -1)
    {
        v2 aLineStart = lineEnd + (Normalize(a_dir) * game_state->balls[closest].radius);
        v2 bLineStart = lineEnd + (Normalize(b_dir) * game_state->balls[closest].radius);

        float ballPreviewLength;
        ballPreviewLength = previewLength -
            DistanceBetween(game_state->cueBall->pos, firstLineEnd);
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

        UseShader(&game_state->rst.sh_texture);
        iRect outline_rect = { 64, 0, 32, 32 };
        iRect outline_dest = { (i32)lineEnd.x-16, (i32)lineEnd.y-16, 32, 32 };
        GL_DrawTexture(outline_rect, outline_dest);
    }
}

