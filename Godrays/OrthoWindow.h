#ifndef ORTHO_WINDOW_H
#define ORTHO_WINDOW_H

#include "d3dUtil.h"

class OrthoWindow {
public:
	OrthoWindow();
	OrthoWindow(const OrthoWindow&);
	~OrthoWindow();

	bool Initialize(ID3D11Device* device, int width, int height);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, ID3D11InputLayout* inputlayout);

	int GetIndexCount() { return _indexCount; }

	XMFLOAT2 GetDimensions() { return XMFLOAT2(width, height); }

private:
	bool InitializeBuffers(ID3D11Device* device, int width, int height);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext, ID3D11InputLayout* inputlayout);

	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;

	int width, height;
};

#endif