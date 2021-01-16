#version 410
layout(vertices = 4) out;

layout (location = 0) in vec3 vertcoords_camera_fs[];
layout (location = 1) in vec3 vertnormal_camera_fs[];

uniform mat3 normalmatrix;
uniform float sigma;

uniform int tessInner0;
uniform int tessInner1;
uniform int tessOuter0;
uniform int tessOuter1;
uniform int tessOuter2;
uniform int tessOuter3;

layout (location = 0) patch out vec3 cp [16];
layout (location = 16) patch out vec3 np [9];

void main() {

    if (gl_InvocationID == 0) {

        vec3 p0 = cp[0] = gl_in[2].gl_Position.xyz;
        vec3 p1 = cp[1] = gl_in[3].gl_Position.xyz;
        vec3 p2 = cp[2] = gl_in[0].gl_Position.xyz;
        vec3 p3 = cp[3] = gl_in[1].gl_Position.xyz;

        vec3 n0 = np[0] = normalize(vertnormal_camera_fs[2]);
        vec3 n1 = np[1] = normalize(vertnormal_camera_fs[3]);
        vec3 n2 = np[2] = normalize(vertnormal_camera_fs[0]);
        vec3 n3 = np[3] = normalize(vertnormal_camera_fs[1]);

        vec3 b01 = cp[4] = (2.0 * p0 + p1 - dot(p1 - p0, n0) * n0) / 3.0;
        vec3 b10 = cp[5] = (2.0 * p1 + p0 - dot(p0 - p1, n1) * n1) / 3.0;

        vec3 b12 = cp[6] = (2.0 * p1 + p2 - dot(p2 - p1, n1) * n1) / 3.0;
        vec3 b21 = cp[7] = (2.0 * p2 + p1 - dot(p1 - p2, n2) * n2) / 3.0;

        vec3 b23 = cp[8] = (2.0 * p2 + p3 - dot(p3 - p2, n2) * n2) / 3.0;
        vec3 b32 = cp[9] = (2.0 * p3 + p2 - dot(p2 - p3, n3) * n3) / 3.0;

        vec3 b30 = cp[10] = (2.0 * p3 + p0 - dot(p0 - p3, n3) * n3) / 3.0;
        vec3 b03 = cp[11] = (2.0 * p0 + p3 - dot(p3 - p0, n0) * n0) / 3.0;

        vec3 q = b01 + b10 + b12 + b21 + b23 + b32 + b30 + b03;

        //b02
        vec3 e = (2.0 * (b01 + b03 + q) - (b21 + b23)) / 18.0;
        vec3 v = (4.0 * p0 + 2.0 * (p3 + p1) + p2) / 9.0;
        cp[12] =  (1.0 + sigma) * e - sigma * v;

        //b13
        e = (2.0 * (b12 + b10 + q) - (b32 + b30)) / 18.0;
        v = (4.0 * p1 + 2.0 * (p0 + p2) + p3) / 9.0;
        cp[13] = (1.0 + sigma) * e - sigma * v;

        //b20
        e = (2.0 * (b23 + b21 + q) - (b03 + b01)) / 18.0;
        v = (4.0 * p2 + 2.0 * (p1 + p3) + p0) / 9.0;
        cp[14] = (1.0 + sigma) * e - sigma * v;

        //b31
        e = (2.0 * (b30 + b32 + q) - (b10 + b12)) / 18.0;
        v = (4.0 * p3 + 2.0 * (p2 + p0) + p1) / 9.0;
        cp[15] = (1.0 + sigma) * e - sigma * v;

        float v01 = (2 * (dot(p1 - p0, n0 + n1) / dot(p1 - p0, p1 - p0)));
        float v12 = (2 * (dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
        float v23 = (2 * (dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
        float v30 = (2 * (dot(p0 - p3, n3 + n0) / dot(p0 - p3, p0 - p3)));

        vec3 n01 = np[4] =  normalize(n0 + n1 - v01 * (p1 - p0));
        vec3 n12 = np[5] = normalize(n1 + n2 - v12 * (p2 - p1));
        vec3 n23 = np[6] = normalize(n2 + n3 - v23 * (p3 - p2));
        vec3 n30 = np[7] = normalize(n3 + n0 - v30 * (p0 - p3));

        np[8] = ((2.*(n01 + n12 + n23 + n30)) + (n0 + n1 + n2 + n3))
                / 12.;


        gl_TessLevelInner[0] = tessInner0;
        gl_TessLevelInner[1] = tessInner1;
        gl_TessLevelOuter[0] = tessOuter0;
        gl_TessLevelOuter[1] = tessOuter1;
        gl_TessLevelOuter[2] = tessOuter2;
        gl_TessLevelOuter[3] = tessOuter3;

    }


    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
