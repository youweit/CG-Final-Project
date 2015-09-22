// #define CHANGE_COLOR
#version 120
varying vec3 fN;
varying vec3 fL;
varying vec3 fE;

varying vec2 DfMapUV; // 輸入貼圖座標
varying vec2 LgMapUV; // 輸入貼圖座標
varying vec3 ReflView;   // View 在該 Vertex 相對應於 Normal 的反射向量 

// 以下為新增的部分
uniform vec4  LightInView;        // Light's position in View Space
uniform vec4  AmbientProduct;  // light's ambient  與 Object's ambient  與 ka 的乘積
uniform vec4  DiffuseProduct;  // light's diffuse  與 Object's diffuse  與 kd 的乘積
uniform vec4  SpecularProduct; // light's specular 與 Object's specular 與 ks 的乘積
uniform float fShininess;
uniform int   iLighting;
uniform vec4  vObjectColor;    // 代表物件的單一顏色

// For Texture Sampler
uniform sampler2D   diffuMap; // 貼圖的參數設定
uniform sampler2D   lightMap; // 貼圖的參數設定
uniform samplerCube cubeMap; // 貼圖的參數設定

void main()
{
	// 先宣告 diffuse 與 specular
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	vec4 LightingColor = vec4(0.0,0.0,0.0,1.0);
	
	if( iLighting != 1 ) {
#ifndef CHANGE_COLOR
		// 直接使用傳入的 LightMap
		gl_FragColor = vObjectColor * (0.325 *texture2D(diffuMap, DfMapUV) + 0.675 * textureCube(cubeMap, ReflView));
#else
		vec4 lightMap = texture2D(lightMap, LgMapUV);	
		lightMap = lightMap * vec4(0.79,0.95, 0.96, 1.0);
		gl_FragColor = vObjectColor * texture2D(diffuMap, DfMapUV) * lightMap;
#endif
		gl_FragColor.w = 1.0f;
	}
	else {	
		// 1. 計算 Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
		vec4 ambient = AmbientProduct; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;

		// 單位化傳入的 Normal Dir
		vec3 vN = normalize(fN); 
		float dist = length(fL);
		// 2. 單位化傳入的 Light Dir
		vec3 vL = normalize(fL); // normalize light vector

		// 5. 計算 L dot N
		float fLdotN = vL.x*vN.x + vL.y*vN.y + vL.z*vN.z;
		if( fLdotN >= 0 ) { // 該點被光源照到才需要計算
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse = fLdotN * DiffuseProduct; 

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

		//LightingColor = ambient + diffuse + specular;  // 計算顏色 ambient + diffuse + specular;
		LightingColor.w = 1.0;	// 設定 alpha 為 1.0

		gl_FragColor = LightingColor * (0.325 *texture2D(diffuMap, DfMapUV) + 0.675 * textureCube(cubeMap, ReflView));
	}
}
