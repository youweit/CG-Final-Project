#include "CQuad.h"
// Example 4 �}�l
// ���P Example 3 �¤W(Y�b)
// �C�@�� Vertex �W�[ Normal �A�令�~���� CShape�A�@�ֳB�z�������]�w�ݨD


CQuad::CQuad()
{
	m_iNumVtx = QUAD_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex1 = new vec2[m_iNumVtx];		// ���ͩһݪ��K�Ϯy��

	m_pPoints[0] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[1] = vec4(  0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[2] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[3] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[4] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[5] = vec4( -0.5f, 0.0f, -0.5f, 1.0f);

	m_pNormals[0] = vec3(  0, 1.0f, 0);  // Normal Vector �� W �� 0
	m_pNormals[1] = vec3(  0, 1.0f, 0);
	m_pNormals[2] = vec3(  0, 1.0f, 0);
	m_pNormals[3] = vec3(  0, 1.0f, 0);
	m_pNormals[4] = vec3(  0, 1.0f, 0);
	m_pNormals[5] = vec3(  0, 1.0f, 0);

	m_pColors[0] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);  // (r, g, b, a)
	m_pColors[1] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[2] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[3] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[4] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[5] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);


	m_pTex1[0] = vec2(0.0f, 0.0f);
	m_pTex1[1] = vec2(1.0f, 0.0f);
	m_pTex1[2] = vec2(1.0f, 1.0f);
	m_pTex1[3] = vec2(0.0f, 0.0f);
	m_pTex1[4] = vec2(1.0f, 1.0f);
	m_pTex1[5] = vec2(0.0f, 1.0f);

	for( int i = 0 ; i < m_iNumVtx ; i++ ) m_pColors[i] = vec4(-1.0f,-1.0f,-1.0f,1.0f);

#ifdef LIGHTING_WITHCPU
	// Default Set shader's name
	SetShaderName("vsLighting_CPU.glsl", "fsLighting_CPU.glsl");
#else // lighting with GPU
#ifdef PERVERTEX_LIGHTING
	SetShaderName("vsLighting_GPU.glsl", "fsLighting_GPU.glsl");
#else
	SetShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");
#endif
#endif  

	// Create and initialize a buffer object �A�N���������]�w���J SetShader ��
	// CreateBufferObject();
}

void CQuad::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 3 ) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2])/3.0f;
		m_pColors[i] = m_pColors[i+1] = m_pColors[i+2] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}

	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}


void CQuad::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	// �H vertex �����A�ھڸ��I����m�P��k�V�q�A�Q�� Phong lighting model �p��۹������C��
	// �N���C���x�s�^�ӳ��I

	// �Ĥ����I���c������ӤT����
	// �p�� 0 1 2 5 �|�ӳ��I���C��Y�i�A0 �P 3�B2 �P 4 ���C��ۦP

	m_pColors[0] = m_pColors[3] = PhongReflectionModel(m_pPoints[0], m_pNormals[0], vLightPos,  vLightI);
	m_pColors[2] = m_pColors[4] = PhongReflectionModel(m_pPoints[2], m_pNormals[2], vLightPos,  vLightI);
	m_pColors[1] = PhongReflectionModel(m_pPoints[1], m_pNormals[1], vLightPos,  vLightI);
	m_pColors[5] = PhongReflectionModel(m_pPoints[5], m_pNormals[5], vLightPos,  vLightI);

	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CQuad::Update(float dt, point4 vLightPos, color4 vLightI)
{

#ifdef LIGHTING_WITHCPU
	if( m_bMVUpdated  ) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x,  m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y,  m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z,  m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
#endif
		m_bMVUpdated = false;
	}
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(vLightPos,  vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if( m_bMVUpdated ) {
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_bMVUpdated = false;
	}
	m_vLightInView = m_mxModelView * vLightPos;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * vLightI;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * vLightI;
	m_SpecularProduct = m_Material.ks * m_Material.specular * vLightI;
#endif
	
}

void CQuad::Update(float dt,const structLightSource &LightSource)
{
#ifdef LIGHTING_WITHCPU
	if( m_bMVUpdated  ) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x,  m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y,  m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z,  m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
#endif
		m_bMVUpdated = false;
	}
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(LightSource.position,  LightSource.diffuse);
	else RenderWithGouraudShading(LightSource.position,  LightSource.diffuse);

#else // Lighting With GPU
	if( m_bMVUpdated ) {
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_bMVUpdated = false;
	}
	m_vLightInView = m_mxModelView * LightSource.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * LightSource.ambient;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * LightSource.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * LightSource.specular;
#endif

}


void CQuad::Update(float dt)
{
	if( m_bMVUpdated  ) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
	}
}

void CQuad::SetVtxColors(vec4 vLFColor, vec4 vLRColor, vec4 vTRColor, vec4 vTLColor)
{
	m_pColors[3] = m_pColors[0] = vLFColor;
	m_pColors[1] = vLRColor;
	m_pColors[4] = m_pColors[2] = vTRColor;
	m_pColors[5] = vTLColor;

	// �N�Ҧ� vertices �C���s�� VBO ��
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::SetVtxColors(vec4 vFColor, vec4 vSColor) // �]�w��ӤT���Ϊ��C��
{
	m_pColors[0] = m_pColors[1] = m_pColors[2] = vFColor;
	m_pColors[3] = m_pColors[4] = m_pColors[5] = vSColor;

	// �N�Ҧ� vertices �C���s�� VBO ��
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::Draw()
{
	DrawingSetShader();
//	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
//	glBindTexture(GL_TEXTURE_2D, 0);  // ����{�����K�ϳ]�w
}

void CQuad::DrawW()
{
	DrawingWithoutSetShader();
//	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
//	glBindTexture(GL_TEXTURE_2D, 0);  // ����{�����K�ϳ]�w
}
