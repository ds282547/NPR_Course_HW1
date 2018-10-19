#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent), addPointMode(false),
    selectedPoint(-1),
    dragingPoint(false),
    drawMode(false)
{
    /*
    QImage image(800,600,QImage::Format_RGBA8888);
    QPainter paint;
    paint.begin(&image);
    paint.setBrush(Qt::NoBrush);
    paint.setPen(Qt::red);
    paint.setFont(QFont("Arial",10));
    paint.drawText(100,100,"Fucker");
    paint.end();
    image.save("image.png");
    */
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}

void GLWidget::listItemChange(int row)
{
    if(row>=0){
        if(pointList->currentItem()->text()!=QString("<New Point>")){
            selectedPoint = row;
            update();
        }
    }

}

void GLWidget::toogleAddPointMode(bool sw)
{
    addPointMode = sw;
}



void GLWidget::debugMat3(GLWidget::mat3 m)
{
    qDebug() << m.m[0] << "," << m.m[1] << "," << m.m[2];
    qDebug() << m.m[3] << "," << m.m[4] << "," << m.m[5];
    qDebug() << m.m[6] << "," << m.m[7] << "," << m.m[8];
}

float GLWidget::getk(const float a,const float b,const float x,const float y)
{
    // Newton's Method
    float k = 50.0f,d;
    float f,fd;
    const float a2=a*a,b2=b*b,x2=x*x,y2=y*y;
    const float c3 = 2*a+2*b;
    const float c2 = a2+b2+4*a*b-x2-y2;
    const float c1 = 2*a*b2+2*a2*b-2*b*x2-2*a*y2;
    const float c0 = a2*b2-b2*x2-a2*y2;
    unsigned iter_count = 0;
    do{
        f = k*k*k*k +
            k*k*k*c3 +
            k*k*c2 +
            k*c1 +
            c0;
        fd = 4*k*k*k +
            3*k*k*c3 +
            2*k*c2 +
            c1;
        d = f/fd;
        k-=d;
        iter_count++;
    }while(d>0.01f && iter_count<100);
    if(iter_count==100) qDebug() << "Iter too much";
    return k;

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
    if (drawMode){
        drawBrush();
    } else{
        drawEdit();
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
    static bool first = true;

    QListWidgetItem *item = new QListWidgetItem(QStringLiteral("Point %1").arg(index));
    lastSelectedItem = pointList->currentItem();
    //
    if (first){
        pointControllers.push_back({static_cast<GLfloat>(x),static_cast<GLfloat>(y)});
        first = false;
    }else
        pointControllers.insert(pointList->currentRow(),{static_cast<GLfloat>(x),static_cast<GLfloat>(y)});

    //
    pointList->insertItem(pointList->currentRow(), item);
    pointList->setCurrentItem(lastSelectedItem);
    selectedPoint = pointList->currentRow();
    pointList->setFocus();


    index++;
    update();
}

void GLWidget::drawCircle(int x, int y, int r, bool sel)
{
    const int r2 = r*r;
    const int ext = 2;
    const int re = (r+ext);
    const int re2 = (r+ext)*(r+ext);
    const vec3f color_normal = {0,0,0};
    const vec3f color_sel = {1,0,0};
    vec3f color = sel ? color_sel : color_normal;

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
                glColor4f(color[0],color[1],color[2],alpha);
                glVertex2i(x+i,y+j);
            }
        }
    }
    glEnd();
}

void GLWidget::drawRotatedEclipse(int x, int y, float a, float b,float expri, float rotAngle)
{
    // equation x^2/a^2+y^2/b^2-(1/a^2-1/b^2)*sin(2*theta)*x*y = 1
    int edge = (int)((a>b?a:b)+expri+2);
    glBegin(GL_POINTS);
    vec3f color= {0,0,0};
    float a2=a*a,b2=b*b;
    float ae2=(a+expri)*(a+expri),be2=(b+expri)*(b+expri);
    float S = sinf(rotAngle),C = cosf(rotAngle);
    float xx;
    float yy;
    float d,de,e;
    GLfloat alpha = 1.0f;
    for(int i=-edge;i<=edge;++i){
        for(int j=-edge;j<=edge;++j){

            xx = C*i-S*j;
            yy = S*i+C*j;
            d = sqrtf(xx*xx/a2+yy*yy/b2);
            de = sqrtf(xx*xx/ae2+yy*yy/be2);
            if(d<=1.0f){
                alpha = 1.0f;
                glColor4f(color[0],color[1],color[2],alpha);
                glVertex2i(x+i,y+j);
            } else if(de<=1.0f){
                alpha = 1.0f-getk(a,b,xx,yy)/expri;
                glColor4f(color[0],color[1],color[2],alpha);
                glVertex2i(x+i,y+j);
            }
        }

    }
    glEnd();
}

