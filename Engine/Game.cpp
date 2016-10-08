/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.cpp																			  *
 *	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <chrono>

//******************* GLOBAL VARS **********************//
float scrnWHalf = Graphics::ScreenWidth * 0.5f;
float scrnHHalf = Graphics::ScreenHeight * 0.5f;
vector2 transToScrn = {scrnWHalf, scrnHHalf};

// 2D Triangles
//triangle2D t1 = { { 10,10 }, { 15,200 }, { 300,80 } };
//triangle2D t2 = { {100,100},{ 500,150}, { 400,400} };
//triangle2D t3 = { {200,200},{ 150,20}, { 30,200} };

//********* TEMPORARY VARIABLE HOLDERS ***********//
float leftSide;
float rightSide;
float top;
float bottom;

float lambda1 = 0.0f;
float lambda2 = 0.0f;
float lambda3 = 0.0f;

float L1P2 = 0.0f;
float L2P3 = 0.0f;
float L3P1 = 0.0f;

// TODO: Make WORLD offset (up here only defines local object space offset)
/*************  3D Polygons  ********************/
float orientation = 0.0f;
float accelFactor = 0.0f;
float dstTet1 = .01f; // CANNOT BE 0!

// tetrahedron
const vector3 offset1 = { .0f,.0f,.0f };
tetrahedron tetra1 = 
    {{0.f+offset1.x,1009.f+offset1.y,0.f+offset1.z},
    { 0.f+offset1.x,-540.f+offset1.y,1150.f+offset1.z}, 
    { -1000.f+offset1.x,-540.f+offset1.y,-580.f+offset1.z}, 
    { 1000.f+offset1.x,-540.f+offset1.y,-580.f+offset1.z}};

tetrahedron tetraRotated = tetra1;

char axis = 'X';

cube cube1 =
// front face
{ {200,200,0},
{ 200,300,0},
{ 300,200,0},
{ 300,300,0},
// back face
{ 200,200,100},
{ 200,300,100},
{ 300,200,100},
{ 300,300,100} };

// Initialize quaternion base rotation to 0
quaternion totalRotation = { 1.f, 0.f, 0.f, 0.f };

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	log( L"alog.txt" )
{}

void Game::Go() // infinite loop
{
	gfx.BeginFrame(); // clears the screen	
	UpdateModel(); // game logic only
    ComposeFrame(); // render instantaneous frame
	gfx.EndFrame(); // swap buffer - present frame
}

void Game::UpdateModel()
{
    if (++m_x + 50 > gfx.ScreenWidth)
    {
        m_x = 0;
        if ((m_y += 50) + 50 > gfx.ScreenHeight)
        {
            m_y = 0;
        }
    }

    // ACCELERATE AND DECELERATE ROTATION
    if (wnd.kbd.KeyIsPressed(VK_END))
    {
    accelFactor = 0.0f;
    orientation = 0.0f;
    }
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    accelFactor += 0.0001;
    if(wnd.kbd.KeyIsPressed(VK_CONTROL))
    accelFactor -= 0.0001;

    // PITCH UP
    if (wnd.kbd.KeyIsPressed(VK_UP))
    {
    orientation += accelFactor;
    axis = 'X';
    }

    // PITCH DOWN
    else if (wnd.kbd.KeyIsPressed(VK_DOWN))
    {
    orientation -= accelFactor;
    axis = 'X';
    }

    // YAW LEFT
    if (wnd.kbd.KeyIsPressed(VK_LEFT))
    {
    orientation -= accelFactor;
    axis = 'Y';
    }

    // YAW RIGHT
    if (wnd.kbd.KeyIsPressed(VK_RIGHT))
    {
    orientation += accelFactor;
    axis = 'Y';
    }

    // ROLL RIGHT
    if (wnd.kbd.KeyIsPressed('Q'))
    {
    orientation += accelFactor;
    axis = 'Z';
    }

    // ROLL LEFT
    if (wnd.kbd.KeyIsPressed('E'))
    {
    orientation -= accelFactor;
    axis = 'Z';
    }
    
   // change distance to polygon
   if (wnd.kbd.KeyIsPressed('S'))
   {
       if (dstTet1 == -0.001)
       dstTet1 += 0.0025f;
       else
       dstTet1 += 0.001f;
   }      
       
       if (wnd.kbd.KeyIsPressed('W'))
   {
       if(dstTet1==0.001)
       dstTet1 -= 0.0025f;
       else
       dstTet1 -= 0.001f;
   }

    if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
    {
        wnd.Kill();
    }
}

