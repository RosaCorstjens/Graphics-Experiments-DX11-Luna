#ifndef GOD_RAY_SHADER
#define GOD_RAY_SHADER

#include "d3dUtil.h"
#include "IShader.h"

class GodRayShader {
public:
	GodRayShader();
	GodRayShader(const GodRayShader&);
	~GodRayShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	bool RenderMask(ID3D11DeviceContext* deviceContext, int indexCount);
	bool RenderBlur(ID3D11DeviceContext* deviceContext, int indexCount);
	bool RenderFinalMix(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetLightPosition(const XMFLOAT2& v) { _lightPos->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }
	void SetScreenSize(const XMFLOAT2& v) { _screenSize->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }
	void SetAspectRatio(float x) { _aspectRatio->SetFloat(x); }
	void SetBlend(float x) { _blend->SetFloat(x); }
	void SetIntensity(float x) { _intensity->SetFloat(x); }

	void SetDiffTexture(ID3D11ShaderResourceView* tex) { _diffuseTexture->SetResource(tex); }
	void SetDepthTexture(ID3D11ShaderResourceView* tex) { _depthTexture->SetResource(tex); }
	void SetShaftTexture(ID3D11ShaderResourceView* tex) { _shaftTexture->SetResource(tex); }

	void SetWorldViewProj(CXMMATRIX M) { _worldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3D11InputLayout* GetInputLayout() { return _inputLayout; }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const char* techName);

	// effect is GodRays.fx
	ID3DX11Effect* _FX;

	// technique is obtained at runtime, switches depending on called Render method
	ID3DX11EffectTechnique* _technique;
	
	// all three techniques (luckily) use the same inputlayout
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[2];
	ID3D11InputLayout* _inputLayout;
	
	// all effect variables, for each is noted in which technique they are used
	// 1: mask tech
	// 2: blur tech
	// 3: final mix tech
	ID3DX11EffectVectorVariable* _lightPos;						// 2
	ID3DX11EffectVectorVariable* _screenSize;					// 1, 2
	ID3DX11EffectScalarVariable* _aspectRatio;					// 2 (?)	
	ID3DX11EffectScalarVariable* _blend;						// (?)
	ID3DX11EffectScalarVariable* _intensity;					// 2

	ID3DX11EffectShaderResourceVariable* _diffuseTexture;		// 1, 3
	ID3DX11EffectShaderResourceVariable* _depthTexture;			// 1
	ID3DX11EffectShaderResourceVariable* _shaftTexture;			// 2, 3

	ID3DX11EffectMatrixVariable* _worldViewProj;				// 1, 2, 3
};

#endif
