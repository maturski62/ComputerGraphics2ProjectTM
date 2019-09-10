// GraphicsProject.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GraphicsProject.h"
#include "Colors.h"
#include "DDSTextureLoader.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

//Shaders
#include "VertexMeshShader.csh"
#include "PixelMeshShader.csh"

//Mesh Files
#include "Assets/StoneHenge.h"
#include "Assets/barrels.h"

//For Init
ID3D11Device* myDevice;
IDXGISwapChain* mySwapChain;
ID3D11DeviceContext* myDeviceContext;

//For drawing
ID3D11RenderTargetView* myRenderTargetView;
D3D11_VIEWPORT myViewPort;
float aspectRatio = 1;
//Dynamic User Variables
float nearPlane = 0.1f;
float farPlane = 50.0f;

struct MyVertex
{
	XMFLOAT4 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
};

//Camera
float cameraX = 0.0f;
float cameraY = 5.0f;
float cameraZ = -20.0f;

//Models
MyVertex* stoneHenge = new MyVertex[ARRAYSIZE(StoneHenge_data)];
unsigned int stoneHengeIndices[ARRAYSIZE(StoneHenge_indicies)];
MyVertex* barrels = new MyVertex[ARRAYSIZE(barrels_data)];
unsigned int barrelsIndices[ARRAYSIZE(barrels_indicies)];

//Wave Variables
XMFLOAT4 waveTime;
XMVECTOR waveSpeed;

//Light
XMVECTOR lightDir;
XMVECTOR lightColor;
XMVECTOR pointLightPos;
XMVECTOR pointLightColor;
XMVECTOR ambient;

//Shader Variables
ID3D11Buffer* constantBuffer;
ID3D11Buffer* lightConstantBuffer;
ID3D11SamplerState* samplerLinear;
ID3D11ShaderResourceView* stonehengeTex;
ID3D11ShaderResourceView* barrelsTex;

//Mesh data
//Stonehenge
ID3D11Buffer* stonehengeVertexBuffer;
ID3D11Buffer* stonehengeIndicesBuffer;
//Barrels
ID3D11Buffer* barrelsVertexBuffer;
ID3D11Buffer* barrelsIndicesBuffer;
ID3D11InputLayout* vertexMeshLayout;
ID3D11VertexShader* vertexMeshShader;
ID3D11PixelShader* pixelMeshShader;

//ZBuffer for Depth
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

//Constant Buffer
struct WVPMatrix
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMFLOAT4 waveTime;
	XMFLOAT4 waveSpeed;
}myMatrices;