// Lambda1, 2, and 3 are the coefficients reprsenting how much of each
// points a,b,and c, goes into the point in question
void Game::CalculateBaryCentricCoordinates(
    vector2 desiredPt,
    vector2 a,
    vector2 b,
    vector2 c,
    float& lambda1,
    float& lambda2,
    float& lambda3)
{
    lambda1 = ((b.y - c.y)*(desiredPt.x - c.x) + (c.x - b.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda2 = ((c.y - a.y)*(desiredPt.x - c.x) + (a.x - c.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda3 = 1.0f - lambda1 - lambda2;
}

void Game::CalculateScanLineCoordinates()
{}

vector4 Game::ProjectionMatrix(const float matrix[4][4], const vector3& vecIn)
{
    float halfHeight = gfx.ScreenHeight*0.5f;
    float halfWidth = gfx.ScreenWidth*0.5f;
    vector4 vec4 = { vecIn.x,vecIn.y,vecIn.z,1.0f };
    return vec4;
}

// TODO: fix winding order and redundancies
vector<triangle3D> Game::GetTriangleList(tetrahedron polygon)
{
    vector<triangle3D> triList;
    triList.push_back({ polygon.v1, polygon.v2, polygon.v3 });
        triList.push_back({ polygon.v1, polygon.v4, polygon.v3 });
            triList.push_back({ polygon.v1, polygon.v2, polygon.v4 });
                 triList.push_back({ polygon.v4, polygon.v2, polygon.v3 });

    return triList;
}

void Game::DrawTriangle(const triangle2D triangle, Color color)
{
    // Bounding box - uses temporary global variables up top
    leftSide = min({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    rightSide = max({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    top = min({ triangle.v1.y,triangle.v2.y, triangle.v3.y });
    bottom = max({ triangle.v1.y,triangle.v2.y, triangle.v3.y });

    vector<lambdas> vertexLambdas;

    for (int j = top; j <= bottom; j++)
    {
        for (int i = leftSide; i <= rightSide; i++)
        {
            if (i > 0 && i < gfx.ScreenWidth&&j>0 && j < gfx.ScreenHeight)
            {
                CalculateBaryCentricCoordinates({ (float)i,(float)j }, triangle.v1, triangle.v2, triangle.v3, lambda1, lambda2, lambda3);

                if (lambda1 >= 0.0f && lambda2 >= 0.0f && lambda3 >= 0.0f)
                {
                    L1P2 = lambda1 + lambda2;
                    L2P3 = lambda2 + lambda3;
                    L3P1 = lambda3 + lambda1;

                    //if (L1P2 >= .98f && L1P2 <= 1.02f ||
                    //    L2P3 >= .98f && L2P3 <= 1.02f ||
                    //    L3P1 >= .98f && L3P1 <= 1.02f)
                    //    gfx.PutPixel(i, j, Colors::Yellow);
                    //else
                        gfx.PutPixel(i, j, color);
                }
            }
            // Draw Outline
            //float L1P2 = lambda1 + lambda2;
            //float L2P3 = lambda2 + lambda3;
            //float L3P1 = lambda3 + lambda1;
            //if (L1P2 >= .98f && L1P2 <= 1.02f ||
            //    L2P3 >= .98f && L2P3 <= 1.02f ||
            //    L3P1 >= .98f && L3P1 <= 1.02f)
            //{
            //    //Color color(Colors::MakeRGB(i, j, 255));
            //    gfx.PutPixel(i, j, Colors::Red);
            //}
        }
    }
}

// TODO: make this recycle same barycentric coordinates from triangle fill function
// TODO: that makes it go MUCH faster, but the issue is that the outlines wont appear
//       in that case because it finishes drawing the triangle before moving onto the
//       outline draw function, so the barycentric coordinates are invalid by that stage.

void Game::DrawTriangleScanLine(const triangle2D triangle, Color color)
{
    // Bounding box - use precalculated vars up top from fill algorithm
    //leftSide = min({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    //rightSide = max({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    //top = min({ triangle.v1.y,triangle.v2.y, triangle.v3.y });
    //bottom = max({ triangle.v1.y,triangle.v2.y, triangle.v3.y });

    // calculate slopes
    float slope21 = (triangle.v2.y - triangle.v1.y) / (triangle.v2.x - triangle.v1.x);
    float slope32 = (triangle.v3.y - triangle.v2.y) / (triangle.v3.x - triangle.v2.x);
    float slope13 = (triangle.v1.y - triangle.v3.y) / (triangle.v1.x - triangle.v3.x);

    //calculate y intercepts(b):
    // y = mx + b
    // b = y - mx
    float b21 = triangle.v1.y - triangle.v1.x*slope21;
    float b32 = triangle.v2.y - triangle.v2.x*slope32;
    float b13 = triangle.v3.y - triangle.v3.x*slope13;

    // line by line method
    for (int j = triangle.v1.y ; j < triangle.v2.y; j++)
    {
        for (int i = triangle.v1.x; i < triangle.v2.x; i++)
        {
            int y = (float)i*slope21 + b21 + 0.5f;

            if (i > 0 && i < gfx.ScreenWidth && y > 0 && y < gfx.ScreenHeight)
            {
                gfx.PutPixel(i, y, Colors::White);
            }
        }
    }

        for (int j = triangle.v2.y ; j < triangle.v3.y; j++)
    {
        for (int i = triangle.v2.x; i < triangle.v3.x; i++)
        {
            int y = (float)i*slope32 + b32 + 0.5f;

            if (i > 0 && i < gfx.ScreenWidth && y > 0 && y < gfx.ScreenHeight)
            {
                gfx.PutPixel(i, y, Colors::White);
            }
        }
    }

      for (int j = triangle.v3.y ; j < triangle.v1.y; j++)
    {
        for (int i = triangle.v3.x; i < triangle.v1.x; i++)
        {
            int y = (float)i*slope13 + b13 + 0.5f;

            if (i > 0 && i < gfx.ScreenWidth && y > 0 && y < gfx.ScreenHeight)
            {
                gfx.PutPixel(i, y, Colors::White);
            }
        }
    }
}

void Game::DrawTriOutline(const triangle2D triangle, Color color)
{
    // coordinate coefficients
    float lambda1 = 0.0f;
    float lambda2 = 0.0f;
    float lambda3 = 0.0f;

    //TODO: find a way to recycle this between outline and fill functions w/o clashing
    // Bounding box - use precalculated vars up top from fill algorithm
    leftSide = min({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    rightSide = max({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    top = min({ triangle.v1.y,triangle.v2.y, triangle.v3.y });
    bottom = max({ triangle.v1.y,triangle.v2.y, triangle.v3.y });

    for (int j = top; j < bottom; j++)
    {
        for (int i = leftSide; i < rightSide; i++)
        {
            if (i > 0 && i < gfx.ScreenWidth&&j>0 && j < gfx.ScreenHeight)
            {
                CalculateBaryCentricCoordinates({ (float)i,(float)j }, triangle.v1, triangle.v2, triangle.v3, lambda1, lambda2, lambda3);
                //validate Barycentric coordinates and draw if on lines
                if (lambda1 >= 0.0f && lambda2 >= 0.0f && lambda3 >= 0.0f)
                {
                    L1P2 = lambda1 + lambda2;
                    L2P3 = lambda2 + lambda3;
                    L3P1 = lambda3 + lambda1;

                    if (L1P2 >= .995f && L1P2 <= 1.005f ||
                        L2P3 >= .995f && L2P3 <= 1.005f ||
                        L3P1 >= .995f && L3P1 <= 1.005f)
                    {
                        gfx.PutPixel(i, j, color);
                    }
                }
            }
        }
    }
}

//TODO: MAKE SCANLINE DRAWING ALGORITHM for triangle
//TODO: Right now its drawing more triangles than it needs to;
// In the triangle list, it's drawing a separate triangle for every
// one in the list, duplicating vertices. Fix the triangle winding
// order to avoid this problem.

vector2 Game::Rotate2D(vector2& vec, float theta)
{
    // Compose rotation matrix
    float matrix[2][2] =
    {   {cos(theta), -sin(theta)},
        {sin(theta),  cos(theta)}
    };

    // multiply by point vector to get transformed point
    //return matrix*vec;
    return MatVecMult2D(matrix, vec);
}

vector3 Game::Rotate3D(vector3& vec, const float& theta, char axis)
{
    if (axis == 'Z')
        // Compose rotation matrix for rotation about Z axis
    {
        float matZ[3][3] =
        { { cos(theta), -sin(theta), 0.0f },
          { sin(theta), cos(theta),  0.0f },
          { 0.0f,       0.0f,        1.0f } };

        // multiply by point vector to get transformed point
        vec= MatVecMult3D(matZ, vec);
    }

    if (axis == 'X')
    {
        float matX[3][3] =
        { { 1.0f, 0.0f, 0.0f },
          { 0.0f, cos(theta), -sin(theta)},
          { 0.0f, sin(theta), cos(theta)} };

        // multiply by point vector to get transformed point
        vec= MatVecMult3D(matX, vec);
    }

    if (axis == 'Y')
    {
        // Compose rotation matrix for rotation about Z axis
        float matZ[3][3] =
        { { cos(theta), 0.0f, sin(theta)},
          { 0.0f, 1.0f, 0.0f },
          { -sin(theta), 0.0f, cos(theta)} };

        // multiply by point vector to get transformed point
        vec= MatVecMult3D(matZ, vec);
    }
    else
        return vec;
}

vector3 Game::Rotate3DALT(vector3& vec, const float& theta, char axis)
{
        float matX[3][3] =
        { { 1.0f, 0.0f, 0.0f },
        { 0.0f, cos(theta), -sin(theta)},
        { 0.0f, sin(theta), cos(theta)} };

        float matY[3][3] =
        { { cos(theta), 0.0f, sin(theta)},
        { 0.0f, 1.0f, 0.0f },
        { -sin(theta), 0.0f, cos(theta)} };

        float matZ[3][3] =
        { { cos(theta), -sin(theta), 0.0f },
        { sin(theta), cos(theta),  0.0f },
        { 0.0f,       0.0f,        1.0f } };

        float matResult[3][3];

        if (axis == 'X')
        {
        }
        if (axis == 'Y')
        {
        }
        if (axis == 'Z')
        {
        }

    Mat3Concat(matResult[0], matY, matX);
    Mat3Concat(matResult[0], matZ, matY);

    // Z Matrix now CONTAINS Concatenated rotation value
    vec = MatVecMult3D(matResult, vec);
    return vec;
}


vector2 Game::ProjectPt(vector3& vecIn, float distance)
{
    //// get reciprocal of distance
    float DR = distance;

    // column major
    float matP[4][4] =
    { { 1.f, 0.f, 0.f, 0.f },
      { 0.f, 1.f, 0.f, 0.f },
      { 0.f, 0.f, 1.f, 0.f },
      { 0.f, 0.f, DR, 0.f } };
    vector4 vecPadded = { vecIn.x, vecIn.y, vecIn.z, 1.f};
    vector4 projectedV = MatVecMult4D(matP, vecPadded);

    vector2 pVFinal;
    if (projectedV.w != 0.0f) // protect from divide by zero
    {
        pVFinal = { projectedV.x / projectedV.w,projectedV.y / projectedV.w/*, projectedV.z / projectedV.w */};
    }
    else
    {
        pVFinal = { projectedV.x / 0.001f,projectedV.y / 0.001f/*, projectedV.z / 0.001f */};
    }
    return{ pVFinal.x,pVFinal.y };

    //return{ vecIn.x/vecIn.z, vecIn.y/vecIn.z };
}
    // translate center of polygon to origin
vector<vector3> Game::Translate(vector<vector3>& vertices, vector3& worldPosition)
{
    for each (vector3 vert in vertices)
    {
        vert = vert + worldPosition;
    }
    return vertices;
}

void Game::ComposeFrame()
{
    //if( wnd.IsActive() )
    //{
    //	using namespace std::chrono;
    //	const auto start = high_resolution_clock::now();
    //	for( int y = m_y; y < m_y + 50; y++ )
    //	{
    //		for( int x = m_x; x < m_x + 50; x++ )
    //		{
    //			gfx.PutPixel( x,y,Colors::Red );
    //		}
    //	}
    //	const auto end = high_resolution_clock::now();
    //	log << L"Operation took "
    //		<< duration_cast<microseconds>(end - start).count()
    //		<< L" microseconds." << std::endl;
    //}
    //for (int i = 50; i < 100; i++)
    //{
    //    for (int j = 50; j < 100; j++)
    //    {
    //        gfx.PutPixel(i, j, Colors::Red);
    //    }
    //}
    // draw screen transition
    //for (int j = 0; j < gfx.ScreenHeight; j++)
    //{
    //    for (int i = 0; i < gfx.ScreenWidth; i++)
    //    {
    //            gfx.PutPixel(i, j, Colors::MakeRGB(i*0.2, j*0.4, 255));full
    //    }
    //}    
    //DrawTriangle(t1, Colors::Red);
    //triangle2D t1R1 = {
    //Rotate2D(t1.v1, 0.05),
    //Rotate2D(t1.v2, 0.05),
    //Rotate2D(t1.v3, 0.05)
    //};
    //DrawTriangle(t1R1, Colors::Blue);
    //triangle2D t1R2 = {
    //    Rotate2D(t1.v1, 0.15),
    //    Rotate2D(t1.v2, 0.15),
    //    Rotate2D(t1.v3, 0.15)
    //};
    //DrawTriangle(t1R2, Colors::Yellow);    
    //triangle2D t1R3 = 
    //{   Rotate2D(t1.v1, angDisp),
    //    Rotate2D(t1.v2, angDisp),
    //    Rotate2D(t1.v3, angDisp)
    //};
    //DrawTriangle(t1R3, Colors::White);

    Rotate3D(tetra1.v1, orientation, axis);
    Rotate3D(tetra1.v2, orientation, axis);
    Rotate3D(tetra1.v3, orientation, axis);
    Rotate3D(tetra1.v4, orientation, axis);

    vector<triangle3D> tRList3D = GetTriangleList(tetra1);
    vector<triangle2D> tRList2D;

    for each(triangle3D t in tRList3D)
    {
        tRList2D.push_back(
        { ProjectPt(t.v1,dstTet1),ProjectPt(t.v2,dstTet1),ProjectPt(t.v3,dstTet1)});
    }
    for (int i = 0; i < tRList2D.size(); i++)
    {
        tRList2D[i].v1 = tRList2D[i].v1 + transToScrn;
        tRList2D[i].v2 = tRList2D[i].v2 + transToScrn;
        tRList2D[i].v3 = tRList2D[i].v3 + transToScrn;
    }

    //for each(triangle2D t in tRList2D)
    //{
    //    DrawTriangle(t, Colors::Cyan);
    //}
        for each(triangle2D t in tRList2D)
    {
        //DrawTriOutline(t, Colors::Yellow);
    //DrawTriangleScanLine(t, Colors::Yellow);
            gfx.DrawLine(t.v1, t.v2, Colors::Yellow);
                        gfx.DrawLine(t.v2, t.v3, Colors::Red);
                                    gfx.DrawLine(t.v3, t.v1, Colors::Blue);
    }
}