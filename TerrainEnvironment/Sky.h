//***************************************************************************************
// Sky.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple class that renders a sky using a cube map.
//***************************************************************************************

#ifndef SKY_H
#define SKY_H

#include "d3dUtil.h"

class Sky
{
public:
	Sky(ID3D11Device* device, const std::wstring& cubemapFilename, float skySphereRadius);
	~Sky();

	ID3D11ShaderResourceView* GetCubeMap();

	void Render(ID3D11DeviceContext* deviceContext, ID3D11InputLayout* inputlayout);

	int GetIndexCount() { return _indexCount; }

private:
	Sky(const Sky& rhs);
	Sky& operator=(const Sky& rhs);

private:
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	ID3D11ShaderResourceView* _cubeMap;

	UINT _indexCount;
};

#endif 