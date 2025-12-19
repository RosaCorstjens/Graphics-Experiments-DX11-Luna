#pragma once

#include "d3dApp.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Model.h"
#include "CameraClass.h"
#include "TextureShader.h"
#include "Sky.h"
#include "SkyShader.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "DepthShader.h"
#include "OrthoWindow.h"
#include "GodRayShader.h"
#include "Primitive.h"

class App : public D3DApp
{
public:
	App(HINSTANCE hInstance);
	~App();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildMatrices();
	void SetLighting();

	// draw methods
	void RenderSceneToDepthTexture();
	void RenderSceneToDiffTexture();
	void RenderMask();
	void RenderBlur();
	void RenderEnd();

	// content in the scene
	Model* _model;		
	Primitive* _primitive;
	Sky* _sky;
	DirectionalLight _dirLights[1];

	// camera's to render world and fullscreen window
	Camera _camera;
	CameraClass* _endCam;

	// RT's for the different passes
	RenderTexture* _depthTexture;
	RenderTexture* _diffTexture;
	RenderTexture* _shaftTexture;
	RenderTexture* _blurredTexture;

	// shaders for the different passes
	TextureShader* _textureShader;			
	SkyShader* _skyShader;
	DepthShader* _depthShader;
	GodRayShader* _godRayShader;

	// full screen window for end rendering
	OrthoWindow *_fullScreenWindow;

	// matrices
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _ortho;
	XMFLOAT4X4 _proj;

	// input vars
	POINT mLastMousePos;
};
