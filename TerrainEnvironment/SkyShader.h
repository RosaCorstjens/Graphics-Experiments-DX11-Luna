#ifndef SKY_SHADER_H
#define SKY_SHADER_H

#include "d3dUtil.h"

class SkyShader {
public:
	SkyShader();
	~SkyShader();

	virtual bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { _cubeMap->SetResource(tex); }

	ID3D11InputLayout* GetInputLayOut() { return _inputLayout; }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	ID3DX11Effect* _FX;
	ID3DX11EffectTechnique* _technique;
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[1];
	ID3D11InputLayout* _inputLayout;

	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectShaderResourceVariable* _cubeMap;

};

#endif
