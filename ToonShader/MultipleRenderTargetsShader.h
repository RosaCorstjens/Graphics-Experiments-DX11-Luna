#ifndef MULTIPLE_RENDER_TARGETS_SHADER_H
#define MULTIPLE_RENDER_TARGETS_SHADER_H

#include "d3dUtil.h"

class MultipleRenderTargetsShader {
public:
	MultipleRenderTargetsShader();
	MultipleRenderTargetsShader(const MultipleRenderTargetsShader&);
	~MultipleRenderTargetsShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { _world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { _worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetMaterial(const Material& mat) { _mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffTexture(ID3D11ShaderResourceView* tex) { _diffTexture->SetResource(tex); }

	void SetEyePosW(const XMFLOAT3& v) { _eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight* lights) { _dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetDiffuseLevels(float diffuseLevels) { _diffuseLevels->SetFloat(diffuseLevels); }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	ID3DX11Effect* _FX;
	ID3DX11EffectTechnique* _technique;

	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];
	ID3D11InputLayout* _inputLayout;

	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectVariable* _mat;
	ID3DX11EffectShaderResourceVariable* _diffTexture;

	// cbuffer per frame variables
	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectVariable* _dirLights;
	ID3DX11EffectScalarVariable* _diffuseLevels;
};

#endif

