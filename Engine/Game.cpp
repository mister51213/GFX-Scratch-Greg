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


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	m_gfx( wnd ),
	log( L"alog.txt" )
{}

void Game::Go() // infinite loop
{
	m_gfx.BeginFrame(); // clears the screen	
	UpdateModel(); // game logic only
    ComposeFrame(); // render instantaneous frame
	m_gfx.EndFrame(); // swap buffer - present frame
}

void Game::DoInput() 
{
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
    
   // change distance to screen
   if (wnd.kbd.KeyIsPressed('S'))
   {
       nearPln += 0.001f;
       farPln += 0.001f;

       //if (distToScreen == -0.001)
       //distToScreen += 0.0025f;
       //else
       distToScreen += 5.f;
   }      
       
       if (wnd.kbd.KeyIsPressed('W'))
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(distToScreen==0.001)
       //distToScreen -= 0.0025f;
       //else
       distToScreen -= 5.f;
   }

   // move polygon
   if (wnd.kbd.KeyIsPressed('U')) // move away from it
   {
       nearPln += 0.001f;
       farPln += 0.001f;

       //if (offset1.z == -0.001)
       //offset1.z += 0.0025f;
       //else
       offset1.z += 5.f;
   }      
       
   if (wnd.kbd.KeyIsPressed('T')) // move toward it
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       offset1.z -= 5.f;
   }

   if (wnd.kbd.KeyIsPressed('G')) // move it right
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       offset1.x -= 5.f;
   }

   if (wnd.kbd.KeyIsPressed('J')) // move it left
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       offset1.x += 5.f;
   }
      if (wnd.kbd.KeyIsPressed('Y')) // move it up
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       offset1.y += 5.f;
   }
         if (wnd.kbd.KeyIsPressed('H')) // move it down
   {
       nearPln -= 0.001f;
       farPln -= 0.001f;

       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       offset1.y -= 5.f;
   }

    if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
    {
        wnd.Kill();
    }
}

void Game::UpdateModel()
{
    if (++m_x + 50 > m_gfx.ScreenWidth)
    {
        m_x = 0;
        if ((m_y += 50) + 50 > m_gfx.ScreenHeight)
        {
            m_y = 0;
        }
    }

    DoInput();
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

    // ROTATE
    m_gfx.Rotate3D(tetra1.v1, orientation, axis);
    m_gfx.Rotate3D(tetra1.v2, orientation, axis);
    m_gfx.Rotate3D(tetra1.v3, orientation, axis);
    m_gfx.Rotate3D(tetra1.v4, orientation, axis);

    // OFFSET (TRANSLATE) the object relative to world space
    tetrahedron tetraTrans =
    {
    tetra1.v1 + offset1,
    tetra1.v2 + offset1,
    tetra1.v3 + offset1,
    tetra1.v4 + offset1 
    };

    // get triangle lists
    vector<triangle3D> tRList3D = m_gfx.GetTriangleList(tetraTrans);
    vector<triangle2D> tRList2D;

    for each(triangle3D t in tRList3D)
    {
        tRList2D.push_back(
        { 
            m_gfx.ProjectPt(t.v1,distToScreen, nearPln, farPln),
            m_gfx.ProjectPt(t.v2,distToScreen,nearPln, farPln),
            m_gfx.ProjectPt(t.v3,distToScreen,nearPln, farPln)});
    }
    for (int i = 0; i < tRList2D.size(); i++)
    {
        tRList2D[i].v1 = tRList2D[i].v1 + transToScrn;
        tRList2D[i].v2 = tRList2D[i].v2 + transToScrn;
        tRList2D[i].v3 = tRList2D[i].v3 + transToScrn;
    }

    for each(triangle2D t in tRList2D)
    {
            m_gfx.DrawLine(t.v1, t.v2, Colors::Yellow);
                        m_gfx.DrawLine(t.v2, t.v3, Colors::Red);
                                    m_gfx.DrawLine(t.v3, t.v1, Colors::Blue);
    }
}