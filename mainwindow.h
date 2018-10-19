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


public slots:

    void removePointController();


private slots:
    void on_pushButtonDraw_released();

    void on_sliderInk_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    int currentInkQ;
    int currentPressure;
    int currentSpeed;
};

#endif // MAINWINDOW_H
