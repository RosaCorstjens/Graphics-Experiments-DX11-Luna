#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H

#include "d3dUtil.h"

class BasicShader {
public:
	BasicShader();
	BasicShader(const BasicShader&);
	~BasicShader();

	virtual bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { _world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { _worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M) { _texTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { _eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight* lights) { _dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { _mat->SetRawValue(&mat, 0, sizeof(Material)); }

	ID3D11InputLayout* GetInputLayout() { return _inputLayout; }

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	// the effect
	ID3DX11Effect* _FX;

	// a technique contains a vs, gs and ps
	// a basic tech with 3 lights and color
	ID3DX11EffectTechnique* _technique;			

	// the input layout
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[2];
	ID3D11InputLayout* _inputLayout;
	
	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectMatrixVariable* _texTransform;
	ID3DX11EffectVariable* _mat;

	// cbuffer per frame variables
	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectVariable* _dirLights;	
};

#endif
