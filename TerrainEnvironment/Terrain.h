#ifndef TERRAIN_H
#define TERRAIN_H

#include "d3dUtil.h"

class Terrain {
public:
	// structure containing important terrain information (probably for the shader)
	struct TerrainInfo {
		std::wstring HeightMapFilename;
		std::wstring LayerMapFilename0;
		std::wstring LayerMapFilename1;
		std::wstring LayerMapFilename2;
		std::wstring LayerMapFilename3;
		std::wstring LayerMapFilename4;
		std::wstring BlendMapFilename;
		float HeightScale;
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;
	};

	Terrain();
	~Terrain();

	float GetWidth()const;
	float GetDepth()const;
	float GetHeight(float x, float z)const;

	XMMATRIX GetWorldMatrix()const;
	// optionally a set world matrix method, but if no need, dont create

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const TerrainInfo& info);
	void Render(ID3D11DeviceContext* deviceContext, ID3D11InputLayout* inputlayout);

	Material GetMaterial() { return _material; }
	TerrainInfo GetTerrainInfo() { return _info; }
	ID3D11ShaderResourceView* GetLayerMapArraySRV() { return _layerMapArraySRV; }
	ID3D11ShaderResourceView* GetBlendMapSRV() { return _blendMapSRV; }
	ID3D11ShaderResourceView* GetHeightMapSRV() { return _heightMapSRV; }
	int GetIndexCount() { return _numPatchQuadFaces*4; }

private:
	void LoadHeightMap();
	void SmoothY();
	bool InBounds(int x, int z);
	float Average(int x, int z);
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT x, UINT z);
	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);
	void BuildHeightmapSRV(ID3D11Device* device);

	static const int _cellsPerPatch = 64;

	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	ID3D11ShaderResourceView* _layerMapArraySRV;
	ID3D11ShaderResourceView* _blendMapSRV;
	ID3D11ShaderResourceView* _heightMapSRV;

	TerrainInfo _info;

	UINT _numPatchVertices;
	UINT _numPatchQuadFaces;

	UINT _numPatchVertRows;
	UINT _numPatchVertCols;

	XMFLOAT4X4 _world;

	Material _material;

	std::vector<XMFLOAT2> _patchBoundsY;
	std::vector<float> _heightMap;
};

#endif