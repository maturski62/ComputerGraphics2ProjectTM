// GraphicsProject.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GraphicsProject.h"
#include "Colors.h"
#include "DDSTextureLoader.h"
#include "CameraMovement.h"
#include <vector>
#include <fstream>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

using namespace std;

//Shaders
#include "VertexMeshShader.csh"
#include "WaterShader.csh"
#include "PixelMeshShader.csh"
#include "WaterPixelShader.csh"

//For Init
ID3D11Device* myDevice;
IDXGISwapChain* mySwapChain;
ID3D11DeviceContext* myDeviceContext;

//For drawing
ID3D11RenderTargetView* myRenderTargetView;
D3D11_VIEWPORT myViewPort;
float aspectRatio = 1;

struct MyVertex
{
	XMFLOAT4 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
};
struct MyVec3
{
	float xyz[3];
};

struct SimpleOBJ
{
	vector<MyVertex> vertexList;
	vector<unsigned int> indicesList;
};

struct SimpleMesh
{
	vector<MyVertex> vertexList;
	vector<int> indicesList;
};

//Models
MyVertex* waterPlane = new MyVertex[725];
unsigned int numWaterVertices = 0;
MyVertex* sandPlane = new MyVertex[725];
unsigned int numSandVertices = 0;
MyVertex* skybox = new MyVertex[36];
unsigned int skyboxIndices[36];
unsigned int numSkyboxVertices = 0;
//OBJ Models
MyVertex* islandArray = new MyVertex[13476];
unsigned int* islandIndicesArray = new unsigned int[13476];
unsigned int numIslandVertices = 0;
unsigned int numIslandIndices = 0;
MyVertex* pirateArray = new MyVertex[5562];
unsigned int* pirateIndicesArray = new unsigned int[5562];
unsigned int numPirateVertices = 0;
unsigned int numPirateIndices = 0;
MyVertex* longDockArray = new MyVertex[11136];
unsigned int* longDockIndicesArray = new unsigned int[11136];
unsigned int numLongDockVertices = 0;
unsigned int numLongDockIndices = 0;
MyVertex* boatArray = new MyVertex[11136];
unsigned int* boatIndicesArray = new unsigned int[11136];
unsigned int numBoatVertices = 0;
unsigned int numBoatIndices = 0;

//Screen Varibales
float screenWidth;
float screenHeight;

//Wave Variables
XMFLOAT4 waveTime;
XMVECTOR waveSpeed;

//Light
XMVECTOR lightDir;
XMVECTOR lightColor;
XMVECTOR pointLightPos;
XMVECTOR pointLightColor;
XMVECTOR ambient;
XMVECTOR spotLightPos;
XMVECTOR spotLightDir;
XMVECTOR spotLightColor;
XMVECTOR spotLightConeRatio;

//Shader Variables
ID3D11Buffer* constantBuffer;
ID3D11Buffer* lightConstantBuffer;
ID3D11SamplerState* samplerLinear;
ID3D11ShaderResourceView* waterTex;
ID3D11ShaderResourceView* sandTex;
ID3D11ShaderResourceView* skyboxTex;
ID3D11ShaderResourceView* islandTex;
ID3D11ShaderResourceView* pirateTex;
ID3D11ShaderResourceView* dockTex;
ID3D11ShaderResourceView* boatTex;

//Mesh data
//Water
ID3D11Buffer* waterVertexBuffer;
//Sand
ID3D11Buffer* sandVertexBuffer;
//Skybox
ID3D11Buffer* skyBoxVertexBuffer;
ID3D11Buffer* skyBoxIndicesBuffer;
//Island
ID3D11Buffer* islandVertexBuffer;
ID3D11Buffer* islandIndicesBuffer;
//Pirate
ID3D11Buffer* pirateVertexBuffer;
ID3D11Buffer* pirateIndicesBuffer;
//Long Dock
ID3D11Buffer* longDockVertexBuffer;
ID3D11Buffer* longDockIndicesBuffer;
//Small Boat
ID3D11Buffer* boatVertexBuffer;
ID3D11Buffer* boatIndicesBuffer;
ID3D11InputLayout* vertexMeshLayout;
ID3D11VertexShader* vertexMeshShader;
ID3D11PixelShader* pixelMeshShader;
ID3D11VertexShader* waterShader;
ID3D11PixelShader* waterPixelShader;

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
	XMFLOAT4 drawingBoat;
}myMatrices;

