#include "DepthShader.h"

const D3D11_INPUT_ELEMENT_DESC DepthShader::_inputLayoutDesc[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

DepthShader::DepthShader() 
: _FX(0), _technique(0), _inputLayout(0), _world(0), _worldInvTranspose(0), _worldViewProj(0), _eyePosW(0) {}

DepthShader::DepthShader(const DepthShader&) {}
DepthShader::~DepthShader() {}

bool DepthShader::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result = InitializeShader(device, hwnd, L"FX/DepthShader.fx");

	if (!result) { return false; }

	return true;
}

void DepthShader::Shutdown() {
	ReleaseCOM(_FX);
	ReleaseCOM(_inputLayout);
	return;
}

bool DepthShader::Render(ID3D11DeviceContext* deviceContext, int indexCount) {
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_inputLayout);

	D3DX11_TECHNIQUE_DESC techDesc;
	_technique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		_technique->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	return true;
}

bool DepthShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) {
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
	_technique = _FX->GetTechniqueByName("Depth");

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

	_eyePosW = _FX->GetVariableByName("gEyePosW")->AsVector();

	return true;
}




