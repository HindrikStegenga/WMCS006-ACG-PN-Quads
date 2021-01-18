#version 410
layout(vertices = 16) out;

uniform mat3 normalmatrix;

uniform int tessInner0;
uniform int tessInner1;
uniform int tessOuter0;
uniform int tessOuter1;
uniform int tessOuter2;
uniform int tessOuter3;

uniform float sigma;

layout (location = 0) in vec3 vertcoords_camera_fs[];
layout (location = 1) in vec3 vertnormal_camera_fs[];

struct PatchOut {
    vec3 normals[9];
};

patch out PatchOut normal_cp;

void main() {

    // There will be 16 invocations per input patch of 4 vertices.

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 n0 = normalize(vertnormal_camera_fs[0]);
    vec3 n1 = normalize(vertnormal_camera_fs[1]);
    vec3 n2 = normalize(vertnormal_camera_fs[2]);
    vec3 n3 = normalize(vertnormal_camera_fs[3]);

    if (gl_InvocationID == 0) {

        gl_TessLevelInner[0] = tessInner0;
        gl_TessLevelInner[1] = tessInner1;
        gl_TessLevelOuter[0] = tessOuter0;
        gl_TessLevelOuter[1] = tessOuter1;
        gl_TessLevelOuter[2] = tessOuter2;
        gl_TessLevelOuter[3] = tessOuter3;

        // Compute normal patch

        float v01 = (2 * (dot(p1 - p0, n0 + n1) / dot(p1 - p0, p1 - p0)));
        float v12 = (2 * (dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
        float v23 = (2 * (dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
        float v30 = (2 * (dot(p0 - p3, n3 + n0) / dot(p0 - p3, p0 - p3)));

        vec3 n01 = normalize(n0 + n1 - v01 * (p1 - p0));
        vec3 n12 = normalize(n1 + n2 - v12 * (p2 - p1));
        vec3 n23 = normalize(n2 + n3 - v23 * (p3 - p2));
        vec3 n30 = normalize(n3 + n0 - v30 * (p0 - p3));

        vec3 n0123 = ((2.0 * (n01 + n12 + n23 + n30)) + (n0 + n1 + n2 + n3)) / 12.0;

        // Store normal patch

        normal_cp.normals[0] = n0;
        normal_cp.normals[1] = n30;
        normal_cp.normals[2] = n3;

        normal_cp.normals[3] = n01;
        normal_cp.normals[4] = n0123;
        normal_cp.normals[5] = n23;

        normal_cp.normals[6] = n1;
        normal_cp.normals[7] = n12;
        normal_cp.normals[8] = n2;
    }

    // Compute boundary points

    vec3 b0  = p0;
    vec3 b01 = (2.0 * p0 + p1 - dot(p1 - p0, n0) * n0) / 3.0;
    vec3 b10 = (2.0 * p1 + p0 - dot(p0 - p1, n1) * n1) / 3.0;

    vec3 b1 = p1;
    vec3 b12 = (2.0 * p1 + p2 - dot(p2 - p1, n1) * n1) / 3.0;
    vec3 b21 = (2.0 * p2 + p1 - dot(p1 - p2, n2) * n2) / 3.0;

    vec3 b2 = p2;
    vec3 b23 = (2.0 * p2 + p3 - dot(p3 - p2, n2) * n2) / 3.0;
    vec3 b32 = (2.0 * p3 + p2 - dot(p2 - p3, n3) * n3) / 3.0;

    vec3 b3 = p3;
    vec3 b30 = (2.0 * p3 + p0 - dot(p0 - p3, n3) * n3) / 3.0;
    vec3 b03 = (2.0 * p0 + p3 - dot(p3 - p0, n0) * n0) / 3.0;

    vec3 q = b01 + b10 + b12 + b21 + b23 + b32 + b30 + b03;

    // Compute center control points

    //b02
    vec3 e = (2.0 * (b01 + b03 + q) - (b21 + b23)) / 18.0;
    vec3 v = (4.0 * p0 + 2.0 * (p3 + p1) + p2) / 9.0;
    vec3 b02 = (1.0 + sigma) * e - sigma * v;

    //b13
    e = (2.0 * (b12 + b10 + q) - (b32 + b30)) / 18.0;
    v = (4.0 * p1 + 2.0 * (p0 + p2) + p3) / 9.0;
    vec3 b13 = (1.0 + sigma) * e - sigma * v;

    //b20
    e = (2.0 * (b23 + b21 + q) - (b03 + b01)) / 18.0;
    v = (4.0 * p2 + 2.0 * (p1 + p3) + p0) / 9.0;
    vec3 b20 = (1.0 + sigma) * e - sigma * v;

    //b31
    e = (2.0 * (b30 + b32 + q) - (b10 + b12)) / 18.0;
    v = (4.0 * p3 + 2.0 * (p2 + p0) + p1) / 9.0;
    vec3 b31 = (1.0 + sigma) * e - sigma * v;

    switch (gl_InvocationID) {
    case 0:
        gl_out[gl_InvocationID].gl_Position = vec4(b0, 1);
        break;
    case 1:
        gl_out[gl_InvocationID].gl_Position = vec4(b03, 1);
        break;
    case 2:
        gl_out[gl_InvocationID].gl_Position = vec4(b30, 1);
        break;
    case 3:
        gl_out[gl_InvocationID].gl_Position = vec4(b3, 1);
        break;

    case 4:
        gl_out[gl_InvocationID].gl_Position = vec4(b01, 1);
        break;
    case 5:
        gl_out[gl_InvocationID].gl_Position = vec4(b02, 1);
        break;
    case 6:
        gl_out[gl_InvocationID].gl_Position = vec4(b31, 1);
        break;
    case 7:
        gl_out[gl_InvocationID].gl_Position = vec4(b32, 1);
        break;

    case 8:
        gl_out[gl_InvocationID].gl_Position = vec4(b10, 1);
        break;
    case 9:
        gl_out[gl_InvocationID].gl_Position = vec4(b13, 1);
        break;
    case 10:
        gl_out[gl_InvocationID].gl_Position = vec4(b20, 1);
        break;
    case 11:
        gl_out[gl_InvocationID].gl_Position = vec4(b23, 1);
        break;

    case 12:
        gl_out[gl_InvocationID].gl_Position = vec4(b1, 1);
        break;
    case 13:
        gl_out[gl_InvocationID].gl_Position = vec4(b12, 1);
        break;
    case 14:
        gl_out[gl_InvocationID].gl_Position = vec4(b21, 1);
        break;
    case 15:
        gl_out[gl_InvocationID].gl_Position = vec4(b2, 1);
        break;
    default:
        return;
    }
}
