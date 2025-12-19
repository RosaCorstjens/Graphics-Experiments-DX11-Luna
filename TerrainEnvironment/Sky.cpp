//***************************************************************************************
// Sky.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Sky.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"

Sky::Sky(ID3D11Device* device, const std::wstring& cubemapFilename, float skySphereRadius)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device, cubemapFilename.c_str(), 0, 0, &_cubeMap, 0));

	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.Vertices.size());

	for(size_t i = 0; i < sphere.Vertices.size(); ++i)
	{
		vertices[i] = sphere.Vertices[i].Position;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&vbd, &vinitData, &_vertexBuffer));
	

	_indexCount = sphere.Indices.size();

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(USHORT) * _indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
    ibd.MiscFlags = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices16[0];

    HR(device->CreateBuffer(&ibd, &iinitData, &_indexBuffer));
}

Sky::~Sky()
{
	ReleaseCOM(_vertexBuffer);
	ReleaseCOM(_indexBuffer);
	ReleaseCOM(_cubeMap);
}

ID3D11ShaderResourceView* Sky::GetCubeMap()
{
	return _cubeMap;
}

void Sky::Render(ID3D11DeviceContext* deviceContext, ID3D11InputLayout* inputlayout)
{
	UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetInputLayout(inputlayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}