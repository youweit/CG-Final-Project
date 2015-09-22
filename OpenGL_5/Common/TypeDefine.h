#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H
#include "../Header/Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

typedef struct MaterialParameters {
	color4 ambient;		// ambient �C��
	color4 diffuse;		
	color4 specular;
	color4 emission;
	float  ka, kd, ks;	// ²�ƭp��, ���] RGB �T���C��W���Ϯg�Y�ƬҬۦP
	float  shininess;
} structMaterial;

typedef struct LightSourceParameters  
{  
    color4 ambient;  
    color4 diffuse;  
    color4 specular;  
    point4 position;  
    point4 halfVector;  
    vec3 spotDirection; 
    float spotExponent;  
    float spotCutoff; // (range: [0.0,90.0], 180.0)  
    float spotCosCutoff; // (range: [1.0,0.0],-1.0)  
    float constantAttenuation;  
    float linearAttenuation;  
    float quadraticAttenuation;  
} structLightSource;


#endif