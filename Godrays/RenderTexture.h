#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "d3dUtil.h"

class RenderTexture {
public:
	RenderTexture();
	RenderTexture(const RenderTexture&) {}
	~RenderTexture() {} 

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, const float* color);

	ID3D11ShaderResourceView* GetShaderResourceView() { return _shaderResourceView; }
	int GetTextureWidth() { return _textureHeight; }
	int GetTextureHeight() { return _textureWidth; }
	XMFLOAT4X4 GetProjectionMatrix() { return _projectionMatrix; }
	XMFLOAT4X4 GetOrthoMatrix() { return _orthoMatrix; }

private:
	int _textureWidth, _textureHeight;
	
	ID3D11Texture2D* _renderTargetTexture;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11ShaderResourceView* _shaderResourceView;

	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;

	D3D11_VIEWPORT _viewport;
	XMFLOAT4X4 _projectionMatrix;
	XMFLOAT4X4 _orthoMatrix;
};

#endif
