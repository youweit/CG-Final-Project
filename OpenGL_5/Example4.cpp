//
//  Multi-Texturing - Light Mapping
//
// ����e���ǳƤu�@
// �}�� CShape.h ���� #define LIGHTING_WITHCPU 
// ���� CShape.h ���� #define LIGHTING_WITHGPU 
// ���� CShape.h ���� #define PERVERTEX_LIGHTING 
//

#include "header/Angel.h"
#include "Common/CQuadMT.h"
#include "Common/CQuad.h"
#include "Common/CWireSphere.h"
#include "Common/CWireCube.h"
#include "png_loader.h"


#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number

// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;

// For Objects
CQuadMT *g_pFloor;
GLuint   g_uiDifTexName; 
GLuint   g_uiLghTexName; 
GLuint   g_uiShaderNo;

// For Translucent Object
CQuad  *g_pAimal[1];
GLuint g_uiAimalTexID[3];

// For View Point
GLfloat g_fRadius = 8.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;

point4  g_vEye( g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi),  1.0 );
point4  g_vAt( 0.0, 0.0, 0.0, 1.0 );
vec4    g_vUp( 0.0, 1.0, 0.0, 0.0 );
mat4 mxT;
//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 1;
float g_fLightRadius = 40;
float g_fLightTheta = 0;

float g_fLightR = 1.05f;
float g_fLightG = 1.05f;
float g_fLightB = 1.05f;

structLightSource g_Light1 = {
    color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
    point4(0.0f, 20.0f, 40.0f, 1.0f),   // position
    point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
    vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
	1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
	45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot ���ө��d��
	1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), �ө���V�P�Q�ө��I���������ר� cos ��, cut off ����
	1	,	// constantAttenuation	(a + bd + cd^2)^-1 ���� a, d ��������Q�ө��I���Z��
	0	,	// linearAttenuation	    (a + bd + cd^2)^-1 ���� b
	0		// quadraticAttenuation (a + bd + cd^2)^-1 ���� c
};
CWireSphere *g_pLight;
//----------------------------------------------------------------------------

// Texture ���ե�
GLuint g_uiFTexID[3]; // �T�Ӫ�����O�����P���K��
int g_iTexWidth,  g_iTexHeight;

//----------------------------------------------------------------------------
// �禡���쫬�ŧi
extern void IdleProcess();

