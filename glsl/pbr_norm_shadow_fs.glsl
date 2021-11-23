#version 330 core
#define N_LIGHTS 1

in FRAG {
	vec2 uv;
	vec3 tangentPos;
	vec3 tangentViewPos;
	vec3 tangentLightDir;
	vec4 lightSpacePos;

} frag;
out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
//uniform sampler2D metallicMap;
uniform sampler2D aoMap;
uniform sampler2D roughMap;
uniform sampler2D depthMap;

const float PI = 3.14159265359;
const vec3 lightColor = vec3(10.0f);


float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float computeShadow(float bias);

void main()
{       
	// 从贴图中取出数据
    vec3 albedo     = pow(texture(albedoMap, frag.uv).rgb, vec3(2.2));
    float metallic  = 0.1; // texture(metallicMap, frag.uv).r;
    float roughness = texture(roughMap, frag.uv).r;
    float ao        = texture(aoMap, frag.uv).r;
    vec3 norm     = texture(normalMap, frag.uv).rgb;
	
	// 法线和观察向量
	norm = norm * 2 - 1.0;
	norm = normalize(norm);
    vec3 N = normalize(norm);
    vec3 V = normalize(frag.tangentViewPos - frag.tangentPos);

	// 折射指数
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // 反射率方程
    // 每个点光源的辐照度（即其光经过衰减后到达片段的强度）
    vec3 L = normalize(frag.tangentLightDir);
    vec3 H = normalize(V + L);
    vec3 radiance = lightColor;        

    // DGF
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       

	// cook - torrance函数
    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;

	// fr函数参数
	vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;     

    // 该光源下的反射能量
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL; 

	// 片段颜色
	float shadow = computeShadow(max(0.05 * (1.0 - dot(N, L)), 0.005));
    vec3 ambient = vec3(0.05) * albedo * ao;
//    vec3 color = ambient + Lo;
    vec3 color = ambient + (1 - shadow) * Lo;

	// Gamma校正
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

	// 最终颜色
    FragColor = vec4(color, 1.0);
//    FragColor = vec4(1.f ,0,0, 1.0);

}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float computeShadow(float bias)
{
	 // 执行透视除法
    vec3 projCoords = frag.lightSpacePos.xyz / frag.lightSpacePos.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
		return 0.0;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
	// PCF阴影
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}
