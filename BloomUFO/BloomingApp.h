#pragma once
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Model.h"
#include "CameraClass.h"
#include "RenderTexture.h"
#include "TextureShader.h"
#include "OrthoWindow.h"
#include "BlurShader.h"
#include "GlowMapShader.h"
#include "GlowShader.h"

class BloomingApp : public D3DApp
{
public:
	BloomingApp(HINSTANCE hInstance);
	~BloomingApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	bool DownSampleTexture();
	bool RenderGlowMapToTexture(float rotation);
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool RenderSceneToTexture(float rotation);
	bool RenderGlowScene(float glowStrength);

	void BuildMatrices();
	void SetLighting();

	Model* _ufo;						// the model to render
	CameraClass* _camera;					// scenes main camera
	CameraClass* _cameraEnd;					// scenes main camera
	DirectionalLight _dirLights[3];		// directional lights

	RenderTexture *_renderTexture, *_downSampleTexture, *_upSampleTexture, *_horizontalBlurTexture, *_verticalBlurTexture;
	OrthoWindow *_fullScreenWindow, *_smallScreenWindow;

	TextureShader* _shader;				// used shader
	BlurShader* _blurShader;
	GlowMapShader* _glowMapShader;
	GlowShader* _glowShader;

	// matrices
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _ortho;
	XMFLOAT4X4 _proj;

	// input varss
	float mTheta;
	float mPhi;
	float mRadius;
	POINT mLastMousePos;
};
