#include "CWall.h"


CWall::CWall(WallSource* params)
{
	m_WallSource = params;
	for (int i = 0; i < WALL_NUM; i++)
		m_pWalls[i] = new CSolidCube(m_WallSource[i].size);
	//m_pWalls = new CSolidCube[WALL_NUM];
}

CWall::~CWall()
{
	delete[] m_pWalls;
}

void CWall::SetShader(mat4 &mxModelView, mat4 &mxProjection)
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->SetShader(mxModelView, mxProjection);
		m_pWalls[i]->SetTRSMatrix(Translate(m_WallSource[i].position));
		m_pWalls[i]->SetShadingMode(GOURAUD_SHADING);
		m_pWalls[i]->SetMaterials(vec4(0.35f, 0.35f, 0.35f, 1), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pWalls[i]->SetKaKdKsShini(0.25f, 0.8f, 0.2f, 2);
		m_pWalls[i]->SetColor(vec3(0, 0, 0));
		//m_pWalls[i]->LoadPngImageAndSetTextureObject("texture/Masonry.Brick.png");
	}
}

void CWall::SetProjectionMatrix(mat4 &mat)
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->SetProjectionMatrix(mat);
	}
}

void CWall::SetModelViewMatrix(mat4 &mat)
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->SetModelViewMatrix(mat);
	}
}

void CWall::SetTRSMatrix(mat4 &mat)
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->SetTRSMatrix(mat);
	}

}
void CWall::SetTextureAndMaterials()
{
	//TODO
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->SetMaterials(vec4(0.35f, 0.35f, 0.35f, 1), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pWalls[i]->SetKaKdKsShini(0.25f, 0.8f, 0.2f, 2);
		m_pWalls[i]->LoadPngImageAndSetTextureObject("texture/Masonry.Brick.png");
	}
}

void CWall::Draw()
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->Draw();
	}

}

void CWall::Update(float dt, const structLightSource &LightSource)
{
	for (int i = 0; i < WALL_NUM; i++){
		m_pWalls[i]->Update(dt, LightSource);
	}

}
