#include "Model.h"

Model::Model() : _vertexBuffer(0), _indexBuffer(0), 
	_vertexCount(0), _indexCount(0), 
	_diffTexture(0), _glowTexture(0), _model(0) {}

Model::Model(const Model&) {}

Model::~Model() {}

bool Model::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* glowFilename,
						XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular,
						XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation)
{
	bool result;

	// Load in the model data.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	// Load the diff texture for this model.
	result = LoadTextures(device, textureFilename, glowFilename);
	if (!result) { return false; }

	// Set the material
	result = SetMaterial(ambient, diffuse, specular);
	if(!result) { return false; }

	// build matrix
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX posMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX intermediateMatrix = XMMatrixMultiply(scaleMatrix, rotMatrix);
	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(intermediateMatrix, posMatrix));

	return true;
}

void Model::Shutdown()
{
	ReleaseTextures();
	ShutdownBuffers();
	ReleaseModel();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

bool Model::InitializeBuffers(ID3D11Device* deviceContext) {
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Vertex) * _vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _model;
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;
	HR(deviceContext->CreateBuffer(&vbd, &vinitData, &_vertexBuffer));

	std::vector<UINT> indices;
	for (int i = 0; i<_vertexCount; i++)
	{
		indices.push_back(i);
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * _indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;
	HR(deviceContext->CreateBuffer(&ibd, &iinitData, &_indexBuffer));

	return true;
}

void Model::ShutdownBuffers() {
	ReleaseCOM(_vertexBuffer);
	ReleaseCOM(_indexBuffer);

	return;
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex::Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Model::LoadTextures(ID3D11Device* deviceContext, WCHAR* diffFilename, WCHAR* glowFilename) {
	_diffTexture = new Texture;
	_glowTexture = new Texture;

	if (!_diffTexture || !_glowTexture) { return false; }

	bool result = _diffTexture->Initialize(deviceContext, diffFilename);
	if (!result) { return false; }

	result = _glowTexture->Initialize(deviceContext, glowFilename);
	if (!result) { return false; }

	return true;
}

void Model::ReleaseTextures() {
	if (_diffTexture) {
		_diffTexture->Shutdown();
		delete _diffTexture;
		_diffTexture = 0;
	}

	if (_glowTexture) {
		_glowTexture->Shutdown();
		delete _glowTexture;
		_glowTexture = 0;
	}

	return;
}

bool Model::SetMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular) {
	_material.Ambient = ambient; 
	_material.Diffuse = diffuse; 
	_material.Specular = specular; 

	return true;
}

bool Model::LoadModel(char* filename){
	std::ifstream fin;
	char input;
	int i;

	// open file
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> _vertexCount;

	_indexCount = _vertexCount;

	_model = new Vertex::Vertex[_vertexCount];
	if (!_model) { return false; }

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < _vertexCount; i++)
	{
		fin >> _model[i].Pos.x >> _model[i].Pos.y >> _model[i].Pos.z;
		fin >> _model[i].DiffTex.x >> _model[i].DiffTex.y;
		_model[i].GlowTex.x = _model[i].DiffTex.x;
		_model[i].GlowTex.y = _model[i].DiffTex.y;
		fin >> _model[i].Normal.x >> _model[i].Normal.y >> _model[i].Normal.z;
	}

	fin.close();

	return true;
}

void Model::ReleaseModel() {
	if (_model)
	{
		delete[] _model;
		_model = 0;
	}

	return;
}