#include "ToonApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd){
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	ToonApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

/// <summary>
/// Initializes a new instance of the <see cref="ToonApp"/> class.
/// </summary>
/// <param name="hInstance">The h instance.</param>
ToonApp::ToonApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mTheta(1.5f*MathHelper::Pi), mPhi(0.5f*MathHelper::Pi), mRadius(10.0f),
	_model(0), _camera(0), _cameraEnd(0), _fullScreenWindow(0)
{
	mMainWndCaption = L"Toon Application";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	_deferredBuffer = 0;
	_edgeTexture = 0;
	_multipleRTShader = 0;
	_edgeShader = 0;
	_toonShader = 0;	

	SetLighting();
	BuildMatrices();
}

/// <summary>
/// Finalizes an instance of the <see cref="ToonApp"/> class.
/// </summary>
ToonApp::~ToonApp()
{
	if(_model) {
		_model->Shutdown();
		delete _model;
		_model = 0;
	}
	
	if (_multipleRTShader) {
		_multipleRTShader->Shutdown();
		delete _multipleRTShader;
		_multipleRTShader = 0;
	}
	
	if (_edgeShader) {
		_edgeShader->Shutdown();
		delete _edgeShader;
		_edgeShader = 0;
	}

	if (_toonShader)
	{
		_toonShader->Shutdown();
		delete _toonShader;
		_toonShader = 0;
	}

	if (_deferredBuffer) {
		_deferredBuffer->Shutdown();
		delete _deferredBuffer;
		_deferredBuffer = 0;
	}
	
	if(_edgeTexture) {
		_edgeTexture->Shutdown();
		delete _edgeTexture;
		_edgeTexture = 0;
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
void ToonApp::BuildMatrices() {
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&_world, I);

	// Setup the projection matrix.
	XMStoreFloat4x4(&_proj, XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f));

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&_ortho, XMMatrixOrthographicLH((float)mClientWidth, (float)mClientHeight, 0.1f, 1000.0f));
}

/// <summary>
/// Initializes the lights.
/// </summary>
void ToonApp::SetLighting() {
	_dirLight.Ambient = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	_dirLight.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	_dirLight.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	_dirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool ToonApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// initialize shaders
	_multipleRTShader = new MultipleRenderTargetsShader;
	_multipleRTShader->Initialize(md3dDevice, mhMainWnd);
	_edgeShader = new EdgeShader;
	_edgeShader->Initialize(md3dDevice, mhMainWnd);
	_toonShader = new ToonShader;
	_toonShader->Initialize(md3dDevice, mhMainWnd);

	// initialize render to textures
	_edgeTexture = new RenderTexture;
	_edgeTexture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000, 0.1);
	_deferredBuffer = new DeferredBuffer;
	_deferredBuffer->Initialize(md3dDevice, mClientWidth, mClientHeight, 1000, 0.1);

	// initialize model
	_model = new Model;
	_model->Initialize(md3dDevice, "UFO.txt", L"UFO2.jpg",
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.8f, 50.0f), XMFLOAT4(1, 1, 1, 1),
		XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.02f, 0.02f, 0.02f), XMFLOAT3(0,0,0));//XMFLOAT3(-MathHelper::Pi / 2.0f, -MathHelper::Pi / 3.0f, 0.0f));

	// initialize full screen plane
	_fullScreenWindow = new OrthoWindow;
	_fullScreenWindow->Initialize(md3dDevice, mClientWidth, mClientHeight);

	// initialize cameras
	_camera = new CameraClass;
	_cameraEnd = new CameraClass;

	// convert spherical to cartesian coordinates
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// set the camera positions
	_camera->SetPosition(x, y, z);
	_cameraEnd->SetPosition(x, y, z);

	return true;
}

/// <summary>
/// Called on resize of the screen space.
/// </summary>
void ToonApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_proj, P);
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt">The deltatime.</param>
void ToonApp::UpdateScene(float dt)
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
void ToonApp::DrawScene()
{
	// gen the view matrix based on cam pos
	_camera->Render();
	_cameraEnd->Render();

	// reset render targets
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
	RenderSceneToTexture();
	RenderEdgesToTexture();
	RenderEnd();

	// end scene
	HR(mSwapChain->Present(0, 0));
}

/// <summary>
/// Renders the 3d scene to the gbuffer textures.
/// </summary>
void ToonApp::RenderSceneToTexture() {
	// set and clear buffer
	_deferredBuffer->SetRenderTargets(md3dImmediateContext);
	_deferredBuffer->ClearRenderTargets(md3dImmediateContext);

	// set cbuffer variables per frame
	_multipleRTShader->SetDirLights(&_dirLight);
	_multipleRTShader->SetEyePosW(_camera->GetPosition());

	// set cbuffer variables via shader
	_multipleRTShader->SetWorld(XMLoadFloat4x4(_model->GetWorldMatrix()));
	_multipleRTShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(_model->GetWorldMatrix())));
	_multipleRTShader->SetWorldViewProj(XMLoadFloat4x4(_model->GetWorldMatrix()) *  XMLoadFloat4x4(&_camera->GetViewMatrix()) * XMLoadFloat4x4(&_proj));
	_multipleRTShader->SetMaterial(_model->GetMaterial());
	_multipleRTShader->SetDiffTexture(_model->GetDiffTexture());
	_multipleRTShader->SetDiffuseLevels(3);

	// put the vertix and index buffers in the pipeline
	_model->Render(md3dImmediateContext);

	_multipleRTShader->Render(md3dImmediateContext, _model->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	
	return;
}

