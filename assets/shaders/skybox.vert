#version 450

layout(set = 0, binding = 0) uniform builtin_props {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 _builtin_light_dir;
    vec3 _builtin_camera_posi;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normalws;
layout(location = 3) out vec3 tangentws;
layout(location = 4) out vec3 binormalws;
layout(location = 5) out vec3 positionws;
layout(location = 6) out vec3 positionms;
void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    positionms = normal;
    positionws = (model * vec4(position, 1.0)).xyz;
    normalws = normalize((model*vec4(normal, 0)).xyz);
    tangentws = normalize((model*vec4(tangent, 0)).xyz);
    binormalws = cross(normalws, tangentws);
    fragColor = color.rgb;
    //mat3x3 tbn = mat3x3(nb, cross(nn, nb), nn);
    fragTexCoord = texcoord;
}