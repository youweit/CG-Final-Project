#ifndef CSOLIDTORUS_H
#define CSOLIDTORUS_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define SOLIDCUBE_NUM 36  // 6 faces, 2 triangles/face , 3 vertices/triangle

// Torus equation
// x(theta, phi) = (R + r cos(phi)) cos(theta)
// y(theta, phi) = (R + r cos(phi)) sin(theta) 
// z(theta, phi) = r sin(phi)
// 0 <= thtea, phi <= 2*PI
// R is the distance from the center of the tube to the center of the torus,
// r is the radius of the tube.

class CSolidTorus : public CShape
{
private:
	GLfloat m_fMarjorRadius; // R
	GLfloat m_fMinorRadius;  // r
	GLint m_iSlices, m_iStacks;
	
	vec4 m_Points[SOLIDCUBE_NUM];
	vec3 m_Normals[SOLIDCUBE_NUM];
	vec4 m_vertices[8];
	int  m_iIndex;

public:
	CSolidTorus(const GLfloat fRadius=1.0f, const int iSlices=12,const  int iStacks = 6);
	~CSolidTorus();

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	void Update(float dt); // ���p��������ө�

	// Sphere ��ø�s��l�ϥΦh�ժ� GL_TRIANGLE_STRIP ��ø�s, �]���S����k���� Flat Shading�A
	// �u���H vertex ����¦���p���C�⪺ Ground Shading
	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void Draw();
	void DrawW(); // �I�s���A���]�w Shader ���yø�覡

};

#endif