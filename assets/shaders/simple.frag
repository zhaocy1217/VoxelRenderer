#version 450
layout(set = 0, binding = 1) uniform builtin_props {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 _builtin_light_dir;
    vec3 _builtin_camera_posi;
};
layout(set = 0, binding = 2) uniform properties {
    vec4 _Color;
};
layout(set = 0, binding = 3) uniform sampler2D _BaseMap;
layout(set = 0, binding = 4) uniform sampler2D _NormalMap;
layout(set = 0, binding = 5) uniform samplerCube _EnvMap;
layout(set = 0, binding = 6) uniform sampler2D _RoughnessMap;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normalws;
layout(location = 3) in vec3 tangentws;
layout(location = 4) in vec3 binormalws;
layout(location = 5) in vec3 positionws;
layout(location = 0) out vec4 outColor;
#define PI 3.14
#define REAL_EPS 0.00001
#define UNITY_SPECCUBE_LOD_STEPS 6
#define kDielectricSpec vec4(0.04, 0.04, 0.04, 1.0 - 0.04)
#define kDieletricSpec kDielectricSpec

float OneMinusReflectivityMetallic(float metallic)
{
    float oneMinusDielectricSpec = kDielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}
float Pow4(float v)
{
    return v*v*v*v;
}
float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}
float PerceptualRoughnessToMipmapLevel(float perceptualRoughness, float NdotR)
{
    float m = PerceptualRoughnessToRoughness(perceptualRoughness);

    // Remap to spec power. See eq. 21 in --> https://dl.dropboxusercontent.com/u/55891920/papers/mm_brdf.pdf
    float n = (2.0 / max(REAL_EPS, m * m)) - 2.0;

    // Remap from n_dot_h formulation to n_dot_r. See section "Pre-convolved Cube Maps vs Path Tracers" --> https://s3.amazonaws.com/docs.knaldtech.com/knald/1.0.0/lys_power_drops.html
    n /= (4.0 * max(NdotR, REAL_EPS));

    // remap back to square root of real roughness (0.25 include both the sqrt root of the conversion and sqrt for going from roughness to perceptualRoughness)
    perceptualRoughness = pow(2.0 / (n + 2.0), 0.25);

    return perceptualRoughness * UNITY_SPECCUBE_LOD_STEPS;
}
float PerceptualRoughnessToMipmapLevel(float perceptualRoughness)
{
    return PerceptualRoughnessToMipmapLevel(perceptualRoughness, UNITY_SPECCUBE_LOD_STEPS);
}
void main() {
    vec3 light_color = vec3(1.0, 1.0, 1.0)*1;
	vec4 baseColor = (texture(_BaseMap, fragTexCoord))*_Color;
    mat3 normal_matrix = mat3(tangentws, binormalws, normalws);
    vec4 normalori = (texture(_NormalMap, fragTexCoord).rgba);
    vec4 occlusion = vec4(1,1,1,1);
    vec3 normalts = (normalori.rgb*2-1);
    vec3 normalws = normalize(normal_matrix*normalts);
    vec3 viewdir = normalize(_builtin_camera_posi - positionws);
    vec3 lightdir = _builtin_light_dir;
    vec3 halfdir = normalize(lightdir + viewdir);
    float NdL = max(0.001, dot(normalws, lightdir));
    float NdV = max(0.001, dot(normalws, viewdir));
    float NdH = max(0.001, dot(normalws, halfdir));
    float HdV = max(0.001, dot(halfdir, viewdir));
    float LdV = max(0.001, dot(lightdir, viewdir));
    float LdH = max(0.001, dot(lightdir, halfdir));
    vec4 rm = (texture(_RoughnessMap, fragTexCoord));
    float metallic = rm.b;
    float smoothness = rm.g;
    float perceptualRoughness =1-smoothness;
    float roughness = perceptualRoughness * perceptualRoughness;
    float roughness2= max(roughness*roughness,0.0001);
    float normalizationTerm = roughness * 4.0 + 2.0;
    float roughness2MinusOne = roughness2-1;
    float oneMinusReflectivity = OneMinusReflectivityMetallic(metallic);
    float reflectivity = 1.0 - oneMinusReflectivity;
    vec3 brdfDiffuse = baseColor.rgb * oneMinusReflectivity;
    vec3 brdfSpecular = mix(kDieletricSpec.rgb, baseColor.rgb, metallic);
    float grazingTerm = max(0.001, (smoothness + reflectivity));;
    vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);
    float d = NdH * NdH * roughness2MinusOne + 1.00001f;
    float LoH2 = LdH * LdH;
    float specularTerm = roughness2 / ((d * d) * max(0.1, LoH2) * normalizationTerm);
    vec3 directionColor = brdfDiffuse + brdfSpecular*specularTerm;

    vec3 reflectVector = reflect(-viewdir, normalws);
    float fresnelTerm = Pow4(1.0 - NdV);
    vec3 bakedGI = vec3(0.2,0.2,0.8);
    vec3 indirectDiffuse = bakedGI * occlusion.r * brdfDiffuse;
    float mip = PerceptualRoughnessToMipmapLevel(perceptualRoughness);
    vec3 envSpecularColor = texture(_EnvMap, reflectVector, mip).rgb * occlusion.r;
    float surfaceReduction = 1.0 / (roughness2 + 1.0);
    vec3 evnSpecularTerm = surfaceReduction * mix(brdfSpecular, grazingTerm.rrr, fresnelTerm);
    vec3 indirectColor = indirectDiffuse + evnSpecularTerm*envSpecularColor;
    outColor = (vec4(directionColor+indirectColor, 1));
}