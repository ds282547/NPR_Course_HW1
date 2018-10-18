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
    connect(ui->glwidget, SIGNAL(insertList(int,QString)), this , SLOT(insertList(int,QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPointController()
{
    ui->glwidget->startAddPointMode();
}

void MainWindow::insertList(int row, QString label)
{
    ui->listWidget->insertItem(row,label);
}

