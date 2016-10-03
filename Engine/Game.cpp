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

// GLOBAL VARS
// Triangle 1
triangle2D t1 = { { 10,10 }, { 15,200 }, { 300,80 } };
// Triangle 2
triangle2D t2 = { {100,100},{ 500,150}, { 400,400} };
// Triangle 3
triangle2D t3 = { {200,200},{ 150,20}, { 30,200} };

// for triangle animation
float angDisp = 0.0f;

// Initialize quaternion base rotation to 0
quaternion totalRotation = { 1.f, 0.f, 0.f, 0.f };

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	log( L"alog.txt" )
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
    angDisp += 0.01;

	if( ++m_x + 50 > gfx.ScreenWidth )
	{
		m_x = 0;
		if( (m_y += 50) + 50 > gfx.ScreenHeight )
		{
			m_y = 0;
		}
	}

	if( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
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
    float& lambda3
)
{
    lambda1 = ((b.y - c.y)*(desiredPt.x - c.x) + (c.x - b.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda2 = ((c.y - a.y)*(desiredPt.x - c.x) + (a.x - c.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda3 = 1.0f - lambda1 - lambda2;
}

// TODO' make nested coordinate space for rotating triangle relative to its center

void Game::DrawTriangle(const triangle2D triangle, Color color)
{
    // coordinate coefficients
    float lambda1 = 0.0f;
    float lambda2 = 0.0f;
    float lambda3 = 0.0f;

    for (int j = 0; j < gfx.ScreenHeight; j++)
    {
        for (int i = 0; i < gfx.ScreenWidth; i++)
        {
            CalculateBaryCentricCoordinates({ (float)i,(float)j }, triangle.v1, triangle.v2, triangle.v3, lambda1, lambda2, lambda3);
            //validate Barycentric coordinates and fill if inside triangle
            if (lambda1 >= 0.0f && lambda2 >= 0.0f && lambda3 >= 0.0f)
            {
                //Color color(Colors::MakeRGB(i, j, 255));
                gfx.PutPixel(i, j, color);
            }
        }
    }
}

//TODO' MAKE SCANLINE DRAWING ALGORITHM for triangle

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

    DrawTriangle(t1, Colors::Red);

    // TODO: later convert this system to radians
    triangle2D t1R1 = {
    Rotate2D(t1.v1, 0.05),
    Rotate2D(t1.v2, 0.05),
    Rotate2D(t1.v3, 0.05)
    };
    DrawTriangle(t1R1, Colors::Blue);

    triangle2D t1R2 = {
        Rotate2D(t1.v1, 0.15),
        Rotate2D(t1.v2, 0.15),
        Rotate2D(t1.v3, 0.15)
    };
    DrawTriangle(t1R2, Colors::Yellow);
    
    triangle2D t1R3 = 
    {   Rotate2D(t1.v1, angDisp),
        Rotate2D(t1.v2, angDisp),
        Rotate2D(t1.v3, angDisp)
    };
    DrawTriangle(t1R3, Colors::White);
}