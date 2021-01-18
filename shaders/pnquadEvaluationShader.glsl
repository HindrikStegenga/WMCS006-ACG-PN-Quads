#version 410

layout(quads, equal_spacing, cw) in;

struct PatchOut {
    vec3 normals[9];
};

patch in PatchOut normal_cp;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

uniform bool analyticalNormals;
uniform float sigma;

layout (location = 0) out vec3 vertcoords_camera_fs;
layout (location = 1) out vec3 vertnormal_camera_fs;

// Basis functions for Bézier surface.
float b_0(float t) {
    return ((1.0 - t) * (1.0 - t) * (1.0 - t));
}

float b_1(float t) {
    return 3.0 * t * (1.0 - t) * (1.0 - t);
}

float b_2(float t) {
    return 3.0 * t * t * (1.0 - t);
}

float b_3(float t) {
    return t * t * t;
}

// Derivates of Bézier surface functions.
float pd_b0(float t) {
    return -3.0 * (t - 1.0) * (t - 1.0);
}

float pd_b1(float t) {
    return 3 * (t - 1.0) * (3 * t - 1.0);
}

float pd_b2(float t) {
    return 6.0 * t - 9 * t * t;
}

float pd_b3(float t) {
    return 3 * t * t;
}


void main() {

    // In memory as cw oriented patch:
    // input[0] -> b0
    // input[1] -> b03
    // input[2] -> b30
    // input[3] -> b3
    // etc.

    //  b0  - b03 - b30 - b3
    //  |     |     |     |
    //  b01 - b02 - b31 - b32
    //  |     |     |     |
    //  b10 - b13 - b20 - b23
    //  |     |     |     |
    //  b1 -  b12 - b21 - b2


    // Geometry input patch

    vec3 b0     = gl_in[0].gl_Position.xyz;
    vec3 b03    = gl_in[1].gl_Position.xyz;
    vec3 b30    = gl_in[2].gl_Position.xyz;
    vec3 b3     = gl_in[3].gl_Position.xyz;

    vec3 b01    = gl_in[4].gl_Position.xyz;
    vec3 b02    = gl_in[5].gl_Position.xyz;
    vec3 b31    = gl_in[6].gl_Position.xyz;
    vec3 b32    = gl_in[7].gl_Position.xyz;

    vec3 b10    = gl_in[8].gl_Position.xyz;
    vec3 b13    = gl_in[9].gl_Position.xyz;
    vec3 b20    = gl_in[10].gl_Position.xyz;
    vec3 b23    = gl_in[11].gl_Position.xyz;

    vec3 b1     = gl_in[12].gl_Position.xyz;
    vec3 b12    = gl_in[13].gl_Position.xyz;
    vec3 b21    = gl_in[14].gl_Position.xyz;
    vec3 b2     = gl_in[15].gl_Position.xyz;

    // Normal input patch

    vec3 n0     = normal_cp.normals[0];
    vec3 n30    = normal_cp.normals[1];
    vec3 n3     = normal_cp.normals[2];

    vec3 n01    = normal_cp.normals[3];
    vec3 n0123  = normal_cp.normals[4];
    vec3 n23    = normal_cp.normals[5];

    vec3 n1     = normal_cp.normals[6];
    vec3 n12    = normal_cp.normals[7];
    vec3 n2     = normal_cp.normals[8];

    float su = gl_TessCoord.x;
    float sv = gl_TessCoord.y;

    gl_Position = vec4(b_0(su) * (b_0(sv) * b0  + b_1(sv) * b01 + b_2(sv) * b10 + b_3(sv) * b1)
                     + b_1(su) * (b_0(sv) * b03 + b_1(sv) * b02 + b_2(sv) * b13 + b_3(sv) * b12)
                     + b_2(su) * (b_0(sv) * b30 + b_1(sv) * b31 + b_2(sv) * b20 + b_3(sv) * b21)
                     + b_3(su) * (b_0(sv) * b3  + b_1(sv) * b32 + b_2(sv) * b23 + b_3(sv) * b2), 1);


    if (analyticalNormals == false) {
        float nu0 = (1.0 - su) * (1.0 - su);
        float nu1 = 2.0 * su * (1.0 - su);
        float nu2 = su * su;

        float nv0 = (1.0 - sv) * (1.0 - sv);
        float nv1 = 2.0 * sv * (1.0 - sv);
        float nv2 = sv * sv;

        vertnormal_camera_fs = nu0 * (nv0 * n0  + nv1 * n01   + nv2 * n1)
                             + nu1 * (nv0 * n30 + nv1 * n0123 + nv2 * n12)
                             + nu2 * (nv0 * n3  + nv1 * n23   + nv2 * n2);


        vertnormal_camera_fs = normalize(normalmatrix * vertnormal_camera_fs);
    } else {
        // use derivatives of the basis functions to compute analytical normal of the vertex using tensor product.
        // I chose to just do the normal computation in world space for the vertices, since the position
        // doesn't matter for computing a normal anyway.


        // Tangent vector in su direction
        vec3 dpdu =   pd_b0(su) * ( b_0(sv) * b0  + b_1(sv) * b01 + b_2(sv) * b10 + b_3(sv) * b1 )
                    + pd_b1(su) * ( b_0(sv) * b03 + b_1(sv) * b02 + b_2(sv) * b13 + b_3(sv) * b12 )
                    + pd_b2(su) * ( b_0(sv) * b30 + b_1(sv) * b31 + b_2(sv) * b20 + b_3(sv) * b21 )
                    + pd_b3(su) * ( b_0(sv) * b3  + b_1(sv) * b32 + b_2(sv) * b23 + b_3(sv) * b2 );

        // Tangent vector in sv direction
        vec3 dpdv =   b_0(su) * ( pd_b0(sv) * b0  + pd_b1(sv) * b01 + pd_b2(sv) * b10 + pd_b3(sv) * b1 )
                    + b_1(su) * ( pd_b0(sv) * b03 + pd_b1(sv) * b02 + pd_b2(sv) * b31 + pd_b3(sv) * b12 )
                    + b_2(su) * ( pd_b0(sv) * b30 + pd_b1(sv) * b31 + pd_b2(sv) * b20 + pd_b3(sv) * b21 )
                    + b_3(su) * ( pd_b0(sv) * b3  + pd_b1(sv) * b32 + pd_b2(sv) * b23 + pd_b3(sv) * b2 );


        // Cross, normalize and multiply with the normals matrix (and another normalize) to get analytical normal.
        // This normal is then interpolated in the fragment shader automatically.
        vec3 normal = normalize(cross(dpdu, dpdv));
        vertnormal_camera_fs = normalize(normalmatrix * normal);
    }


    vertcoords_camera_fs = vec3(modelviewmatrix * gl_Position);


    // Apply projectionmatrix and modelviewmatrix
    gl_Position = projectionmatrix * modelviewmatrix * gl_Position;
}
