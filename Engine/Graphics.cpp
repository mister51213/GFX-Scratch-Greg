#include "MainWindow.h"
#include "Graphics.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL	featureLevelsRequested = D3D_FEATURE_LEVEL_9_1;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	featureLevelsSupported;
	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		&featureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		&featureLevelsSupported,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

    /**********************************
    ******* ESSENTIAL FUNCTIONS *******
    ***********************************/

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::DrawLine(const vector2& start, const vector2& end, Color c)
{

    int distanceX = end.x - start.x;
    int distanceY = end.y - start.y;

        int y2;
        int y1;
        int x2;
        int x1;

    // for shallow slope ~ 45 degrees
    if (abs(distanceY) < abs(distanceX))
    {
        if (start.x > end.x) // swap values to swap looping order
        {
            y2 = start.y;
            y1 = end.y;
            x2 = start.x;
            x1 = end.x;
        }
        else
        {
            y2 = end.y;
            y1 = start.y;
            x2 = end.x;
            x1 = start.x;
        }
        // calculate slope
        float slope = (float)distanceY / (float)distanceX;
        //calculate y intercept(b)
        float b = y1 - x1 * slope;

        for (int x = x1; x < x2; x++)
        {
            int y = (float)x*slope + b + 0.5f;
            // clip drawing to screen
            if (x > 0 && x < ScreenWidth && y > 0 && y < ScreenHeight)
            {
                PutPixel(x, y, c);
                PutPixel(x++, y, c);
            }
        }
    }    
    else // when y > x; for steep slope ~ 45 degrees
    {
        if (start.y > end.y) // swap values to swap looping order
        {
            y2 = start.y;
            y1 = end.y;
            x2 = start.x;
            x1 = end.x;
        }
        else
        {
            y2 = end.y;
            y1 = start.y;
            x2 = end.x;
            x1 = start.x;
        }

        float slope = (float)distanceX / (float)distanceY;
        float b = x1 - y1 * slope;

        for (int y = y1; y <= y2; y++)
        {
            int x = slope * y + b + 0.5f;     
            if (x > 0 && x < ScreenWidth && y > 0 && y < ScreenHeight)
            {
                PutPixel(x, y, c);
                PutPixel(x++, y, c);
            }
        }
    }
}


