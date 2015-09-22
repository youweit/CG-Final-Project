#include "CShapeMT.h"

CShapeMT::CShapeMT()
{
	m_bMVUpdated = m_bProjUpdated = false;

	// ambient 預設為 0, diffuse, specular 的顏色都是灰色 0.5
	// Ka = 0 係數 , kd = 0.8 , ks = 0.2
	m_Material.ambient = vec4(vec3(0));
	m_Material.diffuse = vec4(vec3(0.5f));
	m_Material.specular = vec4(vec3(0.5f));
	m_Material.ka = 0; m_Material.kd = 0.8f; m_Material.ks = 0.2f;
	m_Material.shininess = 2.0f;

	m_iMode = FLAT_SHADING;
	// 預設為 RGBA 為 (0.5,0.5,0.5,1.0) , 由這個灰階顏色來代表的物件顏色
	m_fColor[0] = 0.5f; m_fColor[1] = 0.5f; m_fColor[2] = 0.5f; m_fColor[3] = 1.0f;

	// 如果有使用到 Cube Map 必須在物件呼叫 SetShader 之前透過 SetCubeMapTexName(GLuint uiTexName) 給予正確的 Texture Name
	m_uiCubeMapTexName = 0;


#ifdef LIGHTING_WITHGPU
	m_iLighting = 1; // 預設接受燈光的照明
#endif
}

CShapeMT::~CShapeMT()
{
	if( m_pPoints  != NULL ) delete [] m_pPoints;  
	if( m_pNormals != NULL ) delete	[] m_pNormals;
	if( m_pColors  != NULL ) delete	[] m_pColors;
	if( m_pTex1 != NULL ) delete m_pTex1;
	if( m_pTex2 != NULL ) delete m_pTex2;

	if( m_pVXshader != NULL ) delete [] m_pVXshader;
	if( m_pFSshader != NULL ) delete [] m_pFSshader;
}

void CShapeMT::CreateBufferObject()
{
    glGenVertexArrays( 1, &m_uiVao );
    glBindVertexArray( m_uiVao );

    // Create and initialize a buffer object
    glGenBuffers( 1, &m_uiBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
    glBufferData( GL_ARRAY_BUFFER, (sizeof(vec4) + sizeof(vec3) + sizeof(vec4) + 2*sizeof(vec2))*m_iNumVtx, NULL, GL_STATIC_DRAW );
	// (sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx + sizeof(vec4)*m_iNumVtx <- vertices, normal and color
	// (sizeof(vec4) + sizeof(vec3) + sizeof(vec4) + sizeof(vec2))*m_iNumVtx <- vertices, normal, color and texture coordinates

	// vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vec4)*m_iNumVtx, m_pPoints );  
	// vertices' normal
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx, sizeof(vec3)*m_iNumVtx, m_pNormals );
	// vertcies' Color
	glBufferSubData( GL_ARRAY_BUFFER, (sizeof(vec4)+sizeof(vec3))*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); 
	// Diffuse Map's Texture Coordinates
	glBufferSubData( GL_ARRAY_BUFFER, (sizeof(vec4)+sizeof(vec3)+sizeof(vec4))*m_iNumVtx, sizeof(vec2)*m_iNumVtx, m_pTex1 ); 
	// Light Map's Texture Coordinates
	glBufferSubData( GL_ARRAY_BUFFER, (sizeof(vec4)+sizeof(vec3)+sizeof(vec4)+sizeof(vec2))*m_iNumVtx, sizeof(vec2)*m_iNumVtx, m_pTex2 ); 
}

