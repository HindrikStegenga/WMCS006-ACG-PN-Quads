#ifndef BSPLINERENDERER_H
#define BSPLINERENDERER_H

#include <QOpenGLShaderProgram>

#include "meshrenderer.h"

class BsplineRenderer : public MeshRenderer
{
public:

    BsplineRenderer();
    ~BsplineRenderer();
    void init(QOpenGLFunctions_4_1_Core* f, Settings* s) ;
    void initShaders();
    void updateBuffers(Mesh &currentMesh);
    void draw(bool limit);
};
#endif // MESHRENDERER_H
