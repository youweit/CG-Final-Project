#ifndef CSHAPE_H
#define CSHAPE_H
#define  _CRT_SECURE_NO_WARNINGS 
#include "../Header/Angel.h"
#include "../LibPng/png.h"
#include "../SOIL/SOIL.h"
#include "TypeDefine.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define FLAT_SHADING    0
#define GOURAUD_SHADING 1

// GPU ���p�⥲���ǧ�h���Ѽƶi�J Shader

//#define LIGHTING_WITHCPU
#define LIGHTING_WITHGPU
//#define PERVERTEX_LIGHTING

// ��ҫ������� non-uniform scale ���ާ@�ɡA�����z�L�p��ϯx�}�ӱo�쥿�T�� Normal ��V
// �}�ҥH�U���w�q�Y�i�A�ثe CPU �p�⪺������
// GPU �������h�O�]�w������

// #define GENERAL_CASE 1 

class CShape 
{
protected:
	vec4 *m_pPoints;
	vec3 *m_pNormals;
	vec4 *m_pColors;
	vec2 *m_pTex1;
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

	// For Texture Object
	GLuint m_uiTexObject[4];	// ���w�]�|�ӶK�Ϫ���
	int    m_iTexWidth[4], m_iTexHeight[4]; // �K�Ϫ��e��
	int    m_iTexBitDepth[4];	// �K�ϨC�@�� pixel ���`��
	int    m_iTexFormat[4];	// �K�Ϫ��榡 GL_RGB or GL_RGBA
	bool   m_bWithAlpha[4]; // �K�ϥ����a�� alpha channel
	

#ifdef LIGHTING_WITHGPU
	point4  m_vLightInView;	 // �����b�@�ɮy�Ъ���m
	GLuint  m_uiLightInView;	 // �����b shader ����m
	GLuint  m_uiAmbient;		 // light's ambient  �P Object's ambient  �P ka �����n
	GLuint  m_uiDiffuse;		 // light's diffuse  �P Object's diffuse  �P kd �����n
	GLuint  m_uiSpecular;	 // light's specular �P Object's specular �P ks �����n
	GLuint  m_uiShininess;
	GLuint  m_uiLighting;
	GLuint  m_uiSpotDir;
	structLightSource m_Light1;
	vec4   m_SpotDir;
	color4 m_AmbientProduct;
	color4 m_DiffuseProduct;
	color4 m_SpecularProduct;
	int    m_iLighting;	// �]�w�O�_�n���O
#endif

	// For Matrices
	mat4    m_mxModelView, m_mxProjection, m_mxTRS;
	mat4    m_mxMVFinal;
	mat3    m_mxMV3X3Final;		// �ϥΦb�p�� �������᪺�s Normal
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
	CShape();
	virtual ~CShape();
	virtual void Draw() = 0;
	virtual void DrawW() = 0; // Drawing without setting shaders
	virtual void Update(float dt, point4 vLightPos, color4 vLightI) = 0;

	void SetShaderName(const char vxShader[], const char fsShader[]);
	void SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	void SetColor(vec4 vColor);
	void SetModelViewMatrix(mat4 &mat);
	void SetProjectionMatrix(mat4 &mat);
	void SetTRSMatrix(mat4 &mat);
	vec3* GetFaceNormal(){ return m_pNormals; };
	// For setting materials 
	void SetMaterials(color4 ambient, color4 diffuse, color4 specular);
	void SetKaKdKsShini(float ka, float kd, float ks, float shininess); // ka kd ks shininess

	// For Lighting Calculation
	void SetShadingMode(int iMode) {m_iMode = iMode;}
	vec4 PhongReflectionModel(vec4 vPoint, vec3 vNormal, vec4 vLightPos, vec4 vLightI);


	// For controlling texture mapping
	void SetMirror(bool uAxis, bool vAxis); // U�b �P V�b �O�_�n��g
	void SetTiling(float uTiling, float vTiling);  // �� U�b �P V�b �i����K���Y��

	vec3 GetPosition(){
		mat4 c = m_mxTRS;
		return vec3(c[0][3],c[1][3],c[2][3]);
	}

	// For loading texture and setting texture object
	GLuint LoadPngImageAndSetTextureObject(const char *texMapName);
	GLubyte * LoadPngImageLIBPNG(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat);
	GLubyte * LoadPngImageSOIL(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat);



#ifdef LIGHTING_WITHGPU
	void SetLightingDisable() {m_iLighting = 0;}
#endif

};

#endif
