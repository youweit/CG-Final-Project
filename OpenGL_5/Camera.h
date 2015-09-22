#ifndef CAMERA_H
#define CAMERA_H
#include "Header/Angel.h"
#include "Common/CSolidCube.h"
#define FLY

class Camera
{

protected:
	float angleX;
	float angleY;
	vec4 g_vUp;
	vec3 m_Direction;
	vec3 m_Position;
	vec3 m_TestPosition;
public:

	vec3 GetDirection(){ return m_Direction; }
	vec3 GetPosition(){ return m_Position; }
	vec3 GetTestPosition(){ return m_TestPosition; }
	vec3 MoveForward(float fraction){ 
#ifdef FLY
		m_Position += m_Direction*fraction;
#else
		m_Position.x += m_Direction.x*fraction;
		m_Position.z += m_Direction.z*fraction;
#endif	
		return m_Position;
	}

	vec3 MoveBackward(float fraction){
#ifdef FLY
		m_Position -= m_Direction*fraction;
#else
		m_Position.x -= m_Direction.x*fraction;
		m_Position.z -= m_Direction.z*fraction;
#endif	
		return m_Position;
	}

	vec3 MoveRight(float g_fPhi,float fraction){
		m_Position.x -= -cos(g_fPhi) * fraction;
		m_Position.z -= sin(g_fPhi) * fraction;
		return m_Position;
	}

	vec3 MoveLeft(float g_fPhi, float fraction){
		m_Position.x += -cos(g_fPhi) * fraction;
		m_Position.z += sin(g_fPhi) * fraction;
		return m_Position;
	}

	vec3 TestForward(float fraction){
		m_TestPosition += m_Direction*fraction;
		return m_TestPosition;
	}

	vec3 TestBackward(float fraction){
		m_TestPosition -= m_Direction*fraction;
		return m_TestPosition;
	}

	vec3 TestRight(float g_fPhi, float fraction){
		m_TestPosition.x -= -cos(g_fPhi) * fraction;
		m_TestPosition.z -= sin(g_fPhi) * fraction;
		return m_TestPosition;
	}

	vec3 TestLeft(float g_fPhi, float fraction){
		m_TestPosition.x += -cos(g_fPhi) * fraction;
		m_TestPosition.z += sin(g_fPhi) * fraction;
		return m_TestPosition;
	}

	void SetPosition(vec3 position);
	void SetTestPosition(vec3 position){ m_TestPosition = position; };
	void SetDirection(float x, float y, float z){
		m_Direction.x = x;
		m_Direction.y = y;
		m_Direction.z = z;
		//direction = normalize(direction);
	}

	mat4 GetModelView(){
		return LookAt(
			vec4(m_Position, 1.0),
			vec4(m_Position + m_Direction, 1.0),
			g_vUp);
	}

	void MoveJump(float distance){
		m_Position.y += distance;
	}

	void CheckCubeCollision(){
	
	}

	Camera(vec3 initPosition);
	~Camera();
};

#endif