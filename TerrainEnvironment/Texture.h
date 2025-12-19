#ifndef TEXTURE_H
#define TEXTURE_H
#include <d3dx11tex.h>

// Stores a texture and has some convinient methods. 
class Texture {
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device* device, WCHAR* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* _texture;
};

#endif
