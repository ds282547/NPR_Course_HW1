#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //signals
    connect(ui->pushButtonClear, SIGNAL(released()), ui->glwidget , SLOT(clearScreen()));
    connect(ui->pushButtonAddPoint, SIGNAL(toggled(bool)), ui->glwidget , SLOT(toogleAddPointMode(bool)));
    connect(ui->pushButtonRemovePoint, SIGNAL(released()), this , SLOT(removePointController()));

    ui->glwidget->setPointList(ui->pointListWidget);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::removePointController()
{
    ui->glwidget->removePoint();
}




void MainWindow::on_pushButtonDraw_released()
{
    static bool drawMode = false;
    drawMode = !drawMode;
    ui->pushButtonDraw->setText(drawMode?"Edit Point":"Draw");
    ui->glwidget->setDrawMode(drawMode);


}

void MainWindow::on_sliderInk_valueChanged(int value)
{

}
