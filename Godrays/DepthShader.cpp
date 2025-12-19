#include "DepthShader.h"

const D3D11_INPUT_ELEMENT_DESC DepthShader::_depthInputLayoutDesc[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

DepthShader::DepthShader() 
: _depthEffect(0), _depthTechnique(0), _depthInputLayout(0), _world(0), _worldInvTranspose(0), _worldViewProj(0), _eyePos(0) {}

DepthShader::DepthShader(const DepthShader&) {}
DepthShader::~DepthShader() {}

bool DepthShader::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result = InitializeDepthShader(device, hwnd);
	if (!result) { return false; }

	return true;
}

void DepthShader::Shutdown() {
	ReleaseCOM(_depthEffect);
	ReleaseCOM(_depthInputLayout);
	return;
}

bool DepthShader::RenderDepth(ID3D11DeviceContext* deviceContext, int indexCount) {
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_depthInputLayout);

	D3DX11_TECHNIQUE_DESC techDesc;
	_depthTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		_depthTechnique->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	return true;
}

bool DepthShader::InitializeDepthShader(ID3D11Device* device, HWND hwnd) {
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;

	// Load the fx from given file
	HRESULT hr = D3DX11CompileFromFile(L"FX/DepthShader.fx", 0, 0, 0,
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
		0, device, &_depthEffect));

	// obtain the technique
	_depthTechnique = _depthEffect->GetTechniqueByName("Depth");

	// create input layout
	D3DX11_PASS_DESC passDesc;

	// Basic32
	_depthTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(_depthInputLayoutDesc, sizeof(_depthInputLayoutDesc) / sizeof(_depthInputLayoutDesc[0]), passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &_depthInputLayout));

	// obtain all variables
	_world = _depthEffect->GetVariableByName("gWorld")->AsMatrix();
	_worldInvTranspose = _depthEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	_worldViewProj = _depthEffect->GetVariableByName("gWorldViewProj")->AsMatrix();

	_eyePos = _depthEffect->GetVariableByName("gEyePosW")->AsVector();

	return true;
}