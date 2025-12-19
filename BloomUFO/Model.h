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

	bool Initialize(ID3D11Device* deviceContext, char* modelFilename, 
		WCHAR* diffTextureFilename, WCHAR* glowTextureFilename, 
		XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular,
		XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return _indexCount; }
	
	ID3D11ShaderResourceView* GetDiffTexture() { return _diffTexture->GetTexture(); }
	ID3D11ShaderResourceView* GetGlowTexture() { return _glowTexture->GetTexture(); }
	
	Material GetMaterial() { return _material; }

	XMFLOAT4X4* GetWorldMatrix() { return &_worldMatrix; }

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();

	bool SetMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular);

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;

	Texture* _diffTexture;
	Texture* _glowTexture;

	Material _material;

	Vertex::Vertex* _model;

	XMFLOAT4X4 _worldMatrix;
};

#endif