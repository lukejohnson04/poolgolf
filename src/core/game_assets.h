
#pragma once

struct Resource {
    union {
        GLuint texture;
        Mix_Chunk *chunk;
        Mix_Music *music;
        TTF_Font *font;
        // Shader is just a GLuint id and a bunch of functions
        Shader shader;
    };

    struct {
        union
        {
            time_t last_write;
            struct
            {
                time_t frag_last_write;                
                time_t vert_last_write;                
            };
            
        };
        std::string path;
        union {
            struct {
                i32 width,height;
            };
        };
    } meta;
};


struct game_assets {
    static const int MAX_TEXTURES = 256;
    static const int MAX_CHUNKS = 256;
    static const int MAX_MUSIC = 256;
    static const int MAX_FONTS = 32;
    static const int MAX_SHADERS = 64;

    Resource textures[MAX_TEXTURES];
    Resource chunks[MAX_CHUNKS];
    Resource music[MAX_MUSIC];
    Resource fonts[MAX_FONTS];
    Resource shaders[MAX_SHADERS];

    u32 texture_count=0;
    u32 chunk_count=0;
    u32 music_count=0;
    u32 font_count=0;
    u32 shader_count=0;
};


internal game_assets *private_global_assets=nullptr;


internal Resource *GetTextureResource(std::string path);
internal GLuint GetTexture(std::string path);
internal Mix_Chunk *GetChunk(const std::string path);
internal TTF_Font *GetFont(std::string path);
internal Shader *GetShader(std::string path);

internal void CheckForResourceUpdates(game_assets *assets);
