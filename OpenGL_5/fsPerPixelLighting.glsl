// #define NPR
// #define SILHOUETTE
#version 120
varying vec3 fN;
varying vec3 fL;
varying vec3 fE;

varying vec2 DfMapUV; // ��J�K�Ϯy��

// �H�U���s�W������
uniform vec4  LightInView;        // Light's position in View Space
uniform vec4  AmbientProduct;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct; // light's specular �P Object's specular �P ks �����n
uniform float fShininess;
uniform int   iLighting;
uniform vec4  vObjectColor;    // �N���󪺳�@�C��
uniform vec3 SpotDir;

// For Texture Sampler
uniform sampler2D diffuMap; // �K�Ϫ��ѼƳ]�w

void main()
{
	// ���ŧi diffuse �P specular
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	vec4 LightingColor = vec4(0.0,0.0,0.0,1.0);
	
	if( iLighting != 1 ) {
		gl_FragColor = vObjectColor * texture2D(diffuMap, DfMapUV); 
	}
	else {	
		// 1. �p�� Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
		vec4 ambient = AmbientProduct; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;

		// ���ƶǤJ�� Normal Dir
		vec3 vN = normalize(fN); 

		// 2. ���ƶǤJ�� Light Dir
		float dist = length(fL);
		vec3 vL = normalize(fL); // normalize light vector

		// 5. �p�� L dot N
		float fLdotN = vL.x*vN.x + vL.y*vN.y + vL.z*vN.z;
		if( fLdotN >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse = fLdotN * DiffuseProduct; 

			float intensity = dot(vec3(0,0,-1),-vL);
			float dif = 0.08; //1.0 - 0.92
			float factor = clamp((intensity- 0.88)/dif ,0.0,1.0);
			if ( intensity > 0.88) {
				LightingColor = diffuse*factor/dist* 100;
			}else{
				LightingColor = vec4(0,0,0,0);
			}
			LightingColor += vec4(0.02,0.02,0.02,0);
		}

		  // �p���C�� ambient + diffuse + specular;
		LightingColor.w = 1.0;	// �]�w alpha �� 1.0
//		gl_FragColor = LightingColor;
		gl_FragColor = LightingColor * texture2D(diffuMap, DfMapUV);

		// gl_FragColor = vec4((ambient + diffuse + specular).xyz, 1.0);
#ifdef SILHOUETTE
	vec4 edgeColor = vec4(1.0, 0.0, 0.0, 1.0);
	if( abs(dot(normalize(fN), normalize(fE))) < 0.075 )  gl_FragColor = edgeColor;
#endif

	}
}
