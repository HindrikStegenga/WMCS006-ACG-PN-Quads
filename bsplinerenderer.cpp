#include "bsplinerenderer.h"
#include <iostream>

BsplineRenderer::BsplineRenderer()
{
    meshIBOSize = 0;
}

BsplineRenderer::~BsplineRenderer() {
    gl->glDeleteVertexArrays(1, &vao);
    gl->glDeleteVertexArrays(1, &limitVao);
    gl->glDeleteBuffers(1, &meshLimitCoordsBO);
    gl->glDeleteBuffers(1, &meshLimitNormalsBO);
    gl->glDeleteBuffers(1, &meshIndexBO);
}

void BsplineRenderer::init(QOpenGLFunctions_4_1_Core* f, Settings* s) {
    gl = f;
    settings = s;

    initShaders();
    initBuffers();
}


void BsplineRenderer::initShaders() {
    shaderProg.create();
    shaderProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tessVertShader.glsl");
    shaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shaders/bsplineControlShader.glsl");
    shaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shaders/bsplineEvaluationShader.glsl");
    shaderProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shaderProg.link();

    uniModelViewMatrix = gl->glGetUniformLocation(shaderProg.programId(), "modelviewmatrix");
    uniProjectionMatrix= gl->glGetUniformLocation(shaderProg.programId(), "projectionmatrix");
    uniNormalMatrix = gl->glGetUniformLocation(shaderProg.programId(), "normalmatrix");


}

void BsplineRenderer::updateBuffers(Mesh& currentMesh) {

    qDebug() << ".. updateBuffers";

    //gather attributes for current mesh
    currentMesh.extractAttributes();

    QVector<QVector3D>& vertexCoords = currentMesh.getVertexCoords();
    QVector<QVector3D>& vertexNormals = currentMesh.getVertexNorms();

    QVector<QVector3D>& vertexLimitCoords = currentMesh.getLimitCoords();
    QVector<QVector3D>& vertexLimitNormals = currentMesh.getLimitNorms();

    QVector<unsigned int>& bsplineTessPatchIndices  = currentMesh.getBsplineTessPatchIndices();

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexCoords.size(), vertexCoords.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshCoordsBO";

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexNormals.size(), vertexNormals.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshNormalsBO";

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitCoordsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexLimitCoords.size(), vertexLimitCoords.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshLimitCoordsBO";

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitNormalsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexLimitNormals.size(), vertexLimitNormals.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshLimitNormalsBO";

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*bsplineTessPatchIndices.size(), bsplineTessPatchIndices.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshIndexBO";

    meshIBOSize = bsplineTessPatchIndices.size();
}


void BsplineRenderer::draw(bool limit) {
    GLuint tessVao;
    if (limit) {
        tessVao = limitVao;
    } else {
        tessVao = vao;
    }
    if (settings->uniformUpdateRequired) {
        updateUniforms();
        settings->uniformUpdateRequired = false;
    }

    shaderProg.bind();
    shaderProg.setUniformValue("materialColour", 0.5, 0.5, 0.5);
    shaderProg.setUniformValue("sigma", settings->sigma);

    if (settings->wireframeMode) {
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        shaderProg.setUniformValue("approxFlatShading", false);
        shaderProg.setUniformValue("disableLighting", true);
        shaderProg.setUniformValue("analyticalNormals", false);
    } else {
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
        shaderProg.setUniformValue("approxFlatShading", settings->approxFlatShading);
        shaderProg.setUniformValue("analyticalNormals", settings->analyticalNormals);
        shaderProg.setUniformValue("disableLighting", false);

    }

    // Set all the tesselation levels.

    shaderProg.setUniformValue("tessInner0", settings->tessLevelInner0);
    shaderProg.setUniformValue("tessInner1", settings->tessLevelInner1);
    shaderProg.setUniformValue("tessOuter0", settings->tessLevelOuter0);
    shaderProg.setUniformValue("tessOuter1", settings->tessLevelOuter1);
    shaderProg.setUniformValue("tessOuter2", settings->tessLevelOuter2);
    shaderProg.setUniformValue("tessOuter3", settings->tessLevelOuter3);

    // Bind the tesselation vao and 'render' the control patches.

    gl->glBindVertexArray(tessVao);
    gl->glPatchParameteri(GL_PATCH_VERTICES, 16);
    gl->glDrawElements(GL_PATCHES, meshIBOSize, GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);
    shaderProg.release();

}

