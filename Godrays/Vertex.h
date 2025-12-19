#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex
{
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 DiffTex;
	};

	struct Terrain
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
		XMFLOAT2 BoundsY;
	};

	struct OrthoWindow {
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};
}

#endif 