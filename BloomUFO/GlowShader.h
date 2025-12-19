#ifndef GLOW_SHADER_H
#define GLOW_SHADER_H
#include "TextureShader.h"

class GlowShader : public TextureShader
{
public:
	GlowShader();
	GlowShader(const GlowShader &);
	~GlowShader();

	bool Initialize(ID3D11Device* device, HWND hwnd) override;
	// methods Shutdown and Render are implemented in super class

	// all setters for matrices e.d. are implemented in super class
	void SetGlowTexture(ID3D11ShaderResourceView* tex) { _glowTexture->SetResource(tex); }
	void SetGlowStrength(float value) { _glowStrength->SetFloat(value); }

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) override;

	// per object textures 
	ID3DX11EffectShaderResourceVariable* _glowTexture;

	// per frame variables
	ID3DX11EffectScalarVariable* _glowStrength;
	ID3DX11EffectVectorVariable* _padding;
};

#endif
