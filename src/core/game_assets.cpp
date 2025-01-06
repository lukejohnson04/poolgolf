
#include <sys/types.h>
#include <sys/stat.h>
// #ifndef WIN32
// #include <unistd.h>
// #endif

#ifdef WIN32
#define stat _stat
#endif

#include "game_assets.h"

// In release mode fetching a texture will be O(1), this is just
// ugly and brute force so it will always refresh updated textures!

internal
Resource *GetTextureResource(const std::string path) {
    for (u32 i=0; i<private_global_assets->texture_count; i++) {
        if (private_global_assets->textures[i].meta.path == path) {
            return &private_global_assets->textures[i];
        }
    }
    GetTexture(path);
    for (u32 i=0; i<private_global_assets->texture_count; i++) {
        if (private_global_assets->textures[i].meta.path == path) {
            return &private_global_assets->textures[i];
        }
    }
    return nullptr;
}

internal
GLuint GetTexture(const std::string path) {
    for (u32 i=0; i<private_global_assets->texture_count; i++) {
        if (private_global_assets->textures[i].meta.path == path) {
            return private_global_assets->textures[i].texture;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    glGenTextures(1,&temp.texture);
    i32 loaded = GL_load_texture(temp.texture,path.c_str());
    
    if (!loaded) {
        printf("Texture %s does not exist!\n",path.c_str());
        return 0;
    }

    Assert(private_global_assets->texture_count+1 < game_assets::MAX_TEXTURES);
    Resource *res = &private_global_assets->textures[private_global_assets->texture_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
    
    return res->texture;
}

internal
Mix_Chunk *GetChunk(const std::string path) {
    for (u32 i=0; i<private_global_assets->chunk_count; i++) {
        if (private_global_assets->chunks[i].meta.path == path) {
            return private_global_assets->chunks[i].chunk;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    temp.chunk = Mix_LoadWAV(path.c_str());
    
    if (temp.chunk == NULL) {
        printf("Chunk %s does not exist!\n",path.c_str());
        return NULL;
    }

    Assert(private_global_assets->chunk_count+1 < game_assets::MAX_CHUNKS);
    Resource *res = &private_global_assets->chunks[private_global_assets->chunk_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
    
    return res->chunk;
}

internal
TTF_Font* GetFont(const std::string path)
{
    for (u32 i=0; i<private_global_assets->font_count; i++) {
        if (private_global_assets->fonts[i].meta.path == path) {
            return private_global_assets->fonts[i].font;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    temp.font = TTF_OpenFont(path.c_str(), 16);
    
    if (temp.font == NULL) {
        printf("Font %s does not exist!\n",path.c_str());
        return NULL;
    }

    Assert(private_global_assets->font_count+1 < game_assets::MAX_FONTS);
    Resource *res = &private_global_assets->fonts[private_global_assets->font_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
    
    return res->font;
}


internal
Shader *GetShader(const std::string path)
{
    for (u32 i=0; i<private_global_assets->shader_count; i++) {
        if (private_global_assets->shaders[i].meta.path == path) {
            return &private_global_assets->shaders[i].shader;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    temp.shader = CreateShader(path + ".vert", path + ".frag");
    
    if (temp.shader.id == NULL) {
        printf("Shader %s does not exist!\n", path.c_str());
        return NULL;
    }

    Assert(private_global_assets->shader_count+1 < game_assets::MAX_SHADERS);
    Resource *res = &private_global_assets->shaders[private_global_assets->shader_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat((SHADER_PATH + path + ".vert").c_str(), &file_data)==0) {
        res->meta.vert_last_write = file_data.st_mtime;
        printf("Wow!\n");
    }
    if (stat((SHADER_PATH + path + ".frag").c_str(), &file_data)==0) {
        res->meta.frag_last_write = file_data.st_mtime;
    }
    
    return &res->shader;
}


internal
bool ResourceWasUpdated(Resource *res) {
    struct stat file_data;
    if (stat(res->meta.path.c_str(), &file_data)==0) {
        time_t last_write = file_data.st_mtime;
        if (last_write == res->meta.last_write) {
            return false;
        }
        return true;
    }
    return false;
}


internal
void CheckForResourceUpdates(game_assets *assets) {
    for (u32 id=0; id<assets->texture_count; id++) {
        Resource *res = &assets->textures[id];
        if (ResourceWasUpdated(res)) {
            i32 loaded = GL_load_texture(res->texture,res->meta.path.c_str());
    
            if (!loaded) {
                continue;
            }

            struct stat file_data;
            if (stat(res->meta.path.c_str(), &file_data)==0) {
                res->meta.last_write = file_data.st_mtime;
            }
        }
    }

    for (u32 id=0; id<assets->chunk_count; id++) {
        Resource *res = &assets->chunks[id];
        if (ResourceWasUpdated(res)) {
            res->chunk = Mix_LoadWAV(res->meta.path.c_str());
            
            if (res->chunk == NULL) {
                continue;
            }

            struct stat file_data;
            if (stat(res->meta.path.c_str(), &file_data)==0) {
                res->meta.last_write = file_data.st_mtime;
            }
        }
    }

    for (u32 id=0; id<assets->font_count; id++) {
        Resource *res = &assets->fonts[id];
        if (ResourceWasUpdated(res)) {
            res->font = TTF_OpenFont(res->meta.path.c_str(),16);
            
            if (res->font == NULL) {
                continue;
            }

            struct stat file_data;
            if (stat(res->meta.path.c_str(), &file_data)==0) {
                res->meta.last_write = file_data.st_mtime;
            }
        }
    }

    for (u32 id=0; id<assets->shader_count; id++) {
        Resource *res = &assets->shaders[id];

        struct stat file_data;
        if (stat((SHADER_PATH + res->meta.path + ".vert").c_str(), &file_data)==0) {
            time_t last_write = file_data.st_mtime;
            if (last_write != res->meta.vert_last_write) {
                goto shader_update;
            }
        } if (stat((SHADER_PATH + res->meta.path + ".frag").c_str(), &file_data)==0) {
            time_t last_write = file_data.st_mtime;
            if (last_write != res->meta.frag_last_write) {
                goto shader_update;
            }
        }
        continue;
        
shader_update:
        printf("Creating new shader...\n");
        res->shader = CreateShader(res->meta.path + ".vert", res->meta.path + ".frag");
        
        if (res->shader.id == NULL) {
            printf("Failed to load shader!\n");
            continue;
        }

        if (stat((SHADER_PATH + res->meta.path + ".vert").c_str(), &file_data)==0) {
            res->meta.vert_last_write = file_data.st_mtime;
        }
        if (stat((SHADER_PATH + res->meta.path + ".frag").c_str(), &file_data)==0) {
            res->meta.frag_last_write = file_data.st_mtime;
        }
    }
}

