#ifndef DEPTH_SHADER_H
#define DEPTH_SHADER_H

#include "d3dUtil.h"

class DepthShader {
public:
	DepthShader();
	DepthShader(const DepthShader&);
	~DepthShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool RenderDepth(ID3D11DeviceContext* deviceContext, int indexCount);

	void SetPerFrame(const XMFLOAT3& eyePos) {
		_eyePos->SetRawValue(&eyePos, 0, sizeof(XMFLOAT3));
	}

	void SetPerObject(CXMMATRIX worldViewProj, CXMMATRIX world, CXMMATRIX worldInvTrans) {
		_worldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));
		_world->SetMatrix(reinterpret_cast<const float*>(&world));
		_worldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTrans));
	}

private:
	bool InitializeDepthShader(ID3D11Device* device, HWND hwnd);
	
	ID3DX11Effect* _depthEffect;
	ID3DX11EffectTechnique* _depthTechnique;
	static const D3D11_INPUT_ELEMENT_DESC _depthInputLayoutDesc[1];
	ID3D11InputLayout* _depthInputLayout;

	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;
	ID3DX11EffectMatrixVariable* _worldViewProj;
	ID3DX11EffectVectorVariable* _eyePos;
};

#endif
