#ifndef EDGE_SHADER_H
#define EDGE_SHADER_H

#include "d3dUtil.h"

class EdgeShader {
public:
	EdgeShader();
	EdgeShader(const EdgeShader&);
	~EdgeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { _world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { _worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetDiffTexture(ID3D11ShaderResourceView* tex) { _diffTexture->SetResource(tex); }
	void SetNormalTexture(ID3D11ShaderResourceView* tex) { _normalTexture->SetResource(tex); }
	void SetDepthTexture(ID3D11ShaderResourceView* tex) { _depthTexture->SetResource(tex); }

	void SetEyePosW(const XMFLOAT3& v) { _eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetScreenWidth(float width) { _screenWidth->SetFloat(width); }
	void SetScreenHeigth(float width) { _screenHeigth->SetFloat(width); }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	// the effect
	ID3DX11Effect* _FX;

	// a technique contains a vs, gs and ps
	ID3DX11EffectTechnique* _technique;

	// the input layout
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];
	ID3D11InputLayout* _inputLayout;

	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;

	// textures
	ID3DX11EffectShaderResourceVariable* _diffTexture;
	ID3DX11EffectShaderResourceVariable* _normalTexture;
	ID3DX11EffectShaderResourceVariable* _depthTexture;

	// cbuffer per frame variables
	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectScalarVariable* _screenWidth;
	ID3DX11EffectScalarVariable* _screenHeigth;
};

#endif
