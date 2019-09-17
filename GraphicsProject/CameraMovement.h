#pragma once
#include <DirectXMath.h>
using namespace DirectX;

XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR camPosition;
XMVECTOR camTarget;

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;
float moveUpDown = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;

float speed = 0.5f;

//Camera Variables
XMMATRIX camView;
short deltaWheel;
float FOVDivider = 2.0f;
POINT cursorPoint;
POINT prevCursorPoint;
float nearPlane = 0.1f;
float farPlane = 500.0f;

void UpdateFOV();

XMMATRIX GetViewMatrix()
{
	return camView;
}

void UpdateCam()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);
	
	XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);
	
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
	
	//X
	camPosition += moveLeftRight * camRight;
	//Y
	camPosition += moveUpDown * camUp;
	//Z
	camPosition += moveBackForward * camForward;
	
	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;
	
	camTarget = camPosition + camTarget;

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}

void CheckKeyInputs()
{
	//Adjust the Camera Position
	if (GetAsyncKeyState('W'))
	{
		moveBackForward += speed;
	}
	else if (GetAsyncKeyState('S'))
	{
		moveBackForward -= speed;
	}
	if (GetAsyncKeyState('A'))
	{
		moveLeftRight -= speed;
	}
	else if (GetAsyncKeyState('D'))
	{
		moveLeftRight += speed;
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		moveUpDown += speed;
	}
	else if (GetAsyncKeyState(VK_LSHIFT))
	{
		moveUpDown -= speed;
	}

	//Reset Camera Zoom
	if (GetAsyncKeyState('R') & 0x1)
	{
		FOVDivider = 2.0f;
	}

	GetCursorPos(&cursorPoint);

	if ((prevCursorPoint.x != cursorPoint.x) || (prevCursorPoint.y != cursorPoint.y))
	{
		if (!GetAsyncKeyState(VK_LBUTTON))
		{
			camYaw -= (prevCursorPoint.x - cursorPoint.x) * 0.002f;
			camPitch -= (prevCursorPoint.y - cursorPoint.y) * 0.002f;
		}
		prevCursorPoint = cursorPoint;
	}
	
	UpdateFOV();
	UpdateCam();

}

void UpdateFOV()
{
	//FOV
	if (deltaWheel > 0)
	{
		if (FOVDivider < 4.0f)
		{
			FOVDivider += 0.05f;
		}
	}
	else if (deltaWheel < 0)
	{
		if (FOVDivider > 1.5)
		{
			FOVDivider -= 0.05f;
		}
	}
	deltaWheel = 0;
}

void UpdatePlanes()
{
	//Adjust the Near or Far Plane
	//Increase Far Plane
	if (GetAsyncKeyState('O'))
	{
		if (farPlane < 1000.0f)
		{
			farPlane += 1.0f;
		}
	}
	//Decrease Far Plane
	if (GetAsyncKeyState('L'))
	{
		if (farPlane > nearPlane + 0.1f)
		{
			farPlane -= 1.0f;
		}
	}

	//Increase Near Plane
	if (GetAsyncKeyState('I'))
	{
		if (nearPlane < farPlane - 0.5f)
		{
			nearPlane += 1.0f;
		}
	}
	//Decrease Near Plane
	if (GetAsyncKeyState('K'))
	{
		if (nearPlane > 0.5f)
		{
			nearPlane -= 1.0f;
		}
	}
}