/*void ToonApp::RenderSceneToDepthTexture() {
	// set and clear the render target
	_depthTexture->SetRenderTarget(md3dImmediateContext);
	_depthTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));

	// set cbuffer variables via shader
	_depthShader->SetWorld(XMLoadFloat4x4(_model->GetWorldMatrix()));
	_depthShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(_model->GetWorldMatrix())));
	_depthShader->SetWorldViewProj(XMLoadFloat4x4(_model->GetWorldMatrix()) *  XMLoadFloat4x4(&_camera->GetViewMatrix()) * XMLoadFloat4x4(&_proj));

	// put the vertix and index buffers in the pipeline
	_model->Render(md3dImmediateContext);

	// render the pipeline contents
	_depthShader->Render(md3dImmediateContext, _model->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return;
}

void ToonApp::RenderSceneToNormalTexture() {
	// set and clear the render target
	_normalTexture->SetRenderTarget(md3dImmediateContext);
	_normalTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::Grey));

	// set cbuffer variables via shader
	_normalShader->SetWorld(XMLoadFloat4x4(_model->GetWorldMatrix()));
	_normalShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(_model->GetWorldMatrix())));
	_normalShader->SetWorldViewProj(XMLoadFloat4x4(_model->GetWorldMatrix()) *  XMLoadFloat4x4(&_camera->GetViewMatrix()) * XMLoadFloat4x4(&_proj));

	// put the vertix and index buffers in the pipeline
	_model->Render(md3dImmediateContext);

	// render the pipeline contents
	_normalShader->Render(md3dImmediateContext, _model->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return;
}

void ToonApp::RenderSceneToDiffTexture() {
	// set and clear the render target
	_diffTexture->SetRenderTarget(md3dImmediateContext);
	_diffTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	// set cbuffer variables via shader
	_textureShader->SetTechnique("ToonLightTex");
	_textureShader->SetWorld(XMLoadFloat4x4(_model->GetWorldMatrix()));
	_textureShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(_model->GetWorldMatrix())));
	_textureShader->SetWorldViewProj(XMLoadFloat4x4(_model->GetWorldMatrix()) *  XMLoadFloat4x4(&_camera->GetViewMatrix()) * XMLoadFloat4x4(&_proj));
	_textureShader->SetMaterial(_model->GetMaterial());
	_textureShader->SetDiffTexture(_model->GetDiffTexture());

	// put the vertix and index buffers in the pipeline
	_model->Render(md3dImmediateContext);

	// render the pipeline contents
	_textureShader->Render(md3dImmediateContext, _model->GetIndexCount());

	// reset rendertarget and viewport
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return;
}
*/
void ToonApp::RenderEdgesToTexture() {
	_edgeTexture->SetRenderTarget(md3dImmediateContext);
	_edgeTexture->ClearRenderTarget(md3dImmediateContext, reinterpret_cast<const float*>(&Colors::White));

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	_fullScreenWindow->Render(md3dImmediateContext);
	
	// set cbuffer variables per frame
	_edgeShader->SetEyePosW(_camera->GetPosition());
	_edgeShader->SetScreenWidth(mClientWidth);
	_edgeShader->SetScreenHeigth(mClientHeight);

	// set cbuffer per object
	_edgeShader->SetWorld(XMLoadFloat4x4(&_world));
	_edgeShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&_world)));
	_edgeShader->SetWorldViewProj(XMLoadFloat4x4(&_world) *  XMLoadFloat4x4(&_cameraEnd->GetViewMatrix()) * XMLoadFloat4x4(&_edgeTexture->GetOrthoMatrix()));
	_edgeShader->SetDiffTexture(_deferredBuffer->GetShaderResourceView(0));
	_edgeShader->SetNormalTexture(_deferredBuffer->GetShaderResourceView(1));
	_edgeShader->SetDepthTexture(_deferredBuffer->GetShaderResourceView(2));

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	_edgeShader->Render(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void ToonApp::RenderEnd() {
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Turn off the Z buffer to begin all 2D rendering.
	TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_fullScreenWindow->Render(md3dImmediateContext);

	// set cbuffer per object
	_toonShader->SetWorld(XMLoadFloat4x4(&_world));
	_toonShader->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&_world)));
	_toonShader->SetWorldViewProj(XMLoadFloat4x4(&_world) * XMLoadFloat4x4(&_cameraEnd->GetViewMatrix()) * XMLoadFloat4x4(&_edgeTexture->GetOrthoMatrix()));
	_toonShader->SetDiffTexture(_deferredBuffer->GetShaderResourceView(3));
	_toonShader->SetEdgeTexture(_edgeTexture->GetShaderResourceView());

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	_toonShader->Render(md3dImmediateContext, _fullScreenWindow->GetIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	TurnZBufferOn();
}

void ToonApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void ToonApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ToonApp::OnMouseMove(WPARAM btnState, int x, int y)
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
