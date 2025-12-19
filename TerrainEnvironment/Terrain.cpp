//***************************************************************************************
// Terrain.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Terrain.h"
#include "Camera.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include <fstream>
#include <sstream>

Terrain::Terrain() :
_vertexBuffer(0), _indexBuffer(0), _layerMapArraySRV(0), _blendMapSRV(0), _heightMapSRV(0),
_numPatchVertices(0), _numPatchQuadFaces(0), _numPatchVertRows(0), _numPatchVertCols(0) {
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	_material.Ambient = XMFLOAT4(1,1,1,1);
	_material.Diffuse = XMFLOAT4(1,1,1,1);
	_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	_material.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

Terrain::~Terrain()
{
	ReleaseCOM(_vertexBuffer);
	ReleaseCOM(_indexBuffer);
	ReleaseCOM(_layerMapArraySRV);
	ReleaseCOM(_blendMapSRV);
	ReleaseCOM(_heightMapSRV);
}

float Terrain::GetWidth()const
{
	// Total terrain width.
	return (_info.HeightmapWidth - 1)*_info.CellSpacing;
}

float Terrain::GetDepth()const
{
	// Total terrain depth.
	return (_info.HeightmapHeight - 1)*_info.CellSpacing;
}

float Terrain::GetHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*GetWidth()) / _info.CellSpacing;
	float d = (z - 0.5f*GetDepth()) / -_info.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = _heightMap[row*_info.HeightmapWidth + col];
	float B = _heightMap[row*_info.HeightmapWidth + col + 1];
	float C = _heightMap[(row + 1)*_info.HeightmapWidth + col];
	float D = _heightMap[(row + 1)*_info.HeightmapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

void Terrain::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, const TerrainInfo& initInfo)
{
	_info = initInfo;

	// Divide heightmap into patches such that each patch has _cellsPerPatch.
	_numPatchVertRows = ((_info.HeightmapHeight - 1) / _cellsPerPatch) + 1;
	_numPatchVertCols = ((_info.HeightmapWidth - 1) / _cellsPerPatch) + 1;

	_numPatchVertices = _numPatchVertRows*_numPatchVertCols;
	_numPatchQuadFaces = (_numPatchVertRows - 1)*(_numPatchVertCols - 1);

	LoadHeightMap();
	SmoothY();
	CalcAllPatchBoundsY();

	BuildQuadPatchVB(device);
	BuildQuadPatchIB(device);
	BuildHeightmapSRV(device);

	std::vector<std::wstring> layerFilenames;
	layerFilenames.push_back(_info.LayerMapFilename0);
	layerFilenames.push_back(_info.LayerMapFilename1);
	layerFilenames.push_back(_info.LayerMapFilename2);
	layerFilenames.push_back(_info.LayerMapFilename3);
	layerFilenames.push_back(_info.LayerMapFilename4);
	_layerMapArraySRV = d3dHelper::CreateTexture2DArraySRV(device, dc, layerFilenames);

	HR(D3DX11CreateShaderResourceViewFromFile(device,
		_info.BlendMapFilename.c_str(), 0, 0, &_blendMapSRV, 0));
}

void Terrain::Render(ID3D11DeviceContext* dc, ID3D11InputLayout* inputlayout)
{
	UINT stride = sizeof(Vertex::Terrain);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(inputlayout);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}

