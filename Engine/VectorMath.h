#pragma once

// Points defined as vectors with tail at origin
struct vector2
{
    float x, y;
};

struct vector3
{
    float x, y, z;
};

struct quaternion
{
    float w, x, y, z;
};

struct triangle2D
{
    vector2 v1, v2, v3;
};

struct triangle3D
{
    vector3 v1, v2, v3;
};

static void Vector2Scale(vector2& vec, float scale)
{
    vec.x *= scale;
    vec.y *= scale;
}

static void Vector3Scale(vector3& vec, float scale)
{
    vec.x *= scale;
    vec.y *= scale;
    vec.z *= scale;
}

static vector2 operator + (const vector2& vec1, const vector2& vec2)
{
return { vec1.x + vec2.x, vec1.y + vec2.y};
}

static vector3 operator + (const vector3& vec1, const vector3& vec2)
{
    return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

static vector2 operator * (float scalar, vector2 vecIn)
{
    return{ vecIn.x*scalar, vecIn.y*scalar };
}

// Intuitive way to do it
static vector2 operator * (float matrix[2][2], vector2 vecIn)
{
    // transformed basis vector x (i hat)
    vector2 iHat = { matrix[0][0],matrix[1][0] };
    // transformed basis vector y (j hat)
    vector2 jHat = { matrix[0][1],matrix[1][1] };

    // SCALE each basis vector of matrix by magnitudes 
    // of x and y COMPONENTS of vector to be transformed,
    // then ADD them together to get transformed vector
    vector2 vecOut =
        vecIn.x*iHat +
        vecIn.y*jHat;

    // final transformed vector
    return vecOut;
}

// Quaternion multiplication operator (NOT COMMUTATIVE!)
static quaternion operator * (quaternion Q1, quaternion Q2)
{
    quaternion qRotated;
    qRotated.w = (Q1.w*Q2.w - Q1.x*Q2.x - Q1.y*Q2.y - Q1.z*Q2.z);
    qRotated.x = (Q1.w*Q2.x + Q1.x*Q2.w + Q1.y*Q2.z - Q1.z*Q2.y);
    qRotated.y = (Q1.w*Q2.y - Q1.x*Q2.z + Q1.y*Q2.w + Q1.z*Q2.x);
    qRotated.z = (Q1.w*Q2.z + Q1.x*Q2.y - Q1.y*Q2.x + Q1.z*Q2.w);
    return qRotated;
}

static quaternion quatMatrix (quaternion Q1, quaternion Q2)
{}

// Simple way to do it
static vector2 MatVecMult2D(const float matrix[2][2], const vector2& vecIn)
{
    return
    {
        matrix[0][0] * vecIn.x + matrix[0][1] * vecIn.y,
        matrix[1][0] * vecIn.x + matrix[1][1] * vecIn.y
    };
}