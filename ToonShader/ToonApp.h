#pragma once
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Model.h"
#include "CameraClass.h"
#include "RenderTexture.h"
#include "OrthoWindow.h"
#include "EdgeShader.h"
#include "ToonShader.h"
#include "DeferredBuffer.h"
#include "MultipleRenderTargetsShader.h"

class ToonApp : public D3DApp
{
public:
	ToonApp(HINSTANCE hInstance);
	~ToonApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	// methods called on init
	void BuildMatrices();
	void SetLighting();

	// all render methods 
	void RenderSceneToTexture();
	void RenderEdgesToTexture();
	void RenderEnd();

	Model* _model;						// the model to render
	
	CameraClass* _camera;				// the camera that renders the scene
	CameraClass* _cameraEnd;			// the camera that renders the resulting texture on a plane
	
	DirectionalLight _dirLight;		// directional light

	DeferredBuffer *_deferredBuffer;
	RenderTexture *_edgeTexture;

	MultipleRenderTargetsShader* _multipleRTShader;
	EdgeShader* _edgeShader;
	ToonShader* _toonShader;

	OrthoWindow *_fullScreenWindow;		// the plane to render the 

	// matrices
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _ortho;
	XMFLOAT4X4 _proj;

	// input variables
	float mTheta;
	float mPhi;
	float mRadius;
	POINT mLastMousePos;
};
