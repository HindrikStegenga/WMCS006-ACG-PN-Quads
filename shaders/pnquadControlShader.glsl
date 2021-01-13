#version 410
layout(vertices = 4) out;

layout (location = 0) in vec3 vertcoords_camera_fs[];
layout (location = 1) in vec3 vertnormal_camera_fs[];

uniform mat3 normalmatrix;

uniform int tessInner0;
uniform int tessInner1;
uniform int tessOuter0;
uniform int tessOuter1;
uniform int tessOuter2;
uniform int tessOuter3;

layout (location = 0) out vec3 tess_vertcoords_camera_fs[];
layout (location = 1) out vec3 tess_vertnormal_camera_fs[];

void main() {
    gl_TessLevelInner[0] = tessInner0;
    gl_TessLevelInner[1] = tessInner1;
    gl_TessLevelOuter[0] = tessOuter0;
    gl_TessLevelOuter[1] = tessOuter1;
    gl_TessLevelOuter[2] = tessOuter2;
    gl_TessLevelOuter[3] = tessOuter3;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tess_vertcoords_camera_fs[gl_InvocationID] = vertcoords_camera_fs[gl_InvocationID];
    tess_vertnormal_camera_fs[gl_InvocationID] = vertnormal_camera_fs[gl_InvocationID];
}
