#ifndef MODEL_H
#define MODEL_H

#include "d3dApp.h"
#include "Texture.h"
#include "Vertex.h"

class Model {
public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device* deviceContext, char* modelFilename, WCHAR* diffTextureFilename,
		XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 reflect,
		XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation, XMFLOAT2 textureTransform = XMFLOAT2(1, 1));
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return _indexCount; }
	ID3D11ShaderResourceView* GetDiffTexture() { return _diffTexture->GetTexture(); }
	Material GetMaterial() { return _material; }
	XMFLOAT4X4* GetWorldMatrix() { return &_worldMatrix; }
	XMFLOAT4X4* GetTextureTransform() { return &_texTransform; }

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTextures();

	bool SetMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 reflect);

	bool LoadModel(char*);
	void ReleaseModel();

	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;

	Texture* _diffTexture;
	Material _material;
	Vertex::Vertex* _model;
	XMFLOAT4X4 _worldMatrix;
	XMFLOAT4X4 _texTransform;
};

#endif