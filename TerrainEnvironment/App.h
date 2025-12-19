#pragma once

#include "d3dApp.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "TextureShader.h"
#include "Sky.h"
#include "SkyShader.h"
#include "Terrain.h"
#include "TerrainShader.h"
#include "Camera.h"

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

	Sky* _sky;
	Terrain _terrain;
	Camera _camera;
	DirectionalLight _dirLights[1];

	TextureShader* _shader;			
	SkyShader* _skyShader;
	TerrainShader* _terrainShader;

	// matrices
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _ortho;
	XMFLOAT4X4 _proj;

	// input varss
	POINT mLastMousePos;
};
