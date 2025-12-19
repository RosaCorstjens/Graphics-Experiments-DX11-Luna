#include "RenderTexture.h"

RenderTexture::RenderTexture() : _renderTargetTexture(0), _renderTargetView(0), 
_shaderResourceView(0), _depthStencilBuffer(0), _depthStencilView(0) {}

bool RenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear) {
	_textureWidth = textureWidth;
	_textureHeight = textureHeight;
	
	// describe the texture to render to
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = textureWidth;
	texDesc.Height = textureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// create the texture 
	HR(device->CreateTexture2D(&texDesc, NULL, &_renderTargetTexture));

	// describe render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// create the render target view
	HR(device->CreateRenderTargetView(_renderTargetTexture, &renderTargetViewDesc, &_renderTargetView));

	// describe shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = texDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// create shader resource view
	HR(device->CreateShaderResourceView(_renderTargetTexture, &shaderResourceViewDesc, &_shaderResourceView));

	// describe the depth stencil buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// create the depth stencil buffer
	HR(device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer));

	// describe the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// create the depth stencil view
	HR(device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView));

	// Setup the viewport for rendering.
	_viewport.Width = (float)textureWidth;
	_viewport.Height = (float)textureHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixPerspectiveFovLH(((float)MathHelper::Pi / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth));

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&_orthoMatrix, XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth));

	return true;
}

void RenderTexture::Shutdown() {
	ReleaseCOM(_renderTargetTexture);
	ReleaseCOM(_renderTargetView);
	ReleaseCOM(_shaderResourceView);
	ReleaseCOM(_depthStencilBuffer);
	ReleaseCOM(_depthStencilView);
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// bind the render target view and depth stencil buffer to the output render pipeline
	deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// set the viewport
	deviceContext->RSSetViewports(1, &_viewport);

	return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, const float* color)
{
	// clear back buffer
	deviceContext->ClearRenderTargetView(_renderTargetView, color);//(&Colors::Black));

	// clear depth buffer
	deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}