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
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount() { return _indexCount; }

private:
	bool InitializeBuffers(ID3D11Device* device, int width, int height);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;
};

#endif