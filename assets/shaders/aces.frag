#version 450
layout(set = 0, binding = 1) uniform builtin_props {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 _builtin_light_dir;
    vec3 _builtin_camera_posi;
};

layout(set = 0, binding = 2) uniform sampler2D _BaseMap;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
#define TONEMAP_GAMMA 2.2
#define float3 vec3
//#define float vec
vec3 tonemap_filmic(in vec3 color)
{
    color = max(vec3(0,0,0), color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);
    return pow(color, vec3(TONEMAP_GAMMA,TONEMAP_GAMMA,TONEMAP_GAMMA));
}
vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
vec3 unreal(vec3 x) {
  return x / (x + 0.155) * 1.019;
}
float3 power_float3(float3 v, float factor)
{
  return float3(pow(v.x, factor),pow(v.y, factor),pow(v.z, factor));
}
void main() {
	vec4 baseColor = (texture(_BaseMap, fragTexCoord));
  baseColor.xyz = power_float3(baseColor.xyz, 1/TONEMAP_GAMMA);
  //  baseColor.rgb = pow(baseColor.rgb, vec3(TONEMAP_GAMMA,TONEMAP_GAMMA,TONEMAP_GAMMA));
    baseColor.rgb = tonemap_filmic(baseColor.rgb);
   // baseColor.rgb = unreal(baseColor.rgb);
   baseColor.xyz = power_float3(baseColor.xyz, TONEMAP_GAMMA);
    outColor = baseColor;
}