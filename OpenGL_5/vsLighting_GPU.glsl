// Phong reflection model
// #define BLINN_PHONG
#version 120
attribute  vec4  vPosition;	// Vertex Position
attribute  vec3  vNormal;    // Vertex Normal
attribute  vec4  vVtxColor;  // Vertex Color
varying vec4  vColor;     // ��X���C��

uniform mat4  ModelView;   // Model View Matrix
uniform mat4  Projection;  // Projection Matrix
uniform vec4  vObjectColor;    // �N���󪺳�@�C��

// �W�������P  vsLighting_CPU.glsl �ۦP
// �H�U���s�W������
uniform vec4  LightInView;        // Light's position in View Space
uniform vec4  AmbientProduct;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct; // light's specular �P Object's specular �P ks �����n
uniform float fShininess;
uniform int   iLighting;

void main()
{
	// ���ŧi diffuse �P specular
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);

	if( iLighting != 1 ) {
		gl_Position = Projection * ModelView * vPosition;	// �p�� vPosition ��v�᪺��m
		vColor = vObjectColor;
	}
	else {	
		// 1. �N�I��������Y�y�Шt�ΡA�����H m_mxMVFinal �p��b�@�ɮy�Ъ������m
		vec4 vPosInView = ModelView * vPosition;  

		// 2. �N���� Normal ������Y�y�Шt�ΡA���ন���V�q
		// �̫�@�ӸɤW 0 �A���ݭn�첾�q�A�_�h�X�Ӫ� Normal �|����
		// �P�ɧQ�� normalize �ন���V�q

// �H�U���Ω�p��磌��i�� non-uniform scale �ɡA���� normal ���ץ��p��
//		mat3 ITModelView = transpose(inverse(mat3(ModelView)); 
//		vec3 vN = normalize(ITModelView * vNormal); 

		vec3 vN = normalize(ModelView * vec4(vNormal, 0.0)).xyz; 

		// 3. �p�� Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
		vec4 ambient = AmbientProduct; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;

		// 4. �ǤJ�� Light ����m�w�g�b���Y�y��
		vec3 vL = normalize(vec3(LightInView.xyz - vPosInView.xyz)); // normalize light vector

		// 5. �p�� L dot N
		float fLdotN = vL.x*vN.x + vL.y*vN.y + vL.z*vN.z;
		if( fLdotN >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse = fLdotN * DiffuseProduct; 

			// Specular color
			// Method 1: Phone Model
			// �p�� View Vector
			// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�I��m���t�V�N�O View Dir
			vec3 vV = -normalize(vPosInView.xyz);

#ifndef BLINN_PHONG
			//�p�� Light �� �Ϯg�� vRefL = 2 * fLdotN * vN - L
			// �P�ɧQ�� normalize �ন���V�q
			vec3 vRefL = normalize(2.0f * (fLdotN) * vN - vL);

			//   �p��  vReflectedL dot View
			float RdotV = vRefL.x*vV.x + vRefL.y*vV.y + vRefL.z*vV.z;

			// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
			if( RdotV > 0 ) specular = SpecularProduct * pow(RdotV, fShininess); 
#else

			// Blinn-Phong Reflection Model
			vec3 vH = normalize( vL + vV ); 
			float HdotN = vH.x*vN.x + vH.y*vN.y + vH.z*vN.z; //   �p��  H dot N
			if( HdotN > 0 ) specular = SpecularProduct * pow(HdotN, fShininess); 
#endif
		}

		gl_Position = Projection * vPosInView;	// �p�� vPosition ��v�᪺��m
		vColor = ambient + diffuse + specular;  // �p���C�� ambient + diffuse + specular;
		vColor.w = 1.0;	// �]�w alpha �� 1.0
		// vColor = vec4((ambient + diffuse + specular).xyz, 1.0);
	}
}