void CShapeMT::SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle)
{
	// 改放置這裡, 方便每一個物件的設定
	CreateBufferObject();

    // Load shaders and use the resulting shader program
	if( uiShaderHandle == MAX_UNSIGNED_INT) m_uiProgram = InitShader( m_pVXshader, m_pFSshader );
	else m_uiProgram = uiShaderHandle;

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( m_uiProgram, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( m_uiProgram, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)*m_iNumVtx) );

    GLuint vColorVtx = glGetAttribLocation( m_uiProgram, "vVtxColor" );  // vertices' color 
    glEnableVertexAttribArray( vColorVtx );
    glVertexAttribPointer( vColorVtx, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((sizeof(vec4)+sizeof(vec3))*m_iNumVtx));

	// Diffuse Mape Texture Coordiinates
	GLuint vDifMapCoord = glGetAttribLocation( m_uiProgram, "vDiffuseMapCoord" ); 
	glEnableVertexAttribArray( vDifMapCoord );
	glVertexAttribPointer( vDifMapCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((sizeof(vec4)+sizeof(vec3)+sizeof(vec4))*m_iNumVtx));

	// Light Mape Texture Coordiinates
	GLuint vLghMapCoord = glGetAttribLocation( m_uiProgram, "vLightMapCoord" );  
	glEnableVertexAttribArray( vLghMapCoord );
	glVertexAttribPointer( vLghMapCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((sizeof(vec4)+sizeof(vec3)+sizeof(vec4)+sizeof(vec2))*m_iNumVtx));


	m_uiModelView = glGetUniformLocation( m_uiProgram, "ModelView" );
	m_mxMVFinal = m_mxModelView = mxModelView;
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );

    m_uiProjection = glGetUniformLocation( m_uiProgram, "Projection" );
	m_mxProjection = mxProjection;
	glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );

    m_uiColor = glGetUniformLocation( m_uiProgram, "vObjectColor" );
	glUniform4fv(m_uiColor, 1, m_fColor ); 

	m_uiLightInView = glGetUniformLocation( m_uiProgram, "LightInView" );
	glUniform4fv(m_uiLightInView, 1, m_vLightInView); 

	m_uiAmbient = glGetUniformLocation( m_uiProgram, "AmbientProduct" );
	glUniform4fv(m_uiAmbient, 1, m_AmbientProduct); 

	m_uiDiffuse = glGetUniformLocation( m_uiProgram, "DiffuseProduct" );
	glUniform4fv(m_uiDiffuse, 1, m_DiffuseProduct); 

	m_uiSpecular = glGetUniformLocation( m_uiProgram, "SpecularProduct" );
	glUniform4fv(m_uiSpecular, 1, m_SpecularProduct); 

	m_uiShininess = glGetUniformLocation( m_uiProgram, "fShininess" );
	glUniform1f(m_uiShininess, m_Material.shininess); 

	m_uiLighting = glGetUniformLocation( m_uiProgram, "iLighting" );
	glUniform1i(m_uiLighting, m_iLighting); 

	m_uiDiffuseMap  = glGetUniformLocation( m_uiProgram, "diffuMap" );
	glUniform1i(m_uiDiffuseMap, 0); 

	m_uiLightMap = glGetUniformLocation( m_uiProgram, "lightMap" );
	glUniform1i(m_uiLightMap, 1); 

	// For Cube Map
	m_uiCubeMap = glGetUniformLocation( m_uiProgram, "cubeMap" );
	glUniform1i(m_uiCubeMap, m_uiCubeMapTexName); 

	m_uiViewPos = glGetUniformLocation( m_uiProgram, "ViewPos" );
	glUniform4fv(m_uiViewPos, 1, m_v4Eye); 

	m_uiTRS = glGetUniformLocation( m_uiProgram, "TRSMatrix" );
	glUniformMatrix4fv(m_uiTRS, 1, GL_TRUE, m_mxTRS );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void CShapeMT::SetCubeMapTexName(GLuint uiTexName)
{
	m_uiCubeMapTexName = uiTexName;  
}

void CShapeMT::SetViewPosition(point4 vEye)
{
	m_v4Eye = vEye;
}

void CShapeMT::DrawingSetShader()
{
	glUseProgram( m_uiProgram );
	glBindVertexArray( m_uiVao );
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );

	if( m_bProjUpdated ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bProjUpdated = false;
	}
