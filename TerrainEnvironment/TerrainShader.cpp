#include "TerrainShader.h"

const D3D11_INPUT_ELEMENT_DESC TerrainShader::_inputLayoutDesc[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

TerrainShader::TerrainShader() {}
TerrainShader::TerrainShader(const TerrainShader&) {}
TerrainShader::~TerrainShader() {}

bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result = InitializeShader(device, hwnd, L"FX/Terrain.fx");

	if (!result) { return false; }

	return true;
}

void TerrainShader::Shutdown() {
	ReleaseCOM(_FX);
	ReleaseCOM(_inputLayout);
	return;
}

bool TerrainShader::Render(ID3D11DeviceContext* deviceContext, int indexCount) {
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_inputLayout);

	D3DX11_TECHNIQUE_DESC techDesc;
	_technique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = _technique->GetPassByIndex(p);
		pass->Apply(0, deviceContext);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	deviceContext->HSSetShader(0, 0, 0);
	deviceContext->DSSetShader(0, 0, 0);
	return true;
}

bool TerrainShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) {
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;

	// Load the fx from given file
	HRESULT hr = D3DX11CompileFromFile(filename, 0, 0, 0,
		"fx_5_0", 0, 0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there
	// were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr,
			L"D3DX11CompileFromFile", true);
	}

	// create the effect
	HR(D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0, device, &_FX));

	// obtain the technique
	_technique = _FX->GetTechniqueByName("Light1Fog");

	// create input layout
	D3DX11_PASS_DESC passDesc;

	// Basic32
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(_inputLayoutDesc, sizeof(_inputLayoutDesc) / sizeof(_inputLayoutDesc[0]), passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &_inputLayout));

	// obtain all variables
	_viewProj = _FX->GetVariableByName("gViewProj")->AsMatrix();
	_world = _FX->GetVariableByName("gWorld")->AsMatrix();
	_worldInvTranspose = _FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	_eyePosW = _FX->GetVariableByName("gEyePosW")->AsVector();
	_fogColor = _FX->GetVariableByName("gFogColor")->AsVector();
	_fogStart = _FX->GetVariableByName("gFogStart")->AsScalar();
	_fogRange = _FX->GetVariableByName("gFogRange")->AsScalar();
	_dirLights = _FX->GetVariableByName("gDirLights");
	_material = _FX->GetVariableByName("gMaterial");

	_minDistance = _FX->GetVariableByName("gMinDist")->AsScalar();
	_maxDistance = _FX->GetVariableByName("gMaxDist")->AsScalar();
	_minTessalation = _FX->GetVariableByName("gMinTess")->AsScalar();
	_maxTessalation = _FX->GetVariableByName("gMaxTess")->AsScalar();
	_texelCellSpaceU = _FX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	_texelCellSpaceV = _FX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	_worldCellSpace = _FX->GetVariableByName("gWorldCellSpace")->AsScalar();
	_worldFrustumPlanes = _FX->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	_layerMapArray = _FX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	_blendMap = _FX->GetVariableByName("gBlendMap")->AsShaderResource();
	_heightMap = _FX->GetVariableByName("gHeightMap")->AsShaderResource();

	return true;
}

