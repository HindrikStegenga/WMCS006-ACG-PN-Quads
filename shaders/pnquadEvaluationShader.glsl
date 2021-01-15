#version 410

layout(quads, equal_spacing, ccw) in;

layout (location = 0) in vec3 tess_vertcoords_camera_fs[];
layout (location = 1) in vec3 tess_vertnormal_camera_fs[];

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

void main() {

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


    vec3 p0 = gl_in[2].gl_Position.xyz;
    vec3 p1 = gl_in[3].gl_Position.xyz;
    vec3 p2 = gl_in[0].gl_Position.xyz;
    vec3 p3 = gl_in[1].gl_Position.xyz;

    vec3 n0 = normalize(tess_vertnormal_camera_fs[2]);
    vec3 n1 = normalize(tess_vertnormal_camera_fs[3]);
    vec3 n2 = normalize(tess_vertnormal_camera_fs[0]);
    vec3 n3 = normalize(tess_vertnormal_camera_fs[1]);

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

    float v01 = (2 * (dot(p1 - p0, n0 + n1) / dot(p1 - p0, p1 - p0)));
    float v12 = (2 * (dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
    float v23 = (2 * (dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
    float v30 = (2 * (dot(p0 - p3, n3 + n0) / dot(p0 - p3, p0 - p3)));

    vec3 n01 = normalize(n0 + n1 - v01 * (p1 - p0));
    vec3 n12 = normalize(n1 + n2 - v12 * (p2 - p1));
    vec3 n23 = normalize(n2 + n3 - v23 * (p3 - p2));
    vec3 n30 = normalize(n3 + n0 - v30 * (p0 - p3));

    vec3 n0123 = ((2.*(n01 + n12 + n23 + n30)) + (n0 + n1 + n2 + n3)) / 12.;

    float su = gl_TessCoord.x;
    float sv = gl_TessCoord.y;

    gl_Position = vec4(b_0(su) * (b_0(sv) * b0  + b_1(sv) * b01 + b_2(sv) * b10 + b_3(sv) * b1)
                     + b_1(su) * (b_0(sv) * b03 + b_1(sv) * b02 + b_2(sv) * b13 + b_3(sv) * b12)
                     + b_2(su) * (b_0(sv) * b30 + b_1(sv) * b31 + b_2(sv) * b20 + b_3(sv) * b21)
                     + b_3(su) * (b_0(sv) * b3  + b_1(sv) * b32 + b_2(sv) * b23 + b_3(sv) * b2), 1);



    float nu0 = (1.-su) * (1.-su);
    float nu1 = 2. * su * (1.-su);
    float nu2 = su * su;

    float nv0 = (1.-sv) * (1.-sv);
    float nv1 = 2. * sv * (1.-sv);
    float nv2 = sv * sv;

    vertnormal_camera_fs = nu0*(nv0*n0 + nv1*n01 + nv2*n1)
            + nu1*(nv0*n30 + nv1*n0123 + nv2*n12)
            + nu2*(nv0*n3 + nv1*n23 + nv2*n2);


    vertnormal_camera_fs = normalize(normalmatrix * vertnormal_camera_fs);
    vertcoords_camera_fs = vec3(modelviewmatrix * gl_Position);


    // Apply projectionmatrix and modelviewmatrix
    gl_Position = projectionmatrix * modelviewmatrix * gl_Position;

    /*
    // Repeat this process for vertcoords_camera_fs
    vertcoords_camera_fs = b_0(u) * ( b_0(v) * bc0  + b_1(v) * bc01 + b_2(v) * bc10 + b_3(v) * bc1  )
                         + b_1(u) * ( b_0(v) * bc02 + b_1(v) * bc02 + b_2(v) * bc13 + b_3(v) * bc12 )
                         + b_2(u) * ( b_0(v) * bc30 + b_1(v) * bc31 + b_2(v) * bc20 + b_3(v) * bc21 )
                         + b_3(u) * ( b_0(v) * bc3  + b_1(v) * bc32 + b_2(v) * bc23 + b_3(v) * bc2  );

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
                         + b_3(u) * ( b_0(v) * bn3  + b_1(v) * bn32 + b_2(v) * bn23 + b_3(v) * bn2  );

    // And apply the normalization and normalmatrix multiply.
    vertnormal_camera_fs = normalize(normalmatrix * vertnormal_camera_fs); */
}
