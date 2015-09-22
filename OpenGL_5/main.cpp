/*

(完成) (25%)畫出六個房間，並達成以下的條件（編譯要能過且正常執行才算）
- 每一面牆都必須有貼圖
- 必須至少有三個房間是使用燈光照明，而且是使用 Per Pixel Lighting
- 每一個房間中都必須有放置裝飾品(部分必須有貼圖)
- 鏡頭可以在裡面任意的移動
(完成) (12%)環境與操控
- (2%) 鏡頭的移動是根據目前的視角方向
- (3%) 不會穿牆
- (2%) 會被機關觸動的動態移動光源，使用 Per Pixel Lighting
- (2%) 至少三面牆壁上半透明的玻璃
- (3%) 能處理不同視角下玻璃能正確顯示

(完成) (6%)圖學相關功能的使用，必須在程式碼中以註解清楚標明
- (2%) 針對特定物件實現 Billboards 的功能
- (2%) 使用到  Mipmapped 的功能 （必須有具體的說明在程式碼中）
- (2%) 有房間使用到 Light Map 的功能 （必須有具體的說明在程式碼中）

(3%) 其他你覺得可以展示的技術，包含物理或是數學的運算
-到處擺設玻璃
-可以破壞房間內的擺設,然後再放回去

總分：46

*/

#include "header/Angel.h"
#include "Common/CQuadMT.h"
#include "Common/CQuad.h"
#include "Common/CWireSphere.h"
#include "Common/CSolidSphereMT.h"
#include "Common/CWireCube.h"
#include "png_loader.h"
#include "Camera.h"
#include "Common/CSolidCube.h"
#include <iostream>
#include "CController.h"
#include "CWall.h"
#include "Common/CSolidPrism.h"
#include "Common/LinkedList.h"
using namespace std;

#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number
#define WALL_HEIGHT 8
// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;
Camera *camera;
CController *g_controller;
// For Objects
CQuadMT *g_pFloor,*g_pLightMapFloor;
GLuint   g_uiFloorDiffuseMap; 
GLuint   g_uiFloorLightMap; 
GLuint   g_uiShaderNo;
bool g_bMove = true;
bool g_bHaveCube = false, g_bHaveCubeFlag = false;
// For Reflecting Sphere
CSolidSphereMT *g_pSphere;
GLuint   g_uiSphereDiffuseMap; 
GLuint   g_uiSphereCubeMap; 
GLuint   g_uiTransparentCubeMap;
LinkedList<CSolidCube *> *g_HandCubes;
// For View Point
GLfloat g_fRadius = 10.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;
CQuad  *g_pMatch[1];
GLuint g_uiMatchTexID[3];

int g_dx = 1200;
int g_dy = 1200;


float fraction = 0.007f;

point4  g_vEye( g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi),  1.0 );
point4  g_vAt( 0.0, 0.0, 0.0, 1.0 );


//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 1;
float g_fLightRadius = 40;
float g_fLightTheta = 0;

float g_fLightR = 0.85f;
float g_fLightG = 0.85f;
float g_fLightB = 0.85f;
mat4 mxT;
CWall *g_pWalls;
CSolidCube	*g_pPlayer, *g_pCube, *test;
CSolidPrism *g_pHexPrism;


structLightSource g_Light1 = {
    color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
    point4(0.0f,1.0f, 0.0f, 1.0f),   // position
    point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
    vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
	1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
	45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
	1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0		// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
};

WallSource g_WallsParams[9] = {
	{
		vec3(0, 4, 0),
		vec3(36,1, WALL_HEIGHT)
	},
	{
		vec3(0, 4, 28),
		vec3(11, 1, WALL_HEIGHT)
	},
	{
		vec3(0, 4, -28),
		vec3(11, 1, WALL_HEIGHT)
	},
	{
		vec3(0, 4, 11),
		vec3(1, 22, WALL_HEIGHT)
	},
	{
		vec3(0, 4, -11),
		vec3(1, 22, WALL_HEIGHT)
	},
	{
		vec3(-21, 4, 11),
		vec3(1, 11, WALL_HEIGHT)
	},
	{
		vec3(21, 4, 11),
		vec3(1, 11, WALL_HEIGHT)
	},
	{
		vec3(21, 4, -11),
		vec3(1, 11, WALL_HEIGHT)
	},
	{
		vec3(-21, 4, -11),
		vec3(1, 11, WALL_HEIGHT)
	}
};