struct Lights
{
	XMFLOAT4 vLightDir;
	XMFLOAT4 vLightColor;
	XMFLOAT4 vPointLightPos;
	XMFLOAT4 vPointLightColor;
	XMFLOAT4 vAmbient;
}myLights;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void UploadToVideoCard();
void Render();
void ReleaseInterfaces();
void CheckUserInput();
void CreateStoneHenge();
void CreateBarrels();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPHICSPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICSPROJECT));

	MSG msg;

	// Main message loop:
	while (true)//GetMessage(&msg, nullptr, 0, 0))
	{
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		//Rendering
		Render();
	}

	//Release all our DirectX11 interfaces
	ReleaseInterfaces();

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICSPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHICSPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	RECT myWinRect;
	GetClientRect(hWnd, &myWinRect);

	//Attach d3d11 to out window
	D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC)); //assigns all elements in the swap to null
	swap.BufferCount = 1;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = myWinRect.right - myWinRect.left;
	swap.BufferDesc.Height = myWinRect.bottom - myWinRect.top;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.OutputWindow = hWnd;
	swap.SampleDesc.Count = 1;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.Windowed = true;

	aspectRatio = static_cast<float>(swap.BufferDesc.Width) / static_cast<float>(swap.BufferDesc.Height);

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
		&dx11, 1, D3D11_SDK_VERSION, &swap, &mySwapChain, &myDevice, 0, &myDeviceContext);

	ID3D11Resource* backbuffer;
	hr = mySwapChain->GetBuffer(0, __uuidof(backbuffer), (void**)& backbuffer);

	if (backbuffer)
	{
		hr = myDevice->CreateRenderTargetView(backbuffer, NULL, &myRenderTargetView);
	}

	backbuffer->Release();

	//Assigning view port attributes
	myViewPort.Width = (float)swap.BufferDesc.Width;
	myViewPort.Height = (float)swap.BufferDesc.Height;
	myViewPort.TopLeftX = myViewPort.TopLeftY = 0.0f;
	myViewPort.MinDepth = 0.0f;
	myViewPort.MaxDepth = 1.0f;

	//Load the mesh on the graphics card
	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));
	//Create constant buffer
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVPMatrix);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = myDevice->CreateBuffer(&bDesc, nullptr, &constantBuffer);

	//Create constant buffer
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(Lights);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = myDevice->CreateBuffer(&bDesc, nullptr, &lightConstantBuffer);

	//Load Mesh Data
	CreateStoneHenge();
	CreateBarrels();

	//Creating Texture
	if (myDevice)
	{
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/stoneHenge.dds", nullptr, &stonehengeTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/barrelTexture.dds", nullptr, &barrelsTex);
	}

	//Time for the wave
	waveTime = { 0.0f, 0.0f, 0.0f, 0.0f };

	//Load Stonehenge
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * ARRAYSIZE(StoneHenge_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = stoneHenge;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &stonehengeVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * ARRAYSIZE(stoneHengeIndices);
	subData.pSysMem = stoneHengeIndices;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &stonehengeIndicesBuffer);

	//Load Barrels
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * ARRAYSIZE(barrels_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = barrels;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &barrelsVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * ARRAYSIZE(barrelsIndices);
	subData.pSysMem = barrelsIndices;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &barrelsIndicesBuffer);

	//Load new mesh shader
	hr = myDevice->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vertexMeshShader);
	hr = myDevice->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pixelMeshShader);

	//Make matching input layout for mesh vertex
	//Describe the vertex to D3D11
	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = myDevice->CreateInputLayout(meshInputDesc, ARRAYSIZE(meshInputDesc), VertexMeshShader, sizeof(VertexMeshShader), &vertexMeshLayout);

	//Create Z Buffer and View
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 1;
	hr = myDevice->CreateTexture2D(&zDesc, nullptr, &zBuffer);

	if (zBuffer)
	{
		hr = myDevice->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);
	}

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = myDevice->CreateSamplerState(&sampDesc, &samplerLinear);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	//case WM_PAINT:
	//    {
	//        PAINTSTRUCT ps;
	//        HDC hdc = BeginPaint(hWnd, &ps);
	//        // TODO: Add any drawing code that uses hdc here...
	//        EndPaint(hWnd, &ps);
	//    }
	//    break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Render()
{
	//Clear screen to one color
	float color[] = CYAN;
	myDeviceContext->ClearRenderTargetView(myRenderTargetView, color);
	myDeviceContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

	//Check for user input
	CheckUserInput();

	//world
	XMMATRIX temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, temp);
	//view
	temp = XMMatrixLookAtLH({ cameraX, cameraY, cameraZ }, { cameraX  , cameraY , cameraZ + 10.0f  }, { 0.0f, 1.0f, 0.0f });
	XMStoreFloat4x4(&myMatrices.viewMatrix, temp);
	//projection
	temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, nearPlane, farPlane);
	XMStoreFloat4x4(&myMatrices.projMatrix, temp);
	//time
	waveTime = { (waveTime.x + 0.005f), 0.0f, 0.0f, 0.0f };
	XMVECTOR tempTime = { waveTime.x, waveTime.y, waveTime.z, waveTime.w };
	XMStoreFloat4(&myMatrices.waveTime, tempTime);
	//speed
	waveSpeed = { 5.0f, 0.0f, 0.0f, 0.0f };
	XMStoreFloat4(&myMatrices.waveSpeed, waveSpeed);

	//Setup the pipeline
	//Output merger
	ID3D11RenderTargetView* tempRenderTargetView[] = { myRenderTargetView }; //To remove object, set it to nullptr in the array
	myDeviceContext->OMSetRenderTargets(1, tempRenderTargetView, zBufferView);
	//Rasterizer
	myDeviceContext->RSSetViewports(1, &myViewPort);
	//Upload those matricies to the video card
	UploadToVideoCard();

	//Immediate context 
	//Get a more complex pre-made mesh (FBX, OBJ, custom header) #done
	//load the mesh on the card(vertex buffer, index buffer) #done
	//make sure shaders can process the mesh #done??
	//place mesh somewhere else in the envirnoment
	//setup pipeline
	UINT meshStrides[] = { sizeof(MyVertex) };
	UINT meshOffsets[] = { 0 };
	ID3D11Buffer* tempMeshVertexBuffer[] = { stonehengeVertexBuffer };
	myDeviceContext->IASetVertexBuffers(0, 1, tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(stonehengeIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	// stone henge texture upload
	myDeviceContext->PSSetSamplers(0, 1, &samplerLinear);
	myDeviceContext->PSSetShaderResources(0, 1, &stonehengeTex);
	// set shaders
	myDeviceContext->VSSetShader(vertexMeshShader, nullptr, 0);
	myDeviceContext->PSSetShader(pixelMeshShader, nullptr, 0);
	myDeviceContext->IASetInputLayout(vertexMeshLayout);
	myDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Modify world matrix before drawing next mesh
	XMMATRIX stoneHedge = XMMatrixIdentity();
	XMStoreFloat4x4(&myMatrices.worldMatrix, stoneHedge);
	//Set lighting variables
	//Directional Light
	lightDir = { -0.577f, 0.577f, -0.577f, 1.0f };
	XMStoreFloat4(&myLights.vLightDir, lightDir);
	lightColor = { 0.35f, 0.35f, 0.5f, 1.0f };
	XMStoreFloat4(&myLights.vLightColor, lightColor);
	//Point Light
	pointLightPos = { 5.0f, 5.0f, -2.0f, 1.0f };
	static float pointLightAngle = 0.0f; pointLightAngle += 0.001f;
	XMMATRIX lightRotation = XMMatrixRotationY(pointLightAngle);
	pointLightPos = XMVector4Transform(pointLightPos, lightRotation);
	XMStoreFloat4(&myLights.vPointLightPos, pointLightPos);
	pointLightColor = { 1.0f, 0.0f, 0.0f, 0.0f };
	XMStoreFloat4(&myLights.vPointLightColor, pointLightColor);
	ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&myLights.vAmbient, ambient);
	//Upload those matricies to the video card
	UploadToVideoCard();

	//Draw Stonehenge
	myDeviceContext->DrawIndexed(ARRAYSIZE(StoneHenge_indicies), 0, 0);

	ID3D11Buffer* tempMeshVertexBuffer2[] = { barrelsVertexBuffer };
	myDeviceContext->IASetVertexBuffers(0, 1, tempMeshVertexBuffer2, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(barrelsIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &barrelsTex);
	XMMATRIX barrelMatrix = XMMatrixTranslation(0.0f, 0.0f, -15.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, barrelMatrix);
	UploadToVideoCard();

	//Draw Barrels
	myDeviceContext->DrawIndexed(ARRAYSIZE(barrels_indicies), 0, 0);

	mySwapChain->Present(1, 0);
}

void ReleaseInterfaces()
{
	delete[] stoneHenge;
	barrelsTex->Release();
	lightConstantBuffer->Release();
	samplerLinear->Release();
	stonehengeTex->Release();
	zBuffer->Release();
	zBufferView->Release();
	vertexMeshLayout->Release();
	vertexMeshShader->Release();
	pixelMeshShader->Release();
	myRenderTargetView->Release();
	stonehengeVertexBuffer->Release();
	stonehengeIndicesBuffer->Release();
	barrelsVertexBuffer->Release();
	barrelsIndicesBuffer->Release();
	constantBuffer->Release();
	myDeviceContext->Release();
	mySwapChain->Release();
	myDevice->Release();
}

void UploadToVideoCard()
{
	//Create and update a constant buffer (more variables from c++ to shaders) 
	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	HRESULT hr = myDeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVPMatrix*)(gpuBuffer.pData)) = myMatrices;
	myDeviceContext->Unmap(constantBuffer, 0);

	hr = myDeviceContext->Map(lightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((Lights*)(gpuBuffer.pData)) = myLights;
	myDeviceContext->Unmap(lightConstantBuffer, 0);
	//Apply matrix math in Vertex Shader #done
	//Connect constant buffer to the pipeline #done
	//By default HLSL matricies are column major
	ID3D11Buffer* constants[] = { constantBuffer, lightConstantBuffer };
	myDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constants), constants);
	myDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constants), constants);
}

