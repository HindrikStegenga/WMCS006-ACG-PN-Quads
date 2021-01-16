#ifndef PNQUADRENDERER_H
#define PNQUADRENDERER_H

#include <QOpenGLShaderProgram>

#include "meshrenderer.h"

class PnQuadRenderer : public MeshRenderer
{
public:

    PnQuadRenderer();
    ~PnQuadRenderer();
    void init(QOpenGLFunctions_4_1_Core* f, Settings* s) ;
    void initShaders();
    void updateBuffers(Mesh &currentMesh);
    void draw(bool limit);
};
#endif // MESHRENDERER_H
