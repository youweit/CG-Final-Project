#ifndef CSHAPEMT_H
#define CSHAPEMT_H
#define  _CRT_SECURE_NO_WARNINGS 
#include "../Header/Angel.h"
#include "../LibPng/png.h"
#include "../SOIL/SOIL.h"
#include "TypeDefine.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define FLAT_SHADING    0
#define GOURAUD_SHADING 1

// GPU 的計算必須傳更多的參數進入 Shader

#define LIGHTING_WITHGPU

// 當模型有執行 non-uniform scale 的操作時，必須透過計算反矩陣來得到正確的 Normal 方向
// 開啟以下的定義即可，目前 CPU 計算的有提供
// GPU 的部分則是設定成註解

// #define GENERAL_CASE 1 

class CShapeMT 
{
protected:
	vec4 *m_pPoints;
	vec3 *m_pNormals;
	vec4 *m_pColors;
	vec2 *m_pTex1;		// Diffuse Map
	vec2 *m_pTex2;		// Light Map
	int  m_iNumVtx;

	GLfloat m_fColor[4]; // Object's color
	// For shaders' name
	char *m_pVXshader, *m_pFSshader;

	// For VAO
	GLuint m_uiVao;

	// For Shader
	GLuint  m_uiModelView, m_uiProjection, m_uiColor;
	GLuint  m_uiProgram;
	GLuint  m_uiBuffer;
	GLuint  m_uiTRS;  // For Cube Mapping
	GLuint  m_uiViewPos; // For Cube Mapping
	point4  m_v4Eye; // Camera 的位置

	// For Texture Object
	GLuint m_uiTexObject[4];	// 先預設四個貼圖物件
	int    m_iTexWidth[4], m_iTexHeight[4]; // 貼圖的寬跟高
	int    m_iTexBitDepth[4];	// 貼圖每一個 pixel 的深度
	int    m_iTexFormat[4];	// 貼圖的格式 GL_RGB or GL_RGBA
	bool   m_bWithAlpha[4]; // 貼圖本身帶有 alpha channel
	
	point4  m_vLightInView;	 // 光源在世界座標的位置
	GLuint  m_uiLightInView;	 // 光源在 shader 的位置
	GLuint  m_uiAmbient;		 // light's ambient  與 Object's ambient  與 ka 的乘積
	GLuint  m_uiDiffuse;		 // light's diffuse  與 Object's diffuse  與 kd 的乘積
	GLuint  m_uiSpecular;	 // light's specular 與 Object's specular 與 ks 的乘積
	GLuint  m_uiShininess;
	GLuint  m_uiLighting;
	GLuint  m_uiDiffuseMap;	 // pixel shader 中的 DiffuseMap Texture Name 的位置
	GLuint  m_uiLightMap;    // pixel shader 中的 LightMap Texture Name 的位置
	GLuint  m_uiCubeMap;     // pixel shader 中的 CubeMap Texture Name 的位置

	GLuint  m_uiCubeMapTexName;  // 傳入 pixel shader 中的 Cube map 的貼圖編號， 


	structLightSource m_Light1;

	color4 m_AmbientProduct;
	color4 m_DiffuseProduct;
	color4 m_SpecularProduct;
	int    m_iLighting;	// 設定是否要打燈

	// For Matrices
	mat4    m_mxModelView, m_mxProjection, m_mxTRS;
	mat4    m_mxMVFinal;
	mat3    m_mxMV3X3Final;		// 使用在計算 物體旋轉後的新 Normal
	mat3		m_mxITMV;
	
	bool    m_bMVUpdated, m_bProjUpdated;

	// For materials
	structMaterial m_Material;

	// For Shading Mode
	int m_iMode;		// 0: Flat shading, 1: Gouraud shading, 0 for default

	void		CreateBufferObject();
	void		DrawingSetShader();
	void		DrawingWithoutSetShader();

public:
	CShapeMT();
	virtual ~CShapeMT();
	virtual void Draw() = 0;
	virtual void DrawW() = 0; // Drawing without setting shaders
	virtual void Update(float dt, point4 vLightPos, color4 vLightI) = 0;

	void SetShaderName(const char vxShader[], const char fsShader[]);
	void SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	void SetColor(vec4 vColor);
	void SetModelViewMatrix(mat4 &mat);
	void SetProjectionMatrix(mat4 &mat);
	void SetTRSMatrix(mat4 &mat);

	// For setting materials 
	void SetMaterials(color4 ambient, color4 diffuse, color4 specular);
	void SetKaKdKsShini(float ka, float kd, float ks, float shininess); // ka kd ks shininess

	// For Lighting Calculation
	void SetShadingMode(int iMode) {m_iMode = iMode;}
	vec4 PhongReflectionModel(vec4 vPoint, vec3 vNormal, vec4 vLightPos, vec4 vLightI);


	// For controlling texture mapping
	void SetMirror(bool uAxis, bool vAxis); // U軸 與 V軸 是否要鏡射
	void SetTiling(float uTiling, float vTiling);  // 對 U軸 與 V軸 進行拼貼的縮放

	// For loading texture and setting texture object
	GLuint LoadPngImageAndSetTextureObject(const char *texMapName);
	GLubyte * LoadPngImageLIBPNG(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat);
	GLubyte * LoadPngImageSOIL(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat);

	void SetLightingDisable() {m_iLighting = 0;}
	vec3 GetPosition(){
		mat4 c = m_mxTRS;
		return vec3(c[0][3], c[1][3], c[2][3]);
	}
	// For Cube Map
	void SetCubeMapTexName(GLuint uiTexName);
	void SetViewPosition(point4 vEye) ;
};

#endif
