#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>

#include <QVector3D>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

  Q_OBJECT

public:
  MainView(QWidget *parent = 0);
  ~MainView();

  QVector3D convertHSLtoRGB(float H, float S, float L);
  void updateBuffers(unsigned short n);
  void clearArrays();

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

private:
  QOpenGLDebugLogger* debugLogger;

  QOpenGLShaderProgram* mainShaderProg;
  GLuint fanVAO, fanCoordsBO, fanColourBO, fanIndexBO;

  void createShaderPrograms();
  void createBuffers();
  void updateMatrices();
  void updateUniforms();

  bool updateUniformsRequired;

  QVector<QVector2D> triaCoords;
  QVector<QVector3D> triaColours;
  QVector<unsigned short> triaIndices;

  GLint uniModelViewMatrix, uniProjectionMatrix;
  QMatrix4x4 modelViewMatrix, projectionMatrix;

private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
