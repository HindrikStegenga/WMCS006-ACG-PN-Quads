#include "mainview.h"
#include "math.h"
#include <QLoggingCategory>

MainView::MainView(QWidget *Parent) : QOpenGLWidget(Parent) {
    qDebug() << "✓✓ MainView constructor";

    scale = 1.0f;
}

MainView::~MainView() {
    qDebug() << "✗✗ MainView destructor";
    makeCurrent();
}

void MainView::initializeGL() {

    initializeOpenGLFunctions();
    qDebug() << ":: OpenGL initialized";

    connect(&debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger.initialize() ) {
        QLoggingCategory::setFilterRules("qt.*=false\n"
                                         "qt.text.font.*=false");

        qDebug() << ":: Logging initialized";
        debugLogger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger.enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    makeCurrent();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    //grab the opengl context
    QOpenGLFunctions_4_1_Core *functions = this->context()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    // initialize renderers here with the current context
    mr.init(functions, &settings);
    pqr.init(functions, &settings);
    br.init(functions, &settings);

    updateMatrices();
}

void MainView::resizeGL(int newWidth, int newHeight) {
    qDebug() << ".. resizeGL";

    settings.dispRatio = float(newWidth)/float(newHeight);

    settings.projectionMatrix.setToIdentity();
    settings.projectionMatrix.perspective(settings.FoV, settings.dispRatio, 0.1f, 40.0f);
    updateMatrices();

}

void MainView::updateMatrices() {

    settings.modelViewMatrix.setToIdentity();
    settings.modelViewMatrix.translate(QVector3D(0.0, 0.0, -3.0));
    settings.modelViewMatrix.scale(scale);
    settings.modelViewMatrix.rotate(rotationQuaternion);

    settings.normalMatrix = settings.modelViewMatrix.normalMatrix();

    settings.uniformUpdateRequired = true;

    update();
}

void MainView::updateBuffers(Mesh &currentMesh) {
    mr.updateBuffers(currentMesh);
    pqr.updateBuffers(currentMesh);
    br.updateBuffers(currentMesh);
}


void MainView::paintGL() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (settings.wireframeMode) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }


    if (settings.modelLoaded) {
        switch (settings.renderingMode) {
            case 0:
                mr.draw(false, false);
            break;
            case 1:
                mr.draw(true, false);
            break;
            case 2:
                br.draw(false);
            break;
            case 3:
                br.draw(true);
            break;
            case 4:
                pqr.draw(false);
            break;
            case 5:
                pqr.draw(true);
            break;
            default:
            break;
        }
        if (settings.showNonTesselatedWireframe) {
            settings.uniformUpdateRequired = true;
            mr.draw(false, true);
        }
    }
}

QVector2D MainView::toNormalizedScreenCoordinates(int x, int y) {
    float xRatio, yRatio;
    float xScene, yScene;

    xRatio = float(x)/float(width());
    yRatio = float(y)/float(height());

    xScene = (1-xRatio)*-1 + xRatio*1;
    yScene = yRatio*-1 + (1-yRatio)*1;

    return {xScene, yScene};
}


void MainView::mouseMoveEvent(QMouseEvent* Event) {
    if(Event->buttons() == Qt::LeftButton) {
        QVector2D sPos = toNormalizedScreenCoordinates(Event->x(), Event->y());
        QVector3D newVec = QVector3D(sPos.x(), sPos.y(), 0.0);

        //project onto sphere
        float sqrZ = 1.0 - QVector3D::dotProduct(newVec, newVec);
        if (sqrZ > 0) {
            newVec.setZ(sqrt(sqrZ));
        } else {
            newVec.normalize();
        }

        QVector3D v2 = newVec.normalized();
        // reset if we are starting a drag
        if(!dragging) {
            dragging = true;
            oldVec = newVec;
            return;
        }

        // calculate axis and angle
        QVector3D v1 = oldVec.normalized();
        QVector3D N = QVector3D::crossProduct(v1, v2).normalized();
        if(N.length() == 0.0) {
            oldVec = newVec;
            return;
        }
        float angle = 180.0/M_PI * acos(QVector3D::dotProduct(v1, v2));
        rotationQuaternion = QQuaternion::fromAxisAndAngle(N, angle) * rotationQuaternion;
        updateMatrices();

        // for next iteration
        oldVec = newVec;
    } else {
        // to reset drag
        dragging = false;
        oldVec = QVector3D();
    }
}

void MainView::mousePressEvent(QMouseEvent* event) {
    setFocus();
}

void MainView::wheelEvent(QWheelEvent* event) {
    float Phi;
    // Delta is usually 120
    Phi = 1.0f + (event->delta()/2000.0f);

    scale = fmin(fmax(Phi * scale, 0.01f), 100.0f);
    updateMatrices();
}

void MainView::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case 'Z':
        settings.wireframeMode = !settings.wireframeMode;
        update();
        break;
    }
}

// ---

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
