#ifndef DEFERRED_BUFFER_H
#define DEFERRED_BUFFER_H

#include "d3dUtil.h"

const int BUFFER_COUNT = 4;

// a deferred buffer is like a render texture, but it can contain multiple
class DeferredBuffer {
public:
	DeferredBuffer();
	DeferredBuffer(const DeferredBuffer& other){}
	~DeferredBuffer(){}

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext);

	ID3D11ShaderResourceView* GetShaderResourceView(int index);

private:
	int _textureWidth, _textureHeight;

	ID3D11Texture2D* _renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* _renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* _shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	D3D11_VIEWPORT _viewport;
};

#endif