void init( void )
{
	
	vec4 vT, vColor;

	// Ū�i�K��
	g_uiDifTexName = png_load_SOIL("texture/Masonry.Brick.png",&g_iTexWidth, &g_iTexHeight, false);
	g_uiLghTexName = png_load_SOIL("texture/lightmap1.png",&g_iTexWidth, &g_iTexHeight, false);

	// ���ͩһݤ� Model View �P Projection Matrix

	g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );
	g_mxProjection = Perspective(60.0, (GLfloat)SCREEN_SIZE/(GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// ���ͪ��󪺹���
	g_pFloor = new CQuadMT;
	g_pFloor->SetShader(g_mxModelView, g_mxProjection);
	g_pFloor->SetTRSMatrix(Scale(8,1,8));
	g_pFloor->SetShadingMode(GOURAUD_SHADING);

	g_pFloor->SetTiling(1,1);
	g_pFloor->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pFloor->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pFloor->SetColor(vec4(0.95f,0.95f,0.95f,1.0f));
	g_pFloor->SetLightingDisable();

	g_uiShaderNo = g_pFloor->GetShaderHandle();

	// For g_pAimal1
	g_pAimal[0] = new CQuad;
	g_pAimal[0]->SetShader(g_mxModelView, g_mxProjection);
	
	
	mxT = Translate(5, 2.0f, 3.0f) * RotateX(90) *Scale(2,2,2);

	g_pAimal[0]->SetTRSMatrix(mxT);
	g_pAimal[0]->SetShadingMode(GOURAUD_SHADING);
	// �]�w�K��
	g_pAimal[0]->SetMaterials(vec4(0), vec4(0.85f, 0.85f, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pAimal[0]->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_pAimal[0]->SetColor(vec4(0.9f, 0.9f, 0.9f, 1.0f));


	g_uiAimalTexID[0] = png_load_SOIL("texture/wi_tata.png",&g_iTexWidth, &g_iTexHeight, false);
	g_pAimal[0]->SetLightingDisable();

	// �]�w �N�� Light �� WireSphere �A�b�o�ӽd�Ҥ� �O���O�S���@�Ϊ�
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
	glBindTexture(GL_TEXTURE_2D, g_uiDifTexName); // �P Diffuse Map ���X
	glActiveTexture(GL_TEXTURE1); // select active texture 1
	glBindTexture(GL_TEXTURE_2D, g_uiLghTexName); // �P Light Map ���X

	g_pFloor->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_BLEND);  // �]�w2D Texure Mapping ���@��
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

		glBindTexture(GL_TEXTURE_2D, g_uiAimalTexID[0]); 
		g_pAimal[0]->Draw();
		glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);	// ���� Blending
	glDepthMask(GL_TRUE);	// �}�ҹ� Z-Buffer ���g�J�ާ@

	g_pLight->Draw();
	glutSwapBuffers();	// �洫 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// �C��¶ Y �b�� 90 ��
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

	if( g_bAutoRotating ) { // Part 2 : ���s�p�� Light ����m
		UpdateLightPosition(delta);
	}
	// �p�G�ݭn���s�p��ɡA�b�o��p��C�@�Ӫ����C��
	g_pFloor->Update(delta, g_Light1);

	g_pAimal[0]->Update(delta, g_Light1);

	g_pLight->Update(delta);

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
		delete g_pAimal[0];
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:   // �ثe���U���O�ƹ�����
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_MIDDLE_BUTTON:  // �ثe���U���O�ƹ����� �A���� Y �b
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // �ثe���U���O�ƹ��k��
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT:		// �ثe���U���O�V����V��

			break;
		case GLUT_KEY_RIGHT:	// �ثe���U���O�V�k��V��

			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {

	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE); // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;
	g_vEye.x = g_fRadius*sin(g_fTheta)*sin(g_fPhi);
	g_vEye.y = g_fRadius*cos(g_fTheta);
	g_vEye.z = g_fRadius*sin(g_fTheta)*cos(g_fPhi);


	mxT = Translate(0, 1.0f, 0.0f)  * RotateY((M_PI+g_fPhi)/DegreesToRadians) *RotateX(90) *Scale(2,2,2);
	printf("%f\n",g_fPhi/DegreesToRadians);

	g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );

	// Change ModelView Matrix
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);
	//TODO 
	vec4 tmp = vec4(0,0,-1,0.0);
	float fuck = dot(tmp,g_vEye-vec4(0,1,0,0));

	g_pAimal[0]->SetTRSMatrix(mxT);
	g_pAimal[0]->SetModelViewMatrix(g_mxModelView);

}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;;
	g_vEye.x = g_fRadius*sin(g_fTheta)*sin(g_fPhi);
	g_vEye.y = g_fRadius*cos(g_fTheta);
	g_vEye.z = g_fRadius*sin(g_fTheta)*cos(g_fPhi);

	g_mxModelView = LookAt( g_vEye, g_vAt, g_vUp );

	// Change ModelView Matrix
	g_pFloor->SetModelViewMatrix(g_mxModelView);
	g_pLight->SetModelViewMatrix(g_mxModelView);

	g_pAimal[0]->SetModelViewMatrix(g_mxModelView);
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	//  ���� projection �x�}�A���B�����ͥ���v�x�}
	g_mxProjection = Perspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);

	// �]�w�ѽL�� Projection Matrix
	g_pFloor->SetProjectionMatrix(g_mxProjection);
	g_pLight->SetProjectionMatrix(g_mxProjection); 
	
	g_pAimal[0]->SetProjectionMatrix(g_mxProjection); 
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
    glutKeyboardFunc( Win_Keyboard );	// �B�z ASCI ����p A�Ba�BESC ��...����
	glutSpecialFunc( Win_SpecialKeyboard);	// �B�z NON-ASCI ����p F1�BHome�B��V��...����
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}