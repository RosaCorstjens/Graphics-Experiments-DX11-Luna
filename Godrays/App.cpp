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
	: D3DApp(hInstance), _sky(0), _depthTexture(0), _diffTexture(0), _shaftTexture(0), _blurredTexture(0), _model(0),
	_endCam(0), _godRayShader(0), _textureShader(0), _skyShader(0), _depthShader(0), _fullScreenWindow(0), _primitive(0)
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
	
	if (_textureShader)
	{
		_textureShader->Shutdown();
		delete _textureShader;
		_textureShader = 0;
	}	

	if (_skyShader)
	{
		_skyShader->Shutdown();
		delete _skyShader;
		_skyShader = 0;
	}

	if (_depthShader)
	{
		_depthShader->Shutdown();
		delete _depthShader;
		_depthShader = 0;
	}
	
	if (_godRayShader)
	{
		_godRayShader->Shutdown();
		delete _godRayShader;
		_godRayShader = 0;
	}
	
	if (_depthTexture)
	{
		_depthTexture->Shutdown();
		delete _depthTexture;
		_depthTexture = 0;
	}
	
	if (_diffTexture)
	{
		_diffTexture->Shutdown();
		delete _diffTexture;
		_diffTexture = 0;
	}
	
	if (_shaftTexture)
	{
		_shaftTexture->Shutdown();
		delete _shaftTexture;
		_shaftTexture = 0;
	}
	
	if (_blurredTexture)
	{
		_blurredTexture->Shutdown();
		delete _blurredTexture;
		_blurredTexture = 0;
	}

	if (_fullScreenWindow) {
		_fullScreenWindow->Shutdown();
		delete _fullScreenWindow;
		_fullScreenWindow = 0;
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
	_dirLights[0].Direction = XMFLOAT3(0.8945f, -0.4472f, 0.0f);			// THESE ARE NOT RADIAN!!! ITS A FUCKING DIRECTION
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool App::Init()
{
	if(!D3DApp::Init())
		return false;

	_textureShader = new TextureShader;
	_textureShader->Initialize(md3dDevice, mhMainWnd);
	_skyShader = new SkyShader;
	_skyShader->Initialize(md3dDevice, mhMainWnd);
	_depthShader = new DepthShader;
	_depthShader->Initialize(md3dDevice, mhMainWnd);
	_godRayShader = new GodRayShader;
	_godRayShader->Initialize(md3dDevice, mhMainWnd);

	_sky = new Sky(md3dDevice, L"Data/Skybox/Skybox.dds", 5000.0f);

	_depthTexture = new RenderTexture;
	_depthTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000.0f, 0.1f);
	_diffTexture = new RenderTexture;
	_diffTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000.0f, 0.1f);
	_shaftTexture = new RenderTexture;
	_shaftTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000.0f, 0.1f);
	_blurredTexture = new RenderTexture;
	_blurredTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000.0f, 0.1f);

	_model = new Model;
	_model->Initialize(md3dDevice, "Data/Gate.txt", L"Data/Gate.png",
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(0, 65, 0), XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT3(0.0f, MathHelper::Pi / 2.0f, 0.0f));

	_primitive = new Primitive;
	_primitive->Initialize(md3dDevice, Primitive::Grid, L"Data/Terrain/grass.jpg",
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(0, -20.0f, 0), XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(50.0f, 50.0f));

	// initialize full screen plane
	_fullScreenWindow = new OrthoWindow;
	_fullScreenWindow->Initialize(md3dDevice, mClientWidth, mClientHeight);

	// Convert Spherical to Cartesian coordinates.
	_endCam = new CameraClass;
	float x = 10.0f*sinf(0.5f*MathHelper::Pi)*cosf(1.5f*MathHelper::Pi);
	float z = 10.0f*sinf(0.5f*MathHelper::Pi)*sinf(1.5f*MathHelper::Pi);
	float y = 10.0f*cosf(0.5f*MathHelper::Pi);
	_camera.SetPosition(100, y, z);
	_endCam->SetPosition(x, y, z);
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

	_endCam->Render();

	RenderSceneToDepthTexture();
	RenderSceneToDiffTexture();
	RenderMask();
	RenderBlur();
	RenderEnd();

	// end scene
	HR(mSwapChain->Present(0, 0));
}

