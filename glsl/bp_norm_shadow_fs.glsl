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
	 // 执行透视除法
    vec3 projCoords = frag.lightSpacePos.xyz / frag.lightSpacePos.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
		return 0.0;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(depthTex, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
	// PCF阴影
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

	// 处理法线
	norm = norm * 2 - 1.0;
	norm = normalize(norm);

	// 环境光
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// 漫反射
	vec3 lightDir = normalize(frag.tangentLightDir);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// 镜面反射
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(frag.tangentViewPos - frag.tangentPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfWayDir), 0.0), 4);
	vec3 specular = spec * lightColor;

	// 总共
	float shadow = computeShadow(max(0.05 * (1.0 - dot(norm, lightDir)), 0.005));
	vec3 total = ambient + (1 - shadow) * (diffuse + specular);
	vec4 result = vec4(total, 1.0);
	result *= objectColor;
//	frgColor = vec4(1.f, 0, 0, 1.f);
	frgColor = result;

}