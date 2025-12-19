#include "GodRayShader.h"

const D3D11_INPUT_ELEMENT_DESC GodRayShader::_inputLayoutDesc[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GodRayShader::GodRayShader() 
: _FX(0), _technique(0), _inputLayout(0), _lightPos(0), _screenSize(0),
_aspectRatio(0), _blend(0), _intensity(0), _diffuseTexture(0), _depthTexture(0), 
_shaftTexture(0), _worldViewProj(0) { } 

GodRayShader::GodRayShader(const GodRayShader&) {}
GodRayShader::~GodRayShader() {}

bool GodRayShader::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result = InitializeShader(device, hwnd, L"FX/GodRays.fx");

	if (!result) { return false; }

	return true;
}

void GodRayShader::Shutdown() {
	ReleaseCOM(_FX);
	ReleaseCOM(_inputLayout);
	return;
}

bool GodRayShader::RenderMask(ID3D11DeviceContext* deviceContext, int indexCount) {
	Render(deviceContext, indexCount, "Mask");
	return true;
}

bool GodRayShader::RenderBlur(ID3D11DeviceContext* deviceContext, int indexCount) {
	Render(deviceContext, indexCount, "Blur");
	return true;
}

bool GodRayShader::RenderFinalMix(ID3D11DeviceContext* deviceContext, int indexCount) {
	Render(deviceContext, indexCount, "FinalMix");
	return true;
}

bool GodRayShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, const char* techName) {
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_inputLayout);

	D3DX11_TECHNIQUE_DESC techDesc;
	_technique = _FX->GetTechniqueByName(techName);
	_technique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		_technique->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	return true;
}

bool GodRayShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) {
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
	_technique = _FX->GetTechniqueByName("Mask");

	// create input layout
	D3DX11_PASS_DESC passDesc;

	// Basic32
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(_inputLayoutDesc, sizeof(_inputLayoutDesc) / sizeof(_inputLayoutDesc[0]), passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &_inputLayout));

	// obtain all variables
	_lightPos = _FX->GetVariableByName("gLightPosition")->AsVector();
	_screenSize = _FX->GetVariableByName("gScreenSize")->AsVector();
	_aspectRatio = _FX->GetVariableByName("gAspectRatio")->AsScalar();
	_blend = _FX->GetVariableByName("gBlend")->AsScalar();
	_intensity = _FX->GetVariableByName("gIntensity")->AsScalar();

	_diffuseTexture = _FX->GetVariableByName("gDiffTexture")->AsShaderResource();
	_depthTexture = _FX->GetVariableByName("gDepthTexture")->AsShaderResource();
	_shaftTexture = _FX->GetVariableByName("gShaftTexture")->AsShaderResource();

	_worldViewProj = _FX->GetVariableByName("gWorldViewProj")->AsMatrix();

	return true;
}


