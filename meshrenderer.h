#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <QOpenGLShaderProgram>

#include "renderer.h"
#include "mesh.h"

class MeshRenderer : public Renderer
{
public:
    MeshRenderer();
    ~MeshRenderer();

    void init(QOpenGLFunctions_4_1_Core* f, Settings* s);

    void initShaders();
    void initBuffers();

    void updateUniforms();

    void updateBuffers(Mesh& m);
    void draw();

    void regularDraw();
    void limitDraw();
    void tesselatedBsplineDraw(GLuint tessVao);
    void tesselatedPnQuadDraw(GLuint tessVao);
private:

    GLuint vao,
           limitVao,
           bsplineTessRegularVao,
           bsplineTessLimitVao,
           pnQuadTessRegularVao,
           pnQuadTessLimitVao;


    GLuint meshCoordsBO,
           meshNormalsBO,
           meshIndexBO,
           meshLimitCoordsBO,
           meshLimitNormalsBO,
           bsplineTessIndexBO,
           pnQuadTessIndexBO;


    unsigned int meshIBOSize;
    unsigned int bsplinePatchCount;
    unsigned int pnQuadPatchCount;

    QOpenGLShaderProgram shaderProg,
                         bsplineTessShaderProg,
                         pnQuadTessShaderProg;

    // Uniforms
    GLint uniModelViewMatrix, uniProjectionMatrix, uniNormalMatrix;
    GLint uniModelViewMatrixBsplineTess, uniProjectionMatrixBsplineTess, uniNormalMatrixBsplineTess;
    GLint uniModelViewMatrixPnQuadTess, uniProjectionMatrixPnQuadTess, uniNormalMatrixPnQuadTess;
};

#endif // MESHRENDERER_H
