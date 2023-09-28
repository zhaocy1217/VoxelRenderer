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
#define UVOffset 2/800.0
void main() {
	  vec4 baseColor = vec4(0,0,0,0);
    const vec2 offsets[9] = {vec2( 1, 1),vec2( 0, 1),vec2( -1, 1), vec2( 1, 0),
                                vec2( 0, 0),vec2( -1, 0),vec2( 1, -1),vec2( 0, -1),vec2( -1, -1)};
    for(int i=0;i<9;++i)
    {
        vec2 offset = offsets[i];
        baseColor += texture(_BaseMap, fragTexCoord + offset*UVOffset).rgba;
    }
    baseColor/=9;
    outColor = baseColor;
}