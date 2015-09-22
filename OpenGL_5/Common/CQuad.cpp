#include "CQuad.h"
// Example 4 開始
// 面同 Example 3 朝上(Y軸)
// 每一個 Vertex 增加 Normal ，改成繼曾自 CShape，一併處理相關的設定需求


CQuad::CQuad()
{
	m_iNumVtx = QUAD_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex1 = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex1 = new vec2[m_iNumVtx];		// 產生所需的貼圖座標

	m_pPoints[0] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[1] = vec4(  0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[2] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[3] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[4] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[5] = vec4( -0.5f, 0.0f, -0.5f, 1.0f);

	m_pNormals[0] = vec3(  0, 1.0f, 0);  // Normal Vector 的 W 為 0
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

	// Create and initialize a buffer object ，將此部分的設定移入 SetShader 中
	// CreateBufferObject();
}

void CQuad::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
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


void CQuad::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	// 以 vertex 為單位，根據該點的位置與其法向量，利用 Phong lighting model 計算相對應的顏色
	// 將該顏色儲存回該頂點

	// 第六個點頂構成的兩個三角形
	// 計算 0 1 2 5 四個頂點的顏色即可，0 與 3、2 與 4 的顏色相同

	m_pColors[0] = m_pColors[3] = PhongReflectionModel(m_pPoints[0], m_pNormals[0], vLightPos,  vLightI);
	m_pColors[2] = m_pColors[4] = PhongReflectionModel(m_pPoints[2], m_pNormals[2], vLightPos,  vLightI);
	m_pColors[1] = PhongReflectionModel(m_pPoints[1], m_pNormals[1], vLightPos,  vLightI);
	m_pColors[5] = PhongReflectionModel(m_pPoints[5], m_pNormals[5], vLightPos,  vLightI);

	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

// 此處所給的 vLightPos 必須是世界座標的確定絕對位置
void CQuad::Update(float dt, point4 vLightPos, color4 vLightI)
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
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(vLightPos,  vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if( m_bMVUpdated ) {
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

void CQuad::Update(float dt,const structLightSource &LightSource)
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
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(LightSource.position,  LightSource.diffuse);
	else RenderWithGouraudShading(LightSource.position,  LightSource.diffuse);

#else // Lighting With GPU
	if( m_bMVUpdated ) {
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		m_bMVUpdated = false;
	}
	m_vLightInView = m_mxModelView * LightSource.position;		// 將 Light 轉換到鏡頭座標再傳入
	// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * LightSource.ambient;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * LightSource.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * LightSource.specular;
#endif

}


void CQuad::Update(float dt)
{
	if( m_bMVUpdated  ) { // Model View 的相關矩陣內容有更動
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

	// 將所有 vertices 顏色更新到 VBO 中
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::SetVtxColors(vec4 vFColor, vec4 vSColor) // 設定兩個三角形的顏色
{
	m_pColors[0] = m_pColors[1] = m_pColors[2] = vFColor;
	m_pColors[3] = m_pColors[4] = m_pColors[5] = vSColor;

	// 將所有 vertices 顏色更新到 VBO 中
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::Draw()
{
	DrawingSetShader();
//	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
//	glBindTexture(GL_TEXTURE_2D, 0);  // 釋放現有的貼圖設定
}

void CQuad::DrawW()
{
	DrawingWithoutSetShader();
//	glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]); 
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
//	glBindTexture(GL_TEXTURE_2D, 0);  // 釋放現有的貼圖設定
}