// 將資訊更新到 Shader 中
	glUniform4fv(m_uiColor, 1, m_fColor ); 
	glUniform4fv(m_uiLightInView, 1, m_vLightInView); 
	glUniform4fv(m_uiAmbient, 1, m_AmbientProduct); 
	glUniform4fv(m_uiDiffuse, 1, m_DiffuseProduct); 
	glUniform4fv(m_uiSpecular, 1, m_SpecularProduct); 
	glUniform1f(m_uiShininess, m_Material.shininess); 
	glUniform1i(m_uiLighting, m_iLighting);

	// Update TRS & ViewPos
	glUniform4fv(m_uiViewPos, 1, m_v4Eye); 
	glUniformMatrix4fv(m_uiTRS, 1, GL_TRUE, m_mxTRS );
	//glUniform1i(m_uiDiffuseMap, 0); 
	//glUniform1i(m_uiCubeMap, m_uiCubeMapTexName);
}

// 此處預設使用前一個描繪物件所使用的 Shader
void CShapeMT::DrawingWithoutSetShader()
{
	glBindVertexArray( m_uiVao );
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );

	if( m_bProjUpdated ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bProjUpdated = false;
	}

	glUniform4fv(m_uiColor, 1, m_fColor ); 
	glUniform4fv(m_uiLightInView, 1, m_vLightInView); 
	glUniform4fv(m_uiAmbient,  1, m_AmbientProduct); 
	glUniform4fv(m_uiDiffuse,  1, m_DiffuseProduct); 
	glUniform4fv(m_uiSpecular, 1, m_SpecularProduct); 
	glUniform1f(m_uiShininess, m_Material.shininess); 
	glUniform1i(m_uiLighting, m_iLighting); 

	// Update TRS & ViewPos
	glUniform4fv(m_uiViewPos, 1, m_v4Eye); 
	glUniformMatrix4fv(m_uiTRS, 1, GL_TRUE, m_mxTRS );
	//glUniform1i(m_uiDiffuseMap, 0); 
	//glUniform1i(m_uiLightMap,   1);
}

void CShapeMT::SetShaderName(const char vxShader[], const char fsShader[])
{
	int len;
	len = strlen(vxShader);
	m_pVXshader = new char[len+1];
	memcpy(m_pVXshader, vxShader, len+1);

	len = strlen(fsShader);
	m_pFSshader = new char[len+1];
	memcpy(m_pFSshader, fsShader, len+1);
}

void CShapeMT::SetModelViewMatrix(mat4 &mat)
{
	m_mxModelView = mat;
	m_bMVUpdated = true;
}

void CShapeMT::SetTRSMatrix(mat4 &mat)
{
	m_mxTRS = mat;
	m_bMVUpdated = true;
}

void CShapeMT::SetProjectionMatrix(mat4 &mat)
{
	m_mxProjection = mat;
	m_bProjUpdated = true;
}

void CShapeMT::SetColor(vec4 vColor)
{
	m_fColor[0] = vColor.x;
	m_fColor[1] = vColor.y;
	m_fColor[2] = vColor.z;
	m_fColor[3] = vColor.w;
	glUniform4fv(m_uiColor, 1, m_fColor); 
}

void CShapeMT::SetMaterials(color4 ambient, color4 diffuse, color4 specular)
{
	m_Material.ambient =  ambient;
	m_Material.diffuse =  diffuse;
	m_Material.specular = specular;
}

void CShapeMT::SetKaKdKsShini(float ka, float kd, float ks, float shininess) // ka kd ks shininess
{
	m_Material.ka = ka;
	m_Material.kd = kd;
	m_Material.ks = ks;
	m_Material.shininess = shininess;
}


