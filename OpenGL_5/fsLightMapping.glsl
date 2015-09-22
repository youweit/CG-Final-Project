// #define CHANGE_COLOR
#version 120
varying vec3 fN;
varying vec3 fL;
varying vec3 fE;

varying vec2 DfMapUV; // ��J�K�Ϯy��
varying vec2 LgMapUV; // ��J�K�Ϯy��

// �H�U���s�W������
uniform vec4  LightInView;        // Light's position in View Space
uniform vec4  AmbientProduct;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct; // light's specular �P Object's specular �P ks �����n
uniform float fShininess;
uniform int   iLighting;
uniform vec4  vObjectColor;    // �N���󪺳�@�C��

// For Texture Sampler
uniform sampler2D   diffuMap; // �K�Ϫ��ѼƳ]�w
uniform sampler2D   lightMap; // �K�Ϫ��ѼƳ]�w
uniform samplerCube cubeMap; // �K�Ϫ��ѼƳ]�w

void main()
{
	// ���ŧi diffuse �P specular
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	vec4 LightingColor = vec4(0.0,0.0,0.0,1.0);
	
	if( iLighting != 1 ) {
#ifndef CHANGE_COLOR
		// �����ϥζǤJ�� LightMap
		gl_FragColor = vObjectColor * texture2D(diffuMap, DfMapUV) * texture2D(lightMap, LgMapUV);
#else
		vec4 lightMap = texture2D(lightMap, LgMapUV);	
		lightMap = lightMap * vec4(0.79,0.95, 0.96, 1.0);
		gl_FragColor = vObjectColor * texture2D(diffuMap, DfMapUV) * lightMap;
#endif
		gl_FragColor.w = 1.0f;
	}
	else {	
		// 1. �p�� Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
		vec4 ambient = AmbientProduct; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;

		// ���ƶǤJ�� Normal Dir
		vec3 vN = normalize(fN); 
		float dist = length(fL);
		// 2. ���ƶǤJ�� Light Dir
		vec3 vL = normalize(fL); // normalize light vector

		// 5. �p�� L dot N
		float fLdotN = vL.x*vN.x + vL.y*vN.y + vL.z*vN.z;
		if( fLdotN >= 0 ) { 

			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse = fLdotN * DiffuseProduct; 
			
			float intensity = dot(vec3(0,0,-1),-vL);
			float dif = 0.08; //1.0 - 0.92
			float factor = clamp((intensity- 0.92)/dif,0.0,1.0);
			if ( intensity > 0.92) {
				LightingColor = diffuse*factor/dist*10;
			}else{
				//ambient = vec4(0.0,0.0,0.0,1.0);
				diffuse = vec4(0.0,0.0,0.0,1.0);
				specular = vec4(0.0,0.0,0.0,1.0);
				LightingColor = ambient + diffuse + specular;
			}
			LightingColor += vec4(0.02,0.02,0.02,0);
		}

		LightingColor.w = 1.0;	// �]�w alpha �� 1.0
//		gl_FragColor = LightingColor;
		gl_FragColor = LightingColor * texture2D(diffuMap, DfMapUV);

		// gl_FragColor = vec4((ambient + diffuse + specular).xyz, 1.0);

	}
}
