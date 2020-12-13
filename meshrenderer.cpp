#include "meshrenderer.h"
#include <iostream>

MeshRenderer::MeshRenderer()
{
    meshIBOSize = 0;
}

MeshRenderer::~MeshRenderer() {
    gl->glDeleteVertexArrays(1, &vao);
    gl->glDeleteVertexArrays(1, &limitVao);
    gl->glDeleteVertexArrays(1, &bsplineTessRegularVao);
    gl->glDeleteVertexArrays(1, &bsplineTessLimitVao);

    gl->glDeleteBuffers(1, &meshLimitCoordsBO);
    gl->glDeleteBuffers(1, &meshLimitNormalsBO);

    gl->glDeleteBuffers(1, &bsplineTessIndexBO);

    gl->glDeleteBuffers(1, &meshCoordsBO);
    gl->glDeleteBuffers(1, &meshNormalsBO);
    gl->glDeleteBuffers(1, &meshIndexBO);
}

void MeshRenderer::init(QOpenGLFunctions_4_1_Core* f, Settings* s) {
    gl = f;
    settings = s;

    initShaders();
    initBuffers();
}

void MeshRenderer::initShaders() {
    shaderProg.create();
    shaderProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shaderProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shaderProg.link();

    uniModelViewMatrix = gl->glGetUniformLocation(shaderProg.programId(), "modelviewmatrix");
    uniProjectionMatrix = gl->glGetUniformLocation(shaderProg.programId(), "projectionmatrix");
    uniNormalMatrix = gl->glGetUniformLocation(shaderProg.programId(), "normalmatrix");



    bsplineTessShaderProg.create();
    bsplineTessShaderProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tessVertShader.glsl");
    bsplineTessShaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shaders/bsplineControlShader.glsl");
    bsplineTessShaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shaders/bsplineEvaluationShader.glsl");
    bsplineTessShaderProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    bsplineTessShaderProg.link();

    uniModelViewMatrixBsplineTess = gl->glGetUniformLocation(bsplineTessShaderProg.programId(), "modelviewmatrix");
    uniProjectionMatrixBsplineTess = gl->glGetUniformLocation(bsplineTessShaderProg.programId(), "projectionmatrix");
    uniNormalMatrixBsplineTess = gl->glGetUniformLocation(bsplineTessShaderProg.programId(), "normalmatrix");



    pnQuadTessShaderProg.create();
    pnQuadTessShaderProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tessVertShader.glsl");
    pnQuadTessShaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shaders/bsplineControlShader.glsl");
    pnQuadTessShaderProg.addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shaders/bsplineEvaluationShader.glsl");
    pnQuadTessShaderProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    pnQuadTessShaderProg.link();

    uniModelViewMatrixPnQuadTess = gl->glGetUniformLocation(pnQuadTessShaderProg.programId(), "modelviewmatrix");
    uniProjectionMatrixPnQuadTess = gl->glGetUniformLocation(pnQuadTessShaderProg.programId(), "projectionmatrix");
    uniNormalMatrixPnQuadTess = gl->glGetUniformLocation(pnQuadTessShaderProg.programId(), "normalmatrix");


}

void MeshRenderer::initBuffers() {

    // Regular drawing
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);

    gl->glGenBuffers(1, &meshCoordsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glGenBuffers(1, &meshNormalsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glGenBuffers(1, &meshIndexBO);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);

    gl->glBindVertexArray(0);

    // Limit surface variant
    gl->glGenVertexArrays(1, &limitVao);
    gl->glBindVertexArray(limitVao);

    gl->glGenBuffers(1, &meshLimitCoordsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitCoordsBO);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glGenBuffers(1, &meshLimitNormalsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitNormalsBO);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);

    gl->glBindVertexArray(0);

    // Tesselated drawing (non limit surface)
    gl->glGenVertexArrays(1, &bsplineTessRegularVao);
    gl->glBindVertexArray(bsplineTessRegularVao);

    // We can reuse buffers for tesselation.
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // We do need a new index buffer however.
    gl->glGenBuffers(1, &bsplineTessIndexBO);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsplineTessIndexBO);

    gl->glBindVertexArray(0);


    // Tesselated drawing (limit surface)
    gl->glGenVertexArrays(1, &bsplineTessLimitVao);
    gl->glBindVertexArray(bsplineTessLimitVao);

    // We can reuse buffers for tesselation.
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitCoordsBO);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshLimitNormalsBO);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsplineTessIndexBO);

    gl->glBindVertexArray(0);

}

