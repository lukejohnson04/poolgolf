#include "Renderer.h"
#include "Shader.cpp"

const float CUBE_VERTICES[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


float CUBE_VERTICES_NORMALS[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

internal
void GL_DrawRect(iRect rect) {
    float vertices[] = {
        (float)rect.x+rect.w,    (float)rect.y,           0.0f,
        (float)rect.x+rect.w,    (float)rect.y+rect.h,    0.0f,
        (float)rect.x,           (float)rect.y+rect.h,    0.0f,
        (float)rect.x,           (float)rect.y,           0.0f
    };

    // we don't have to repass the projection every frame, unless it changes
    local_persist GLuint VAO,VBO;
    local_persist bool generated=false;
    if (!generated) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        generated=true;
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);    
}

internal
void GL_DrawTexture(fRect dest, bool flip_x, bool flip_y) {
    float vertices[] = {
        (float)dest.x,           (float)dest.y,        0.0f, 0.0f, 0.0f,
        (float)dest.x+dest.w,    (float)dest.y,        0.0f, 1.0f, 0.0f,
        (float)dest.x+dest.w,    (float)dest.y+dest.h, 0.0f, 1.0f, 1.0f,
        (float)dest.x,           (float)dest.y+dest.h, 0.0f, 0.0f, 1.0f,
    };
    
    local_persist GLuint VAO,VBO;
    local_persist bool generated=false;
    if (!generated) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        generated=true;
    }
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);    
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

glm::mat4 rotate_model_matrix(float angleRadians, fRect rect, v2 origin) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec2 pos = glm::vec2(rect.x, rect.y);
    // Calculate the center of the object for rotation
    glm::vec2 center = pos + glm::vec2(origin.x,origin.y);
    model = glm::translate(model, glm::vec3(center, 0.0f)); // Move pivot to center
    model = glm::rotate(model, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
    model = glm::translate(model, glm::vec3(-center, 0.0f)); // Move pivot back
    return model;
}

glm::mat4 rotate_model_matrix(float angleRadians, iRect rect) {
    v2 origin = {rect.w/2.f,rect.h/2.f};
    return rotate_model_matrix(angleRadians,rect,origin);
}


internal i32 GL_load_texture(GLuint tex, const char* path) {
    SDL_Surface *tex_surf = IMG_Load(path);
    if (tex_surf == NULL) {
        std::cout << "ERROR: Failed to load texture at path " << path << std::endl;
        return 0;
    }

    int Mode = GL_RGB;
    if(tex_surf->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_surf->w, tex_surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return 1;
}

internal void GL_load_texture_from_sdl_surface(GLuint tex,SDL_Surface *tex_surf) {
    GLenum format = 0, type = GL_UNSIGNED_BYTE; // Default to unsigned byte for type

    switch (tex_surf->format->format) {
        case SDL_PIXELFORMAT_RGBA8888:
            format = GL_RGBA;
            break;
        case SDL_PIXELFORMAT_ABGR8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_BGRA8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_ARGB8888:
            format = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_RGB888:
        case SDL_PIXELFORMAT_RGB24:
            format = GL_RGB;
            break;
        case SDL_PIXELFORMAT_BGR888:
            format = GL_BGR;
            break;
        case SDL_PIXELFORMAT_RGB565:
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case SDL_PIXELFORMAT_RGBA4444:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case SDL_PIXELFORMAT_RGB332:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE_3_3_2;
            break;
        default:
            break;
    }

    if (format == 0) {
        std::cerr << "No valid OpenGL format found. Texture creation aborted." << std::endl;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format == GL_RGB ? GL_RGB8 : GL_RGBA8, tex_surf->w, tex_surf->h, 0, format, type, tex_surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
}




internal
void generate_text(GLuint tex, TTF_Font *font, std::string str, Color col, i32 wrap) {
    SDL_Surface* temp_surface =
        TTF_RenderText_Solid_Wrapped(font, str.c_str(),*(SDL_Color*)&col, wrap);
    temp_surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_ARGB8888, 0);

    if (tex != NULL) {
        glDeleteTextures(1,&tex);
    }
    glGenTextures(1,&tex);

    GL_load_texture_from_sdl_surface(tex,temp_surface);
    
    SDL_FreeSurface(temp_surface);
}

iRect generic_drawable::getDrawRect() {
    if (bound.w==0||bound.h==0) {
        glBindTexture(GL_TEXTURE_2D,gl_texture);
        // mipmap level is 0
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &bound.w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &bound.h);
    }
    int w=bound.w,h=bound.h;
    iRect res={position.x,position.y,(int)((float)w*scale.x),(int)((float)h*scale.y)};
    return res;
}


internal
generic_drawable GenerateTextObj(TTF_Font *font, std::string str, Color col, GLuint tex) {
    generic_drawable res;
    SDL_Surface* temp_surface =
        TTF_RenderText_Solid(font, str.c_str(),*(SDL_Color*)&col);
    temp_surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_ARGB8888, 0);

    if (tex == NULL) {
        glGenTextures(1,&tex);
    }

    res.gl_texture = tex;
    GL_load_texture_from_sdl_surface(res.gl_texture,temp_surface);
    
    SDL_FreeSurface(temp_surface);
    return res;
}

Uint32 getPixelU32(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

Color getPixel(SDL_Surface *surface, int x, int y)
{
    // Value from getPixelU32 is flipped for some reason? Lol
    Uint32 a = getPixelU32(surface, x, y);
    Color c((u8)(a&255),(u8)((a>>8)&255),(u8)((a>>16)&255),(u8)(a>>24));
    return c;
}

internal
GLuint GL_create_framebuffer(GLuint texture) {
    GLuint fb;
    glGenFramebuffers(1,&fb);
    glBindFramebuffer(GL_FRAMEBUFFER,fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return fb;
}

internal
void GL_load_texture_for_framebuffer(GLuint texture,i32 width,i32 height) {
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);
}

internal
void GL_DrawLine(v2 p1, v2 p2) {
    float line_vertices[] = {
        p1.x,p1.y,0.0f,
        p2.x,p2.y,0.0f
    };

    local_persist GLuint VAO,VBO;
    local_persist bool generated=false;
    if (!generated) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        generated=true;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES,0,2);

    glBindVertexArray(0);
}

internal
void GetUvCoordinates(iRect source, v2 *uv_offset, v2 *uv_scale)
{
    v2i tex_size = {0,0};
    bool flip_x = false;
    bool flip_y = false;
    if (source.w != 0) {
        // store this info in some data structure maybe?
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_size.x);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_size.y);
    }
    *uv_offset = v2((float)source.x / (float)tex_size.x, (float)source.y / (float)tex_size.y);
    *uv_scale = v2((float)source.w / (float)tex_size.x, (float)source.h / (float)tex_size.y);
}

