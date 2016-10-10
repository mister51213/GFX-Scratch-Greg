#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "Colors.h"
#include <vector>
#include "VectorMath.h"

using namespace std;


// TODO: make these all actual member variables, NOT DANGLING IN GRAPHICS HEADER!!!
// TODO: Put them ALL in teh appropriate graphics classes! (vertex info should probably by
// TODO: in game class, correspodnign to game objects, and more drawing specific info
// TODO: like triangle lists can be in graphics class

// OR, make two separate versions of each 3d polygon, containing gameplay specific info
// and drawing specific info

//////////////////////
// EXPEDIENT VARIABLES //
//////////////////////

float scrnWHalf = Graphics::ScreenWidth * 0.5f;
float scrnHHalf = Graphics::ScreenHeight * 0.5f;
vector2 transToScrn = {scrnWHalf, scrnHHalf};


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

float nearPln = 50.f;
float farPln = 100.f;

/******************************************
***************TEST VALUES*************** for tetrahedron
******************************************/
float distToScreen = 500.0f; // CANNOT BE 0!

vector3 offset1 = { .0f,.0f,5.0f };

vector3 displacement = { 0.0f,0.0f,0.0f };

/******************************************
***************TEST VALUES*************** for tetrahedron
******************************************/

// TODO: add this offset between rotation and display
tetrahedron tetra1 = 
    {{0.f, 109.f, 0.f},
    { 0.f, -54.f, 115.f}, 
    { -100.f,-54.f,-58.f}, 
    { 100.f,-54.f,-58.f}};

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

class Graphics
{
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
    /**********************************
    ************* OVERHEAD ************
    ***********************************/
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();

    /**********************************
    ******* ESSENTIAL FUNCTIONS *******
    ***********************************/
    vector3 Rotate3D(vector3 & vec, const float& theta, char axis);
    vector2 ProjectPt(vector3 & vec, float distance, float near, float far);
    vector<triangle3D> GetTriangleList(tetrahedron polygon);

    vector<vector3> Translate(vector<vector3>& vertices, vector3& worldPosition);

    /**********************************
    ******* SPARE FUNCTIONS ***********
    ***********************************/
    void CalculateBaryCentricCoordinates(vector2 desiredPt, vector2 a, vector2 b, vector2 c, float & lambda1, float & lambda2, float & lambda3);
    void DrawTriangle(const triangle2D& triangle, Color color);
    void DrawTriOutline(const triangle2D triangle, Color color);
    vector2 Rotate2D(vector2& vec, float theta);

    /**********************************
    ******* NON-WORKING *********
    ***********************************/
    vector4 ProjectionMatrix(const float matrix[4][4], const vector3 & vecIn);
    void DrawTriangleScanLine(const triangle2D triangle, Color color);

	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
    void DrawLine(const vector2& start, const vector2& end, Color c);

	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr unsigned int ScreenWidth = 800u;
	static constexpr unsigned int ScreenHeight = 600u;
};