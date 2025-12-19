#ifndef CAMERA_H
#define CAMERA_H

#include "d3dApp.h"

class CameraClass {
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition() { return _position; }
	XMFLOAT3 GetRotation() { return _rotation; }

	void Render();
	XMFLOAT4X4 GetViewMatrix() { return _viewMatrix; }
	XMFLOAT4X4 GetRotationMatrix(){ 
		XMStoreFloat4x4(&_rotationMatrix, XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z));
		return _rotationMatrix;
	}
private:
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _rotationMatrix;
};


#endif
