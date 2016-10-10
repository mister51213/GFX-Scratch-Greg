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

struct vector4
{
    float x, y, z, w;
}; 

struct bounds
{
    float left, right, top, bottom;
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

struct tetrahedron
{
    vector3 v1, v2, v3, v4;
};

struct cube
{
    vector3 v1, v2, v3, v4, v5, v6, v7, v8;
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

/*********************************************************/
/****************** OPERATOR OVERLOADS *******************/
/*********************************************************/

static vector2 operator + (const vector2& vec1, const vector2& vec2)
{
return { vec1.x + vec2.x, vec1.y + vec2.y};
}

static vector3 operator + (const vector3& vec1, const vector3& vec2)
{
    return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

static triangle2D operator + (const triangle2D& t1, const vector2& t2)
{
return { t1.v1 + t2, t1.v2 + t2, t1.v3 + t2};
}

static vector3 operator - (const vector3& vec1, const vector3& vec2)
{
    return { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

static vector2 operator * (float scalar, vector2 vecIn)
{
    return{ vecIn.x*scalar, vecIn.y*scalar };
}

static vector3 operator * (float scalar, vector3 vecIn)
{
    return{ vecIn.x*scalar, vecIn.y*scalar, vecIn.z*scalar };
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

static vector3 MatVecMult3D(const float matrix[3][3], const vector3& vecIn)
{
    return
    {
        matrix[0][0] * vecIn.x + matrix[0][1] * vecIn.y + matrix[0][2] * vecIn.z,
        matrix[1][0] * vecIn.x + matrix[1][1] * vecIn.y + matrix[1][2] * vecIn.z,
        matrix[2][0] * vecIn.x + matrix[2][1] * vecIn.y + matrix[2][2] * vecIn.z
    };
}

static void Mat3Concat(float* result, float mat2[3][3], float mat1[3][3])
{

float matResult[3][3] = 
    {
            {
        mat1[0][0] * mat2[0][0] + mat1[0][1] * mat2[1][0] + mat1[0][2] * mat2[2][0], 
        mat1[0][0] * mat2[0][1] + mat1[0][1] * mat2[1][1] + mat1[0][2] * mat2[2][1],
        mat1[0][0] * mat2[0][2] + mat1[0][1] * mat2[1][2] + mat1[0][2] * mat2[2][2]
        },
            {
        mat1[1][0] * mat2[0][0] + mat1[1][1] * mat2[1][0] + mat1[1][2] * mat2[2][0], 
        mat1[1][0] * mat2[0][1] + mat1[1][1] * mat2[1][1] + mat1[1][2] * mat2[2][1],
        mat1[1][0] * mat2[0][2] + mat1[1][1] * mat2[1][2] + mat1[1][2] * mat2[2][2]
        },
            {
        mat1[2][0] * mat2[0][0] + mat1[2][1] * mat2[1][0] + mat1[2][2] * mat2[2][0], 
        mat1[2][0] * mat2[0][1] + mat1[2][1] * mat2[1][1] + mat1[2][2] * mat2[2][1],
        mat1[2][0] * mat2[0][2] + mat1[2][1] * mat2[1][2] + mat1[2][2] * mat2[2][2]
        }
    };

result = *matResult;
}

static vector4 MatVecMult4D(const float matrix[4][4], const vector4& vecIn)
{
    return
    {
        matrix[0][0] * vecIn.x + matrix[0][1] * vecIn.y + matrix[0][2] * vecIn.z + matrix[0][3] * vecIn.w,
        matrix[1][0] * vecIn.x + matrix[1][1] * vecIn.y + matrix[1][2] * vecIn.z + matrix[1][3] * vecIn.w,
        matrix[2][0] * vecIn.x + matrix[2][1] * vecIn.y + matrix[2][2] * vecIn.z + matrix[2][3] * vecIn.w,
        matrix[3][0] * vecIn.x + matrix[3][1] * vecIn.y + matrix[3][2] * vecIn.z + matrix[3][3] * vecIn.w
    };
}

static triangle2D FlattenTriangle(const triangle3D& t)
{
    return
    { { t.v1.x,t.v1.y},
      { t.v2.x,t.v2.y},
      { t.v3.x,t.v3.y} };
}

static triangle2D ProjectTriangle(const triangle3D& t)
{
    return
    { { t.v1.x,t.v1.y},
      { t.v2.x,t.v2.y},
      { t.v3.x,t.v3.y} };
}

struct lambdas { float lambda1, lambda2, lambda3; };