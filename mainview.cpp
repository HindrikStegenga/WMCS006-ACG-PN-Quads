#include "mainview.h"
#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {
  debugLogger->stopLogging();
  clearArrays();

  glDeleteBuffers(1, &fanCoordsBO);
  glDeleteBuffers(1, &fanColourBO);
  glDeleteBuffers(1, &fanIndexBO);
  glDeleteVertexArrays(1, &fanVAO);

  delete mainShaderProg;
  delete debugLogger;
  qDebug() << "✗✗ MainView destructor";
}

// ---

void MainView::createShaderPrograms() {

  // Qt approach
  mainShaderProg = new QOpenGLShaderProgram();
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

  mainShaderProg->link();

  // Qt wrappers
  // uniModelViewMatrix = mainShaderProg->uniformLocation("modelviewmatrix");
  // uniProjectionMatrix = mainShaderProg->uniformLocation("projectionmatrix");

  // Pure OpenGL
  uniModelViewMatrix = glGetUniformLocation(mainShaderProg->programId(), "modelviewmatrix");
  uniProjectionMatrix = glGetUniformLocation(mainShaderProg->programId(), "projectionmatrix");
}

void MainView::createBuffers() {

  // Pure OpenGL
  glGenVertexArrays(1, &fanVAO);
  glBindVertexArray(fanVAO);

  glGenBuffers(1, &fanCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, fanCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fanColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, fanColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fanIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fanIndexBO);

  glBindVertexArray(0);
}

void MainView::updateBuffers(unsigned short n) {
  unsigned short k;
  float length = 0.6f;
  float tau = 6.283185f;

  clearArrays();

  triaCoords.reserve(n+1);
  triaColours.reserve(n+1);
  triaIndices.reserve(n+2);

  triaCoords.append( QVector2D(0.0, 0.0) );
  triaColours.append( convertHSLtoRGB(0.0, 1.0, 1.0) );
  triaIndices.append(0);

  for (k=0; k<n; k++) {
    triaCoords.append( QVector2D(length*cos(tau * (float)k/n), length*sin(tau * (float)k/n)) );
    triaColours.append( convertHSLtoRGB( (float)k/n, 0.8, 0.5 ) );

    triaIndices.append(k+1);
  }

  triaIndices.append(1);

  glBindBuffer(GL_ARRAY_BUFFER, fanCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, fanColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaColours.size(), triaColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fanIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();

  projectionMatrix.setToIdentity();
  projectionMatrix.perspective(80.0, (float)width()/height(), 0.2, 2.0);

  updateUniformsRequired = true;
}

void MainView::updateUniforms() {

  // Qt wrappers
  // mainShaderProg->setUniformValue(uniModelViewMatrix, modelViewMatrix);
  // mainShaderProg->setUniformValue(uniProjectionMatrix, projectionMatrix);

  // Pure OpenGL
  glUniformMatrix4fv(uniModelViewMatrix, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjectionMatrix, 1, false, projectionMatrix.data());

  updateUniformsRequired = false;
}

void MainView::clearArrays() {
  triaCoords.clear();
  triaCoords.squeeze();
  triaColours.clear();
  triaColours.squeeze();
  triaIndices.clear();
  triaIndices.squeeze();
}

// ---

QVector3D MainView::convertHSLtoRGB(float H, float S, float L) {

  float C, Hp, X, m;
  QVector3D preRGB;

  // Grey values, S=0.0, so C=0.0
  C = (1.0 - abs(2.0*L - 1.0)) * S;
  m = L - (C / 2.0);
  Hp = (H * 360.0) / 60.0;
  X = C * (1.0 - abs(fmod(Hp, 2.0) - 1.0));

  if (Hp > 5.0) {
    preRGB = QVector3D(C,0.0,X);
  } else if (Hp > 4.0) {
    preRGB = QVector3D(X,0.0,C);
  } else if (Hp > 3.0) {
    preRGB = QVector3D(0.0,X,C);
  } else if (Hp > 2.0) {
    preRGB = QVector3D(0.0,C,X);
  } else if (Hp > 1.0) {
    preRGB = QVector3D(X,C,0.0);
  } else if (Hp >= 0.0) {
    preRGB = QVector3D(C,X,0.0);
  }

  return preRGB + QVector3D(m,m,m);
}

// ---

void MainView::initializeGL() {

  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  debugLogger = new QOpenGLDebugLogger();
  connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

  if ( debugLogger->initialize() ) {
    qDebug() << ":: Logging initialized";
    debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
    debugLogger->enableMessages();
  }

  updateUniformsRequired = true;

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  glPointSize(16);

  // ---

  createShaderPrograms();
  createBuffers();
  updateBuffers(6);
  updateMatrices();
}

void MainView::resizeGL(int newWidth, int newHeight) {
  updateMatrices();
}

void MainView::paintGL() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mainShaderProg->bind();

  if (updateUniformsRequired) {
    updateUniforms();
  }

  glBindVertexArray(fanVAO);

  glDrawElements(GL_TRIANGLE_FAN, triaIndices.size(), GL_UNSIGNED_SHORT, nullptr);
  glDrawElements(GL_POINTS, triaIndices.size()-2, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(GLushort)));

  glBindVertexArray(0);

  mainShaderProg->release();
}

// ---

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
