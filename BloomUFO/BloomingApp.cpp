#include "BloomingApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BloomingApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

/// <summary>
/// Initializes a new instance of the <see cref="BloomingApp"/> class.
/// </summary>
/// <param name="hInstance">The h instance.</param>
BloomingApp::BloomingApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mTheta(1.5f*MathHelper::Pi), mPhi(0.5f*MathHelper::Pi), mRadius(20.0f),
	_ufo(0), _camera(0), _cameraEnd(0), _shader(0), _renderTexture(0), _downSampleTexture(0), _upSampleTexture(0),
	_fullScreenWindow(0), _smallScreenWindow(0), _blurShader(0), _horizontalBlurTexture(0), _verticalBlurTexture(0),
	_glowMapShader(0), _glowShader(0)
{
	mMainWndCaption = L"Blooming Application";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	SetLighting();
	BuildMatrices();
}

/// <summary>
/// Finalizes an instance of the <see cref="BloomingApp"/> class.
/// </summary>
BloomingApp::~BloomingApp()
{
	if(_ufo) {
		_ufo->Shutdown();
		delete _ufo;
		_ufo = 0;
	}
	
	if (_shader)
	{
		_shader->Shutdown();
		delete _shader;
		_shader = 0;
	}

	if (_blurShader)
	{
		_blurShader->Shutdown();
		delete _blurShader;
		_blurShader = 0;
	}

	if (_renderTexture)
	{
		_renderTexture->Shutdown();
		delete _renderTexture;
		_renderTexture = 0;
	}

	if (_downSampleTexture)
	{
		_downSampleTexture->Shutdown();
		delete _downSampleTexture;
		_downSampleTexture = 0;
	}

	if (_upSampleTexture)
	{
		_upSampleTexture->Shutdown();
		delete _upSampleTexture;
		_upSampleTexture = 0;
	}

	if (_horizontalBlurTexture)
	{
		_horizontalBlurTexture->Shutdown();
		delete _horizontalBlurTexture;
		_horizontalBlurTexture = 0;
	}

	if (_verticalBlurTexture)
	{
		_verticalBlurTexture->Shutdown();
		delete _verticalBlurTexture;
		_verticalBlurTexture = 0;
	}

	if (_glowMapShader)
	{
		_glowMapShader->Shutdown();
		delete _glowMapShader;
		_glowMapShader = 0;
	}

	if (_glowShader)
	{
		_glowShader->Shutdown();
		delete _glowShader;
		_glowShader = 0;
	}

	if(_fullScreenWindow) {
		_fullScreenWindow->Shutdown();
		delete _fullScreenWindow;
		_fullScreenWindow = 0;
	}

	if(_smallScreenWindow) {
		_smallScreenWindow->Shutdown();
		delete _smallScreenWindow;
		_smallScreenWindow = 0;
	}
}

/// <summary>
/// Builds the matrices for positioning and rotating.
/// </summary>
void BloomingApp::BuildMatrices() {
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&_world, I);

	// Setup the projection matrix.
	XMStoreFloat4x4(&_proj, XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f));

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&_ortho, XMMatrixOrthographicLH((float)mClientWidth, (float)mClientHeight, 0.1f, 1000.0f));
}

void BloomingApp::SetLighting() {
	_dirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	_dirLights[0].Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	_dirLights[0].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	_dirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	_dirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_dirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	_dirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	_dirLights[2].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[2].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	_dirLights[2].Direction = XMFLOAT3(0.707f, 0.0f, -0.707f);
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool BloomingApp::Init()
{
	if(!D3DApp::Init())
		return false;

	_shader = new TextureShader;
	_shader->Initialize(md3dDevice, mhMainWnd);
	_blurShader = new BlurShader;
	_blurShader->Initialize(md3dDevice, mhMainWnd);
	_glowMapShader = new GlowMapShader;
	_glowMapShader->Initialize(md3dDevice, mhMainWnd);
	_glowShader = new GlowShader;
	_glowShader->Initialize(md3dDevice, mhMainWnd);

	_ufo = new Model;
	_ufo->Initialize(md3dDevice, "UFO.txt", L"UFO2.jpg", L"UFOGlow.jpg", 
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f),
		XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(0.03f, 0.03f, 0.03f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	_renderTexture = new RenderTexture();
	_renderTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000, 0.1f);
	_downSampleTexture = new RenderTexture();
	_downSampleTexture->Initialize(md3dDevice, mClientWidth/2, mClientHeight/2, 1000, 0.1f);
	_upSampleTexture = new RenderTexture();
	_upSampleTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000, 0.1f);
	_horizontalBlurTexture = new RenderTexture();
	_horizontalBlurTexture->Initialize(md3dDevice, mClientWidth/2, mClientHeight/2, 1000, 0.1f);
	_verticalBlurTexture = new RenderTexture();
	_verticalBlurTexture->Initialize(md3dDevice, mClientWidth/2, mClientHeight/2, 1000, 0.1f);

	_camera = new CameraClass;
	_cameraEnd = new CameraClass;

	_fullScreenWindow = new OrthoWindow;
	_fullScreenWindow->Initialize(md3dDevice, mClientWidth, mClientHeight);

	_smallScreenWindow = new OrthoWindow;
	_smallScreenWindow->Initialize(md3dDevice, mClientWidth/2, mClientHeight/2);

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);
	_camera->SetPosition(x, y, z);
	_cameraEnd->SetPosition(x, y, z);

	return true;
}

