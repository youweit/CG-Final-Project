#include "CSolidPrism.h"

CSolidPrism::CSolidPrism()
{

	float radius = 1.5f;
	float hight = 1.6f;

	m_iNumVtx = SOLIDPRISM_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex1    = new vec2[m_iNumVtx];

	for (int i = 0; i < 6; i++) {
		m_vertices[i] = vec4(radius * cosf(M_PI*2.0f*i / 6), 0.0f, radius * sinf(M_PI*2.0f*i / 6), 1.0f);
		//printf("%f %f %f \n", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
	}

	for (int i = 6; i < 12; i++) {
		m_vertices[i] = vec4(radius * cosf(M_PI*2.0f*i / 6), hight, radius * sinf(M_PI*2.0f*i / 6), 1.0f);
		//printf("%f %f %f \n", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
	}

	
	m_iIndex = 0;

	Quad(1, 0, 2, 5);
	Quad(2, 3, 4, 5);

	//side
	for (int i = 0; i < 5; i++){
		Quad(i, i+6, i+7, i+1);
	}
	Quad(5, 11, 6, 0);

	Quad(11, 8, 7, 6);
	Quad(11, 10, 9, 8);
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
//	CreateBufferObject();

	// �]�w����
	SetMaterials(vec4(0), vec4(0.5f, 0.5f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

void CSolidPrism::Quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = m_vertices[b] - m_vertices[a];
    vec4 v = m_vertices[c] - m_vertices[b];
    vec3 normal = normalize( cross(u, v) );

    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[b]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[d]; m_iIndex++;
	
}

void CSolidPrism::Draw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
	DrawingSetShader();
	glDrawArrays(GL_TRIANGLES, 0, SOLIDPRISM_NUM);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Return to solid mode
}


void CSolidPrism::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays(GL_TRIANGLES, 0, SOLIDPRISM_NUM);
}


void CSolidPrism::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
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


void CSolidPrism::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 6 ) {
		m_pColors[i]   = m_pColors[i+3] = PhongReflectionModel(m_pPoints[i], m_pNormals[i], vLightPos,  vLightI);
		m_pColors[i+2] = m_pColors[i+4] = PhongReflectionModel(m_pPoints[i+2], m_pNormals[i+2], vLightPos,  vLightI);
		m_pColors[i+1] = PhongReflectionModel(m_pPoints[i+1], m_pNormals[i+1], vLightPos,  vLightI);
		m_pColors[i+5] = PhongReflectionModel(m_pPoints[i+5], m_pNormals[i+5], vLightPos,  vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CSolidPrism::Update(float dt, point4 vLightPos, color4 vLightI)
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
	//for (int i = 0; i < LIGHTS_NUM; i++){
		m_vLightInView = m_mxModelView * vLightPos;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
		// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
		m_AmbientProduct = m_Material.ka * m_Material.ambient  * vLightI;
		m_DiffuseProduct = m_Material.kd * m_Material.diffuse  * vLightI;
		m_SpecularProduct = m_Material.ks * m_Material.specular * vLightI;
	//}
#endif
}


void CSolidPrism::Update(float dt, const structLightSource &LightSource)
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
	//for (int i = 0; i < LIGHTS_NUM; i++){

		m_vLightInView = m_mxModelView * LightSource.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
		// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
		m_AmbientProduct = m_Material.ka * m_Material.ambient  * LightSource.ambient;
		m_DiffuseProduct = m_Material.kd * m_Material.diffuse  * LightSource.diffuse;
		m_SpecularProduct = m_Material.ks * m_Material.specular * LightSource.specular;

	//}
#endif

}

void CSolidPrism::Update(float dt)
{
	if( m_bMVUpdated  ) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
	}
}