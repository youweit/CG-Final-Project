//
//  Multi-Texturing - Environment Mapping
//
// 執行前的準備工作
// 開啟 CShape.h 中的 #define LIGHTING_WITHCPU 
// 關閉 CShape.h 中的 #define LIGHTING_WITHGPU 
// 關閉 CShape.h 中的 #define PERVERTEX_LIGHTING 
//

#include "header/Angel.h"
#include "Common/CQuadMT.h"
#include "Common/CQuad.h"
#include "Common/CWireSphere.h"
#include "Common/CSolidSphereMT.h"
#include "Common/CWireCube.h"
#include "png_loader.h"
#include "Camera.h"

#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number

// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;
Camera *camera;
// For Objects
CQuadMT *g_pFloor;
GLuint   g_uiFloorDiffuseMap; 
GLuint   g_uiFloorLightMap; 
GLuint   g_uiShaderNo;

// For Reflecting Sphere
CSolidSphereMT *g_pSphere;
GLuint   g_uiSphereDiffuseMap; 
GLuint   g_uiSphereCubeMap; 

// For View Point
GLfloat g_fRadius = 10.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;
int g_dx = 1200;
int g_dy = 1200;
point4  g_vEye( g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi),  1.0 );
point4  g_vAt( 0.0, 0.0, 0.0, 1.0 );
vec4    g_vUp( 0.0, 1.0, 0.0, 0.0 );

//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 1;
float g_fLightRadius = 40;
float g_fLightTheta = 0;

float g_fLightR = 0.85f;
float g_fLightG = 0.85f;
float g_fLightB = 0.85f;

structLightSource g_Light1 = {
    color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
    point4(0.0f, 20.0f, 40.0f, 1.0f),   // position
    point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
    vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
	1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
	45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
	1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0		// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
};
CWireSphere *g_pLight;
//----------------------------------------------------------------------------

// Texture 測試用
GLuint g_uiFTexID[3]; // 三個物件分別給不同的貼圖
int g_iTexWidth,  g_iTexHeight;

//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();

void init( void )
{
	mat4 mxT;
	vec4 vT, vColor;
	// 產生所需之 Model View 與 Projection Matrix

	g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );
	g_mxProjection = Perspective(60.0, (GLfloat)SCREEN_SIZE/(GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// 讀進地板的貼圖
	g_uiFloorDiffuseMap = png_load_SOIL("texture/Masonry.Brick.png",&g_iTexWidth, &g_iTexHeight, false);
	g_uiFloorLightMap = png_load_SOIL("texture/lightmap2.png",&g_iTexWidth, &g_iTexHeight, false);

	// 讀進金屬的貼圖與 Cube Map
	g_uiSphereDiffuseMap = png_load_SOIL("texture/metal.png",&g_iTexWidth, &g_iTexHeight, false); 
	g_uiSphereCubeMap = CubeMap_load_SOIL(); 
	camera = new Camera();

	// 產生物件的實體
	g_pFloor = new CQuadMT;
	g_pFloor->SetShader(g_mxModelView, g_mxProjection);
	g_pFloor->SetTRSMatrix(Scale(999,1,999));
	g_pFloor->SetShadingMode(GOURAUD_SHADING);
	g_pFloor->SetTiling(200,200);
	g_pFloor->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pFloor->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pFloor->SetColor(vec4(0.95f,0.95f,0.95f,1.0f));
	g_pFloor->SetLightingDisable();
	g_uiShaderNo = g_pFloor->GetShaderHandle();

	// For Reflecting Sphere
	g_pSphere = new CSolidSphereMT(1.0f, 24,12);
	g_pSphere->SetCubeMapTexName(1); // 使用 Texture1
	g_pSphere->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(0, 2.0f, 0.0f) * RotateX(180) *Scale(2,2,2);
	g_pSphere->SetTRSMatrix(mxT);
	g_pSphere->SetShadingMode(GOURAUD_SHADING);
	g_pSphere->SetViewPosition(g_vEye);
	// 設定貼圖
	g_pSphere->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pSphere->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pSphere->SetColor(vec4(0.9f, 0.9f, 0.9f, 1.0f));
//	g_pSphere->SetLightingDisable();

	// 設定 代表 Light 的 WireSphere ，在這個範例中 燈光是沒有作用的
	g_pLight = new CWireSphere(0.25f, 6, 3);
	g_pLight->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(g_Light1.position);
	g_pLight->SetTRSMatrix(mxT);
	g_pLight->SetColor(g_Light1.diffuse);
	g_pLight->SetLightingDisable();	
}

//----------------------------------------------------------------------------
void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiFloorDiffuseMap); // 與 Diffuse Map 結合
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_2D, g_uiFloorLightMap); // 與 Light Map 結合

	g_pFloor->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);

	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiSphereDiffuseMap); // 與 Diffuse Map 結合
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiSphereCubeMap); // 與 Light Map 結合
	g_pSphere->Draw();

	g_pLight->Draw();
	glutSwapBuffers();	// 交換 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// 每秒繞 Y 軸轉 90 度
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if( g_fLightTheta >= (float)M_PI*2.0f ) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}
	g_Light1.position.x = g_fLightRadius * cosf(g_fLightTheta);
	g_Light1.position.z = g_fLightRadius * sinf(g_fLightTheta);
	mxT = Translate(g_Light1.position);
	g_pLight->SetTRSMatrix(mxT);
}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{

	if( g_bAutoRotating ) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}
	// 如果需要重新計算時，在這邊計算每一個物件的顏色
	g_pSphere->Update(delta, g_Light1);
	g_pFloor->Update(delta, g_Light1);
	g_pLight->Update(delta);
	
	glutWarpPointer(400, 400);
	g_mxModelView = LookAt(
		point4(camera->xPos, camera->yPos, camera->zPos,1.0), 
		point4(
			camera->xPos+camera->directionX, 
			camera->yPos+camera->directionY, 
			camera->zPos+camera->directionZ,1.0), 
		g_vUp
		);
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);
	GL_Display();
}

