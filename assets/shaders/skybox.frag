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
//layout(set = 0, binding = 3) uniform sampler2D _BaseMap;
layout(set = 0, binding = 3) uniform samplerCube _EnvMap2;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normalws;
layout(location = 3) in vec3 tangentws;
layout(location = 4) in vec3 binormalws;
layout(location = 5) in vec3 positionws;
layout(location = 6) in vec3 positionms;
layout(location = 0) out vec4 outColor;
#define PI 3.14
void main() {
	//vec4 baseColor = (sampleSky(_BaseMap, positionms))*_Color;
    vec4 baseColor = texture(_EnvMap2, normalws);
    outColor = baseColor;//linear2gamma(vec4(baseColor.rgb, 1));
}