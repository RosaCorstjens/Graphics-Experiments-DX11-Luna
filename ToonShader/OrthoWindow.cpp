#include "OrthoWindow.h"
#include "Vertex.h"

OrthoWindow::OrthoWindow() : _vertexBuffer(0), _indexBuffer(0) {}

OrthoWindow::OrthoWindow(const OrthoWindow&) {}

OrthoWindow::~OrthoWindow() {}

bool OrthoWindow::Initialize(ID3D11Device* device, int width, int height) {
	bool result = InitializeBuffers(device, width, height);

	return result;
}

void OrthoWindow::Shutdown() {
	ShutdownBuffers();
}

void OrthoWindow::Render(ID3D11DeviceContext* deviceContext) {
	RenderBuffers(deviceContext);
}

bool OrthoWindow::InitializeBuffers(ID3D11Device* device, int width, int height) {
	// calculate the screen coordinates
	float left = (float)((width / 2) * -1);
	float right = left + (float)width;
	float top = (float)(height / 2);
	float bottom = top - (float)height;

	// set the vertex and index count
	_vertexCount = 6;
	_indexCount = _vertexCount;

	// create vectors for vertices and indices
	// (yes it could be an array but I like to work with vectors and it's my holiday)
	std::vector<Vertex::Vertex> vertices = std::vector<Vertex::Vertex>(6);
	std::vector<UINT> indices;

	// 1st triangle
	vertices[0].Pos = XMFLOAT3(left, top, 0);
	vertices[0].Normal = XMFLOAT3(0, 0, 0);
	vertices[0].DiffTex = XMFLOAT2(0, 0);

	vertices[1].Pos = XMFLOAT3(right, bottom, 0);
	vertices[1].Normal = XMFLOAT3(0, 0, 0);
	vertices[1].DiffTex = XMFLOAT2(1, 1);

	vertices[2].Pos = XMFLOAT3(left, bottom, 0);
	vertices[2].Normal = XMFLOAT3(0, 0, 0);
	vertices[2].DiffTex = XMFLOAT2(0, 1);

	// 2nd triangle
	vertices[3].Pos = XMFLOAT3(left, top, 0);
	vertices[3].Normal = XMFLOAT3(0, 0, 0);
	vertices[3].DiffTex = XMFLOAT2(0, 0);

	vertices[4].Pos = XMFLOAT3(right, top, 0);
	vertices[4].Normal = XMFLOAT3(0, 0, 0);
	vertices[4].DiffTex = XMFLOAT2(1, 0);

	vertices[5].Pos = XMFLOAT3(right, bottom, 0);
	vertices[5].Normal = XMFLOAT3(0, 0, 0);
	vertices[5].DiffTex = XMFLOAT2(1, 1);

	// load the index vector with the data
	for (int i = 0; i<_vertexCount; i++)
	{
		indices.push_back(i);
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Vertex) * _vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;
	HR(device->CreateBuffer(&vbd, &vinitData, &_vertexBuffer));

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
	HR(device->CreateBuffer(&ibd, &iinitData, &_indexBuffer));

	return true;
}

void OrthoWindow::ShutdownBuffers() {
	ReleaseCOM(_vertexBuffer);
	ReleaseCOM(_indexBuffer);

	return;
}

void OrthoWindow::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride, offset;

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