//----------------------------------------------------------------------------

void Win_Keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case  SPACE_KEY:

		break;
//----------------------------------------------------------------------------
// Part 2 : for single light source
	case 65: // A key
	case 97: // a key
		g_bAutoRotating = !g_bAutoRotating;
		break;
	case 82: // R key
		if( g_fLightR <= 0.95f ) g_fLightR += 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 114: // r key
		if( g_fLightR >= 0.05f ) g_fLightR -= 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 71: // G key
		if( g_fLightG <= 0.95f ) g_fLightG += 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 103: // g key
		if( g_fLightG >= 0.05f ) g_fLightG -= 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 66: // B key
		if( g_fLightB <= 0.95f ) g_fLightB += 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 98: // b key
		if( g_fLightB >= 0.05f ) g_fLightB -= 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
//----------------------------------------------------------------------------
    case 033:
		glutIdleFunc( NULL );
		delete g_pFloor;
		delete g_pLight;
		delete g_pSphere;
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 ，換成 Y 軸
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {
	float fraction = 0.1f;
	switch(key) {
		case GLUT_KEY_LEFT:
            camera->xPos +=  fraction * -cos(g_fPhi);
            camera->zPos +=  fraction * sin(g_fPhi);
            break;
        case GLUT_KEY_RIGHT:
            camera->xPos -=  fraction * -cos(g_fPhi);
            camera->zPos -=  fraction * sin(g_fPhi);
            break;
        case GLUT_KEY_UP:
            camera->xPos += camera->directionX * fraction;
            camera->zPos += camera->directionZ * fraction;
            camera->yPos += camera->directionY * fraction;
            break;
        case GLUT_KEY_DOWN:
            camera->xPos -= camera->directionX * fraction;
            camera->zPos -= camera->directionZ * fraction;
            camera->yPos -= camera->directionY * fraction;
            break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	g_dx += x-400;
	g_dy -= y-400;
	g_fPhi = (float)-M_PI*(g_dx - HALF_SIZE)/(HALF_SIZE); // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)g_dy/SCREEN_SIZE;
	camera->directionX =  g_fRadius*sin(g_fTheta)*sin(g_fPhi);
	camera->directionY =  g_fRadius*cos(g_fTheta);
	camera->directionZ =  g_fRadius*sin(g_fTheta)*cos(g_fPhi);
	
	g_mxModelView = LookAt(
		point4(camera->xPos, camera->yPos, camera->zPos,1.0), 
		point4(
			camera->xPos+camera->directionX, 
			camera->yPos+camera->directionY, 
			camera->zPos+camera->directionZ,1.0), 
		g_vUp
		);
	// Change ModelView Matrix
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);

	// For Cube Map
	g_pSphere->SetViewPosition(g_vEye);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;


	g_vEye.x = g_fRadius*sin(g_fTheta)*sin(g_fPhi);
	g_vEye.y = g_fRadius*cos(g_fTheta);
	g_vEye.z = g_fRadius*sin(g_fTheta)*cos(g_fPhi);

	g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );

	// Change ModelView Matrix
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);

	// For Cube Map
	g_pSphere->SetViewPosition(g_vEye);
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	//  產生 projection 矩陣，此處為產生正投影矩陣
	g_mxProjection = Perspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);

	// 設定棋盤的 Projection Matrix
	g_pFloor->SetProjectionMatrix(g_mxProjection);
	g_pLight->SetProjectionMatrix(g_mxProjection); 
	g_pSphere->SetProjectionMatrix(g_mxProjection);

	glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    
	glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( SCREEN_SIZE, SCREEN_SIZE );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow("Multi-Texturing");

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);
	glutPassiveMotionFunc(Win_PassiveMotion);  
    glutKeyboardFunc( Win_Keyboard );	// 處理 ASCI 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc( Win_SpecialKeyboard);	// 處理 NON-ASCI 按鍵如 F1、Home、方向鍵...等等
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}