struct Lights
{
	XMFLOAT4 vLightDir;
	XMFLOAT4 vLightColor;
	XMFLOAT4 vPointLightPos;
	XMFLOAT4 vPointLightColor;
	XMFLOAT4 vAmbient;
	XMFLOAT4 vSpotLightPos;
	XMFLOAT4 vSpotLightDir;
	XMFLOAT4 vSpotLightColor;
	XMFLOAT4 vSpotLightConeRatio;
	XMFLOAT4 drawingSkybox;
	XMFLOAT4 vWaterTime;
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
unsigned int CreatePlane(MyVertex* array);
void CreateSkyBox();
void LoadDotMesh(const char* meshFileName, SimpleMesh& mesh);
bool LoadOBJ(const char* meshFileName, SimpleOBJ& objMesh, MyVertex* vertArray, unsigned int* indicesArray, unsigned int* numVerts, unsigned int* numIndices);
void MakeLights();

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
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

	screenWidth = swap.BufferDesc.Width;
	screenHeight = swap.BufferDesc.Height;
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

	//Create Water and Skybox
	numWaterVertices = CreatePlane(waterPlane);
	numSandVertices = CreatePlane(sandPlane);
	CreateSkyBox();

	//Creating Texture
	if (myDevice)
	{
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/water.dds", nullptr, &waterTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/sandTex.dds", nullptr, &sandTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/SkyboxOcean.dds", nullptr, &skyboxTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/islandTex.dds", nullptr, &islandTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/pirateTex.dds", nullptr, &pirateTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/dockTex.dds", nullptr, &dockTex);
		hr = CreateDDSTextureFromFile(myDevice, L"Assets/boatTex.dds", nullptr, &boatTex);
	}

	//Time for the wave
	waveTime = { 0.0f, 0.0f, 0.0f, 0.0f };

	//Load Skybox
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numSkyboxVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = skybox;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &skyBoxVertexBuffer);

	//Load Water
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numWaterVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = waterPlane;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &waterVertexBuffer);

	//Load Sand
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numSandVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = sandPlane;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &sandVertexBuffer);

	//Load Island
	const char* filename = "Assets/island.obj";
	SimpleOBJ OBJIsland;
	LoadOBJ(filename, OBJIsland, islandArray, islandIndicesArray, &numIslandVertices, &numIslandIndices);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numIslandVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = islandArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &islandVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * numIslandIndices;
	subData.pSysMem = islandIndicesArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &islandIndicesBuffer);

	//Load Pirate
	filename = "Assets/pirate.obj";
	SimpleOBJ OBJPirate;
	LoadOBJ(filename, OBJPirate, pirateArray, pirateIndicesArray, &numPirateVertices, &numPirateIndices);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numPirateVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = pirateArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &pirateVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * numPirateIndices;
	subData.pSysMem = pirateIndicesArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &pirateIndicesBuffer);

	filename = "Assets/longDock.obj";
	SimpleOBJ OBJLongDock;
	LoadOBJ(filename, OBJLongDock, longDockArray, longDockIndicesArray, &numLongDockVertices, &numLongDockIndices);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numLongDockVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = longDockArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &longDockVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * numLongDockIndices;
	subData.pSysMem = longDockIndicesArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &longDockIndicesBuffer);

	filename = "Assets/smallBoat.obj";
	SimpleOBJ OBJBoat;
	LoadOBJ(filename, OBJBoat, boatArray, boatIndicesArray, &numBoatVertices, &numBoatIndices);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numBoatVertices;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = boatArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &boatVertexBuffer);
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(unsigned int) * numBoatIndices;
	subData.pSysMem = boatIndicesArray;
	hr = myDevice->CreateBuffer(&bDesc, &subData, &boatIndicesBuffer);

	//Load new mesh shader
	hr = myDevice->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vertexMeshShader);
	hr = myDevice->CreateVertexShader(WaterShader, sizeof(WaterShader), nullptr, &waterShader);
	hr = myDevice->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pixelMeshShader);
	hr = myDevice->CreatePixelShader(WaterPixelShader, sizeof(WaterPixelShader), nullptr, &waterPixelShader);

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
	case WM_MOUSEWHEEL:
		deltaWheel += GET_WHEEL_DELTA_WPARAM(wParam);
 		break;
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
	static float t = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;

	//Clear screen to one color
	float color[] = CYAN;
	myDeviceContext->ClearRenderTargetView(myRenderTargetView, color);

	//Check for user input
	UpdatePlanes();
	CheckKeyInputs();

	//world
	XMMATRIX temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, temp);
	//view
	XMStoreFloat4x4(&myMatrices.viewMatrix, camView);
	//projection
	temp = XMMatrixPerspectiveFovLH(3.14 / FOVDivider, aspectRatio, nearPlane, farPlane);
	XMStoreFloat4x4(&myMatrices.projMatrix, temp);
	//time
	waveTime = { (waveTime.x + 0.05f), 0.0f, 0.0f, 0.0f };
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
	// set shaders
	myDeviceContext->VSSetShader(vertexMeshShader, nullptr, 0);
	myDeviceContext->PSSetShader(pixelMeshShader, nullptr, 0);
	myDeviceContext->PSSetSamplers(0, 1, &samplerLinear);
	myDeviceContext->IASetInputLayout(vertexMeshLayout);
	myDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Set lighting variables
	MakeLights();
	//Upload to the video card
	UploadToVideoCard();

	//Immediate context 
	//Get a more complex pre-made mesh (FBX, OBJ, custom header) #done
	//load the mesh on the card(vertex buffer, index buffer) #done
	//make sure shaders can process the mesh #done??
	//place mesh somewhere else in the envirnoment
	//setup pipeline
	UINT meshStrides[] = { sizeof(MyVertex) };
	UINT meshOffsets[] = { 0 };

	//Draw Skybox
	ID3D11Buffer* tempMeshVertexBuffer = skyBoxVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(skyBoxIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShaderResources(1, 1, &skyboxTex);
	XMMATRIX skyboxMatrix = XMMatrixTranslation(XMVectorGetX(camPosition), XMVectorGetY(camPosition), XMVectorGetZ(camPosition));
	XMStoreFloat4x4(&myMatrices.worldMatrix, skyboxMatrix);
	XMStoreFloat4(&myLights.drawingSkybox, { 1.0f });
	UploadToVideoCard();
	myDeviceContext->Draw(numSkyboxVertices, 0);
	myDeviceContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
	XMStoreFloat4(&myLights.drawingSkybox, { -1.0f });

	//Draw Island
	tempMeshVertexBuffer = islandVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(islandIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &islandTex);
	XMMATRIX islandMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, islandMatrix);
	UploadToVideoCard();
	myDeviceContext->DrawIndexed(numIslandIndices, 0, 0);

	//Draw Pirate
	tempMeshVertexBuffer = pirateVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(pirateIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &pirateTex);
	XMMATRIX pirateMatrix = XMMatrixTranslation(25.0f, 1.45f, -15.0f);
	XMMATRIX rotation = XMMatrixRotationY(135);
	pirateMatrix = XMMatrixMultiply(rotation, pirateMatrix);
	XMStoreFloat4x4(&myMatrices.worldMatrix, pirateMatrix);
	UploadToVideoCard();
	myDeviceContext->DrawIndexed(numPirateIndices, 0, 0);

	//Draw Long Dock
	tempMeshVertexBuffer = longDockVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(longDockIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &dockTex);
	XMMATRIX longDockMatrix = XMMatrixTranslation(32.5f, 1.0f, -30.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, longDockMatrix);
	UploadToVideoCard();
	myDeviceContext->DrawIndexed(numLongDockIndices, 0, 0);

	//Draw Sand
	tempMeshVertexBuffer = sandVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->PSSetShaderResources(0, 1, &sandTex);
	XMMATRIX sandMatrix = XMMatrixTranslation(-100.0f, -1.0f, -100.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, sandMatrix);
	UploadToVideoCard();
	myDeviceContext->Draw(numSandVertices, 0);

	//Draw Water
	tempMeshVertexBuffer = waterVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->VSSetShader(waterShader, nullptr, 0);
	myDeviceContext->PSSetShader(waterPixelShader, nullptr, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &waterTex);
	XMMATRIX waterMatrix = XMMatrixTranslation(-100.0f, 0.0f, -100.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, waterMatrix);
	static float wavePixelTime = 0.0f; wavePixelTime += 0.01f;
	XMVECTOR tempWaterTime = { wavePixelTime, 0.0f, 0.5f, 0.0f };
	XMStoreFloat4(&myLights.vWaterTime, tempWaterTime);
	XMStoreFloat4(&myMatrices.drawingBoat, { -1.0f });
	UploadToVideoCard();
	myDeviceContext->Draw(numWaterVertices, 0);

	//Draw Small Boat
	tempMeshVertexBuffer = boatVertexBuffer;
	myDeviceContext->IASetVertexBuffers(0, 1, &tempMeshVertexBuffer, meshStrides, meshOffsets);
	myDeviceContext->IASetIndexBuffer(boatIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	myDeviceContext->PSSetShader(pixelMeshShader, nullptr, 0);
	myDeviceContext->PSSetShaderResources(0, 1, &boatTex);
	XMMATRIX boatMatrix = XMMatrixTranslation(42.0f, 0.1f, -34.0f);
	XMStoreFloat4x4(&myMatrices.worldMatrix, boatMatrix);
	XMStoreFloat4(&myMatrices.drawingBoat, { 1.0f });
	UploadToVideoCard();
	myDeviceContext->DrawIndexed(numBoatIndices, 0, 0);


	mySwapChain->Present(1, 0);
}

