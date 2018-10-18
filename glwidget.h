#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "GL/glut.h"
#include <QVector>
#include <QGridLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QDebug>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void addPointController();
    void drawCircle(int x,int y,int r);

    void mouseMoveEvent(QMouseEvent *event);
public slots:
    void clearScreen();
protected:
    struct vec2{
        GLint x;
        GLint y;
        GLint* operator ()(){
            return &x;
        }
    };
    QVector<vec2> pointControllers;
};

#endif // GLWIDGET_H