void Terrain::LoadHeightMap()
{
	// A height for each vertex
	std::vector<unsigned char> in(_info.HeightmapWidth * _info.HeightmapHeight);

	// Open the file.
	std::ifstream inFile;
	inFile.open(_info.HeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array and scale it.
	_heightMap.resize(_info.HeightmapHeight * _info.HeightmapWidth, 0);
	for (UINT i = 0; i < _info.HeightmapHeight * _info.HeightmapWidth; ++i)
	{
		_heightMap[i] = (in[i] / 255.0f)*_info.HeightScale;
	}
}

void Terrain::SmoothY()
{
	std::vector<float> dest(_heightMap.size());

	for (UINT i = 0; i < _info.HeightmapHeight; ++i)
	{
		for (UINT j = 0; j < _info.HeightmapWidth; ++j)
		{
			dest[i*_info.HeightmapWidth + j] = Average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	_heightMap = dest;
}

bool Terrain::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int)_info.HeightmapHeight &&
		j >= 0 && j < (int)_info.HeightmapWidth;
}

float Terrain::Average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += _heightMap[m*_info.HeightmapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::CalcAllPatchBoundsY()
{
	_patchBoundsY.resize(_numPatchQuadFaces);

	// For each patch
	for (UINT i = 0; i < _numPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < _numPatchVertCols - 1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void Terrain::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.

	UINT x0 = j*_cellsPerPatch;
	UINT x1 = (j + 1)*_cellsPerPatch;

	UINT y0 = i*_cellsPerPatch;
	UINT y1 = (i + 1)*_cellsPerPatch;

	float minY = +MathHelper::Infinity;
	float maxY = -MathHelper::Infinity;
	for (UINT y = y0; y <= y1; ++y)
	{
		for (UINT x = x0; x <= x1; ++x)
		{
			UINT k = y*_info.HeightmapWidth + x;
			minY = MathHelper::Min(minY, _heightMap[k]);
			maxY = MathHelper::Max(maxY, _heightMap[k]);
		}
	}

	UINT patchID = i*(_numPatchVertCols - 1) + j;
	_patchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

void Terrain::BuildQuadPatchVB(ID3D11Device* device)
{
	std::vector<Vertex::Terrain> patchVertices(_numPatchVertRows*_numPatchVertCols);

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	float patchWidth = GetWidth() / (_numPatchVertCols - 1);
	float patchDepth = GetDepth() / (_numPatchVertRows - 1);
	float du = 1.0f / (_numPatchVertCols - 1);
	float dv = 1.0f / (_numPatchVertRows - 1);

	for (UINT i = 0; i < _numPatchVertRows; ++i)
	{
		float z = halfDepth - i*patchDepth;
		for (UINT j = 0; j < _numPatchVertCols; ++j)
		{
			float x = -halfWidth + j*patchWidth;

			patchVertices[i*_numPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i*_numPatchVertCols + j].Tex.x = j*du;
			patchVertices[i*_numPatchVertCols + j].Tex.y = i*dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < _numPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < _numPatchVertCols - 1; ++j)
		{
			UINT patchID = i*(_numPatchVertCols - 1) + j;
			patchVertices[i*_numPatchVertCols + j].BoundsY = _patchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Terrain) * patchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &patchVertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &_vertexBuffer));
}

void Terrain::BuildQuadPatchIB(ID3D11Device* device)
{
	std::vector<USHORT> indices(_numPatchQuadFaces * 4); // 4 indices per quad face

														 // Iterate over each quad and compute indices.
	int k = 0;
	for (UINT i = 0; i < _numPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < _numPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i*_numPatchVertCols + j;
			indices[k + 1] = i*_numPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1)*_numPatchVertCols + j;
			indices[k + 3] = (i + 1)*_numPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &_indexBuffer));
}

void Terrain::BuildHeightmapSRV(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = _info.HeightmapWidth;
	texDesc.Height = _info.HeightmapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// HALF is defined in xnamath.h, for storing 16-bit float.
	std::vector<HALF> hmap(_heightMap.size());
	std::transform(_heightMap.begin(), _heightMap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = _info.HeightmapWidth * sizeof(HALF);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* hmapTex = 0;
	HR(device->CreateTexture2D(&texDesc, &data, &hmapTex));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	HR(device->CreateShaderResourceView(hmapTex, &srvDesc, &_heightMapSRV));

	// SRV saves reference.
	ReleaseCOM(hmapTex);
}