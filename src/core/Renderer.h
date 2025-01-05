
const Color COLOR_BLACK = Color(0,0,0,255);
const Color COLOR_WHITE = Color(255,255,255,255);
const Color COLOR_RED = Color(255,0,0,255);
const Color COLOR_BLUE = Color(0,0,255,255);
const Color COLOR_GREEN = Color(0,255,0,255);
const Color COLOR_YELLOW = Color(255,255,0,255);
const Color COLOR_TRANSPARENT = Color(0,0,0,0);

struct generic_drawable {
    v2i position={0,0};
    GLuint gl_texture=NULL;
    v2 scale={1,1};
    iRect bound = {0,0,0,0};

    iRect getDrawRect();
};


internal void GL_DrawRect(iRect rect);
internal void GL_DrawTexture(fRect dest, bool flip_x=false, bool flip_y=false);
internal void GL_DrawLine(v2 p1, v2 p2);
internal void GetUvCoordinates(iRect source, v2 *uv_offset, v2 *uv_scale);

internal void generate_text(GLuint tex, TTF_Font *font, std::string str, Color col, i32 wrap);
internal generic_drawable GenerateTextObj(TTF_Font *font, std::string str, Color col={255,255,255,255}, GLuint tex=NULL);

Uint32 getPixelU32(SDL_Surface *surface, int x, int y);
Color getPixel(SDL_Surface *surface, int x, int y);

glm::mat4 rotate_model_matrix(float angleRadians, fRect rect, v2 origin);
glm::mat4 rotate_model_matrix(float angleRadians, iRect rect);

