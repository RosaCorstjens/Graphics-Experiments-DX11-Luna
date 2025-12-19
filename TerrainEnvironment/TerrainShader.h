#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

#include "IShader.h"

class TerrainShader : public IShader {
public:
	TerrainShader();
	TerrainShader(const TerrainShader&);
	~TerrainShader();

	bool Initialize(ID3D11Device* device, HWND hwnd) override;
	void Shutdown() override;
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount) override;

	// setters
	void SetViewProj(CXMMATRIX M) { _viewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { _eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v) { _fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f) { _fogStart->SetFloat(f); }
	void SetFogRange(float f) { _fogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights) { _dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { _material->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinDist(float f) { _minDistance->SetFloat(f); }
	void SetMaxDist(float f) { _maxDistance->SetFloat(f); }
	void SetMinTess(float f) { _minTessalation->SetFloat(f); }
	void SetMaxTess(float f) { _maxTessalation->SetFloat(f); }
	void SetTexelCellSpaceU(float f) { _texelCellSpaceU->SetFloat(f); }
	void SetTexelCellSpaceV(float f) { _texelCellSpaceV->SetFloat(f); }
	void SetWorldCellSpace(float f) { _worldCellSpace->SetFloat(f); }
	void SetWorldFrustumPlanes(XMFLOAT4 planes[6]) { _worldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6); }

	void SetLayerMapArray(ID3D11ShaderResourceView* tex) { _layerMapArray->SetResource(tex); }
	void SetBlendMap(ID3D11ShaderResourceView* tex) { _blendMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex) { _heightMap->SetResource(tex); }

	ID3D11InputLayout* GetInputLayout() { return _inputLayout; }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);

	static const D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[3];

	ID3DX11EffectMatrixVariable* _viewProj;
	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTranspose;

	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectVectorVariable* _fogColor;
	ID3DX11EffectScalarVariable* _fogStart;
	ID3DX11EffectScalarVariable* _fogRange;
	ID3DX11EffectVariable* _dirLights;
	ID3DX11EffectVariable* _material;

	ID3DX11EffectScalarVariable* _minDistance;
	ID3DX11EffectScalarVariable* _maxDistance;
	ID3DX11EffectScalarVariable* _minTessalation;
	ID3DX11EffectScalarVariable* _maxTessalation;
	ID3DX11EffectScalarVariable* _texelCellSpaceU;
	ID3DX11EffectScalarVariable* _texelCellSpaceV;
	ID3DX11EffectScalarVariable* _worldCellSpace;
	ID3DX11EffectVectorVariable* _worldFrustumPlanes;

	ID3DX11EffectShaderResourceVariable* _layerMapArray;
	ID3DX11EffectShaderResourceVariable* _blendMap;
	ID3DX11EffectShaderResourceVariable* _heightMap;

};

#endif
