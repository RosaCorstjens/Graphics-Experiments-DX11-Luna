#ifndef TOON_SHADER_H
#define TOON_SHADER_H

#include "d3dUtil.h"

class ToonShader {
public:
	ToonShader();
	ToonShader(const ToonShader&);
	~ToonShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { _world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { _worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetMaterial(const Material& mat) { _mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffTexture(ID3D11ShaderResourceView* tex) { _diffTexture->SetResource(tex); }
	void SetEdgeTexture(ID3D11ShaderResourceView* tex) { _edgeTexture->SetResource(tex); }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	
	// the effect
	ID3DX11Effect* _FX;

	// a technique contains a vs, gs and ps
	ID3DX11EffectTechnique* _technique;

	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];
	ID3D11InputLayout* _inputLayout;

	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectVariable* _mat;

	// per object textures 
	ID3DX11EffectShaderResourceVariable* _diffTexture;
	ID3DX11EffectShaderResourceVariable* _edgeTexture;
};
#endif
