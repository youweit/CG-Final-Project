#ifndef CSOLIDPRISM_H
#define CSOLIDPRISM_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define SOLIDPRISM_NUM 60  // 6 faces, 2 triangles/face , 3 vertices/triangle

class CSolidPrism : public CShape
{
private:
	vec4 m_vertices[12];
	int  m_iIndex;

	void Quad( int a, int b, int c, int d );
public:
	CSolidPrism();
	~CSolidPrism(){};

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	void Update(float dt); // 不計算光源的照明

	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void Draw();
	void DrawW();
};

#endif