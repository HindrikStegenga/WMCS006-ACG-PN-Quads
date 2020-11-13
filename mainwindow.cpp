#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  qDebug() << "✓✓ MainWindow constructor";
}

MainWindow::~MainWindow() {
  delete ui;
  qDebug() << "✗✗ MainWindow destructor";
}

void MainWindow::on_ngonSlider_valueChanged(int value) {
  ui->ngonValue->setNum(value);
  ui->mainView->updateBuffers(value);
  ui->mainView->update();
}
