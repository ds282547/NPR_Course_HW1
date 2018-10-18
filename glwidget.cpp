#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent), addPointMode(false)
{
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}

void GLWidget::debugMat3(GLWidget::mat3 m)
{
    qDebug() << m.m[0] << "," << m.m[1] << "," << m.m[2];
    qDebug() << m.m[3] << "," << m.m[4] << "," << m.m[5];
    qDebug() << m.m[6] << "," << m.m[7] << "," << m.m[8];
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


    if (pointControllers.size()>=2)
        for(auto it=pointControllers.begin()+1;it!=pointControllers.end();++it){
            glColor4f(0,0,0,1);
            glBegin(GL_LINES);
                //glVertex2iv((*(it-1))());
                //glVertex2iv((*(it))());
            glEnd();
        }
    drawSmoothCurve();

    for(vec2f &pt : pointControllers){
        drawCircle(static_cast<int>(pt.x), static_cast<int>(pt.y) , 2);
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
    static int index = 0;

    pointControllers.push_back({static_cast<GLfloat>(x),static_cast<GLfloat>(y)});
    emit insertList(index,QStringLiteral("Point %1").arg(index++));
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

void GLWidget::drawSmoothCurve()
{
    //refs: https://www.particleincell.com/2012/bezier-splines/
    if (pointControllers.size()<2) return;
    //
    QVector<vec2f> &K = pointControllers;
    int n = pointControllers.size()-1,i;
    QVector<vec2f> p1(n);
    QVector<vec2f> p2(n);
    QVector<GLfloat> a(n);
    QVector<GLfloat> b(n);
    QVector<GLfloat> c(n);
    QVector<GLfloat> r(n);

    /*left most segment*/
    a[0]=0;b[0]=2;c[0]=1;
    r[0]=K[0].x+2*K[1].x;
    /*internal segments*/
    for (i = 1; i < n - 1; i++)
    {
        a[i]=1;
        b[i]=4;
        c[i]=1;
        r[i] = 4 * K[i].x + 2 * K[i+1].x;
    }
    /*right segment*/
    a[n-1]=2;b[n-1]=7;c[n-1]=0;
    r[n-1] = 8*K[n-1].x+K[n].x;

    /*solves Ax=b with the Thomas algorithm (from Wikipedia)*/
    for (i = 1; i < n; i++)
    {
        GLfloat m = a[i]/b[i-1];
        b[i] = b[i] - m * c[i - 1];
        r[i] = r[i] - m *r[i-1];
    }

    p1[n-1].x = r[n-1]/b[n-1];

    for (i = n - 2; i >= 0; --i) {
        p1[i].x = (r[i] - c[i] * p1[i+1].x) / b[i];
    }
    for (i=0;i<n-1;i++)
        p2[i].x=2*K[i+1].x-p1[i+1].x;
        p2[n-1].x=0.5*(K[n].x+p1[n-1].x);

    // calc Y
    /*left most segment*/
    a[0]=0;b[0]=2;c[0]=1;
    r[0]=K[0].y+2*K[1].y;
    /*internal segments*/
    for (i = 1; i < n - 1; i++)
    {
        a[i]=1;
        b[i]=4;
        c[i]=1;
        r[i] = 4 * K[i].y + 2 * K[i+1].y;
    }
    /*right segment*/
    a[n-1]=2;b[n-1]=7;c[n-1]=0;
    r[n-1] = 8*K[n-1].y+K[n].y;
    /*solves Ax=b with the Thomas algorithm (from Wikipedia)*/
    for (i = 1; i < n; i++)
    {
        GLfloat m = a[i]/b[i-1];
        b[i] = b[i] - m * c[i - 1];
        r[i] = r[i] - m *r[i-1];
    }

    p1[n-1].y = r[n-1]/b[n-1];
    for (i = n - 2; i >= 0; --i)
        p1[i].y = (r[i] - c[i] * p1[i+1].y) / b[i];

    for (i=0;i<n-1;i++)
        p2[i].y=2*K[i+1].y-p1[i+1].y;
        p2[n-1].y=0.5*(K[n].y+p1[n-1].y);

    for (i=0;i<n;++i)
        drawCubicCurve(pointControllers[i],p1[i],p2[i],pointControllers[i+1]);

}

void GLWidget::drawCubicCurve(GLWidget::vec2f p0, GLWidget::vec2f p1, GLWidget::vec2f p2, GLWidget::vec2f p3)
{

    glBegin(GL_LINE_STRIP);
    GLfloat t,omt;
    vec2f d = p3-p0;
    d.absCoord();
    int step,i;
    if(d.x>d.y)
        step = d.x/2;
    else
        step = d.y/2;

    for(i = 0;i<=step;++i){
        t = (GLfloat)i/step;
        omt = 1.0f - t;
        vec2f B = p0*(omt*omt*omt)+p1*((t-t*t*2+t*t*t)*3)+p2*((t*t-t*t*t)*3)+p3*(t*t*t);
        glVertex2fv(B());
    }
    glEnd();
}

