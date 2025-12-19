#include "Model.h"
#include "Primitive.h"
#include "GeometryGenerator.h"

Primitive::Primitive() : _vertexBuffer(0), _indexBuffer(0),
_vertexCount(0), _indexCount(0), _diffTexture(0), _model(0) {}

Primitive::Primitive(const Primitive&) {}

Primitive::~Primitive() {}

bool Primitive::Initialize(ID3D11Device* device, Type type, WCHAR* textureFilename,
	XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 reflect,
	XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation, XMFLOAT2 textureTransform)
{
	bool result;

	// Load in the model data and vertex and index buffers.
	result = InitializeGeometryBuffers(device, type);
	if (!result) { return false; }

	// Load the diff texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }

	// Set the material
	result = SetMaterial(ambient, diffuse, specular, reflect);
	if (!result) { return false; }

	// build matrix
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX posMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX intermediateMatrix = XMMatrixMultiply(scaleMatrix, rotMatrix);
	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(intermediateMatrix, posMatrix));

	// set tex transform
	XMMATRIX texScale = XMMatrixScaling(textureTransform.x, textureTransform.y, 0.0f);
	XMStoreFloat4x4(&_texTransform, texScale);

	return true;
}

void Primitive::Shutdown()
{
	ReleaseTextures();
	ShutdownBuffers();
	ReleaseModel();

	return;
}

void Primitive::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

/// <summary>
/// Initializes the geometry buffers. Note: type is ignored for now, since this class is only used for grids
/// </summary>
/// <param name="deviceContext">The device context.</param>
/// <param name="type">The type.</param>
/// <returns></returns>
bool Primitive::InitializeGeometryBuffers(ID3D11Device* deviceContext, Type type) {
	GeometryGenerator::MeshData meshdata;
	GeometryGenerator geoGen;
	geoGen.CreateGrid(1000, 1000, 1000, 1000, meshdata);

	_vertexCount = meshdata.Vertices.size();
	_indexCount = meshdata.Indices.size();

	_model = new Vertex::Vertex[_vertexCount];
	if (!_model) { return false; }

	for (int i = 0; i < _vertexCount; ++i) {
		_model[i].Pos = meshdata.Vertices[i].Position;
		_model[i].Normal = meshdata.Vertices[i].Normal;
		_model[i].DiffTex = meshdata.Vertices[i].TexC;
	}

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
	indices.insert(indices.end(), meshdata.Indices.begin(), meshdata.Indices.end());

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

void Primitive::ShutdownBuffers() {
	ReleaseCOM(_vertexBuffer);
	ReleaseCOM(_indexBuffer);

	return;
}

void Primitive::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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

bool Primitive::LoadTexture(ID3D11Device* deviceContext, WCHAR* diffFilename) {
	_diffTexture = new Texture;

	if (!_diffTexture) { return false; }

	bool result = _diffTexture->Initialize(deviceContext, diffFilename);
	if (!result) { return false; }

	return true;
}

void Primitive::ReleaseTextures() {
	if (_diffTexture) {
		_diffTexture->Shutdown();
		delete _diffTexture;
		_diffTexture = 0;
	}

	return;
}

bool Primitive::SetMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 reflect) {
	_material.Ambient = ambient;
	_material.Diffuse = diffuse;
	_material.Specular = specular;
	_material.Reflect = reflect;

	return true;
}

void Primitive::ReleaseModel() {
	if (_model)
	{
		delete[] _model;
		_model = 0;
	}

	return;
}