void MeshRenderer::updateBuffers(Mesh& currentMesh) {

    qDebug() << ".. updateBuffers";

    //gather attributes for current mesh
    currentMesh.extractAttributes();

    QVector<QVector3D>& vertexCoords = currentMesh.getVertexCoords();
    QVector<QVector3D>& vertexNormals = currentMesh.getVertexNorms();
    QVector<unsigned int>& polyIndices = currentMesh.getPolyIndices();

    QVector<QVector3D>& vertexLimitCoords = currentMesh.getLimitCoords();
    QVector<QVector3D>& vertexLimitNormals = currentMesh.getLimitNorms();

    QVector<unsigned int>& tessPatchIndices = currentMesh.getTessPatchIndices();


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
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*polyIndices.size(), polyIndices.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated meshIndexBO";

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsplineTessIndexBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*tessPatchIndices.size(), tessPatchIndices.data(), GL_DYNAMIC_DRAW);

    qDebug() << " → Updated tessIndexBO";

    bsplinePatchCount = tessPatchIndices.size();
    meshIBOSize = polyIndices.size();
}

void MeshRenderer::updateUniforms() {

    shaderProg.bind();
    gl->glUniformMatrix4fv(uniModelViewMatrix, 1, false, settings->modelViewMatrix.data());
    gl->glUniformMatrix4fv(uniProjectionMatrix, 1, false, settings->projectionMatrix.data());
    gl->glUniformMatrix3fv(uniNormalMatrix, 1, false, settings->normalMatrix.data());
    shaderProg.release();

    bsplineTessShaderProg.bind();
    gl->glUniformMatrix4fv(uniModelViewMatrixBsplineTess, 1, false, settings->modelViewMatrix.data());
    gl->glUniformMatrix4fv(uniProjectionMatrixBsplineTess, 1, false, settings->projectionMatrix.data());
    gl->glUniformMatrix3fv(uniNormalMatrixBsplineTess, 1, false, settings->normalMatrix.data());

    bsplineTessShaderProg.release();

    pnQuadTessShaderProg.bind();
    gl->glUniformMatrix4fv(uniModelViewMatrixPnQuadTess, 1, false, settings->modelViewMatrix.data());
    gl->glUniformMatrix4fv(uniProjectionMatrixPnQuadTess, 1, false, settings->projectionMatrix.data());
    gl->glUniformMatrix3fv(uniNormalMatrixPnQuadTess, 1, false, settings->normalMatrix.data());

    pnQuadTessShaderProg.release();
}

void MeshRenderer::draw() {
    if (settings->uniformUpdateRequired) {
        updateUniforms();
        settings->uniformUpdateRequired = false;
    }

    switch (settings->renderingMode) {
        case 0:
            regularDraw();
        break;
        case 1:
            limitDraw();
        break;
        case 2:
            tesselatedBsplineDraw(bsplineTessRegularVao);
        break;
        case 3:
            tesselatedBsplineDraw(bsplineTessLimitVao);
        break;
        case 4:
            tesselatedPnQuadDraw(pnQuadTessRegularVao);
        break;
        case 5:
            tesselatedPnQuadDraw(pnQuadTessLimitVao);
        break;
        default:
        break;
    }
}

void MeshRenderer::regularDraw() {

    shaderProg.bind();
    //enable primitive restart
    gl->glEnable(GL_PRIMITIVE_RESTART);
    gl->glPrimitiveRestartIndex(INT_MAX);

    shaderProg.setUniformValue("materialColour", 0.53, 0.80, 0.87);

    gl->glBindVertexArray(vao);

    if (settings->wireframeMode) {
        shaderProg.setUniformValue("approxFlatShading", false);
        shaderProg.setUniformValue("disableLighting", true);

        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        gl->glDrawElements(GL_LINE_LOOP, meshIBOSize, GL_UNSIGNED_INT, 0);
    } else {
        shaderProg.setUniformValue("approxFlatShading", settings->approxFlatShading);
        shaderProg.setUniformValue("disableLighting", false);

        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        gl->glDrawElements(GL_TRIANGLE_FAN, meshIBOSize, GL_UNSIGNED_INT, 0);
    }

    gl->glBindVertexArray(0);

    shaderProg.release();

    //disable it again as you might want to draw something else at some point
    gl->glDisable(GL_PRIMITIVE_RESTART);
}

