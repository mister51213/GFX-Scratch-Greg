#pragma once
#include "VectorMath.h"

class Polyhedron
{
public:
    Polyhedron();
    ~Polyhedron();

public:
    vector3 offset1 = { .0f,.0f,5.0f };
    vector3 displacement = { 0.0f,0.0f,0.0f };
    float orientation = 0.0f;
    float accelFactor = 0.0f;

        tetrahedron m_tetra =     
        { {0.f, 109.f, 0.f},
        { 0.f, -54.f, 115.f},
        { -100.f,-54.f,-58.f},
        { 100.f,-54.f,-58.f} };
};

