#ifndef DEPTH_SHADER_H
#define DEPTH_SHADER_H

#include "d3dUtil.h"

// do not inherit from basicshader since many of the variables and setters are not needed.
// do implement the same methods to enable working the same with this shader as the others.
// TODO: shaders should implement an interface, but do not inherit anymore! It's just to confusing.
class DepthShader {
public:
	DepthShader();
	DepthShader(const DepthShader&);
	~DepthShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { _world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { _worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetEyePosW(const XMFLOAT3& v) { _eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	
	// the effect
	ID3DX11Effect* _FX;

	// a technique contains a vs, gs and ps
	// a basic tech with 3 lights and color
	ID3DX11EffectTechnique* _technique;

	// the input layout
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[1];
	ID3D11InputLayout* _inputLayout;

	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;

	// cbuffer per frame variables
	ID3DX11EffectVectorVariable* _eyePosW;
};

#endif