void CreateStoneHenge() 
{
	for (size_t i = 0; i < ARRAYSIZE(StoneHenge_data); i++)
	{
		//Set POSITION
		stoneHenge[i].position.x = StoneHenge_data[i].pos[0];
		stoneHenge[i].position.y = StoneHenge_data[i].pos[1];
		stoneHenge[i].position.z = StoneHenge_data[i].pos[2];
		stoneHenge[i].position.w = 1.0f;
		//Set UV
		stoneHenge[i].texture.x = StoneHenge_data[i].uvw[0];
		stoneHenge[i].texture.y = StoneHenge_data[i].uvw[1];
		//SET NORMAL
		stoneHenge[i].normal.x = StoneHenge_data[i].nrm[0];
		stoneHenge[i].normal.y = StoneHenge_data[i].nrm[1];
		stoneHenge[i].normal.z = StoneHenge_data[i].nrm[2];
	}

	for (size_t i = 0; i < ARRAYSIZE(StoneHenge_indicies); i++)
	{
		stoneHengeIndices[i] = StoneHenge_indicies[i];
	}
}

void CreateBarrels()
{
	for (size_t i = 0; i < ARRAYSIZE(barrels_data); i++)
	{
		//Set POSITION
		barrels[i].position.x = barrels_data[i].pos[0];
		barrels[i].position.y = barrels_data[i].pos[1];
		barrels[i].position.z = barrels_data[i].pos[2];
		barrels[i].position.w = 1.0f;
		//Set UV
		barrels[i].texture.x = barrels_data[i].uvw[0];
		barrels[i].texture.y = barrels_data[i].uvw[1];
		//SET NORMAL
		barrels[i].normal.x = barrels_data[i].nrm[0];
		barrels[i].normal.y = barrels_data[i].nrm[1];
		barrels[i].normal.z = barrels_data[i].nrm[2];
	}

	for (size_t i = 0; i < ARRAYSIZE(barrels_indicies); i++)
	{
		barrelsIndices[i] = barrels_indicies[i];
	}
}

