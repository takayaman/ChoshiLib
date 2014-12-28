#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::on_actionNew_triggered() {

}

void MainWindow::on_actionOpen_triggered() {

}

void MainWindow::on_actionSave_triggered() {

}

void MainWindow::on_actionSaveAs_triggered() {

}

void MainWindow::on_actionExit_triggered() {

}

void MainWindow::on_action2DWindow_triggered() {

}

void MainWindow::on_action3DWindow_triggered() {

}

void MainWindow::on_actionTreeWindow_triggered() {

}

void MainWindow::on_actionParameterWindow_triggered() {

}

void MainWindow::on_actionCrossSectionWindow_triggered() {

}