// Phong Reflection Model
vec4 CShapeMT::PhongReflectionModel(vec4 vPoint, vec3 vNormal, vec4 vLightPos, vec4 vLightI)
{
	vec3 vN;		// 用 vec3 來宣告是為了節省計算, 如果要讓程式寫起來更方便，可改用 vec4 來宣告
	vec4 vColor;	// 該頂點的顏色
	vec4 vPosInView, vLightInView;

	// 1. 將點頂轉到鏡頭座標系統，因為頂點會根據 TRS Matrix 移動，所以必須以 m_mxMVFinal 計算在世界座標的絕對位置
	vPosInView = m_mxMVFinal * vPoint;

	// 2. 將面的 Normal 轉到鏡頭座標系統，並轉成單位向量
	//    只有物件的 rotation 會改變 Normal 的方向，乘上物件的 Final ModelView Matrix，同時轉換到鏡頭座標
	//    m_mxMV3X3Final 在 Update 處取自  m_mxMVFinal 的左上的 3X3 部分, 不需要位移量
#ifdef GENERAL_CASE
	//	  m_mxITMV 已經在呼叫 SetModelViewMatrix(mat4 &mat) 與 SetTRSMatrix(mat4 &mat) 就同時計算
	//    此處就不用再重新計算
	vN = m_mxITMV * vNormal;
#else
	vN = m_mxMV3X3Final * vNormal;
#endif

	vN = normalize(vN);

	// 3. 計算 Ambient color :  Ia = Ka * Material.ambient * La
	vColor = m_Material.ka * m_Material.ambient * vLightI;

	// 4. 計算 Light 在鏡頭座標系統的位置
	//    此處假設 Light 的位置已經在世界座標的絕對位置
	vLightInView = m_mxModelView * vLightPos; 

	vec3 vLight; // 用 vec3 來宣告是為了節省計算, 如果要讓程式寫起來更方便，可改用 vec4 來宣告
	vLight.x = vLightInView.x - vPosInView.x;   
	vLight.y = vLightInView.y - vPosInView.y;
	vLight.z = vLightInView.z - vPosInView.z;
	vLight = normalize(vLight); // normalize light vector

	// 5. 計算 L dot N
	GLfloat fLdotN = vLight.x*vN.x + vLight.y*vN.y + vLight.z*vN.z;
	if( fLdotN > 0 ) { // 該點被光源照到才需要計算
		// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
		vColor +=  m_Material.kd * m_Material.diffuse * vLightI * fLdotN; 

		// Specular color
		// Method 1: Phone Model
		//   計算 View Vector
		vec3 vView;	
		vView.x = 0 - vPosInView.x;  // 目前已經以鏡頭座標為基礎, 所以 View 的位置就是 (0,0,0)
		vView.y = 0 - vPosInView.y;
		vView.z = 0 - vPosInView.z;
		vView = normalize(vView);

		//	 計算 Light 的 反射角 vRefL
		vec3 vRefL = 2.0f * (fLdotN) * vN - vLight;

		vRefL = normalize(vRefL);
		//   計算  vReflectedL dot View
		GLfloat RdotV = vRefL.x*vView.x + vRefL.y*vView.y + vRefL.z*vView.z;

		// Specular Color : Is = Ks * Ls * (R dot V)^Shininess;
		if( RdotV > 0 ) vColor += m_Material.ks * m_Material.specular * * vLightI * powf(RdotV, m_Material.shininess); 
	}

	vColor.w = 1; // Alpha 改設定成 1，預設都是不透明物體
	// Method 2: Modified Phone Model 

	// 將顏色儲存到  m_Colors 中，因為 Quad 是兩個共平面的三角面所構成, 所以設定兩個三角面都有相同的顏色
	// 也就是設定所有的頂點都是這個顏色
	return vColor;
}

// For controlling texture mapping ----------------
void CShapeMT::SetMirror(bool uAxis, bool vAxis) // U軸 與 V軸 是否要鏡射
{
	if( uAxis )  {
		if( vAxis ) { // U V 軸同時鏡射
			for(int i = 0 ; i < m_iNumVtx ; i++ ) { // 將每一個 vertex 的貼圖座標用 1 去減
				m_pTex1[i].x -= 1.0f; m_pTex1[i].y -= 1.0f;
			}
		}
		else { // 只有 U 軸鏡射
			for(int i = 0 ; i < m_iNumVtx ; i++ ) { // 將每一個 vertex 的貼圖座標用 1 去減
				m_pTex1[i].x -= 1.0f; // x 就是 U 軸
			}
		}
	}
	else if( vAxis ) { // 只有 V 軸鏡射
		for(int i = 0 ; i < m_iNumVtx ; i++ ) { // 將每一個 vertex 的貼圖座標用 1 去減
			m_pTex1[i].y -= 1.0f; // y 為 V 軸
		}
	}
	else ;
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, (sizeof(vec4)+sizeof(vec3)+sizeof(vec4))*m_iNumVtx, sizeof(vec2)*m_iNumVtx, m_pTex1 ); // vertcies' Color

}

