#ifndef CWALL_H
#define CWALL_H
#include "Header/Angel.h"
#include "Common/CSolidCube.h"

#define WALL_NUM 9

typedef struct WallParameters
{
	vec3 position;
	vec3 size;
} WallSource;

class CWall
{

protected:
	CSolidCube *m_pWalls[WALL_NUM];
	WallSource *m_WallSource;
	int m_iMode;
public:

	CWall(WallSource * params);
	~CWall();
	
	void SetShader(mat4 &mxModelView, mat4 &mxProjection);
	void SetProjectionMatrix(mat4 &mat);
	void SetModelViewMatrix(mat4 &mat);
	void SetTRSMatrix(mat4 &mat);
	void SetShadingMode(int iMode) { m_iMode = iMode; }
	void SetTextureAndMaterials();
	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const structLightSource &LightSource);
	vec3 GetMaxBound(int index){ return m_pWalls[index]->GetMaxBound(); }
	vec3 GetMinBound(int index){ return m_pWalls[index]->GetMinBound(); }

	void Draw();
};

#endif