void App::RenderSceneToDepthTexture() {
	// set and clear the texture to render to
	_depthTexture->SetRenderTarget(md3dImmediateContext);
	_depthTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));
	
	// set per frame shader parameters 
	_depthShader->SetPerFrame(_camera.GetPosition());

	XMMATRIX world, worldInvTranspose, worldViewProj, view, proj;
	view = _camera.View();
	proj = _camera.Proj();

	/// ---------- RENDER BUILDING -----------
	world = XMLoadFloat4x4(_model->GetWorldMatrix());
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world * view * proj;

	_depthShader->SetPerObject(worldViewProj, world, worldInvTranspose);

	_model->Render(md3dImmediateContext);
	_depthShader->RenderDepth(md3dImmediateContext, _model->GetIndexCount());

	/// ---------- RENDER GRID -----------
	world = XMLoadFloat4x4(_primitive->GetWorldMatrix());
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world * view * proj;

	_depthShader->SetPerObject(worldViewProj, world, worldInvTranspose);

	_primitive->Render(md3dImmediateContext);
	_depthShader->RenderDepth(md3dImmediateContext, _primitive->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void App::RenderSceneToDiffTexture() {
	_diffTexture->SetRenderTarget(md3dImmediateContext);
	_diffTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));

	// declare the required matrices per object
	XMMATRIX world, worldInvTranspose, worldViewProj, view, proj;
	view = _camera.View();
	proj = _camera.Proj();

	/// ---------- RENDER BUILDING -----------
	// set buffer per frame
	_textureShader->SetPerFrame(_camera.GetPosition(), &_dirLights[0]);

	// RENDER MODEL 
	world = XMLoadFloat4x4(_model->GetWorldMatrix());
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world * view * proj;

	// set buffer per object
	_textureShader->SetPerObject(worldViewProj, world, worldInvTranspose, XMLoadFloat4x4(_model->GetTextureTransform()), _model->GetMaterial(), _model->GetDiffTexture());

	// render
	_model->Render(md3dImmediateContext);
	_textureShader->Render(md3dImmediateContext, _model->GetIndexCount());

	/// ---------- RENDER GRID -----------
	world = XMLoadFloat4x4(_primitive->GetWorldMatrix());
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world * view * proj;

	_textureShader->SetPerObject(worldViewProj, world, worldInvTranspose, XMLoadFloat4x4(_primitive->GetTextureTransform()), _primitive->GetMaterial(), _primitive->GetDiffTexture());

	_primitive->Render(md3dImmediateContext);
	_textureShader->Render(md3dImmediateContext, _primitive->GetIndexCount());

	/// --------- RENDER SKYBOX ------------
	// center Sky about eye in world space
	worldViewProj = XMMatrixMultiply(XMMatrixTranslation(_camera.GetPosition().x, _camera.GetPosition().y, _camera.GetPosition().z), 
									XMMatrixMultiply(view, proj));

	_skyShader->SetWorldViewProj(worldViewProj);
	_skyShader->SetCubeMap(_sky->GetCubeMap());

	_sky->Render(md3dImmediateContext, _skyShader->GetInputLayOut());
	_skyShader->Render(md3dImmediateContext, _sky->GetIndexCount());

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void App::RenderMask() {
	_shaftTexture->SetRenderTarget(md3dImmediateContext);
	_shaftTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	XMMATRIX world, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	view = XMLoadFloat4x4(&_endCam->GetViewMatrix());
	proj = XMLoadFloat4x4(&_diffTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// set variables
	_godRayShader->SetScreenSize(XMFLOAT2(mClientWidth, mClientHeight));

	_godRayShader->SetWorldViewProj(worldViewProj);

	_godRayShader->SetDepthTexture(_depthTexture->GetShaderResourceView());
	_godRayShader->SetDiffTexture(_diffTexture->GetShaderResourceView());

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_fullScreenWindow->Render(md3dImmediateContext, _godRayShader->GetInputLayout());

	_godRayShader->RenderMask(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void App::RenderBlur() {
	_blurredTexture->SetRenderTarget(md3dImmediateContext);
	_blurredTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	XMMATRIX world, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	view = XMLoadFloat4x4(&_endCam->GetViewMatrix());
	proj = XMLoadFloat4x4(&_diffTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// -----------------------------------------------
	// KIND OF CORRECT METHOD FOR POSITON CALCULATION
	// calculates an x and y pos between -1 and 1 if in screen
	// really high values outside screen
	// z is between 0.9999 (close) and 1.00something (far)

	XMFLOAT4X4 matrix;
	XMMATRIX posMatrix = XMMatrixTranslation(_dirLights[0].Direction.x * -1000.0f,
		_dirLights[0].Direction.y * -1000.0f,
		_dirLights[0].Direction.z * -1000.0f);//-1000.0f, 150.0f, 0.0f);
	XMStoreFloat4x4(&matrix, XMMatrixMultiply(posMatrix, _camera.ViewProj()));

	XMFLOAT4 pos4 = XMFLOAT4(matrix._41, matrix._42, matrix._43, matrix._44);
	XMFLOAT3 pos;
	pos = XMFLOAT3(pos4.x, pos4.y, pos4.z);

	pos.x = pos4.x / pos4.w;
	pos.y = -pos4.y / pos4.w;
	pos.z = pos4.z / pos4.w;

	float dotproduct = (_camera.GetLook().x * -_dirLights[0].Direction.x +
		_camera.GetLook().y * -_dirLights[0].Direction.y +
		_camera.GetLook().z * -_dirLights[0].Direction.z);

	float intensity = dotproduct * 0.5f + 0.5f;

	// set variables
	_godRayShader->SetScreenSize(XMFLOAT2(mClientWidth, mClientHeight));
	_godRayShader->SetAspectRatio(mClientWidth / mClientHeight);
	_godRayShader->SetLightPosition(XMFLOAT2(pos.x, pos.y));
	_godRayShader->SetIntensity(intensity);
	_godRayShader->SetWorldViewProj(worldViewProj);
	_godRayShader->SetShaftTexture(_shaftTexture->GetShaderResourceView());

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_fullScreenWindow->Render(md3dImmediateContext, _godRayShader->GetInputLayout());

	_godRayShader->RenderBlur(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void App::RenderEnd() {
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	XMMATRIX world, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	view = XMLoadFloat4x4(&_endCam->GetViewMatrix());
	proj = XMLoadFloat4x4(&_diffTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_fullScreenWindow->Render(md3dImmediateContext, _godRayShader->GetInputLayout());

	// set shader variables
	_godRayShader->SetShaftTexture(_blurredTexture->GetShaderResourceView());
	_godRayShader->SetDiffTexture(_diffTexture->GetShaderResourceView());
	_godRayShader->SetWorldViewProj(worldViewProj);

	// render final mix
	_godRayShader->RenderFinalMix(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	TurnZBufferOn();
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
