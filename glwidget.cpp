#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}


void GLWidget::initializeGL()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

}

void GLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPointSize(1);
    glColor3f(1,0,0);


    glBegin(GL_TRIANGLES);// 開始劃三角形
    glColor3f(1.0f, 0.0f, 0.0f);// 設定輸出色為紅色
    glVertex2f(0, 0 );//(x1,y1)=(0, 1)
    glColor3f(0.0f, 1.0f, 0.0f);// 設定輸出色為綠色
    glVertex2f(0, 40);//(x2,y2)=(1.0,-0.5)
    glColor3f(0.0f, 0.0f, 1.0f);// 設定輸出色為藍色
    glVertex2f(40, 0);//(x3,y3)=(-1.0,-0.5)
    glEnd();// 結束劃三角形
    for(vec2 &pt : pointControllers){
        glVertex2iv(pt());
    }

    drawCircle(200,50,10);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width(),0, height());
}

void GLWidget::addPointController()
{
    pointControllers.push_back({20,40});
    paintGL();
}

void GLWidget::drawCircle(int x, int y, int r)
{
    const int r2 = r*r;
    glBegin(GL_POINTS);
    for(int i=-r;i<=r;++i)
        for(int j=-r;j<=r;++j)
            if(i*i+j*j<r2){
                float l=1.0f-sqrtf((float)((i-2)*(i-2)+(j-2)*(j-2)))/r;
                if(l<0) l=0;
                glColor3f(l,l,l);
                glVertex2i(x+i,y+j);
            }



    glEnd();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

}
