#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //signals
    connect(ui->pushButtonClear, SIGNAL(released()), ui->glwidget , SLOT(clearScreen()));
    connect(ui->pushButtonAddPoint, SIGNAL(released()), this , SLOT(addPointController()));
    connect(ui->pushButtonRemovePoint, SIGNAL(released()), this , SLOT(removePointController()));

    ui->glwidget->setPointList(ui->pointListWidget);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPointController()
{
    ui->glwidget->startAddPointMode();
}

void MainWindow::removePointController()
{
    ui->glwidget->removePoint();
}



