#version 330 core

in FRAG {
	vec2 uv;
	vec3 tangentPos;
	vec3 tangentViewPos;
	vec3 tangentLightDir;
	vec4 lightSpacePos;

} frag;
out vec4 frgColor;


uniform sampler2D surfaceTex;
uniform sampler2D normTex;
uniform sampler2D depthTex;

const vec3 lightColor = vec3(1.0f);

float computeShadow(float bias)
{
	 // ִ��͸�ӳ���
    vec3 projCoords = frag.lightSpacePos.xyz / frag.lightSpacePos.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
		return 0.0;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(depthTex, projCoords.xy).r; 
    // ȡ�õ�ǰƬԪ�ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
	// PCF��Ӱ
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthTex, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthTex, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}


void main()
{	
	vec4 objectColor = texture(surfaceTex, frag.uv);
	vec3 norm = texture(normTex, frag.uv).rgb;

	// ������
	norm = norm * 2 - 1.0;
	norm = normalize(norm);

	// ������
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// ������
	vec3 lightDir = normalize(frag.tangentLightDir);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// ���淴��
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(frag.tangentViewPos - frag.tangentPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfWayDir), 0.0), 4);
	vec3 specular = spec * lightColor;

	// �ܹ�
	float shadow = computeShadow(max(0.05 * (1.0 - dot(norm, lightDir)), 0.005));
	vec3 total = ambient + (1 - shadow) * (diffuse + specular);
	vec4 result = vec4(total, 1.0);
	result *= objectColor;
//	frgColor = vec4(1.f, 0, 0, 1.f);
	frgColor = result;

}