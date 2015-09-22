#ifndef CSOLIDCUBE_H
#define CSOLIDCUBE_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define SOLIDCUBE_NUM 36  // 6 faces, 2 triangles/face , 3 vertices/triangle

class CSolidCube : public CShape
{
private:
	vec4 m_vertices[8];
	vec3 m_max;
	vec3 m_min;
	int  m_iIndex;
	float m_fLength;
	float m_fWidth;
	float m_fHeight;
	float m_fDistance;
	void Quad( int a, int b, int c, int d );
public:
	CSolidCube();
	CSolidCube(vec3 size);
	~CSolidCube(){};
	void SetDistance(float distance){ m_fDistance = distance; };
	float GetDistance(){ return m_fDistance; };
	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	void Update(float dt); // 不計算光源的照明

	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void Draw();
	void DrawW();

	vec3 GetMaxBound(){ return m_max; }
	vec3 GetMinBound(){ return m_min; }
	void SetWallSize(vec3 size);
};

#endif