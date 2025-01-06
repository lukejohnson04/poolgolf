#include "maths.h"

#define MAX(a,b) ((a>=b)?(a):(b))
#define MIN(a,b) ((a<=b)?(a):(b))
#define CLAMP(min,max,val) (((val)<(min))?(min):((val)>(max))?(max):(val))

#define PI 3.1415926
#define PIf 3.1415926f

// v2
v2::v2() {}
v2::v2(float X, float Y) {
    x = X;
    y = Y;
}

v2::v2(i32 X, i32 Y) {
    x = (float) X;
    y = (float) Y;
}

v2::v2(u32 X, u32 Y) {
    x = (float) X;
    y = (float) Y;
}

v2::v2(v2i A) {
    x = (float) A.x;
    y = (float) A.y;
}

// v2i
v2i::v2i() {}
v2i::v2i(i32 X, i32 Y) {
    x = X;
    y = Y;
}

v2i::v2i(float X, float Y) {
    x = (i32) X;
    y = (i32) Y;
}

v2i::v2i(u32 X, u32 Y) {
    x = (i32) X;
    y = (i32) Y;
}

v2i::v2i(v2 A) {
    x = (i32) A.x;
    y = (i32) A.y;
}


// Operators for:
// Vector addition/subtraction
v2i operator + (v2i A, v2i B) {
    v2i res;
    res.x = A.x + B.x;
        res.y = A.y + B.y;
    return res;
}

v2 operator + (v2 A, v2 B) {
    v2 res;
    res.x = A.x + B.x;
    res.y = A.y + B.y;
    return res;
}


v3 operator + (v3 A, v3 B) {
    v3 res;
    res.x = A.x + B.x;
    res.y = A.y + B.y;
    res.z = A.z + B.z;
    return res;
}

v2i operator - (v2i A, v2i B) {
    v2i res;
    res.x = A.x - B.x;
    res.y = A.y - B.y;
    return res;
}

v2 operator - (v2 A, v2 B) {
    v2 res;
    res.x = A.x - B.x;
    res.y = A.y - B.y;
    return res;
}


v3 operator - (v3 A, v3 B) {
    v3 res;
    res.x = A.x - B.x;
    res.y = A.y - B.y;
    res.z = A.z - B.z;
    return res;
}


v2i &operator += (v2i &A, v2i B) {
    A = A + B;
    return A;
}

v2 &operator += (v2 &A, v2 B) {
    A = A + B;
    return A;
}


v3 &operator += (v3 &A, v3 B) {
    A = A + B;
    return A;
}


v2i &operator -= (v2i &A, v2i B) {
    A = A - B;
    return A;
}

v2 &operator -= (v2 &A, v2 B) {
    A = A - B;
    return A;
}


v3 &operator -= (v3 &A, v3 B) {
    A = A - B;
    return A;
}


// Number multiplication
v2 operator * (v2 A, float B) {
    v2 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    
    return Result;
}

v2 operator * (v2 A, i32 B) {
    v2 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    
    return Result;
}


v3 operator * (v3 A, float B) {
    v3 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    
    return Result;
}

v3 operator * (v3 A, i32 B) {
    v3 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    
    return Result;
}


v2i operator * (v2i A, i32 B) {
    v2i Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    
    return Result;
}

v2 &operator *= (v2 &A, float B) {
    A = A * B;
    return A;
}

v2 &operator *= (v2 &A, i32 B) {
    A = A * B;
    return A;
}


v3 &operator *= (v3 &A, float B) {
    A = A * B;
    return A;
}


v3 &operator *= (v3 &A, i32 B) {
    A = A * B;
    return A;
}


v2i &operator *= (v2i &A, i32 B) {
    A = A * B;
    return A;
}


// Equality
bool operator==(v2 A, v2 B) {
    return A.x==B.x && A.y==B.y;
}

bool operator==(v3 A, v3 B) {
    return A.x==B.x && A.y==B.y && A.z==B.z;
}


bool operator==(v2i A, v2i B) {
    return A.x==B.x && A.y==B.y;
}

// Inequality
bool operator!=(v2 A, v2 B) {
    return A.x!=B.x || A.y!=B.y;
}

bool operator!=(v3 A, v3 B) {
    return A.x!=B.x || A.y!=B.y || A.z!=B.z;
}


bool operator!=(v2i A, v2i B) {
    return A.x!=B.x || A.y!=B.y;
}

// Negation
v2 operator - (v2 A) {
    return {-A.x, -A.y};
}

v3 operator - (v3 A) {
    return {-A.x, -A.y, -A.z};
}


v2i operator - (v2i A) {
    return {-A.x, -A.y};
}

//
// Math functions for:

// Length
float Length(v2 A) {
    return sqrt((A.x * A.x) + (A.y * A.y));
}

float Length(v3 A) {
    return sqrt((A.x * A.x) + (A.y * A.y) + (A.z * A.z));
}


// Normalize
v2 Normalize(v2 A) {
    if (A == v2{0,0}) {
        return A;
    }
    float len = Length(A);
    return v2(A.x / len, A.y / len);
}

v3 Normalize(v3 A) {
    if (A == v3{0,0,0}) {
        return A;
    }
    float len = Length(A);
    return v3(A.x / len, A.y / len, A.z / len);
}


// Perpendicular
v2 Perpendicular(v2 A) {
    return {-A.y, A.x};
}


