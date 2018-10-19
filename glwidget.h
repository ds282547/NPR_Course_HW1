#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "GL/glut.h"
#include <QVector>
#include <QGridLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QDebug>
#include <QListWidget>
#include <QTimer>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:

    explicit GLWidget(QWidget *parent = 0);
    struct vec3f{
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat* operator ()(){
            return &x;
        }
        GLfloat operator [](int i){
            return *(&x+i);
        }
    };
    struct vec2;
    struct vec2f{
        GLfloat x;
        GLfloat y;
        GLfloat* operator ()(){
            return &x;
        }
        vec2f operator *(GLfloat t){
            return {x*t,y*t};
        }
        vec2f operator +(vec2f a){
            return {x+a.x,y+a.y};
        }
        vec2f operator -(vec2f &a){
            return {x-a.x,y-a.y};
        }
        vec2 conv2i(){
            return {(int)x,(int)y};
        }
        void absCoord(){
            if(x<0) x=-x;if(y<0) y=-y;
        }
        float angle(){
            return atan2f(y,-x);
        }
        float distance(){
            return sqrtf(x*x+y*y);
        }
    };

    /*friend vec2f operator+(GLWidget::vec2f &a,GLWidget::vec2f &b){
        return {a.x+b.x,a.y+b.y};
    }*/
    struct vec2{
        GLint x;
        GLint y;
        GLint* operator ()(){
            return &x;
        }
        vec2 operator +(vec2 &a){
            return {x+a.x,y+a.y};
        }
        vec2 operator -(vec2 &a){
            return {x-a.x,y-a.y};
        }
        void absCoord(){
            if(x<0) x=-x;if(y<0) y=-y;
        }
        vec2f conv2f(){
            return {static_cast<GLfloat>(x),static_cast<GLfloat>(y)};
        }
    };

    // 0 1 2
    // 3 4 5
    // 6 7 8
    struct mat3{
        GLfloat m[9];
        GLfloat det(){
            return m[0]*(m[4]*m[8]-m[5]*m[7])-m[1]*(m[3]*m[8]-m[5]*m[6])+m[2]*(m[3]*m[7]-m[4]*m[6]);
        }
        GLfloat& operator [](int i){
            return m[i];
        }
        GLfloat& cel(unsigned i,unsigned j) {return m[i*3+j];}
        mat3 inverse(){
           mat3 adj;

           adj.cel(0,0)=cel(1,1)*cel(2,2)-cel(1,2)*cel(2,1);
           adj.cel(0,1)=-(cel(0,1)*cel(2,2)-cel(0,2)*cel(2,1));
           adj.cel(0,2)=cel(0,1)*cel(1,2)-cel(0,2)*cel(1,1);
           adj.cel(1,0)=-(cel(1,0)*cel(2,2)-cel(1,2)*cel(2,0));
           adj.cel(1,1)=cel(0,0)*cel(2,2)-cel(0,2)*cel(2,0);
           adj.cel(1,2)=-(cel(0,0)*cel(1,2)-cel(0,2)*cel(1,0));
           adj.cel(2,0)=cel(1,0)*cel(2,1)-cel(1,1)*cel(2,0);
           adj.cel(2,1)=-(cel(0,0)*cel(2,1)-cel(0,1)*cel(2,0));
           adj.cel(2,2)=cel(0,0)*cel(1,1)-cel(0,1)*cel(1,0);

           adj/=det();
           return adj;
        }

        mat3& operator /=(GLfloat den){
            for(unsigned i=0;i<9;++i) m[i]/=den;
            return *this;
        }
        mat3 operator *(mat3& m){
            mat3 mul;
            for(unsigned i=0;i<3;++i)
                for(unsigned j=0;j<3;++j){
                    mul.cel(i,j) = 0;
                    for(unsigned k=0;k<3;++k)
                         mul.cel(i,j)=mul.cel(i,j) + cel(i,k)*m.cel(k,j);
                }
            return mul;
        }
    };
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void addPointController(int x, int y);
    void drawCircle(int x,int y,int r,bool sel);
    void drawRotatedEclipse(int x,int y,float a,float b,float expr,float rotAngle);
    void drawRotatedEclipseOutline(int x,int y,float a,float b,float rotAngle);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    void removePoint();

    void drawSmoothCurve();
    vec2f getDirectionOfCurve(int seg, float t);
    vec2f getPointOfCurve(int seg, float t);


    void drawCubicCurve(int seg, vec2f p0, vec2f p1, vec2f p2, vec2f p3);

    void setPointList(QListWidget *list);

    void setDrawMode(bool drawmode);
    void drawBrush();
    void drawEdit();

    const int currentInkQ = 80;
    const int currentPressure = 60;
    const int currentSpeed = 60;
signals:
public slots:
    void clearScreen();
    void listItemChange(int row);
    void toogleAddPointMode(bool sw);
protected:
    const int POINT_SIZE = 4;
    const int POINT_SEL_SIZE = 6;
protected:
    void debugMat3(mat3 m);
    QVector<vec2f> pointControllers;
    //modes
    bool addPointMode;
    bool drawMode;
    //selection
    QListWidget *pointList;
    QListWidgetItem *lastSelectedItem; // for list
    int selectedPoint; // for GL widget in screen
    //draw eclipse
    float getk(const float a, const float b, const float x, const float y);
    //drag
    vec2 dragOffset;
    bool dragingPoint;

    //point controller
    QVector<vec2f> p1;
    QVector<vec2f> p2;

    //argLength
    QVector< QVector<float> > isometricPoints;


};


#endif // GLWIDGET_H
