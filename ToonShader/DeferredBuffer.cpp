#include "DeferredBuffer.h"

DeferredBuffer::DeferredBuffer()
{
	int i;

	for (i = 0; i<BUFFER_COUNT; i++)
	{
		_renderTargetTextureArray[i] = 0;
		_renderTargetViewArray[i] = 0;
		_shaderResourceViewArray[i] = 0;
	}

	_depthStencilBuffer = 0;
	_depthStencilView = 0; 
}

bool DeferredBuffer::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear) {
	// store the w and h of the render texture
	_textureWidth = textureWidth;
	_textureHeight = textureHeight;

	// describe the texture to render to
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = textureWidth;
	texDesc.Height = textureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R32G32B32A32_FLOAT
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	int i;

	// create the render target textures
	for (i = 0; i<BUFFER_COUNT; i++)
	{
		HR(device->CreateTexture2D(&texDesc, NULL, &_renderTargetTextureArray[i]));
	}

	// describe render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// create the render target views
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		HR(device->CreateRenderTargetView(_renderTargetTextureArray[i], &renderTargetViewDesc, &_renderTargetViewArray[i]));
	}

	// describe shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = texDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// create shader resource views
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		HR(device->CreateShaderResourceView(_renderTargetTextureArray[i], &shaderResourceViewDesc, &_shaderResourceViewArray[i]));
	}

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

	return true;
}

void DeferredBuffer::Shutdown()
{
	int i;

	if (_depthStencilView)
	{
		_depthStencilView->Release();
		_depthStencilView = 0;
	}

	if (_depthStencilBuffer)
	{
		_depthStencilBuffer->Release();
		_depthStencilBuffer = 0;
	}

	for (i = 0; i<BUFFER_COUNT; i++)
	{
		if (_shaderResourceViewArray[i])
		{
			_shaderResourceViewArray[i]->Release();
			_shaderResourceViewArray[i] = 0;
		}

		if (_renderTargetViewArray[i])
		{
			_renderTargetViewArray[i]->Release();
			_renderTargetViewArray[i] = 0;
		}

		if (_renderTargetTextureArray[i])
		{
			_renderTargetTextureArray[i]->Release();
			_renderTargetTextureArray[i] = 0;
		}
	}

	return;
}

void DeferredBuffer::SetRenderTargets(ID3D11DeviceContext* deviceContext) {
	// bind the render target view array and depth stencil buffer to the output render pipeline
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, _renderTargetViewArray, _depthStencilView);

	// set the viewport
	deviceContext->RSSetViewports(1, &_viewport);

	return;
}

void DeferredBuffer::ClearRenderTargets(ID3D11DeviceContext* deviceContext)
{
	// Clear the render target buffers.
	// The first buffer is the diffuse texture color, init to black
	deviceContext->ClearRenderTargetView(_renderTargetViewArray[0], reinterpret_cast<const float*>(&Colors::Black));
	
	// Second is normal, init to grey
	deviceContext->ClearRenderTargetView(_renderTargetViewArray[1], reinterpret_cast<const float*>(&Colors::Grey));

	// Third is depth, init to white
	deviceContext->ClearRenderTargetView(_renderTargetViewArray[2], reinterpret_cast<const float*>(&Colors::White));

	// Forth is toon and later on actually presented, init to whatever color you like
	deviceContext->ClearRenderTargetView(_renderTargetViewArray[3], reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	// If there are other buffers left, init them to white
	//for (int i = 0; i<BUFFER_COUNT; i++)
	//{
		//deviceContext->ClearRenderTargetView(_renderTargetViewArray[i], color);
	//}

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

ID3D11ShaderResourceView* DeferredBuffer::GetShaderResourceView(int view)
{
	return _shaderResourceViewArray[view];
}