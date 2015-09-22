#include "CWireCube.h"

CWireCube::CWireCube()
{

	m_iNumVtx = WIRECUBE_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex1     = new vec2[m_iNumVtx];

    m_vertices[0] = point4( -0.5, -0.5,  0.5, 1.0 );
    m_vertices[1] = point4( -0.5,  0.5,  0.5, 1.0 );
    m_vertices[2] = point4(  0.5,  0.5,  0.5, 1.0 );
    m_vertices[3] = point4(  0.5, -0.5,  0.5, 1.0 );
    m_vertices[4] = point4( -0.5, -0.5, -0.5, 1.0 );
    m_vertices[5] = point4( -0.5,  0.5, -0.5, 1.0 );
    m_vertices[6] = point4(  0.5,  0.5, -0.5, 1.0 );
	m_vertices[7] = point4(  0.5, -0.5, -0.5, 1.0 );

	m_iIndex = 0;
	// generate 12 triangles: 36 vertices and 36 colors
    Quad( 1, 0, 3, 2 );
    Quad( 2, 3, 7, 6 );
    Quad( 3, 0, 4, 7 );
    Quad( 6, 5, 1, 2 );
    Quad( 4, 5, 6, 7 );
    Quad( 5, 4, 0, 1 );

	// �w�]�N�Ҧ��������]�w���Ǧ�
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

	// �]�w����
	SetMaterials(vec4(0), vec4(0.5f, 0.5f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

void CWireCube::Quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = m_vertices[b] - m_vertices[a];
    vec4 v = m_vertices[c] - m_vertices[b];
    vec3 normal = normalize( cross(u, v) );

    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_pTex1[m_iIndex] = vec2(0,0);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[b]; m_pTex1[m_iIndex] = vec2(1,0);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_pTex1[m_iIndex] = vec2(1,1);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[d]; m_pTex1[m_iIndex] = vec2(0,1);m_iIndex++;
}


void CWireCube::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 4 ) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2] + m_pPoints[i+3])/4.0f;
		m_pColors[i] = m_pColors[i+1] = m_pColors[i+2] = m_pColors[i+3] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}


void CWireCube::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	for( int i = 0 ; i < m_iNumVtx ; i += 4 ) {
		m_pColors[i]   = PhongReflectionModel(m_pPoints[i], m_pNormals[i], vLightPos,  vLightI);
		m_pColors[i+1] = PhongReflectionModel(m_pPoints[i+1], m_pNormals[i+1], vLightPos,  vLightI);
		m_pColors[i+2] = PhongReflectionModel(m_pPoints[i+2], m_pNormals[i+2], vLightPos,  vLightI);
		m_pColors[i+3] = PhongReflectionModel(m_pPoints[i+3], m_pNormals[i+3], vLightPos,  vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}


// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CWireCube::Update(float dt, point4 vLightPos, color4 vLightI)
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
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(vLightPos, vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if( m_bMVUpdated  ) {
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

void CWireCube::Update(float dt, const structLightSource &LightSource)
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
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(LightSource.position, LightSource.diffuse);
	else RenderWithGouraudShading(LightSource.position, LightSource.diffuse);

#else // Lighting With GPU
	if( m_bMVUpdated  ) {
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


void CWireCube::Update(float dt)
{
	if( m_bMVUpdated  ) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
	}
}


void CWireCube::Draw()
{
	DrawingSetShader();
	glDrawArrays( GL_LINE_LOOP,  0, 4 );
	glDrawArrays( GL_LINE_LOOP,  4, 4 );
	glDrawArrays( GL_LINE_LOOP,  8, 4 );
	glDrawArrays( GL_LINE_LOOP, 12, 4 );
	glDrawArrays( GL_LINE_LOOP, 16, 4 );
	glDrawArrays( GL_LINE_LOOP, 20, 4 );
}


void CWireCube::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays( GL_LINE_LOOP,  0, 4 );
	glDrawArrays( GL_LINE_LOOP,  4, 4 );
	glDrawArrays( GL_LINE_LOOP,  8, 4 );
	glDrawArrays( GL_LINE_LOOP, 12, 4 );
	glDrawArrays( GL_LINE_LOOP, 16, 4 );
	glDrawArrays( GL_LINE_LOOP, 20, 4 );
}


