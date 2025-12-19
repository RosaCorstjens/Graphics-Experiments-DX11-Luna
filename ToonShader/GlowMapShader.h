#ifndef GLOWMAP_SHADER_H
#define GLOWMAP_SHADER_H
#include "TextureShader.h"

class GlowMapShader : public TextureShader
{
public:
	GlowMapShader();
	GlowMapShader(const GlowMapShader&);
	~GlowMapShader();

	bool Initialize(ID3D11Device* device, HWND hwnd) override;
	// methods Shutdown and Render are implemented in super class

	// all setters for matrices e.d. are implemented in super class
	void SetGlowTexture(ID3D11ShaderResourceView* tex) { _glowTexture->SetResource(tex); }

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) override;

	// per object textures 
	ID3DX11EffectShaderResourceVariable* _glowTexture;

};

#endif
