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
layout(location = 0) out vec2 fragTexCoord;
void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    fragTexCoord = texcoord;
}