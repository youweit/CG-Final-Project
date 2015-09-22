// Phong reflection model
#version 120
attribute vec4 vPosition;	  // Vertex Position
attribute vec3 vNormal;    // Vertex Normal
attribute vec4 vVtxColor;  // Vertex Color
attribute vec2 vDiffuseMapCoord;
attribute vec2 vLightMapCoord;

varying vec3 fN; // ��X Normal �b���Y�y�ФU����V
varying vec3 fL; // ��X Light Direction �b���Y�y�ФU����V
varying vec3 fE; // ��X View Direction �b���Y�y�ФU����V
varying vec2 DfMapUV; // ��X Diffuse Map �K�Ϯy��
varying vec2 LgMapUV; // ��X Light Map �K�Ϯy��

uniform mat4  ModelView;   // Model View Matrix
uniform mat4  Projection;  // Projection Matrix
uniform vec4  LightInView; // Light's position in View Space

void main()
{
	vec4 vPosInView = ModelView * vPosition;
	// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�t��[��m]�N�O View Dir
	fE = -vPosInView.xyz;

	// �H�U���Ω�p��磌��i�� non-uniform scale �ɡA���� normal ���ץ��p��
	//		mat3 ITModelView = transpose(inverse(mat3(ModelView)); 
	//		vec3 vN = normalize(ITModelView * vNormal); 
	fN = (ModelView * vec4(vNormal, 0.0)).xyz;

	fL = vec3(LightInView.xyz - vPosInView.xyz);
	gl_Position = Projection * vPosInView;
	DfMapUV = vDiffuseMapCoord;
	LgMapUV = vLightMapCoord;

}