// Lerp
// Dot product
// Get length
struct Circle {
    v2 pos;
    float radius;
};


struct fRect;
struct iRect {
    iRect() {}
    iRect(int nx, int ny, int nw, int nh) : x(nx),y(ny),w(nw),h(nh){}
    iRect(fRect a);
    
    bool contains(int px, int py);
    bool contains(v2 A);
    int x=0, y=0, w=0, h=0;
};

struct fRect {
    fRect() {}
    fRect(float nx, float ny, float nw, float nh) : x(nx),y(ny),w(nw),h(nh){}
    fRect(iRect a) : x((float)a.x),y((float)a.y),w((float)a.w),h((float)a.h) {}
    bool contains(int px, int py);
    bool contains(v2 A);
    float x=0, y=0, w=0, h=0;
};

iRect::iRect(fRect a) : x((i32)a.x),y((i32)a.y),w((i32)a.w),h((i32)a.h) {}

bool fRect::contains(int px, int py) {
    if (!w || !h) {
        return false;
    }
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}

bool fRect::contains(v2 A) {
    if (!w || !h) {
        return false;
    }
    return (A.x >= x && A.x <= x + w && A.y >= y && A.y <= y + h);
}


bool iRect::contains(int px, int py) {
    if (!w || !h) {
        return false;
    }
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}

bool iRect::contains(v2 A) {
    if (!w || !h) {
        return false;
    }
    return (A.x >= x && A.x <= x + w && A.y >= y && A.y <= y + h);
}


// Check for collision between rectangles
bool DoRectsCollide(fRect a, fRect b)
{
    if (a.w == 0 || a.h == 0 || b.w == 0 || b.h == 0)
    {
        return false;
    }
    bool res = a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
    return res;
}

// Check for collision between iRects
bool DoRectsCollide(iRect a, iRect b) {
    return DoRectsCollide(fRect(a),fRect(b));
}

// Check for collision between fRect and an iRect
bool DoRectsCollide(fRect a, iRect b) {
    return DoRectsCollide(a,fRect(b));
}

// Rotate vector by rad degrees
v2 V2Rotate(v2 A, float rad) {
    v2 res;
    float cs = cos(rad);
    float sn = sin(rad);

    res.x = A.x * cs - A.y * sn; 
    res.y = A.x * sn + A.y * cs;
    return res;
}

// Compute the dot product of two vectors
internal
float V2DotProduct(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

// Get the angle from one vector to another
internal
float GetAngleToPoint(v2 a, v2 b) {
    float angle = atan2(a.y - b.y, a.x - b.x);
    return angle;
}


// Converts an angle in radians into a
// normalized vector pointing in that direction
internal
v2 ConvertAngleToVec(float angle) {
    v2 p = {-cos(angle), -sin(angle)};
    return p;
}

// Function to determine difference between angles, accounting for wrapping (i.e. comparing 720 and 30)
internal
float angle_diff(float a, float b) {
    return PIf - abs(abs(a-b) - PIf);
}

// Gets a normalized vector pointing from
// point A to point B
internal
v2 GetVecToPoint(v2 a, v2 b) {
    v2 diff = {b.x - a.x, b.y - a.y};
    return Normalize(diff);
}

internal
float ConvertVecToAngle(v2 a) {
    return GetAngleToPoint({0,0},a);
}

internal
int Lerp(int a, int b, float t) {
    return int(a * (1.0 - t) + (b * t));
}

internal
float Lerp(float a, float b, float t) {
    return a * (1.0f - t) + (b * t);
}


internal
v2 Lerp(v2 a, v2 b, float t) {
    return v2(Lerp(a.x,b.x,t), Lerp(a.y,b.y,t));
}

internal
float deg_2_rad(float d) {
    return d * (PIf/180.f);
}

internal
float rad_2_deg(float d) {
    return d / (PIf/180.f);
}

internal
float DistanceBetween(v2 a, v2 b) {
    float x_sqr = (a.x-b.x)*(a.x-b.x);
    float y_sqr = (a.y-b.y)*(a.y-b.y);
    return sqrt(x_sqr + y_sqr);
}

internal
float lerp_rotation(float rot, float dest, float t) {
    if (dest - rot > PI) {
        dest -= PIf*2;
    }
    float res = Lerp(rot,dest,t);
    if (res < 0) {
        res += PIf*2;
    }
    return res;
}

internal
inline float wrap_rotation(float rot) {
    while (rot >= PIf*2) {
        rot -= PIf*2;
    }
    while (rot < 0) {
        rot += PIf*2;
    }
    return rot;
}


v2 GetClosestPointOnLine(v2 l1, v2 l2, v2 point) {
    float A1 = l2.y - l1.y;
    float B1 = l1.x - l2.x;

    double C1 = (l2.y - l1.y) * l1.x + (l1.x - l2.x) * l1.y;
    double C2 = -B1 * point.x + A1 * point.y;
    double det = A1 * A1 - ( - B1 * B1);
    
    v2 ans = point;
    if (det != 0) {
        ans.x = (float)((A1 * C1 - (B1 * C2)) / det);
        ans.y = (float)((A1 * C2 - ( - B1 * C1)) / det);
    }

    return ans;
}

v3::v3()
{
    x = 0.f;
    y = 0.f;
    z = 0.f;
}

v3::v3(float a, float b, float c) : x(a), y(b), z(c) {}

v3::v3(glm::vec3 a) : x(a.x), y(a.y), z(a.z) {}

