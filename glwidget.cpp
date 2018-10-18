#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent), addPointMode(false)
{
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}


void GLWidget::initializeGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

}

void GLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPointSize(1);
    glLineWidth(1);
    bool firstPt = true;
    vec2 ptLast;

    for(vec2 &pt : pointControllers){
        if (firstPt){
            firstPt = false;
        } else {
            glColor4f(1,0,0,1);
            glBegin(GL_LINES);
                glVertex2iv(ptLast());
                glVertex2iv(pt());
            glEnd();
        }
        drawCircle(pt.x, pt.y , 4);
        ptLast = pt;
    }


}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width(),height(),0);
}

void GLWidget::addPointController(int x,int y)
{
    pointControllers.push_back({x,y});
    update();
}

void GLWidget::drawCircle(int x, int y, int r)
{
    const int r2 = r*r;
    const int ext = 2;
    const int re = (r+ext);
    const int re2 = (r+ext)*(r+ext);

    int i2,j2;
    GLfloat alpha;
    glBegin(GL_POINTS);
    for(int i=-re;i<=re;++i){
        i2 = i*i;
        for(int j=-re;j<=re;++j){
            j2 = j*j;
            if(i2+j2<=re2){
                float alpha = 1.0f - (float)(i2+j2-r2)/(re2-r2);
                alpha = alpha > 1.0 ? 1.0 : (alpha < 0 ? 0 : alpha);
                glColor4f(0,0,0,alpha);
                glVertex2i(x+i,y+j);
            }
        }
    }
    glEnd();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(addPointMode){
        addPointController(event->x(),event->y());
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void GLWidget::startAddPointMode()
{
    addPointMode = true;
}