vector3 Graphics::Rotate3D(vector3& vec, const float& theta, char axis)
{
    if (axis == 'Z')
        // Compose rotation matrix for rotation about Z axis
    {
        float matZ[3][3] =
        { { cos(theta), sin(theta), 0.0f },
          { -sin(theta), cos(theta),  0.0f },
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

vector2 Graphics::ProjectPt(vector3& vecIn, float distScreen, float nearP, float farP)
{
    ////// get reciprocal of distance
    //float DR = distance;
    //// column major
    //float matP[4][4] =
    //{ { 1.f, 0.f, 0.f, 0.f },
    //  { 0.f, 1.f, 0.f, 0.f },
    //  { 0.f, 0.f, 1.f, 0.f },
    //  { 0.f, 0.f, DR, 0.f } };
    //vector4 vecPadded = { vecIn.x, vecIn.y, vecIn.z, 1.f};
    //vector4 projectedV = MatVecMult4D(matP, vecPadded);
    //vector2 pVFinal;
    //if (projectedV.w != 0.0f) // protect from divide by zero
    //{
    //    pVFinal = { projectedV.x / projectedV.w,projectedV.y / projectedV.w/*, projectedV.z / projectedV.w */};
    //}
    //else
    //{
    //    pVFinal = { projectedV.x / 0.001f,projectedV.y / 0.001f/*, projectedV.z / 0.001f */};
    //}
    //return{ pVFinal.x,pVFinal.y };

    /******************ALTERNATIVE WAY*************************
    http://www.codinglabs.net/article_world_view_projection_matrix.aspx
    ***********************************************************/        
//    float diff = farP - nearP;
//    float sum = farP + nearP;
//
//        float matP[4][4] =
//    { { 1.f/10.0f, 0.f, 0.f, 0.f },
//      { 0.f, 1.f/10.0f, 0.f, 0.f },
//      { 0.f, 0.f, -(2.f/diff), -(sum/diff) },
//      { 0.f, 0.f, 0.0f, 1.f } };
//
//vector4 vecPadded = { vecIn.x, vecIn.y, vecIn.z, 1.f};
//vector4 projectedV = MatVecMult4D(matP, vecPadded);
//vector2 pVFinal = { projectedV.x / projectedV.z, projectedV.y / projectedV.z/*, projectedV.z / 0.001f */};
//
//return pVFinal;
    // ORTHAGONAL PROJECTION METHOD
    // TODO: separate this equation into steps, 2 separate equations
    // make sure that the PROJECTION side (distance to Screen)
    // and physical vector side (offset) are being calculated independently

    return
    {
        (vecIn.x/vecIn.z)*distScreen,
        (vecIn.y/vecIn.z)*distScreen};
}

vector<triangle3D> Graphics::GetTriangleList(tetrahedron polygon)
{
    vector<triangle3D> triList;
    triList.push_back({ polygon.v1, polygon.v2, polygon.v3 });
        triList.push_back({ polygon.v1, polygon.v4, polygon.v3 });
            triList.push_back({ polygon.v1, polygon.v2, polygon.v4 });
                 triList.push_back({ polygon.v4, polygon.v2, polygon.v3 });

    return triList;
}

vector<vector3> Graphics::Translate(vector<vector3>& vertices, vector3& worldPosition)
{
    for each (vector3 vert in vertices)
    {
        vert = vert + worldPosition;
    }
    return vertices;
}

    /**********************************
    ******* SPARE FUNCTIONS ***********
    ***********************************/

// Lambda1, 2, and 3 are the coefficients reprsenting how much of each points a,b,and c, goes into the point in question
void Graphics::CalculateBaryCentricCoordinates( vector2 desiredPt, vector2 a, vector2 b, vector2 c, float& lambda1, float& lambda2, float& lambda3)
{
    lambda1 = ((b.y - c.y)*(desiredPt.x - c.x) + (c.x - b.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda2 = ((c.y - a.y)*(desiredPt.x - c.x) + (a.x - c.x)*(desiredPt.y - c.y))/
        ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));

    lambda3 = 1.0f - lambda1 - lambda2;
}

void Graphics::DrawTriangle(const triangle2D& triangle, Color color)
{
    // Bounding box - uses temporary global variables up top
    float left = min({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    float right = max({ triangle.v1.x,triangle.v2.x, triangle.v3.x });
    float top = min({ triangle.v1.y,triangle.v2.y, triangle.v3.y });
    float bottom = max({ triangle.v1.y,triangle.v2.y, triangle.v3.y });

    vector<lambdas> vertexLambdas;

    for (int j = top; j <= bottom; j++)
    {
        for (int i = left; i <= right; i++)
        {
            if (i > 0 && i < ScreenWidth&&j>0 && j < ScreenHeight)
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
                        PutPixel(i, j, color);
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

void Graphics::DrawTriOutline(const triangle2D triangle, Color color)
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
            if (i > 0 && i < ScreenWidth&&j>0 && j < ScreenHeight)
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
                        PutPixel(i, j, color);
                    }
                }
            }
        }
    }
}

vector2 Graphics::Rotate2D(vector2& vec, float theta)
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

    /**********************************
    ******* NON-WORKING *********
    ***********************************/
//vector3 Rendering::Rotate3DALT(vector3& vec, const float& theta, char axis)
//{
//        float matX[3][3] =
//        { { 1.0f, 0.0f, 0.0f },
//        { 0.0f, cos(theta), -sin(theta)},
//        { 0.0f, sin(theta), cos(theta)} };
//
//        float matY[3][3] =
//        { { cos(theta), 0.0f, sin(theta)},
//        { 0.0f, 1.0f, 0.0f },
//        { -sin(theta), 0.0f, cos(theta)} };
//
//        float matZ[3][3] =
//        { { cos(theta), -sin(theta), 0.0f },
//        { sin(theta), cos(theta),  0.0f },
//        { 0.0f,       0.0f,        1.0f } };
//
//        float matResult[3][3];
//
//        if (axis == 'X')
//        {
//        }
//        if (axis == 'Y')
//        {
//        }
//        if (axis == 'Z')
//        {
//        }
//
//    Mat3Concat(matResult[0], matY, matX);
//    Mat3Concat(matResult[0], matZ, matY);
//
//    // Z Matrix now CONTAINS Concatenated rotation value
//    vec = MatVecMult3D(matResult, vec);
//    return vec;
//}

vector4 Graphics::ProjectionMatrix(const float matrix[4][4], const vector3& vecIn)
{
    float halfHeight = ScreenHeight*0.5f;
    float halfWidth = ScreenWidth*0.5f;
    vector4 vec4 = { vecIn.x,vecIn.y,vecIn.z,1.0f };
    return vec4;
}

void Graphics::DrawTriangleScanLine(const triangle2D triangle, Color color)
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

            if (i > 0 && i < ScreenWidth && y > 0 && y < ScreenHeight)
            {
                PutPixel(i, y, Colors::White);
            }
        }
    }

        for (int j = triangle.v2.y ; j < triangle.v3.y; j++)
    {
        for (int i = triangle.v2.x; i < triangle.v3.x; i++)
        {
            int y = (float)i*slope32 + b32 + 0.5f;

            if (i > 0 && i < ScreenWidth && y > 0 && y < ScreenHeight)
            {
                PutPixel(i, y, Colors::White);
            }
        }
    }

      for (int j = triangle.v3.y ; j < triangle.v1.y; j++)
    {
        for (int i = triangle.v3.x; i < triangle.v1.x; i++)
        {
            int y = (float)i*slope13 + b13 + 0.5f;

            if (i > 0 && i < ScreenWidth && y > 0 && y < ScreenHeight)
            {
                PutPixel(i, y, Colors::White);
            }
        }
    }
}


// TODO: fix winding order and redundancies
// TODO: make this recycle same barycentric coordinates from triangle fill function
// TODO: that makes it go MUCH faster, but the issue is that the outlines wont appear
//       in that case because it finishes drawing the triangle before moving onto the
//       outline draw function, so the barycentric coordinates are invalid by that stage.

//TODO: MAKE SCANLINE DRAWING ALGORITHM for triangle
//TODO: Right now its drawing more triangles than it needs to;
// In the triangle list, it's drawing a separate triangle for every
// one in the list, duplicating vertices. Fix the triangle winding
// order to avoid this problem.