CWireSphere *g_pLight;
//----------------------------------------------------------------------------

// Texture 測試用
GLuint g_uiFTexID[3]; // 三個物件分別給不同的貼圖
int g_iTexWidth,  g_iTexHeight;

//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();
bool Check(CSolidCube *a, CSolidCube *b);
bool Check(CSolidCube *a, CWall *b);
void quickSort(LinkedList<CSolidCube *> &arr, int left, int right);
void init( void )
{
	
	vec4 vT, vColor;
	// 產生所需之 Model View 與 Projection Matrix

	//g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );
	g_mxProjection = Perspective(60.0, (GLfloat)SCREEN_SIZE/(GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// 讀進地板的貼圖
	g_uiFloorDiffuseMap = png_load_SOIL("texture/planks_spruce.png",&g_iTexWidth, &g_iTexHeight, true);
	g_uiFloorLightMap = png_load_SOIL("texture/lightmap2.png",&g_iTexWidth, &g_iTexHeight, false);

	// 讀進金屬的貼圖與 Cube Map
	g_uiSphereDiffuseMap = png_load_SOIL("texture/metal.png",&g_iTexWidth, &g_iTexHeight, false); 
	g_uiSphereCubeMap = CubeMap_load_SOIL(); 

	g_uiTransparentCubeMap = png_load_SOIL("texture/glass_cyan.png", &g_iTexWidth, &g_iTexHeight, false);
	
	camera = new Camera(vec3(-13,3,-24));
	g_controller = new CController();

	//cube 
	g_pWalls = new CWall(g_WallsParams);
	g_pWalls->SetShader(g_mxModelView, g_mxProjection);

	test = new CSolidCube(vec3(2, 2,2));
	test->SetShader(g_mxModelView, g_mxProjection);
	vT.x = 7.0f; vT.y = 2.0f; vT.z = 2.0f;
	mxT = Translate(vT);
	//mxT._m[0][0] = 2.0f; mxT._m[1][1] = 2.0f; mxT._m[2][2] = 2.0f;
	test->SetTRSMatrix(mxT);
	test->SetColor(vec3(0.4, 0, 0));
	test->SetShadingMode(GOURAUD_SHADING);
	//test->SetLightingDisable();
	test->SetMaterials(vec4(0.35f, 0.35f, 0.35f, 1), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	test->SetKaKdKsShini(0.25f, 0.8f, 0.2f, 2);

	g_pCube = new CSolidCube(vec3(1,1,1));
	g_pCube->SetShader(g_mxModelView, g_mxProjection);

	vT.x = 4.0f; vT.y = 2.0f; vT.z = 0.0f;
	mxT = Translate(vT);
	//mxT._m[0][0] = 2.0f; mxT._m[1][1] = 2.0f; mxT._m[2][2] = 2.0f;
	g_pCube->SetTRSMatrix(mxT);
	g_pCube->SetShadingMode(GOURAUD_SHADING);
	// materials
	g_pCube->SetMaterials(vec4(0.35f, 0.35f, 0.35f, 1), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pCube->SetKaKdKsShini(0.25f, 0.8f, 0.2f, 2);
	g_pCube->LoadPngImageAndSetTextureObject("texture/planks_spruce.png");

	//hex prism
	g_pHexPrism = new CSolidPrism;
	g_pHexPrism->SetShader(g_mxModelView, g_mxProjection);
	vT.x = -15; vT.y = 0.01f; vT.z = 0;
	mxT = Translate(vT);
	g_pHexPrism->SetTRSMatrix(mxT);
	g_pHexPrism->SetShadingMode(GOURAUD_SHADING);

	g_pHexPrism->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pHexPrism->SetKaKdKsShini(0.0f, 0.8f, 0.2f, 2);

	//g_pPlayer
	g_pPlayer = new CSolidCube(vec3(2 ,2, 1.5));
	g_pPlayer->SetShader(g_mxModelView, g_mxProjection);

	/*vT.x = 1.5f; vT.y = 1.5f; vT.z = 1.5f;
	mxT = Translate(vT);
	mxT._m[0][0] = 2.0f; mxT._m[1][1] = 2.0f; mxT._m[2][2] = 2.0f;
	g_pPlayer->SetTRSMatrix(mxT);*/

	

	// 產生物件的實體
	g_pFloor = new CQuadMT;
	g_pFloor->SetShader(g_mxModelView, g_mxProjection);
	g_pFloor->SetTRSMatrix(Scale(52,1,66));
	g_pFloor->SetShadingMode(GOURAUD_SHADING);
	g_pFloor->SetTiling(16,21);
	g_pFloor->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pFloor->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pFloor->SetColor(vec4(0.95f,0.95f,0.95f,1.0f));
	//g_pFloor->SetLightingDisable();
	g_uiShaderNo = g_pFloor->GetShaderHandle();



	// 產生物件的實體
	g_pLightMapFloor = new CQuadMT;
	g_pLightMapFloor->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(13, 0.01f, 22.0f) *Scale(26, 1, 22);
	g_pLightMapFloor->SetTRSMatrix(mxT);
	g_pLightMapFloor->SetShadingMode(GOURAUD_SHADING);
	g_pLightMapFloor->SetTiling(8, 8);
	g_pLightMapFloor->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pLightMapFloor->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pLightMapFloor->SetColor(vec4(0.95f, 0.95f, 0.95f, 1.0f));
	g_pLightMapFloor->SetLightingDisable();
	g_uiShaderNo = g_pLightMapFloor->GetShaderHandle();

	// For Reflecting Sphere
	g_pSphere = new CSolidSphereMT(1.0f, 24, 12);
	g_pSphere->SetCubeMapTexName(1); // 使用 Texture1
	g_pSphere->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(-13, 1.0f, 22.0f) * RotateX(180) *Scale(2, 2, 2);
	g_pSphere->SetTRSMatrix(mxT);
	g_pSphere->SetShadingMode(GOURAUD_SHADING);
	g_pSphere->SetViewPosition(g_vEye);
	// 設定貼圖


	// 設定貼圖
	g_pSphere->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pSphere->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pSphere->SetColor(vec4(0.9f, 0.9f, 0.9f, 1.0f));
	//g_pSphere->SetLightingDisable();

	// 設定 代表 Light 的 WireSphere ，在這個範例中 燈光是沒有作用的
	g_pLight = new CWireSphere(0.25f, 6, 3);
	g_pLight->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(camera->GetPosition());
	g_pLight->SetTRSMatrix(mxT);
	g_pLight->SetColor(g_Light1.diffuse);
	g_pLight->SetLightingDisable();	

	g_HandCubes = new LinkedList<CSolidCube *>;
	g_HandCubes->add(test);

	//match
	g_pMatch[0] = new CQuad;
	g_pMatch[0]->SetShader(g_mxModelView, g_mxProjection);
	mxT = Translate(13, 1.0f, 22.0f) * RotateX(90) *Scale(2, 2, 2);
	g_pMatch[0]->SetTRSMatrix(mxT);
	g_pMatch[0]->SetShadingMode(GOURAUD_SHADING);
	// 設定貼圖
	g_pMatch[0]->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pMatch[0]->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pMatch[0]->SetColor(vec4(0.9f, 0.9f, 0.9f, 1.0f));
	g_uiMatchTexID[0] = png_load_SOIL("texture/torch_on.png", &g_iTexWidth, &g_iTexHeight, false);
	g_pMatch[0]->SetLightingDisable();
}

//----------------------------------------------------------------------------
void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	//地板
	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiFloorDiffuseMap); // 與 Diffuse Map 結合
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_2D, g_uiFloorLightMap); // 與 Light Map 結合
	g_pLightMapFloor->Draw();

	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiFloorDiffuseMap); // 與 Diffuse Map 結合
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_2D, g_uiFloorLightMap); // 與 Light Map 結合
	g_pFloor->Draw();
	
	//牆壁
	g_pWalls->Draw();

	//六角柱
	g_pHexPrism->Draw();

	//可被移動方塊
	if (!g_bHaveCube)
		g_pCube->Draw();

	//玩家Billboard方塊
	//g_pPlayer->Draw(); //hide it!

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);

	//Cube Map球體
	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiSphereDiffuseMap); // 與 Diffuse Map 結合
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiSphereCubeMap); // 與 Light Map 結合
	g_pSphere->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_uiMatchTexID[0]);
	g_pMatch[0]->Draw();
	//TODO 透明方塊

	glActiveTexture(GL_TEXTURE0); // select active texture 0
	glBindTexture(GL_TEXTURE_2D, g_uiTransparentCubeMap);
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiSphereCubeMap); // 與 Light Map 結合
	
	
	
	glDepthMask(GL_FALSE);
	//draw match
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);



	g_HandCubes->get(0)->Draw();
	for (int i = 1; i < g_HandCubes->size();i++)
		g_HandCubes->get(i)->DrawW();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	//g_pLight->Draw();
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
	g_Light1.position.x = camera->GetPosition().x;
	g_Light1.position.y = camera->GetPosition().y;
	g_Light1.position.z = camera->GetPosition().z;
	mxT = Translate(camera->GetPosition());
	g_pLight->SetTRSMatrix(mxT);
}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{

	if( true ) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}
	// 如果需要重新計算時，在這邊計算每一個物件的顏色
	g_pSphere->Update(delta, g_Light1);
	g_pFloor->Update(delta, g_Light1);
	g_pLightMapFloor->Update(delta, g_Light1);
	///g_pLightMapFloor->
	g_pLight->Update(delta,vec4(camera->GetDirection(),1.0f));
	//---walls
	g_pHexPrism->Update(delta, g_Light1);
	g_pWalls->Update(delta, g_Light1);
	if (g_bHaveCube)
		g_pCube->SetTRSMatrix(Translate(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z));
	g_pCube->Update(delta, g_Light1);
	//test->Update(delta, g_Light1);
	for (int i = 0; i < g_HandCubes->size(); i++)
		g_HandCubes->get(i)->Update(delta, g_Light1);
	//--
	g_pMatch[0]->Update(delta, g_Light1);
	g_pPlayer->Update(delta, g_Light1);
	glutWarpPointer(400, 400);	

	g_mxModelView = camera->GetModelView();
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLightMapFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);
	g_pHexPrism->SetModelViewMatrix(g_mxModelView);
	//TODO
	
	camera->SetTestPosition(camera->GetPosition());
	if (g_controller->GetKeyState("w")) // w key
		camera->TestForward(fraction);
	if (g_controller->GetKeyState("a")) // a key
		camera->TestLeft(g_fPhi, 0.01f);
	if (g_controller->GetKeyState("s"))// s key
		camera->TestBackward(fraction);
	if (g_controller->GetKeyState("d")) // d key
		camera->TestRight(g_fPhi, 0.01f);

	float distance = dot(vec3(0, 1, 0), camera->GetTestPosition());
	//TODO
	if (distance < 3);
		//printf("ras");
	g_pPlayer->SetTRSMatrix(Translate(camera->GetTestPosition().x, camera->GetTestPosition().y, camera->GetTestPosition().z)*RotateY(g_fPhi/DegreesToRadians));
	g_pMatch[0]->SetTRSMatrix(Translate(g_pMatch[0]->GetPosition()) * RotateY(g_fPhi / DegreesToRadians) * RotateX(90)*Scale(2, 2, 2));

	//取得目前相機位置
	//vec3 camerapos = camera->GetPosition();

	//移動物體到相機位置
	//camerapos -= g_pCube->GetPosition(); //mode the object only need one time

	//判斷是否碰撞
	//float distance_fraction = 8; //for cube collide
	
	vec3 tmp_dir = normalize(camera->GetDirection());
	for (int i = 0; i < g_HandCubes->size(); i++){
		vec3 tmp_cube_pos = camera->GetPosition()- g_HandCubes->get(i)->GetPosition() ;
		//vec3 tmp_cube_pos = vec3(camera->GetPosition().x, 0, camera->GetPosition().z) - vec3(g_HandCubes->get(i)->GetPosition().x, 0, g_HandCubes->get(i)->GetPosition().z);
		float distance = dot(-tmp_dir, tmp_cube_pos);
		g_HandCubes->get(i)->SetDistance(distance);
		//printf("%d  = %f\n ", i ,distance);
	}
	quickSort(*g_HandCubes, 0, g_HandCubes->size() - 1);

	if (Check(g_pPlayer, g_pWalls)){
		
		//printf("yes\n");
	} else {
		//printf("no\n");
		
		if (g_controller->GetKeyState("w")) // w key
			camera->MoveForward(fraction);
		if (g_controller->GetKeyState("a")) // a key
			camera->MoveLeft(g_fPhi, 0.01f);
		if (g_controller->GetKeyState("s"))// s key
			camera->MoveBackward(fraction);
		if (g_controller->GetKeyState("d")) // d key
			camera->MoveRight(g_fPhi, 0.01f);
		if (g_controller->GetKeyState(" ")) // space key
			camera->MoveJump(0.1);
		
	}

	
	camera->SetTestPosition(camera->GetPosition());
	
	//cout << camera->GetPosition() << endl;
	GL_Display();
}

