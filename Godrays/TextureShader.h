#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "d3dUtil.h"

class TextureShader {
public:
	TextureShader();
	TextureShader(const TextureShader&);
	~TextureShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetPerFrame(const XMFLOAT3& eyePos, const DirectionalLight* lights) {
		_eyePosW->SetRawValue(&eyePos, 0, sizeof(XMFLOAT3));
		_dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight));
	}

	void SetPerObject(CXMMATRIX worldViewProj, CXMMATRIX world, CXMMATRIX worldInvTrans, CXMMATRIX texTransform,
		const Material& mat, ID3D11ShaderResourceView* tex) {
		_worldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));
		_world->SetMatrix(reinterpret_cast<const float*>(&world));
		_worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTrans));
		_mat->SetRawValue(&mat, 0, sizeof(Material));
		_diffTexture->SetResource(tex);
		_texTransform->SetMatrix(reinterpret_cast<const float*>(&texTransform));
	}

	void SetTechnique(const char* name) { _technique = _FX->GetTechniqueByName(name); }

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	ID3DX11Effect* _FX;
	ID3DX11EffectTechnique* _technique;
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];
	ID3D11InputLayout* _inputLayout;

	// cbuffer per object variables
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectMatrixVariable* _texTransform;
	ID3DX11EffectVariable* _mat;
	ID3DX11EffectShaderResourceVariable* _diffTexture;

	// cbuffer per frame variables
	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectVariable* _dirLights;
};

#endif
