#version 410

layout(quads, equal_spacing, ccw) in;

in vec3 tess_vertcoords_camera_fs[];
in vec3 tess_vertnormal_camera_fs[];

patch in vec3 out_positions[16];

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

uniform bool analyticalNormals;

out vec3 vertcoords_camera_fs;
out vec3 vertnormal_camera_fs;


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


void main() {

    vec3 b0  = out_positions[0];
    vec3 b01 = out_positions[1];
    vec3 b10 = out_positions[2];
    vec3 b1  = out_positions[3];
    vec3 b03 = out_positions[4];
    vec3 b02 = out_positions[5];
    vec3 b13 = out_positions[6];
    vec3 b12 = out_positions[7];
    vec3 b30 = out_positions[8];
    vec3 b31 = out_positions[9];
    vec3 b20 = out_positions[10];
    vec3 b21 = out_positions[11];
    vec3 b3  = out_positions[12];
    vec3 b32 = out_positions[13];
    vec3 b23 = out_positions[14];
    vec3 b2  = out_positions[15];

    vec3 bc0  = tess_vertcoords_camera_fs[0];
    vec3 bc01 = tess_vertcoords_camera_fs[1];
    vec3 bc10 = tess_vertcoords_camera_fs[2];
    vec3 bc1  = tess_vertcoords_camera_fs[3];
    vec3 bc03 = tess_vertcoords_camera_fs[4];
    vec3 bc02 = tess_vertcoords_camera_fs[5];
    vec3 bc13 = tess_vertcoords_camera_fs[6];
    vec3 bc12 = tess_vertcoords_camera_fs[7];
    vec3 bc30 = tess_vertcoords_camera_fs[8];
    vec3 bc31 = tess_vertcoords_camera_fs[9];
    vec3 bc20 = tess_vertcoords_camera_fs[10];
    vec3 bc21 = tess_vertcoords_camera_fs[11];
    vec3 bc3  = tess_vertcoords_camera_fs[12];
    vec3 bc32 = tess_vertcoords_camera_fs[13];
    vec3 bc23 = tess_vertcoords_camera_fs[14];
    vec3 bc2  = tess_vertcoords_camera_fs[15];

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    gl_Position = vec4(b_0(u) * ( b_0(v) * b0  + b_1(v) * b01 + b_2(v) * b10 + b_3(v) * b1  )
                     + b_1(u) * ( b_0(v) * b02 + b_1(v) * b02 + b_2(v) * b13 + b_3(v) * b12 )
                     + b_2(u) * ( b_0(v) * b30 + b_1(v) * b31 + b_2(v) * b20 + b_3(v) * b21 )
                     + b_3(u) * ( b_0(v) * b3  + b_1(v) * b32 + b_2(v) * b23 + b_3(v) * b2 ), 1);


    // Apply projectionmatrix and modelviewmatrix
    gl_Position = projectionmatrix * modelviewmatrix * gl_Position;


    // Repeat this process for vertcoords_camera_fs
    vertcoords_camera_fs = b_0(u) * ( b_0(v) * bc0  + b_1(v) * bc01 + b_2(v) * bc10 + b_3(v) * bc1  )
                         + b_1(u) * ( b_0(v) * bc02 + b_1(v) * bc02 + b_2(v) * bc13 + b_3(v) * bc12 )
                         + b_2(u) * ( b_0(v) * bc30 + b_1(v) * bc31 + b_2(v) * bc20 + b_3(v) * bc21 )
                         + b_3(u) * ( b_0(v) * bc3  + b_1(v) * bc32 + b_2(v) * bc23 + b_3(v) * bc2 );

    // Apply model view matrix
    vertcoords_camera_fs = vec3(modelviewmatrix * vec4(vertcoords_camera_fs, 1.0));


    vec3 bn0  = tess_vertnormal_camera_fs[0];
    vec3 bn01 = tess_vertnormal_camera_fs[1];
    vec3 bn10 = tess_vertnormal_camera_fs[2];
    vec3 bn1  = tess_vertnormal_camera_fs[3];
    vec3 bn03 = tess_vertnormal_camera_fs[4];
    vec3 bn02 = tess_vertnormal_camera_fs[5];
    vec3 bn13 = tess_vertnormal_camera_fs[6];
    vec3 bn12 = tess_vertnormal_camera_fs[7];
    vec3 bn30 = tess_vertnormal_camera_fs[8];
    vec3 bn31 = tess_vertnormal_camera_fs[9];
    vec3 bn20 = tess_vertnormal_camera_fs[10];
    vec3 bn21 = tess_vertnormal_camera_fs[11];
    vec3 bn3  = tess_vertnormal_camera_fs[12];
    vec3 bn32 = tess_vertnormal_camera_fs[13];
    vec3 bn23 = tess_vertnormal_camera_fs[14];
    vec3 bn2  = tess_vertnormal_camera_fs[15];

    // Repeat the aforementioned process for vertnormal_camera_fs
    vertnormal_camera_fs = b_0(u) * ( b_0(v) * bn0  + b_1(v) * bn01 + b_2(v) * bn10 + b_3(v) * bn1  )
                         + b_1(u) * ( b_0(v) * bn02 + b_1(v) * bn02 + b_2(v) * bn13 + b_3(v) * bn12 )
                         + b_2(u) * ( b_0(v) * bn30 + b_1(v) * bn31 + b_2(v) * bn20 + b_3(v) * bn21 )
                         + b_3(u) * ( b_0(v) * bn3  + b_1(v) * bn32 + b_2(v) * bn23 + b_3(v) * bn2 );

    // And apply the normalization and normalmatrix multiply.
    vertnormal_camera_fs = normalize(normalmatrix * vertnormal_camera_fs);
}
