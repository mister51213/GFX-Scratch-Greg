/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <fstream>
#include "VectorMath.h"
#include <vector>

using namespace std;

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
    vector<vector3> Translate(vector<vector3>& vertices, vector3 & worldPosition);
    void ComposeFrame();
	void UpdateModel();
    void CalculateBaryCentricCoordinates(vector2 desiredPt, vector2 a, vector2 b, vector2 c, float & lambda1, float & lambda2, float & lambda3);
    void CalculateScanLineCoordinates();
    vector4 ProjectionMatrix(const float matrix[4][4], const vector3 & vecIn);
    vector<triangle3D> GetTriangleList(tetrahedron polygon);
    void DrawTriangle(const triangle2D triangle, Color color);
    void DrawTriangleScanLine(const triangle2D triangle, Color color);
    void DrawTriOutline(const triangle2D triangle, Color color);
    vector2 Rotate2D(vector2& vec, float theta);
    vector3 Rotate3DZ(vector3 & vec, float theta);

    vector3 Rotate3DX(vector3 & vec, float theta);

    vector3 Rotate3DY(vector3 & vec, float theta);

    vector2 ProjectPt(vector3 & vec, float distance);

    /****************************************
            Operator Overloads   
    *****************************************/

	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	int m_x = 0;
	int m_y = 0;
	std::wofstream log;
	/********************************/
};