//----------------------------------------------------------------------------
void KeyboardUp(unsigned char key, int x, int y)
{
	g_controller->onKeyUp(key);
	
}


void Win_Keyboard( unsigned char key, int x, int y )
{
	g_controller->onKeyDown(key);
	
    switch ( key ) {
	case 65: // A key
		g_bAutoRotating = !g_bAutoRotating;
		break;
	case 101: // e key
		
		if (Check(g_pPlayer, g_pCube)){
			g_bHaveCube = !g_bHaveCube;
			if (g_bHaveCube)
				g_pCube->SetTRSMatrix(Translate(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z));
			else{
				vec3 tmp = camera->GetPosition();
				vec3 tmp_dir = camera->GetDirection()*0.005f;
				//cout << "init" << tmp << endl;
				while (true){
					
					if (tmp.y < 0)
						break;
					tmp += tmp_dir;
				}
				//cout << "final" << tmp << endl;
				g_pCube->SetTRSMatrix(Translate(tmp.x, 0.5f, tmp.z));
			}
		}
		break;
	//walking
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
		delete g_pHexPrism;
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
			if (state == GLUT_DOWN){
				vec3 tmp = camera->GetPosition();
				CSolidCube *tmp_cube = new CSolidCube(vec3(2, 2, 2));
				tmp_cube->SetShader(g_mxModelView, g_mxProjection);
				//vT.x = 7.0f; vT.y = 2.0f; vT.z = 2.0f;
				tmp_cube->SetColor(vec3(0.4, 0, 0));
				tmp_cube->SetShadingMode(GOURAUD_SHADING);
				//test->SetLightingDisable();
				tmp_cube->SetMaterials(vec4(0.35f, 0.35f, 0.35f, 1), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
				tmp_cube->SetKaKdKsShini(0.25f, 0.8f, 0.2f, 2);

				vec3 tmp_dir = camera->GetDirection()*0.004f;
				//cout << "init" << tmp << endl;
				while (true){
					if (tmp.y < 0)
						break;
					tmp += tmp_dir;
				}
				//cout << "final" << tmp << endl;
				tmp_cube->SetTRSMatrix(Translate(tmp.x, 1.01f, tmp.z));
				g_HandCubes->add(tmp_cube);
			}
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {
	//vec3 n = normalize(cross(vec4(-1, 0, 0, 1), vec4(-1, 0, -1, 1)));
	//float distance = dot(vec3(0,1,0), camera->GetPosition);
	/*for (int i = 0; i < 36;i++)
	cout << i <<" = "<<g_pCube->GetFaceNormal()[i] << endl;*/

	switch(key) {
		case GLUT_KEY_LEFT:
			camera->MoveLeft(g_fPhi, 0.1);
            break;
        case GLUT_KEY_RIGHT:
			camera->MoveRight(g_fPhi, 0.1);
            break;
        case GLUT_KEY_UP:
			camera->MoveForward(fraction);
            break;
        case GLUT_KEY_DOWN:
			camera->MoveBackward(fraction);
            break;
		default:
			break;
	}
	
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	
	g_dx += x-400;
	g_dy -= y - 400;

	if (g_dy > 800 && g_dy <1600)
		g_fTheta = (float)M_PI*(float)g_dy / SCREEN_SIZE;
	else if (g_dy < 800)
		g_dy = 800;
	else if (g_dy > 1600)
		g_dy = 1600;


	//printf("%d\n", g_dy);
	g_fPhi = (float)-M_PI*(g_dx - HALF_SIZE)/(HALF_SIZE); // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	
	camera->SetDirection(
		g_fRadius*sin(g_fTheta)*sin(g_fPhi), 
		g_fRadius*cos(g_fTheta), 
		g_fRadius*sin(g_fTheta)*cos(g_fPhi)
	);

	g_mxModelView = camera->GetModelView();

	// Change ModelView Matrix
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLightMapFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);
	g_pWalls->SetModelViewMatrix(g_mxModelView);
	g_pCube->SetModelViewMatrix(g_mxModelView);

	for (int i = 0; i < g_HandCubes->size(); i++)
		g_HandCubes->get(i)->SetModelViewMatrix(g_mxModelView);
	//test->SetModelViewMatrix(g_mxModelView);
	g_pHexPrism->SetModelViewMatrix(g_mxModelView);
	g_pPlayer->SetModelViewMatrix(g_mxModelView);
	g_pMatch[0]->SetModelViewMatrix(g_mxModelView);
	// For Cube Map
	g_pSphere->SetViewPosition(vec4(camera->GetPosition(),1.0f));
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;

	
	g_vEye.x = g_fRadius*sin(g_fTheta)*sin(g_fPhi);
	g_vEye.y = g_fRadius*cos(g_fTheta);
	g_vEye.z = g_fRadius*sin(g_fTheta)*cos(g_fPhi);

	g_mxModelView = camera->GetModelView();
	// Change ModelView Matrix
	g_pWalls->SetModelViewMatrix(g_mxModelView);
	g_pCube->SetModelViewMatrix(g_mxModelView);

	for (int i = 1; i < g_HandCubes->size(); i++)
		g_HandCubes->get(i)->SetModelViewMatrix(g_mxModelView);

	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLightMapFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	g_pSphere->SetModelViewMatrix(g_mxModelView);
	g_pHexPrism->SetModelViewMatrix(g_mxModelView);
	// For Cube Map
	g_pSphere->SetViewPosition(vec4(camera->GetPosition(), 1.0f));
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	//  產生 projection 矩陣，此處為產生正投影矩陣
	g_mxProjection = Perspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);

	// 設定棋盤的 Projection Matrix
	g_pFloor->SetProjectionMatrix(g_mxProjection);
	g_pLightMapFloor->SetProjectionMatrix(g_mxProjection);
	g_pLight->SetProjectionMatrix(g_mxProjection); 
	g_pSphere->SetProjectionMatrix(g_mxProjection);
	g_pWalls->SetProjectionMatrix(g_mxProjection);
	g_pMatch[0]->SetModelViewMatrix(g_mxModelView);
	g_pHexPrism->SetProjectionMatrix(g_mxProjection);
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

    glutCreateWindow("Final Project");

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
	glutIgnoreKeyRepeat(true);
	glutKeyboardUpFunc(KeyboardUp);
	glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}

bool Check(CSolidCube *a, CSolidCube *b)
{
	vec3 a_Max = a->GetMaxBound();
	vec3 a_Min = a->GetMinBound();
	vec3 b_Max = b->GetMaxBound();
	vec3 b_Min = b->GetMinBound();
	//cout << "Amax= " << a_Max << " Amin= " << a_Min << endl;
	//cout << "Bmin= " << b_Min << " Bmax= " << b_Max << endl;
	if (a_Max.x > b_Min.x && 
		a_Max.y > b_Min.y && 
		a_Max.z > b_Min.z &&
		a_Min.x < b_Max.x &&
		a_Min.y < b_Max.y &&
		a_Min.z < b_Max.z)
		return true;
	else
		return false;
}

bool Check(CSolidCube *a, CWall *b)
{
	vec3 a_Max = a->GetMaxBound();
	vec3 a_Min = a->GetMinBound();
	vec3 b_Max;
	vec3 b_Min;
	for (int i = 0; i < WALL_NUM; i++){
		b_Max = b->GetMaxBound(i);
		b_Min = b->GetMinBound(i);
		//cout << "Amax= " << a_Max << " Amin= " << a_Min << endl;
		//cout << "Bmin= " << b_Min << " Bmax= " << b_Max << endl;
		if (a_Max.x > b_Min.x &&
			a_Max.y > b_Min.y &&
			a_Max.z > b_Min.z &&
			a_Min.x < b_Max.x &&
			a_Min.y < b_Max.y &&
			a_Min.z < b_Max.z)
			return true;
	}
	return false;
}


void quickSort(LinkedList<CSolidCube *> &arr, int left, int right) {
	int i = left, j = right;
	CSolidCube *tmp;
	float pivot = arr.get((left  + right) / 2)->GetDistance();

	if (arr.size() == 1) return;

	/* partition */
	while (i <= j) {
		while (arr.get(i)->GetDistance() > pivot)
			i++;
		while (arr.get(j)->GetDistance() < pivot)
			j--;
		if (i <= j) {
			tmp = arr.get(i);
			arr.set(i, arr.get(j));
			arr.set(j, tmp);
			//arr.get(i)->SetDistance(arr.get(j)->GetDistance());
			//arr.get(j)->SetDistance(tmp);
			i++;
			j--;
		}
	};
	/* recursion */
	if (left < j)
		quickSort(arr, left, j);
	if (i < right)
		quickSort(arr, i, right);
}