void GLWidget::drawRotatedEclipseOutline(int x, int y, float a, float b, float rotAngle)
{
    int edge = (int)((a>b?a:b)+2);
    glBegin(GL_POINTS);
    vec3f color= {0,0,0};
    float a2=a*a,b2=b*b;
    float S = sinf(rotAngle),C = cosf(rotAngle);
    float xx;
    float yy;
    float d;
    float border=0.8f/edge;

    GLfloat alpha = 1.0f;
    for(int i=-edge;i<=edge;++i){
        for(int j=-edge;j<=edge;++j){

            xx = C*i-S*j;
            yy = S*i+C*j;
            d = sqrtf(xx*xx/a2+yy*yy/b2);
            if(d<=1.0f+border && d>=1.0f-border){
                alpha = 1.0f;
                glColor4f(color[0],color[1],color[2],alpha);
                glVertex2i(x+i,y+j);
            }
        }

    }
    glEnd();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(drawMode) return;
    if (dragingPoint){
        vec2 mouse = {event->x(),event->y()};
        pointControllers[selectedPoint] = mouse.conv2f();
        update();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(drawMode) return;
    if(addPointMode && event->button()==Qt::RightButton){
        addPointController(event->x(),event->y());
    }
    if(event->button()==Qt::LeftButton){
        // select point
        vec2 mouse = {event->x(),event->y()};
        for(int i=0;i<pointControllers.size();++i){
            vec2 pt = pointControllers[i].conv2i();
            vec2 d = mouse - pt;
            if(d.x*d.x+d.y*d.y<POINT_SEL_SIZE*POINT_SEL_SIZE){
                selectedPoint = i;
                pointList->setCurrentRow(selectedPoint);
                pointList->setFocus();
                update();
                // start draging
                dragOffset = d;
                dragingPoint = true;
                break;
            }
        }

    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(drawMode) return;
    dragingPoint = false;
}



void GLWidget::removePoint()
{

    if (pointList->selectedItems().at(0)->text()==QString("<New Point>"))
        return;
    qDeleteAll(pointList->selectedItems());
    pointList->setFocus();
}

void GLWidget::drawSmoothCurve()
{
    //refs: https://www.particleincell.com/2012/bezier-splines/
    glColor4f(0,0,0,1);
    if (pointControllers.size()==2) {
        p1.push_back(pointControllers[0]);
        p2.push_back(pointControllers[1]);
        drawCubicCurve(0,pointControllers[0],pointControllers[0],pointControllers[1],pointControllers[1]);
        return;
    }


    //
    QVector<vec2f> &K = pointControllers;
    int n = pointControllers.size()-1,i;

    p1.resize(n);

    p2.resize(n);
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
        drawCubicCurve(i,pointControllers[i],p1[i],p2[i],pointControllers[i+1]);

}

GLWidget::vec2f GLWidget::getDirectionOfCurve(int seg, float t)
{
    float omt = 1.0f - t;

    vec2f &_p0 = pointControllers[seg];
    vec2f &_p1 = p1[seg];
    vec2f &_p2 = p2[seg];
    vec2f &_p3 = pointControllers[seg+1];
    return _p0*(-3.0f*omt*omt)+_p1*((1-t*4+t*t*3)*3)+_p2*((t*2-t*t*3)*3)+_p3*(t*t*3);
}

GLWidget::vec2f GLWidget::getPointOfCurve(int seg, float t)
{
    float omt = 1.0f - t;

    vec2f &_p0 = pointControllers[seg];
    vec2f &_p1 = p1[seg];
    vec2f &_p2 = p2[seg];
    vec2f &_p3 = pointControllers[seg+1];
    return _p0*(omt*omt*omt)+_p1*((t-t*t*2+t*t*t)*3)+_p2*((t*t-t*t*t)*3)+_p3*(t*t*t);
}


void GLWidget::drawCubicCurve(int seg,GLWidget::vec2f p0, GLWidget::vec2f p1, GLWidget::vec2f p2, GLWidget::vec2f p3)
{

    vec2f d = p3-p0;
    float arcLength = 0;

    d.absCoord();
    int step,i;
    if(d.x>d.y)
        step = d.x;
    else
        step = d.y;

    vec2f B,B_prev;

    float t_offset = 1.0f/step; // initial t offset for testing
    float tt,t,omt,dis;
    const float target_dis = 1.0f;
    const float THRESHOLD = 0.1f;

    glBegin(GL_LINE_STRIP);

    B_prev = B = p0;
    tt = 0;
    glVertex2fv(B());
    //find isometric point for length=1
    unsigned isopoint_counter = 0;
    isometricPoints[seg].clear();
    while(tt<=1.0f){
        while(true){
            t = tt+t_offset;
            omt = 1.0f - t;
            B = p0*(omt*omt*omt)+p1*((t-t*t*2+t*t*t)*3)+p2*((t*t-t*t*t)*3)+p3*(t*t*t);
            dis = (B - B_prev).distance();
            if(fabsf(dis-target_dis)>THRESHOLD){
                t_offset *= target_dis/dis;
            } else
                break;
        }
        tt = t;
        isometricPoints[seg].push_back(tt);
        B_prev = B;
        glVertex2fv(B());
    }
    qDebug()<<isometricPoints[seg].size();
    glEnd();
}

void GLWidget::setPointList(QListWidget *list)
{
    pointList = list;

    QListWidgetItem *item = new QListWidgetItem("<New Point>");
    pointList->addItem(item);
    pointList->setCurrentRow(0);
    connect(pointList,SIGNAL(currentRowChanged(int)),this,SLOT(listItemChange(int)));
}

void GLWidget::setDrawMode(bool drawmode)
{
    drawMode = drawmode;
    update();
}

void GLWidget::drawBrush()
{
    const float a=1,b=5,expr=15;
    int step;
    int feq=8;
    float t;
    for(int i=0;i<pointControllers.size()-1;++i){

        for(int j=0;j<isometricPoints[i].size();j+=feq){
            t=isometricPoints[i][j];
            vec2f dir = getDirectionOfCurve(i,t);
            vec2f pt = getPointOfCurve(i,t);
            drawRotatedEclipse(pt.x,pt.y,a,b,expr,dir.angle());
        }
    }
}

void GLWidget::drawEdit()
{
    glPointSize(1);
    glLineWidth(1);


    if (pointControllers.size()>=2){
        isometricPoints.resize( pointControllers.size()-1 );
        drawSmoothCurve();
    }

    for(int i=0;i<pointControllers.size();++i){
        vec2f &ptc = pointControllers[i];
        drawCircle(static_cast <int>(ptc.x),
                   static_cast<int>(ptc.y), POINT_SIZE, i==selectedPoint);
    }
}

