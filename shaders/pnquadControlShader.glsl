#version 410
layout(vertices = 4) out;

in vec3 vertcoords_camera_fs[];
in vec3 vertnormal_camera_fs[];

uniform mat3 normalmatrix;

uniform int tessInner0;
uniform int tessInner1;
uniform int tessOuter0;
uniform int tessOuter1;
uniform int tessOuter2;
uniform int tessOuter3;

const float sigma = 0.5;

patch out vec3 out_positions[16];

out vec3 tess_vertcoords_camera_fs[];
out vec3 tess_vertnormal_camera_fs[];

float wij(int i, int j) {
    return dot(gl_in[j].gl_Position.xyz - gl_in[i].gl_Position.xyz, tess_vertnormal_camera_fs[j]);
}


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

    //  b0  - b03 - b30 - b3
    //  |     |     |     |
    //  b01 - b02 - b31 - p32
    //  |     |     |     |
    //  b10 - b13 - b20 - p23
    //  |     |     |     |
    //  b1 -  b12 - b21 - b2

    // Maps to: (where p0 is array[0] etc.)

    //  p0 -  p4 - p8 - p12
    //  |     |     |     |
    //  p1 -  p5 - p9 - p13
    //  |     |     |     |
    //  p2 -  p6 - p10 - p14
    //  |     |     |     |
    //  p3 -  p7 - p11 - p15


    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 n0 = tess_vertnormal_camera_fs[0];
    vec3 n1 = tess_vertnormal_camera_fs[1];
    vec3 n2 = tess_vertnormal_camera_fs[2];
    vec3 n3 = tess_vertnormal_camera_fs[3];

    n0 = (n0);
    n1 = (n1);
    n2 = (n2);
    n3 = (n3);

    // Compute boundary points

    vec3 b0  = p0;
    vec3 b01 = (2.0 * p0 + p1 - wij(0, 1) * n0) / 3.0;
    vec3 b10 = (2.0 * p1 + p0 - wij(1, 0) * n1) / 3.0;

    vec3 b1 = p1;
    vec3 b12 = (2.0 * p1 + p2 - wij(1, 2) * n1) / 3.0;
    vec3 b21 = (2.0 * p2 + p1 - wij(2, 1) * n2) / 3.0;

    vec3 b2 = p2;
    vec3 b23 = (2.0 * p2 + p3 - wij(2, 3) * n2) / 3.0;
    vec3 b32 = (2.0 * p3 + p2 - wij(3, 2) * n3) / 3.0;

    vec3 b3 = p3;
    vec3 b30 = (2.0 * p3 + p0 - wij(3, 0) * n3) / 3.0;
    vec3 b03 = (2.0 * p0 + p3 - wij(0, 3) * n0) / 3.0;

    vec3 q = b01 + b10 + b12 + b21 + b23 + b32 + b30 + b03;


    // We need to compute b0, b01, b03, b02.

    //b0
    out_positions[0] = p0;
    //b01
    out_positions[1] = b01;
    //b03
    out_positions[2] = b10;

    //b02
    vec3 e = (2.0 * (b01 + b03 + q) - (b21 + b23)) / 18.0;
    vec3 v = (4.0 * p0 + 2.0 * (p3 + p1) + p2) / 9.0;
    vec3 b02 = (1.0 + sigma) * e - sigma * v;

    out_positions[5] = b02;


    // We need to compute b1, b12, b21, b13.

    //b1
    out_positions[3] = p1;
    //b12
    out_positions[7] = b12;
    //b21
    out_positions[11] = b21;

    //b13
    e = (2.0 * (b12 + b10 + q) - (b32 + b30)) / 18.0;
    v = (4.0 * p1 + 2.0 * (p0 + p2) + p3) / 9.0;
    vec3 b13 = (1.0 + sigma) * e - sigma * v;

    out_positions[6] = b13;

    // We need to compute b2, b23, b32, b20.

    //b2
    out_positions[15] = p2;
    //b23
    out_positions[14] = b23;
    //b32
    out_positions[13] = b32;

    //b20
    e = (2.0 * (b23 + b21 + q) - (b03 + b01)) / 18.0;
    v = (4.0 * p2 + 2.0 * (p1 + p3) + p0) / 9.0;
    vec3 b20 = (1.0 + sigma) * e - sigma * v;

    out_positions[10] = b20;


    // We need to compute b3, b30, b03, b31.

    //b3
    out_positions[12] = p3;
    //b30
    out_positions[8] = b30;
    //b03
    out_positions[4] = b03;

    //b31
    e = (2.0 * (b30 + b32 + q) - (b10 + b12)) / 18.0;
    v = (4.0 * p3 + 2.0 * (p2 + p0) + p1) / 9.0;
    vec3 b31 = (1.0 + sigma) * e - sigma * v;

    out_positions[9] = b31;


    // Output 4 control points per input point. 2 boundaries, 1 original and 1 of centre points.
    switch (gl_InvocationID) {
    case 0: {

    }
        break;
    case 1: {

    }
        break;
    case 2: {

    }
        break;
    case 3: {

    }
        break;
    }


}
