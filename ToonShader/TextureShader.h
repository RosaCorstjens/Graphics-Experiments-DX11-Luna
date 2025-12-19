#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H
#include "BasicShader.h"

class TextureShader : public BasicShader 
{
public:
	TextureShader();
	TextureShader(const TextureShader&);
	~TextureShader();

	bool Initialize(ID3D11Device* device, HWND hwnd) override;
	// methods Shutdown and Render are implemented in super class

	// all setters for matrices e.d. are implemented in super class
	void SetDiffTexture(ID3D11ShaderResourceView* tex) { _diffTexture->SetResource(tex); }
	void SetTechnique(const char* name) { _technique = _FX->GetTechniqueByName(name); }

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) override;

	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];

	// per object textures 
	ID3DX11EffectShaderResourceVariable* _diffTexture;
	ID3DX11EffectShaderResourceVariable* _edgeTexture;
private:

};

#endif