void CheckUserInput()
{
	//Adjust the Near or Far Plane
	//Increase Far Plane
	if (GetAsyncKeyState('O'))
	{
		if (farPlane < 1000.0f) 
		{
			farPlane += 1.0f;
		}
	}
	//Decrease Far Plane
	if (GetAsyncKeyState('L'))
	{
		if (farPlane > nearPlane + 0.1f)
		{
			farPlane -= 1.0f;
		}
	}

	//Increase Near Plane
	if (GetAsyncKeyState('I'))
	{
		if (nearPlane < farPlane - 0.5f )
		{
			nearPlane += 1.0f;
		}
	}
	//Decrease Near Plane
	if (GetAsyncKeyState('K'))
	{
		if (nearPlane > 0.5f)
		{
			nearPlane -= 1.0f;
		}
	}

	//Adjust the Camera
	if (GetAsyncKeyState('W'))
	{
		cameraZ += 0.5f;
	}
	else if(GetAsyncKeyState('S'))
	{
		cameraZ -= 0.5f;
	}
	if (GetAsyncKeyState('A'))
	{
		cameraX -= 0.5f;
	}
	else if(GetAsyncKeyState('D'))
	{
		cameraX += 0.5f;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		cameraY += 0.5f;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		cameraY -= 0.5f;
	}
}
