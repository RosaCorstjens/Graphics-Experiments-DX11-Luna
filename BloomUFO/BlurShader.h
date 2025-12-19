#ifndef HORZIONTAL_BLUR_SHADER
#define HORZIONTAL_BLUR_SHADER
#include "TextureShader.h"

class BlurShader : public TextureShader {
public:
	BlurShader();
	BlurShader(const BlurShader&);
	~BlurShader();

	bool Initialize(ID3D11Device* device, HWND hwnd) override;
	// methods Shutdown and Render are implemented in super class

	void SetScreenWidth(float width) { _screenWidth->SetFloat(width); }
	void SetScreenHeigth(float width) { _screenHeigth->SetFloat(width); }
	void SetPadding(const XMFLOAT3& v) { _padding->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetTechniqueToHorizontal() { _technique = _FX->GetTechniqueByName("HorizontalBlur"); }
	void SetTechniqueToVertical() { _technique = _FX->GetTechniqueByName("VerticalBlur"); }

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) override;
	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];

private:
	ID3DX11EffectScalarVariable* _screenWidth;
	ID3DX11EffectScalarVariable* _screenHeigth;
	ID3DX11EffectVectorVariable* _padding;
};

#endif
