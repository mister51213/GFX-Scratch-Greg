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
    DoInput();
    ComposeFrame(); // render instantaneous frame
	m_gfx.EndFrame(); // swap buffer - present frame
}

void Game::DoInput() 
{
    // ACCELERATE AND DECELERATE ROTATION
    if (wnd.kbd.KeyIsPressed(VK_END))
    {
    m_poly.accelFactor = 0.0f;
    m_poly.orientation = 0.0f;
    }

    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    m_poly.accelFactor += 0.0001;
    if(wnd.kbd.KeyIsPressed(VK_CONTROL))
    m_poly.accelFactor -= 0.0001;

    // PITCH UP
    if (wnd.kbd.KeyIsPressed(VK_UP))
    {
    m_poly.orientation += m_poly.accelFactor;
    m_gfx.rotationAxis = 'X';
    }

    // PITCH DOWN
    else if (wnd.kbd.KeyIsPressed(VK_DOWN))
    {
    m_poly.orientation -= m_poly.accelFactor;
    m_gfx.rotationAxis = 'X';
    }

    // YAW LEFT
    if (wnd.kbd.KeyIsPressed(VK_LEFT))
    {
    m_poly.orientation -= m_poly.accelFactor;
    m_gfx.rotationAxis = 'Y';
    }

    // YAW RIGHT
    if (wnd.kbd.KeyIsPressed(VK_RIGHT))
    {
    m_poly.orientation +=m_poly. accelFactor;
    m_gfx.rotationAxis = 'Y';
    }

    // ROLL RIGHT
    if (wnd.kbd.KeyIsPressed('Q'))
    {
    m_poly.orientation += m_poly.accelFactor;
    m_gfx.rotationAxis = 'Z';
    }

    // ROLL LEFT
    if (wnd.kbd.KeyIsPressed('E'))
    {
    m_poly.orientation -= m_poly.accelFactor;
    m_gfx.rotationAxis = 'Z';
    }
    
   // change distance to screen
   if (wnd.kbd.KeyIsPressed('S'))
   {
       changeNearPln(m_gfx,0.001f);
       changeFarPln(m_gfx,0.001f);

       //if (distToScreen == -0.001)
       //distToScreen += 0.0025f;
       //else
       changeLensToScrn(m_gfx,5.f);
   }      
       
       if (wnd.kbd.KeyIsPressed('W'))
   {
       changeNearPln(m_gfx,-0.001f);
       changeFarPln(m_gfx,-0.001f);

       //if(distToScreen==0.001)
       //distToScreen -= 0.0025f;
       //else
       changeLensToScrn(m_gfx,-5.f);
   }

   // move polyhedron
   if (wnd.kbd.KeyIsPressed('U')) // move away from it
   {
       //if (offset1.z == -0.001)
       //offset1.z += 0.0025f;
       //else
       m_poly.offset1.z += 5.f;
   }      
       
   if (wnd.kbd.KeyIsPressed('T')) // move toward it
   {
       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       m_poly.offset1.z -= 5.f;
   }

   if (wnd.kbd.KeyIsPressed('G')) // move it right
   {
       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       m_poly.offset1.x -= 5.f;
   }

   if (wnd.kbd.KeyIsPressed('J')) // move it left
   {
       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       m_poly.offset1.x += 5.f;
   }
      if (wnd.kbd.KeyIsPressed('Y')) // move it up
   {
       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       m_poly.offset1.y += 5.f;
   }
         if (wnd.kbd.KeyIsPressed('H')) // move it down
   {
       //if(offset1.z==0.001)
       //offset1.z -= 0.0025f;
       //else
       m_poly.offset1.y -= 5.f;
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

    // ROTATE poly 1
    m_gfx.Rotate3D(m_poly.m_tetra.v1, m_poly.orientation, m_gfx.rotationAxis);
    m_gfx.Rotate3D(m_poly.m_tetra.v2, m_poly.orientation, m_gfx.rotationAxis);
    m_gfx.Rotate3D(m_poly.m_tetra.v3, m_poly.orientation, m_gfx.rotationAxis);
    m_gfx.Rotate3D(m_poly.m_tetra.v4, m_poly.orientation, m_gfx.rotationAxis);

    // ROTATE poly 2
    //m_gfx.Rotate3D(m_poly2.m_tetra.v1, m_poly2.orientation, m_gfx.rotationAxis);
    //m_gfx.Rotate3D(m_poly2.m_tetra.v2, m_poly2.orientation, m_gfx.rotationAxis);
    //m_gfx.Rotate3D(m_poly2.m_tetra.v3, m_poly2.orientation, m_gfx.rotationAxis);
    //m_gfx.Rotate3D(m_poly2.m_tetra.v4, m_poly2.orientation, m_gfx.rotationAxis);

    // OFFSET (TRANSLATE) the object relative to world space
    tetrahedron tetraTrans = {
    m_poly.m_tetra.v1 + m_poly.offset1,
    m_poly.m_tetra.v2 + m_poly.offset1,
    m_poly.m_tetra.v3 + m_poly.offset1,
    m_poly.m_tetra.v4 + m_poly.offset1
    };

    //tetrahedron tetraTrans2 = {
    //m_poly2.m_tetra.v1 + m_poly2.offset2,
    //m_poly2.m_tetra.v2 + m_poly2.offset2,
    //m_poly2.m_tetra.v3 + m_poly2.offset2,
    //m_poly2.m_tetra.v4 + m_poly2.offset2 
    //};

    // TODO: make just one big triangle draw list 
    // get triangle lists
    vector<triangle3D> tRList3D = m_gfx.GetTriangleList(tetraTrans);
    vector<triangle2D> tRList2D;

    //vector<triangle3D> tRList3D2 = m_gfx.GetTriangleList(tetraTrans2);
    //vector<triangle2D> tRList2D2;

    // PROJECT ALL POINTS
    for (triangle3D &t : tRList3D)
    {
        t.v1 = m_gfx.ProjectPt(t.v1);
        t.v2 = m_gfx.ProjectPt(t.v2);
        t.v3 = m_gfx.ProjectPt(t.v3);
    }

    for each(triangle3D t in tRList3D)
    {
        tRList2D.push_back({
            {t.v1.x, t.v1.y},
            {t.v2.x, t.v2.y},
            {t.v3.x, t.v3.y} });
    }

        //for each(triangle3D t in tRList3D2)
        //{
        //    tRList2D.push_back({ 
        //        m_gfx.ProjectPt(t.v1),
        //        m_gfx.ProjectPt(t.v2),
        //        m_gfx.ProjectPt(t.v3)});
        //}

        for (int i = 0; i < tRList2D.size(); i++)
        {
            tRList2D[i].v1 = m_gfx.TransToScreen(tRList2D[i].v1);
            tRList2D[i].v2 = m_gfx.TransToScreen(tRList2D[i].v2);
            tRList2D[i].v3 = m_gfx.TransToScreen(tRList2D[i].v3);
        }
        //for (int i = 0; i < tRList2D2.size(); i++)
        //{
        //    tRList2D2[i].v1 = m_gfx.TransToScreen(tRList2D2[i].v1);
        //    tRList2D2[i].v2 = m_gfx.TransToScreen(tRList2D2[i].v2);
        //    tRList2D2[i].v3 = m_gfx.TransToScreen(tRList2D2[i].v3);
        //}

        for each(triangle2D t in tRList2D) {
            m_gfx.DrawLine(t.v1, t.v2, Colors::Yellow);
            m_gfx.DrawLine(t.v2, t.v3, Colors::Red);
            m_gfx.DrawLine(t.v3, t.v1, Colors::Blue);
        }
        /*for each(triangle2D t in tRList2D2){
                m_gfx.DrawLine(t.v1, t.v2, Colors::White);
                            m_gfx.DrawLine(t.v2, t.v3, Colors::Gray);
                                        m_gfx.DrawLine(t.v3, t.v1, Colors::LightGray);}*/
    
}