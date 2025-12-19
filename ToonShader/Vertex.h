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
}

#endif 