void CShapeMT::SetTiling(float uTiling, float vTiling)  // 對 U軸 與 V軸 進行拼貼的縮放
{
	for(int i = 0 ; i < m_iNumVtx ; i++ ) { // 將每一個 vertex 的貼圖座標用 1 去減
		m_pTex1[i].x *= uTiling; m_pTex1[i].y *= vTiling;
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, (sizeof(vec4)+sizeof(vec3)+sizeof(vec4))*m_iNumVtx, sizeof(vec2)*m_iNumVtx, m_pTex1 ); // vertcies' Color
}


// For loading texture and setting texture object
GLuint CShapeMT::LoadPngImageAndSetTextureObject(const char *texMapName)
{
	GLubyte *texArray = NULL;
	texArray = LoadPngImageSOIL(texMapName, m_iTexWidth[0], m_iTexHeight[0], m_iTexBitDepth[0], m_iTexFormat[0]);
	if( texArray == NULL) { // 檔案讀取錯誤
		printf("%s 檔案讀取錯誤\n",texMapName);
	}
	else { // 設定 Texture Object
		glGenTextures(1, &m_uiTexObject[0]);
		glBindTexture(GL_TEXTURE_2D, m_uiTexObject[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, m_iTexFormat[0], m_iTexWidth[0], m_iTexHeight[0], 0,
			m_iTexFormat[0], GL_UNSIGNED_BYTE, texArray);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// clean up
		free(texArray);
	}
	return (m_uiTexObject[0]);
}

//-----------------------------------------------------------------------------------------------------------------------------
// LoadPngImageSOIL 使用 SOIL 讀取 PNG 檔
//   使用  SOIL 不需要 DLL

GLubyte * CShapeMT::LoadPngImageSOIL(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat)
{
	GLint iwidth, iheight, ichannel, row_bytes;
	GLubyte *texArray, *imageData;
	texArray = SOIL_load_image(file_name, &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
	row_bytes = iwidth*ichannel;
	imageData = (GLubyte *)malloc(row_bytes*iheight*sizeof(GLubyte));

	// 對 texArray 執行垂直鏡射，放到 imageData 中
    for (int i = 0; i < iheight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
    }
	outWidth = iwidth;
	outHeight = iheight;
	outBitDepth = 8; // 預設設定為 8, SOIL 沒有辦法檢查

	switch(ichannel) 
	{
		case 3:
			outFormat = GL_RGB;
			break;
		case 4:
			outFormat = GL_RGBA;
			break;
		default:
			fprintf(stderr, "%s: Unknown libpng with %d channel.\n", file_name, ichannel);
			return NULL;
			break;
	}
	free(texArray);
	return imageData;
}

//-----------------------------------------------------------------------------------------------------------------------------
// LoadPngImageLIBPNG 使用 LibPng 讀取 PNG 檔，提供參考用
//   使用  LIBPNG 不需要 libpng16.dll 放在工作目錄下
// 
GLubyte * CShapeMT::LoadPngImageLIBPNG(const char *file_name, int &outWidth, int &outHeight, int &outBitDepth, int &outFormat) {

    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return NULL;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return NULL;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return NULL;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return NULL;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return NULL;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type, interlace_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        &interlace_type, NULL, NULL);

    outWidth = temp_width;
    outHeight = temp_height;
	outBitDepth = bit_depth;


    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

    if (bit_depth != 8)
    {
        fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
        return NULL;
    }

    switch(color_type)
    {
    case PNG_COLOR_TYPE_RGB:
        outFormat = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        outFormat = GL_RGBA;
        break;
    default:
        fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
        return NULL;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return NULL;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return NULL;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(row_pointers);
    fclose(fp);
    return image_data;
}

//-----------------------------------------------------------------------------------------------------------------------------