void MeshRenderer::limitDraw() {
    shaderProg.bind();
    //enable primitive restart
    gl->glEnable(GL_PRIMITIVE_RESTART);
    gl->glPrimitiveRestartIndex(INT_MAX);


    // Draw limit surface.
    shaderProg.setUniformValue("materialColour", 0.5, 0.5, 0.5);

    // Check wireframe mode.
    if(settings->wireframeMode) {
        shaderProg.setUniformValue("approxFlatShading", false);
        shaderProg.setUniformValue("disableLighting", true);

        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        gl->glBindVertexArray(limitVao);
        gl->glDrawElements(GL_LINE_LOOP, meshIBOSize, GL_UNSIGNED_INT, 0);
        gl->glBindVertexArray(0);
    } else {
        shaderProg.setUniformValue("disableLighting", false);
        shaderProg.setUniformValue("approxFlatShading", settings->approxFlatShading);
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
        gl->glBindVertexArray(limitVao);
        gl->glDrawElements(GL_TRIANGLE_FAN, meshIBOSize, GL_UNSIGNED_INT, 0);
        gl->glBindVertexArray(0);
    }

    // Draw regular variant
    shaderProg.setUniformValue("materialColour", 0.0, 0.0, 1.0);
    shaderProg.setUniformValue("approxFlatShading", false);
    shaderProg.setUniformValue("disableLighting", true);
    gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    gl->glBindVertexArray(vao);
    gl->glDrawElements(GL_LINE_LOOP, meshIBOSize, GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);

    shaderProg.release();

    //disable it again as you might want to draw something else at some point
    gl->glDisable(GL_PRIMITIVE_RESTART);
}

void MeshRenderer::tesselatedBsplineDraw(GLuint tessVao) {

    bsplineTessShaderProg.bind();
    bsplineTessShaderProg.setUniformValue("materialColour", 0.5, 0.5, 0.5);

    if (settings->wireframeMode) {
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        bsplineTessShaderProg.setUniformValue("approxFlatShading", false);
        bsplineTessShaderProg.setUniformValue("disableLighting", true);
        bsplineTessShaderProg.setUniformValue("analyticalNormals", false);
    } else {
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
        bsplineTessShaderProg.setUniformValue("approxFlatShading", settings->approxFlatShading);
        bsplineTessShaderProg.setUniformValue("analyticalNormals", settings->analyticalNormals);
        bsplineTessShaderProg.setUniformValue("disableLighting", false);
    }

    // Set all the tesselation levels.

    bsplineTessShaderProg.setUniformValue("tessInner0", settings->tessLevelInner0);
    bsplineTessShaderProg.setUniformValue("tessInner1", settings->tessLevelInner1);
    bsplineTessShaderProg.setUniformValue("tessOuter0", settings->tessLevelOuter0);
    bsplineTessShaderProg.setUniformValue("tessOuter1", settings->tessLevelOuter1);
    bsplineTessShaderProg.setUniformValue("tessOuter2", settings->tessLevelOuter2);
    bsplineTessShaderProg.setUniformValue("tessOuter3", settings->tessLevelOuter3);

    // Bind the tesselation vao and 'render' the control patches.

    gl->glBindVertexArray(tessVao);
    gl->glPatchParameteri(GL_PATCH_VERTICES, 16);
    gl->glDrawElements(GL_PATCHES, bsplinePatchCount, GL_UNSIGNED_INT, 0);

    gl->glBindVertexArray(0);
    bsplineTessShaderProg.release();


    if (settings->showNonTesselatedWireframe) {

        shaderProg.bind();
        //enable primitive restart
        gl->glEnable(GL_PRIMITIVE_RESTART);
        gl->glPrimitiveRestartIndex(INT_MAX);

        // Draw regular variant
        shaderProg.setUniformValue("materialColour", 0.0, 0.0, 1.0);
        shaderProg.setUniformValue("approxFlatShading", false);
        shaderProg.setUniformValue("disableLighting", true);
        gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        gl->glBindVertexArray(vao);
        gl->glDrawElements(GL_LINE_LOOP, meshIBOSize, GL_UNSIGNED_INT, 0);
        gl->glBindVertexArray(0);

        shaderProg.release();

        //disable it again as you might want to draw something else at some point
        gl->glDisable(GL_PRIMITIVE_RESTART);
    }
}

void MeshRenderer::tesselatedPnQuadDraw(GLuint tessVao) {

}