/// <summary>
/// Called on resize of the screen space.
/// </summary>
void BloomingApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_proj, P);
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt">The deltatime.</param>
void BloomingApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	_camera->SetPosition(x, y, z);
}

/// <summary>
/// Draws the scene.
/// </summary>
void BloomingApp::DrawScene()
{
	// gen the view matrix based on cam pos
	_camera->Render();
	_cameraEnd->Render();

	// set cbuffer per frame 
	_shader->SetDirLights(_dirLights);
	_shader->SetEyePosW(_camera->GetPosition());

	static float rotation = 0.0f;
	rotation += (float)MathHelper::Pi * 0.0001f;
	if (rotation > 360.0f) rotation -= 360.0f;

	static float glowStrength = 0.1f;
	static float addToStrength = 0.00005f;
	glowStrength += (float)MathHelper::Pi * addToStrength;
	if (glowStrength > 1 || glowStrength < 0.1f) addToStrength *= -1;

	RenderGlowMapToTexture(0);
	DownSampleTexture();
	RenderHorizontalBlurToTexture();
	RenderVerticalBlurToTexture();
	UpSampleTexture();
	RenderSceneToTexture(0);
	RenderGlowScene(1.0f);

	// end scene
	HR(mSwapChain->Present(0, 0));
}

