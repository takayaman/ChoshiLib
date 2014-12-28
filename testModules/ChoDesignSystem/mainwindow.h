#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionExit_triggered();

    void on_action2DWindow_triggered();

    void on_action3DWindow_triggered();

    void on_actionTreeWindow_triggered();

    void on_actionParameterWindow_triggered();

    void on_actionCrossSectionWindow_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
