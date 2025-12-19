#ifndef ISHADER_H
#define ISHADER_H

#include "d3dUtil.h"

class IShader {
public:
	virtual bool Initialize(ID3D11Device* device, HWND hwnd) = 0;
	virtual void Shutdown() = 0;
	virtual bool Render(ID3D11DeviceContext* deviceContext, int indexCount) = 0;

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) = 0;

	ID3DX11Effect* _FX;
	ID3DX11EffectTechnique* _technique;
	ID3D11InputLayout* _inputLayout;
};

#endif