void ReleaseInterfaces()
{
	waterTex->Release();
	sandTex->Release();
	skyboxTex->Release();
	islandTex->Release();
	pirateTex->Release();
	dockTex->Release();
	boatTex->Release();
	lightConstantBuffer->Release();
	samplerLinear->Release();
	zBuffer->Release();
	zBufferView->Release();
	vertexMeshLayout->Release();
	vertexMeshShader->Release();
	waterShader->Release();
	pixelMeshShader->Release();
	waterPixelShader->Release();
	myRenderTargetView->Release();
	waterVertexBuffer->Release();
	skyBoxVertexBuffer->Release();
	islandVertexBuffer->Release();
	islandIndicesBuffer->Release();
	sandVertexBuffer->Release();
	pirateVertexBuffer->Release();
	pirateIndicesBuffer->Release();
	longDockVertexBuffer->Release();
	longDockIndicesBuffer->Release();
	boatVertexBuffer->Release();
	boatIndicesBuffer->Release();
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

unsigned int CreatePlane(MyVertex* vertArray)
{
	int amountOfVertices = 121;
	int index = 0;
	int ratioDivider = sqrt(amountOfVertices) - 1;
	int size = static_cast<int>(sqrt(amountOfVertices));
	float spacing = 20.0f;
	float xPos = 0.0f;
	float xuRatio = 0.0f;
	float zPos = 0.0f;
	float zvRatio = 0.0f;
	float yPos = 0.0f;

	for (int x = 0; x < size; x++)
	{
		for (int z = 0; z < size; z++)
		{

			if (x == 0)
				xPos = static_cast<float>(x);
			else
				xPos = static_cast<float>(x) * spacing;

			if (z == 0)
				zPos = static_cast<float>(z);
			else
				zPos = static_cast<float>(z) * spacing;

			xuRatio = static_cast<float>(x) / (size - 1);
			zvRatio = static_cast<float>(z) / (size - 1);

			//Bottom Left Triangle
			//Bottom Left Vertex
			vertArray[index].position = { xPos, yPos, zPos, 1.0f };
			vertArray[index].texture = { xuRatio, zvRatio };
			vertArray[index].normal = { 0.0f, 1.0f, 0.0f };
			index++;
			//Top Left Vertex
			zvRatio = (static_cast<float>(z) + 1.0f) / (size - 1);
			vertArray[index].position = { xPos, yPos, zPos + spacing, 1.0f };
			vertArray[index].texture = { xuRatio, zvRatio };
			vertArray[index].normal = { 0.0f, 1.0f, 0.0f };
			index++;
			//Bottom Right Vertex
			xuRatio = (static_cast<float>(x) + 1.0f) / (size - 1);
			zvRatio = static_cast<float>(z) / (size - 1);
			vertArray[index].position = { xPos + spacing, yPos, zPos, 1.0f };
			vertArray[index].texture = { xuRatio, zvRatio };
			vertArray[index].normal = { 0.0f, 1.0f, 0.0f };
			index++;

			//Top Right Triangle
			//Bottom Right Vertex
			vertArray[index] = vertArray[index - 1];
			index++;
			//Top Left Vertex
			vertArray[index] = vertArray[index - 3];
			index++;
			//Top Right Vertex
			xuRatio = (static_cast<float>(x) + 1.0f) / (size - 1);
			zvRatio = (static_cast<float>(z) + 1.0f) / (size - 1);
			vertArray[index].position = { xPos + spacing, yPos, zPos + spacing, 1.0f };
			vertArray[index].texture = { xuRatio, zvRatio };
			vertArray[index].normal = { 0.0f, 1.0f, 0.0f };
			index++;
		}
	}
	return index;
}

void CreateSkyBox()
{
	float bottomY = -1.0f;
	float topY = 1.0f;

	//Front
	skybox[0].position = { -1.0f, bottomY, -1.0f, 1.0f };//Bottom Left
	skybox[1].position = { -1.0f, topY, -1.0f, 1.0f };//Top Left
	skybox[2].position = { 1.0f, bottomY, -1.0f, 1.0f };//Bottom Right
	skybox[3].position = { 1.0f, bottomY, -1.0f, 1.0f };//Bottom Right
	skybox[4].position = { -1.0f, topY, -1.0f, 1.0f };//Top Left
	skybox[5].position = { 1.0f, topY, -1.0f, 1.0f };//Top Right

	//Right
	skybox[6].position = { 1.0f, bottomY, -1.0f, 1.0f };//Bottom Left
	skybox[7].position = { 1.0f, topY, -1.0f, 1.0f };//Top Left
	skybox[8].position = { 1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[9].position = { 1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[10].position = { 1.0f, topY, -1.0f, 1.0f };//Top Left
	skybox[11].position = { 1.0f, topY, 1.0f, 1.0f };//Top Right

	//Back
	skybox[12].position = { 1.0f, bottomY, 1.0f, 1.0f };//Bottom Left
	skybox[13].position = { 1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[14].position = { -1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[15].position = { -1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[16].position = { 1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[17].position = { -1.0f, topY, 1.0f, 1.0f };//Top Right

	//Left
	skybox[18].position = { -1.0f, bottomY, 1.0f, 1.0f };//Bottom Left
	skybox[19].position = { -1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[20].position = { -1.0f, bottomY, -1.0f, 1.0f };//Bottom Right
	skybox[21].position = { -1.0f, bottomY, -1.0f, 1.0f };//Bottom Right
	skybox[22].position = { -1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[23].position = { -1.0f, topY, -1.0f, 1.0f };//Top Right

	//Top
	skybox[24].position = { -1.0f, topY, -1.0f, 1.0f };//Bottom Left
	skybox[25].position = { -1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[26].position = { 1.0f, topY, -1.0f, 1.0f };//Bottom Right
	skybox[27].position = { 1.0f, topY, -1.0f, 1.0f };//Bottom Right
	skybox[28].position = { -1.0f, topY, 1.0f, 1.0f };//Top Left
	skybox[29].position = { 1.0f, topY, 1.0f, 1.0f };//Top Right

	//Bottom
	skybox[30].position = { -1.0f, bottomY, 1.0f, 1.0f };//Bottom Left
	skybox[31].position = { -1.0f, bottomY, -1.0f, 1.0f };//Top Left
	skybox[32].position = { 1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[33].position = { 1.0f, bottomY, 1.0f, 1.0f };//Bottom Right
	skybox[34].position = { -1.0f, bottomY, -1.0f, 1.0f };//Top Left
	skybox[35].position = { 1.0f, bottomY, -1.0f, 1.0f };//Top Right

	numSkyboxVertices = 36;

	for (int i = 0; i < numSkyboxVertices; i++)
	{
		skybox[i].position.x = -skybox[i].position.x;
		skybox[i].position.y = -skybox[i].position.y;
		skybox[i].position.z = -skybox[i].position.z;
	}
}

void LoadDotMesh(const char* meshFileName, SimpleMesh& mesh)
{
	std::fstream file{ meshFileName, std::ios_base::in | std::ios_base::binary };

	assert(file.is_open());

	uint32_t player_index_count;
	file.read((char*)& player_index_count, sizeof(uint32_t));

	mesh.indicesList.resize(player_index_count);

	file.read((char*)mesh.indicesList.data(), sizeof(uint32_t) * player_index_count);

	uint32_t player_vertex_count;
	file.read((char*)& player_vertex_count, sizeof(uint32_t));

	mesh.vertexList.resize(player_vertex_count);

	file.read((char*)mesh.vertexList.data(), sizeof(MyVertex) * player_vertex_count);
	file.close();
}

bool LoadOBJ(const char* meshFileName, SimpleOBJ& objMesh, MyVertex* vertArray, unsigned int* indicesArray, unsigned int* numVerts, unsigned int* numIndices)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<MyVec3> tempVertices;
	vector<MyVec3> tempTexture;
	vector<MyVec3> tempNormals;

	FILE* file;
	fopen_s(&file, meshFileName, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0)
		{
			MyVec3 tempVert;
			fscanf_s(file, "%f %f %f\n", &tempVert.xyz[0], &tempVert.xyz[1], &tempVert.xyz[2]);
			tempVertices.push_back(tempVert);
		}
		else if (strcmp(lineHeader, "vt") == 0) 
		{
			MyVec3 tempTex;
			fscanf_s(file, "%f %f\n", &tempTex.xyz[0], &tempTex.xyz[1]);
			tempTexture.push_back(tempTex);
		}
		else if (strcmp(lineHeader, "vn") == 0) 
		{
			MyVec3 tempNrm;
			fscanf_s(file, "%f %f %f\n", &tempNrm.xyz[0], &tempNrm.xyz[1], &tempNrm.xyz[2]);
			tempNormals.push_back(tempNrm);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		MyVertex currVertex;
		unsigned int vertexIndex = vertexIndices[i];
		float x = tempVertices[vertexIndex - 1].xyz[0];
		float y = tempVertices[vertexIndex - 1].xyz[1];
		float z = tempVertices[vertexIndex - 1].xyz[2];
		float w = 1.0f;
		currVertex.position = { x, y, z, w };
		
		unsigned int uvIndex = uvIndices[i];
		float ux = tempTexture[uvIndex - 1].xyz[0];
		float vy = 1.0f - tempTexture[uvIndex - 1].xyz[1];
		currVertex.texture = { ux, vy };

		unsigned int nrmIndex = normalIndices[i];
		float nrmX = tempNormals[nrmIndex - 1].xyz[0];
		float nrmY = tempNormals[nrmIndex - 1].xyz[1];
		float nrmZ = tempNormals[nrmIndex - 1].xyz[2];
		currVertex.normal = { nrmX, nrmY, nrmZ };

		objMesh.vertexList.push_back(currVertex);
		objMesh.indicesList.push_back(i);
	}

	*numVerts = objMesh.vertexList.size();
	*numIndices = objMesh.indicesList.size();

	//Populate Vertex Array
	for (unsigned int i = 0; i < objMesh.vertexList.size(); i++)
	{
		vertArray[i] = objMesh.vertexList[i];
	}

	//Populate Indices Array
	for (unsigned int i = 0; i < objMesh.indicesList.size(); i++)
	{
		indicesArray[i] = objMesh.indicesList[i];
	}
}

void MakeLights()
{
	//Direction Light
	lightDir = { 1.0f, 0.9f, 0.0f, 1.0f };
	//static float dirLightAngle = 0.0f; dirLightAngle += 0.005f;
	//XMMATRIX dirLightRotation = XMMatrixRotationZ(-dirLightAngle);
	//lightDir = XMVector4Transform(lightDir, dirLightRotation);
	XMStoreFloat4(&myLights.vLightDir, lightDir);
	lightColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMStoreFloat4(&myLights.vLightColor, lightColor);

	//Point Light
	//pointLightPos = { 5.0f, 5.0f, -2.0f, 1.0f };
	//static float pointLightAngle = 0.0f; pointLightAngle += 0.05f;
	//XMMATRIX pointLightRotation = XMMatrixRotationY(pointLightAngle);
	//pointLightPos = XMVector4Transform(pointLightPos, pointLightRotation);
	//XMStoreFloat4(&myLights.vPointLightPos, pointLightPos);
	//pointLightColor = RED;
	//XMStoreFloat4(&myLights.vPointLightColor, pointLightColor);

	//Spot Light
	//spotLightDir = { 1.0f, 0.0f, 0.0f, 0.0f };
	//static float spotLightDirAngle = 0.0f; spotLightDirAngle += 0.05f;
	//XMMATRIX spotLightDirRotation = XMMatrixRotationY(spotLightDirAngle);
	//spotLightDir = XMVector4Transform(spotLightDir, spotLightDirRotation);
	//XMStoreFloat4(&myLights.vSpotLightDir, spotLightDir);
	//spotLightPos = { -6.0f, 3.0f, -13.0f, 1.0f };
	//static float spotLightRotAngle = 0.0f; spotLightRotAngle += 0.01f;
	//XMMATRIX spotLightPosRotation = XMMatrixRotationY(spotLightRotAngle);
	//spotLightPos = XMVector4Transform(spotLightPos, spotLightPosRotation);
	//XMStoreFloat4(&myLights.vSpotLightPos, spotLightPos);
	//spotLightColor = MAGENTA;
	//XMStoreFloat4(&myLights.vSpotLightColor, spotLightColor);
	//spotLightConeRatio = { 0.8f, 0.0f, 0.0f, 0.0f };
	//XMStoreFloat4(&myLights.vSpotLightConeRatio, spotLightConeRatio);

	//Other
	ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	XMStoreFloat4(&myLights.vAmbient, ambient);
}
