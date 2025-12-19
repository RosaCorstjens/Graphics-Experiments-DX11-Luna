#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd){
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	App theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

/// <summary>
/// Initializes a new instance of the <see cref="App"/> class.
/// </summary>
/// <param name="hInstance">The h instance.</param>
App::App(HINSTANCE hInstance)
	: D3DApp(hInstance), _sky(0), _shader(0), _skyShader(0), _terrainShader(0)
{
	mMainWndCaption = L"Godrays Application";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	SetLighting();
	BuildMatrices();
}

/// <summary>
/// Finalizes an instance of the <see cref="App"/> class.
/// </summary>
App::~App()
{
	SafeDelete(_sky);
	
	if (_shader)
	{
		_shader->Shutdown();
		delete _shader;
		_shader = 0;
	}	

	if (_skyShader)
	{
		_skyShader->Shutdown();
		delete _skyShader;
		_skyShader = 0;
	}

	if (_terrainShader)
	{
		_terrainShader->Shutdown();
		delete _terrainShader;
		_terrainShader = 0;
	}
}

/// <summary>
/// Builds the matrices for positioning and rotating.
/// </summary>
void App::BuildMatrices() {
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&_world, I);

	// Setup the projection matrix.
	XMStoreFloat4x4(&_proj, XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f));

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&_ortho, XMMatrixOrthographicLH((float)mClientWidth, (float)mClientHeight, 0.1f, 1000.0f));
}

void App::SetLighting() {
	_dirLights[0].Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_dirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_dirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 16.0f);
	_dirLights[0].Direction = XMFLOAT3(0.107f, -0.807f, 0.0f);
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool App::Init()
{
	if(!D3DApp::Init())
		return false;

	_shader = new TextureShader;
	_shader->Initialize(md3dDevice, mhMainWnd);
	_skyShader = new SkyShader;
	_skyShader->Initialize(md3dDevice, mhMainWnd);
	_terrainShader = new TerrainShader;
	_terrainShader->Initialize(md3dDevice, mhMainWnd);

	_sky = new Sky(md3dDevice, L"Data/Skybox/Skybox.dds", 5000.0f);

	Terrain::TerrainInfo tii;
	tii.HeightMapFilename = L"Data/Terrain/terrain.raw";
	tii.LayerMapFilename0 = L"Data/Terrain/grass.jpg";
	tii.LayerMapFilename1 = L"Data/Terrain/darkdirt.jpg";
	tii.LayerMapFilename2 = L"Data/Terrain/stone.jpg";
	tii.LayerMapFilename3 = L"Data/Terrain/lightdirt.jpg";
	tii.LayerMapFilename4 = L"Data/Terrain/snow.jpg";
	tii.BlendMapFilename = L"Data/Terrain/blend.dds";
	tii.HeightScale = 40.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	_terrain.Initialize(md3dDevice, md3dImmediateContext, tii);

	_camera.SetPosition(0, 2, 0);
	return true;
}

/// <summary>
/// Called on resize of the screen space.
/// </summary>
void App::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_proj, P);
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt">The deltatime.</param>
void App::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		_camera.Walk(50.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		_camera.Walk(-50.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		_camera.Strafe(-50.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		_camera.Strafe(50.0f*dt);

	XMFLOAT3 camPos = _camera.GetPosition();
	float y = _terrain.GetHeight(camPos.x, camPos.z);
	_camera.SetPosition(camPos.x, y + 5.0f, camPos.z);

	_camera.UpdateViewMatrix();
}

/// <summary>
/// Draws the scene.
/// </summary>
void App::DrawScene()
{
	// clear the render targets 
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// declare the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = _camera.View();
	proj = _camera.Proj();
	worldViewProj = world*view*proj;

	/// --------- RENDER TERRAIN -----------
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, viewProj);

	_terrainShader->SetViewProj(viewProj);
	_terrainShader->SetEyePosW(_camera.GetPosition());
	_terrainShader->SetDirLights(_dirLights);
	_terrainShader->SetFogColor(Colors::Silver);
	_terrainShader->SetFogStart(50.0f);
	_terrainShader->SetFogRange(175.0f);
	_terrainShader->SetMinDist(10.0f);
	_terrainShader->SetMaxDist(500.0f);
	_terrainShader->SetMinTess(0.0f);
	_terrainShader->SetMaxTess(6.0f);
	_terrainShader->SetTexelCellSpaceU(1.0f / _terrain.GetTerrainInfo().HeightmapWidth);
	_terrainShader->SetTexelCellSpaceV(1.0f / _terrain.GetTerrainInfo().HeightmapHeight);
	_terrainShader->SetWorldCellSpace(_terrain.GetTerrainInfo().CellSpacing);
	_terrainShader->SetWorldFrustumPlanes(worldPlanes);

	_terrainShader->SetLayerMapArray(_terrain.GetLayerMapArraySRV());
	_terrainShader->SetBlendMap(_terrain.GetBlendMapSRV());
	_terrainShader->SetHeightMap(_terrain.GetHeightMapSRV());

	_terrainShader->SetMaterial(_terrain.GetMaterial());

	_terrain.Render(md3dImmediateContext, _terrainShader->GetInputLayout());

	_terrainShader->Render(md3dImmediateContext, _terrain.GetIndexCount());

	/// --------- RENDER SKYBOX ------------
	// center Sky about eye in world space
	XMMATRIX T = XMMatrixTranslation(_camera.GetPosition().x, _camera.GetPosition().y, _camera.GetPosition().z);

	XMMATRIX WVP = XMMatrixMultiply(T, XMMatrixMultiply(view, proj));

	_skyShader->SetWorldViewProj(WVP);
	_skyShader->SetCubeMap(_sky->GetCubeMap());

	_sky->Render(md3dImmediateContext, _skyShader->GetInputLayOut());

	_skyShader->Render(md3dImmediateContext, _sky->GetIndexCount());

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// end scene
	HR(mSwapChain->Present(0, 0));
}

void App::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void App::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void App::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.7f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.7f * static_cast<float>(y - mLastMousePos.y));

		_camera.RotateY(-dx);
		_camera.Pitch(dy);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
