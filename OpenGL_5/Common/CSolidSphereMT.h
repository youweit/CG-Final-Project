#ifndef CSOLIDSPHEREMT_H
#define CSOLIDSPHEREMT_H
#include "../header/Angel.h"
#include "CShapeMT.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class CSolidSphereMT : public CShapeMT
{
private:
	GLfloat m_fRadius;
	GLint m_iSlices, m_iStacks;

public:
	CSolidSphereMT(const GLfloat fRadius=1.0f, const int iSlices=12,const  int iStacks = 6);
	~CSolidSphereMT();

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	void Update(float dt); // 不計算光源的照明

	// Sphere 的繪製方始使用多組的 GL_TRIANGLE_STRIP 來繪製, 因此沒有辦法提供 Flat Shading，
	// 只有以 vertex 為基礎的計算顏色的 Ground Shading
	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void Draw();
	void DrawW(); // 呼叫不再次設定 Shader 的描繪方式

};

#endif