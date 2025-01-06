
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

struct mat4
{
    float vals[4][4];
};

