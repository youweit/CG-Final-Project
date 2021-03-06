#include "CSolidCube.h"
//for testing
#define CUBE_LENGTH 3
#define CUBE_WIDTH 3
#define CUBE_HEIGHT 3

CSolidCube::CSolidCube()
{
	m_iNumVtx = SOLIDCUBE_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex1     = new vec2[m_iNumVtx];
	
	m_vertices[0] = point4(-m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[1] = point4(-m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[2] = point4(m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[3] = point4(m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[4] = point4(-m_fWidth, -m_fHeight, -m_fLength, 1.0);
	m_vertices[5] = point4(-m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[6] = point4(m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[7] = point4(m_fWidth, -m_fHeight, -m_fLength, 1.0);

	m_iIndex = 0;
	// generate 12 triangles: 36 vertices and 36 colors
    Quad( 1, 0, 3, 2 ); //0
    Quad( 2, 3, 7, 6 ); //6
    Quad( 3, 0, 4, 7 ); //12
    Quad( 6, 5, 1, 2 ); //18
    Quad( 4, 5, 6, 7 ); //24
    Quad( 5, 4, 0, 1 ); //32

	// 預設將所有的面都設定成灰色
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

	// Create and initialize a buffer object ，將此部分的設定移入 SetShader 中
//	CreateBufferObject();

	// 設定材質
	SetMaterials(vec4(0), vec4(0.4f, 0.4f, 0.4f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

//長寬高
void CSolidCube::SetWallSize(vec3 size)
{
	m_fLength = size.x / 2;
	m_fWidth = size.y / 2;
	m_fHeight = size.z / 2;

	m_vertices[0] = point4(-m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[1] = point4(-m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[2] = point4(m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[3] = point4(m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[4] = point4(-m_fWidth, -m_fHeight, -m_fLength, 1.0);
	m_vertices[5] = point4(-m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[6] = point4(m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[7] = point4(m_fWidth, -m_fHeight, -m_fLength, 1.0);

	m_max = GetPosition() + vec3(m_fWidth, m_fLength, m_fHeight);
	m_min = GetPosition() - vec3(m_fWidth, m_fLength, m_fHeight);

}
CSolidCube::CSolidCube(vec3 size)
{
	m_fLength = size.x / 2;
	m_fWidth = size.y / 2;
	m_fHeight = size.z / 2;
	
	m_iNumVtx = SOLIDCUBE_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors = new vec4[m_iNumVtx];
	m_pTex1 = new vec2[m_iNumVtx];
	
	m_vertices[0] = point4(-m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[1] = point4(-m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[2] = point4(m_fWidth, m_fHeight, m_fLength, 1.0);
	m_vertices[3] = point4(m_fWidth, -m_fHeight, m_fLength, 1.0);
	m_vertices[4] = point4(-m_fWidth, -m_fHeight, -m_fLength, 1.0);
	m_vertices[5] = point4(-m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[6] = point4(m_fWidth, m_fHeight, -m_fLength, 1.0);
	m_vertices[7] = point4(m_fWidth, -m_fHeight, -m_fLength, 1.0);

	m_iIndex = 0;
	// generate 12 triangles: 36 vertices and 36 colors
	Quad(1, 0, 3, 2); //0
	Quad(2, 3, 7, 6); //6
	Quad(3, 0, 4, 7); //12
	Quad(6, 5, 1, 2); //18
	Quad(4, 5, 6, 7); //24
	Quad(5, 4, 0, 1); //32

	// 預設將所有的面都設定成灰色
	for (int i = 0; i < m_iNumVtx; i++) m_pColors[i] = vec4(-1.0f, -1.0f, -1.0f, 1.0f);

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

	// Create and initialize a buffer object ，將此部分的設定移入 SetShader 中
	//	CreateBufferObject();

	// 設定材質
	SetMaterials(vec4(0), vec4(0.4f, 0.4f, 0.4f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

void CSolidCube::Quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = m_vertices[b] - m_vertices[a];
    vec4 v = m_vertices[c] - m_vertices[b];
    vec3 normal = normalize( cross(u, v) );

    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_pTex1[m_iIndex] = vec2(0,0);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[b]; m_pTex1[m_iIndex] = vec2(1,0);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_pTex1[m_iIndex] = vec2(1,1);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_pTex1[m_iIndex] = vec2(0,0);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_pTex1[m_iIndex] = vec2(1,1);m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[d]; m_pTex1[m_iIndex] = vec2(0,1);m_iIndex++;
}

void CSolidCube::Draw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
	DrawingSetShader();
	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, SOLIDCUBE_NUM );
	glBindTexture(GL_TEXTURE_2D, 0); 
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Return to solid mode
}


void CSolidCube::DrawW()
{
	DrawingWithoutSetShader();
	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, SOLIDCUBE_NUM );
	glBindTexture(GL_TEXTURE_2D, 0); 
}


void CSolidCube::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// 以每一個面的三個頂點計算其重心，以該重心作為顏色計算的點頂
	// 根據 Phong lighting model 計算相對應的顏色，並將顏色儲存到此三個點頂
	// 因為每一個平面的頂點的 Normal 都相同，所以此處並沒有計算此三個頂點的平均 Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 3 ) {
		// 計算三角形的重心
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2])/3.0f;
		m_pColors[i] = m_pColors[i+1] = m_pColors[i+2] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}


void CSolidCube::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
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

// 此處所給的 vLightPos 必須是世界座標的確定絕對位置
void CSolidCube::Update(float dt, point4 vLightPos, color4 vLightI)
{
#ifdef LIGHTING_WITHCPU
	if( m_bMVUpdated  ) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxModelView * vLightPos;		// 將 Light 轉換到鏡頭座標再傳入
	// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * vLightI;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * vLightI;
	m_SpecularProduct = m_Material.ks * m_Material.specular * vLightI;
#endif
}


void CSolidCube::Update(float dt, const structLightSource &LightSource)
{
#ifdef LIGHTING_WITHCPU
	if( m_bMVUpdated  ) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxModelView * LightSource.position;		// 將 Light 轉換到鏡頭座標再傳入
	// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * LightSource.ambient;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * LightSource.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * LightSource.specular;
#endif

	m_max = GetPosition() + vec3(m_fWidth, m_fHeight, m_fLength);
	m_min = GetPosition() - vec3(m_fWidth, m_fHeight, m_fLength);

}

void CSolidCube::Update(float dt)
{
	if( m_bMVUpdated  ) { // Model View 的相關矩陣內容有更動
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
	}
}