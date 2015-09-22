#ifndef CWIRESPHERE_H
#define CWIRESPHERE_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define WIRESPHERE_NUM 36  // 6 faces, 2 triangles/face , 3 vertices/triangle

class CWireSphere : public CShape
{
private:
	GLfloat m_fRadius;
	GLint m_iSlices, m_iStacks;

	vec4 m_Points[WIRESPHERE_NUM];
	vec3 m_Normals[WIRESPHERE_NUM];
	vec4 m_vertices[8];
	int  m_iIndex;

public:
	CWireSphere(const GLfloat fRadius=1.0f, const int iSlices=12,const  int iStacks = 6);
	~CWireSphere();

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	void Update(float dt,vec4 spotdir); // ���p��������ө�

	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void Draw();
	void DrawW(); // �I�s���A���]�w Shader ���yø�覡
};

#endif