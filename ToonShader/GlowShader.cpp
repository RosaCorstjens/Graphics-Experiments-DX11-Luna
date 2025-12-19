#include "GlowShader.h"

GlowShader::GlowShader()
	: _glowTexture(0), _glowStrength(0), _padding(0), TextureShader() {}

GlowShader::GlowShader(const GlowShader&) {}

GlowShader::~GlowShader() {}

bool GlowShader::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result = InitializeShader(device, hwnd, L"FX/GlowShader.fx");

	if (!result) { return false; }

	return true;
}

bool GlowShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) {
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
	_technique = _FX->GetTechniqueByName("Glow");

	// create input layout
	D3DX11_PASS_DESC passDesc;

	// Basic32
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(_inputLayoutDesc, sizeof(_inputLayoutDesc) / sizeof(_inputLayoutDesc[0]), passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &_inputLayout));

	// obtain all variables
	_world = _FX->GetVariableByName("gWorld")->AsMatrix();
	_worldInvTranspose = _FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	_worldViewProj = _FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	_texTransform = _FX->GetVariableByName("gTexTransform")->AsMatrix();
	_mat = _FX->GetVariableByName("gMaterial");

	_diffTexture = _FX->GetVariableByName("gDiffTexture")->AsShaderResource();
	_glowTexture = _FX->GetVariableByName("gGlowTexture")->AsShaderResource();

	_eyePosW = _FX->GetVariableByName("gEyePosW")->AsVector();
	_dirLights = _FX->GetVariableByName("gDirLights");

	_glowStrength = _FX->GetVariableByName("gGlowStrength")->AsScalar();
	_padding = _FX->GetVariableByName("gPadding")->AsVector();

	return true;
}


