#include "CameraClass.h"

CameraClass::CameraClass() : _position(0, 0, 0), _rotation(0, 0, 0) {}

CameraClass::CameraClass(const CameraClass&) {}

CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z) {
	_position.x = x;
	_position.y = y;
	_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z) {
	_rotation.x = x;
	_rotation.y = y;
	_rotation.z = z;
}

void CameraClass::Render() {
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(_position.x, _position.y, _position.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&_viewMatrix, V);
}