bool BloomingApp::RenderGlowMapToTexture(float rotation) {
	_renderTexture->SetRenderTarget(md3dImmediateContext);
	_renderTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(_ufo->GetWorldMatrix());
	world = XMMatrixMultiply(world, XMMatrixRotationY(rotation));
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_camera->GetViewMatrix());
	proj = XMLoadFloat4x4(&_proj);
	worldViewProj = world*view*proj;

	// set cbuffer per object, for ufo
	_glowMapShader->SetWorld(world);
	_glowMapShader->SetWorldInvTranspose(worldInvTranspose);
	_glowMapShader->SetWorldViewProj(worldViewProj);
	_glowMapShader->SetMaterial(_ufo->GetMaterial());
	_glowMapShader->SetDiffTexture(_ufo->GetDiffTexture());
	_glowMapShader->SetGlowTexture(_ufo->GetGlowTexture());

	// put the models vertex and index buffers on the pipeline, ready for rendering
	_ufo->Render(md3dImmediateContext);

	_glowMapShader->Render(md3dImmediateContext, _ufo->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::DownSampleTexture() {
	_downSampleTexture->SetRenderTarget(md3dImmediateContext);
	_downSampleTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_cameraEnd->GetViewMatrix());
	proj = XMLoadFloat4x4(&_downSampleTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	_smallScreenWindow->Render(md3dImmediateContext);

	// set cbuffer per object
	_shader->SetWorld(world);
	_shader->SetWorldInvTranspose(worldInvTranspose);
	_shader->SetWorldViewProj(worldViewProj);
	_shader->SetTexTransform(world);
	_shader->SetMaterial(_ufo->GetMaterial());
	_shader->SetDiffTexture(_renderTexture->GetShaderResourceView());

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	_shader->SetTechnique("Light0Tex");
	_shader->Render(md3dImmediateContext, _smallScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::RenderHorizontalBlurToTexture() {
	float screenWidth = (float)_horizontalBlurTexture->GetTextureWidth();

	_horizontalBlurTexture->SetRenderTarget(md3dImmediateContext);
	_horizontalBlurTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_cameraEnd->GetViewMatrix());
	proj = XMLoadFloat4x4(&_horizontalBlurTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	_smallScreenWindow->Render(md3dImmediateContext);

	// set shader to horizontal blur tech
	_blurShader->SetTechniqueToHorizontal();

	// set cbuffer per object
	_blurShader->SetWorld(world);
	_blurShader->SetWorldInvTranspose(worldInvTranspose);
	_blurShader->SetWorldViewProj(worldViewProj);
	_blurShader->SetTexTransform(world);
	_blurShader->SetMaterial(_ufo->GetMaterial());
	_blurShader->SetDiffTexture(_downSampleTexture->GetShaderResourceView());
	_blurShader->SetScreenWidth(screenWidth);
	_blurShader->SetScreenHeigth(0);
	_blurShader->SetPadding(XMFLOAT3(0,0,0));

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	//_shader->SetTechnique("Light0Tex");
	_blurShader->Render(md3dImmediateContext, _smallScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::RenderVerticalBlurToTexture() {
	float screenHeight = (float)_verticalBlurTexture->GetTextureHeight();

	_verticalBlurTexture->SetRenderTarget(md3dImmediateContext);
	_verticalBlurTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_cameraEnd->GetViewMatrix());
	proj = XMLoadFloat4x4(&_verticalBlurTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	_smallScreenWindow->Render(md3dImmediateContext);

	// set shader to horizontal blur tech
	_blurShader->SetTechniqueToVertical();

	// set cbuffer per object
	_blurShader->SetWorld(world);
	_blurShader->SetWorldInvTranspose(worldInvTranspose);
	_blurShader->SetWorldViewProj(worldViewProj);
	_blurShader->SetTexTransform(world);
	_blurShader->SetMaterial(_ufo->GetMaterial());
	_blurShader->SetDiffTexture(_horizontalBlurTexture->GetShaderResourceView());
	_blurShader->SetScreenWidth(0);
	_blurShader->SetScreenHeigth(screenHeight);
	_blurShader->SetPadding(XMFLOAT3(0, 0, 0));

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	//_shader->SetTechnique("Light0Tex");
	_blurShader->Render(md3dImmediateContext, _smallScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::UpSampleTexture() {
	_upSampleTexture->SetRenderTarget(md3dImmediateContext);
	_upSampleTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_cameraEnd->GetViewMatrix());
	proj = XMLoadFloat4x4(&_upSampleTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	_fullScreenWindow->Render(md3dImmediateContext);

	// set cbuffer per object
	_shader->SetWorld(world);
	_shader->SetWorldInvTranspose(worldInvTranspose);
	_shader->SetWorldViewProj(worldViewProj);
	_shader->SetTexTransform(world);
	_shader->SetMaterial(_ufo->GetMaterial());
	_shader->SetDiffTexture(_verticalBlurTexture->GetShaderResourceView());

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	_shader->SetTechnique("Light0Tex");
	_shader->Render(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::RenderSceneToTexture(float rotation) {
	_renderTexture->SetRenderTarget(md3dImmediateContext);
	_renderTexture->ClearRenderTarget(md3dImmediateContext);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(_ufo->GetWorldMatrix());
	world = XMMatrixMultiply(world, XMMatrixRotationY(rotation));
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_camera->GetViewMatrix());
	proj = XMLoadFloat4x4(&_proj);
	worldViewProj = world*view*proj;

	TurnZBufferOff();

	// set cbuffer per object, for ufo
	_shader->SetWorld(world);
	_shader->SetWorldInvTranspose(worldInvTranspose);
	_shader->SetWorldViewProj(worldViewProj);
	_shader->SetMaterial(_ufo->GetMaterial());
	_shader->SetDiffTexture(_ufo->GetDiffTexture());

	// put the models vertex and index buffers on the pipeline, ready for rendering
	_ufo->Render(md3dImmediateContext);

	// render the model with the texture shader
	_shader->SetTechnique("Light3Tex");
	bool result = _shader->Render(md3dImmediateContext, _ufo->GetIndexCount());

	if (!result) { return false; }

	TurnZBufferOn();

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

bool BloomingApp::RenderGlowScene(float glowStrength)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// calculate the required matrices per object
	XMMATRIX world, worldInvTranspose, view, proj, worldViewProj;
	world = XMLoadFloat4x4(&_world);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	view = XMLoadFloat4x4(&_cameraEnd->GetViewMatrix());
	proj = XMLoadFloat4x4(&_upSampleTexture->GetOrthoMatrix());
	worldViewProj = world*view*proj;

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_fullScreenWindow->Render(md3dImmediateContext);

	// set cbuffer per object
	_glowShader->SetWorld(world);
	_glowShader->SetWorldInvTranspose(worldInvTranspose);
	_glowShader->SetWorldViewProj(worldViewProj);
	_glowShader->SetMaterial(_ufo->GetMaterial());
	_glowShader->SetDiffTexture(_renderTexture->GetShaderResourceView());
	_glowShader->SetGlowTexture(_upSampleTexture->GetShaderResourceView());
	_glowShader->SetGlowStrength(glowStrength);

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	_glowShader->Render(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	return true;
}

void BloomingApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BloomingApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BloomingApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 200.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
