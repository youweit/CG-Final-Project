#include "Camera.h"

Camera::Camera(vec3 initPosition)
{
	m_Direction = vec3(0, 0, -1);
	m_TestPosition = m_Position = initPosition;
	g_vUp = vec4(0.0, 1.0, 0.0, 0.0);
}


void Camera::SetPosition(vec3 pos)
{
	//m_Direction = dir;
	m_Position = pos;
	m_TestPosition = pos;
}

