#include "Texture.h"
#include "d3dUtil.h"

Texture::Texture() : _texture(0) {}

Texture::Texture(const Texture& other) { }

Texture::~Texture() { }

bool Texture::Initialize(ID3D11Device* device, WCHAR* filename) {
	HR(D3DX11CreateShaderResourceViewFromFile(device, filename, 0, 0, &_texture, 0));
	return true;
}


void Texture::Shutdown() {
	ReleaseCOM(_texture);
	return;
}


ID3D11ShaderResourceView* Texture::GetTexture() {
	return _texture;
}