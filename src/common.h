
#pragma once
#include <inttypes.h>

#define FPS_MAX 60.0f
#define FRAME_TIME_MS (1000.f / FPS_MAX)
#define FRAME_TIME (1.0f / FPS_MAX)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t bool32;
typedef bool32 b32;

typedef float real32;
typedef double real64;

typedef real32 f32; // float
typedef real64 d64; // double

typedef u16 entity_id;

#define local_persist static
#define global_variable static
#define internal static

#define Kilobytes(size) (1024LL*size)
#define Megabytes(size) (1024LL*Kilobytes(size))
#define Gigabytes(size) (1024LL*Megabytes(size))

struct GameMemory {
    bool is_initialized;
    u64 size;
    void *permanentStorage;
};

#define ID_DONT_EXIST 999999999

// collision layers
enum {
    LAYER_NONE=0,
    LAYER_PHYSICS=1
};

struct Color {
    u8 r, g, b, a;
    Color(u8 nr=0, u8 ng=0, u8 nb=0, u8 na=0) : r(nr), g(ng), b(nb), a(na) {}

    u32 hex() {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    static Color hexToColor(const u32 &right) {
        Color c;
        c.r = (u8)((right) & 255);
        c.g = (u8)((right >> 8) & 255);
        c.b = (u8)((right >> 16) & 255);
        c.a = (u8)(right >> 24);
        return c;
    }
};


inline bool operator == (Color left, Color right) {
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

inline bool operator != (Color left, Color right) {
    return !(left == right);
}

inline bool operator == (Color left, u32 right) {
    return left.hex() == right;
}

struct v2;
struct v2i;

struct v2 {
    union {
        struct {
            float x, y;
        };
    
        struct {
            float u, v;
        };
    
        float values[2];
    };

    v2();
    v2(float X, float Y);
    v2(i32 X, i32 Y);
    v2(u32 X, u32 Y);
    v2(v2i A);
};

struct v2i {
    union {
        struct {
            i32 x, y;
        };
        
        i32 values[2];
    };

    v2i();
    v2i(float X, float Y);
    v2i(i32 X, i32 Y);
    v2i(u32 X, u32 Y);
    v2i(v2 A);
};

struct v3
{
    union
    {
        struct
        {
            float x, y, z;
        };
        
        struct
        {
            float r, g, b;
        };
    };

    v3();
    v3(float a, float b, float c);
    v3(glm::vec3 a);
};
