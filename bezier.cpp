#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <time.h>
using namespace std;
#include <cstdlib>
#include <GLUT/glut.h>
//#include <graphics.h> 

GLfloat redMaterial[] = {.7, .1, .2, 1};
GLfloat greenMaterial[] = {.1, .7, .4, 1};
GLfloat brightGreenMaterial[] = {.1, .9, .1, 1};
GLfloat blueMaterial[] = {.1, .2, .7, 1};
GLfloat grayMaterial[] = {.3, .3, .3, 1};
GLfloat pinkMaterial[] = {1, .8, .8, 1};
GLfloat yellowMaterial[] = {.9, .9, 0, 1};
GLfloat purpleMaterial[] = {.9, 0, 1, 1};
GLfloat whiteMaterial[] = {1, 1, 1, 1};

float screen_x = 900;
float screen_y = 700;

class Point2 //2d point
{
public:
   float x, y; 
};

const float control_point_radius = 4;
class Bezier
{
    public:
    Point2 Evaluate(float t);
    void DrawCurve();
    void DrawControlPoints(); 
    int IsPicked(float x, float y);
    void SetColor(GLfloat c[]);
    Point2 p[4];
    GLfloat color[4];
};

const int MAX_CURVES = 5;
int current_curves = 1;
Bezier b[MAX_CURVES]; 

int selected_point = -1; //-1 means none of the four points are selected
int selected_curve = -1;
int last_selected_curve = 0; //for changing color

void DrawCircle(float x1, float y1, float radius)
{
    glBegin(GL_POLYGON);
    for(int i=0; i<32; i++)
    {
        double theta = (double)i/32.0 * 2 * 3.1415926;
        double x = x1 + radius * cos(theta);
        double y = y1 + radius * sin(theta);
        glVertex2f(x,y);
    }
    glEnd();
} 

void Bezier::SetColor(GLfloat c[])
{ 
    for(int i=0; i<4; i++)
    {
        color[i] = c[i];
    }
} 

int Bezier::IsPicked(float x, float y)
{
    for(int i=0; i<4; i++)
    {
        float distance = sqrt( (p[i].x-x)*(p[i].x-x) + (p[i].y-y)*(p[i].y-y) ); // goes off page but guessed distance formula??
        if(distance < control_point_radius)
        {
            return i;
        }
    }
    return -1;
}

Point2 Bezier::Evaluate(float t)
{
    Point2 result;
    result.x = p[0].x*(1-t)*(1-t)*(1-t) + 3*p[1].x*(1-t)*(1-t)*t + 3*p[2].x*(1-t)*t*t + p[3].x*t*t*t; 
    result.y = p[0].y*(1-t)*(1-t)*(1-t) + 3*p[1].y*(1-t)*(1-t)*t + 3*p[2].y*(1-t)*t*t + p[3].y*t*t*t; 
    return result;
}

void Bezier::DrawCurve()
{
    const float steps = 21;
    const float inc = 1.0 / (steps-1);
    float t=0;
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<steps; i++)
    {
        Point2 m = Evaluate(t);
        glVertex2f(m.x,m.y);
        t += inc;
    }
    glEnd();
}

void Bezier::DrawControlPoints()
{
    for(int i=0; i<4; i++)
    {
        DrawCircle(p[i].x, p[i].y, control_point_radius);
    }
}

void DrawRectangle(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);
    glVertex2f(x1,y1);
    glVertex2f(x2,y1);
    glVertex2f(x2,y2);
    glVertex2f(x1,y2);
    glEnd();
} 

void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glVertex2d(x3,y3);
	glEnd();
}

void text_output(float x, float y, char *string)
{
    void *font = GLUT_BITMAP_9_BY_15;
    int len, i;
    glRasterPos2f(x,y);
    len = (int) strlen(string);
    for(i=0; i<len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
    glDisable(GL_BLEND);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    for(int i=0; i<current_curves; i++)
    {
        glColor3fv(b[i].color);
        b[i].DrawControlPoints();
        b[i].DrawCurve();
    }
    glutSwapBuffers();
}

void keyboard(unsigned char c, int x, int y)
{
    switch (c)
    {
        case 27: //esc char means quit
            exit(0);
            break;
        default:
            return;
    }
    glutPostRedisplay();
} 

void SetPerspectiveView(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = (GLdouble) w/(GLdouble) h;
    gluPerspective(
        /* field of view in degree */ 38.0,
        /* aspect ratio */ aspectRatio,
        /* Z near */ 1000.1, /* Z far */ 30000.0);
    glMatrixMode(GL_MODELVIEW);
}

void SetTopView(int w, int h)
{
    // go into 2D mode
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);
    glMatrixMode(GL_MODELVIEW);
}

//void DrawFramesPerSecond() //too much code goes off screen lines 266 288

void reshape(int w, int h)
{
    screen_x = w;
    screen_y = h;
    //glViewPort(0, 0, w, h);
    glViewport(0, 0, w, h);
    SetTopView(w,h);
}



void mouse(int mouse_button, int state, int x, int y)
{ 
    y = screen_y - y - 1; //upside down problem

    if(mouse_button == GLUT_LEFT_BUTTON and state == GLUT_DOWN)// goes off screen??)
    {
        selected_point = -1;
        selected_curve = -1;
        for(int i=0; i<current_curves; i++)
        {
            selected_point = b[i].IsPicked(x,y);
            if(selected_point != -1)
            {
                selected_curve = i;
                last_selected_curve = selected_curve;
                break;
            }
        }
    }
    if(mouse_button == GLUT_LEFT_BUTTON and state == GLUT_UP)
    {
        selected_point = -1;
        selected_curve = -1;
    }
    glutPostRedisplay();
}

void motion(int x, int y) // gets called during a mouse drag.
{
    y = screen_y - y - 1; //upside down problem

    if(selected_point != -1)
    {
        b[selected_curve].p[selected_point].x = x;
        b[selected_curve].p[selected_point].y = y;
        glutPostRedisplay();
    }
    cout << x << " " << y << endl;
}

void InitializeMyStuff()
{
    b[0].p[0].x = 100; b[0].p[0].y = 200;
    b[0].p[1].x = 200; b[0].p[1].y = 400;
    b[0].p[2].x = 300; b[0].p[2].y = 400;
    b[0].p[3].x = 400; b[0].p[3].y = 300;
    for(int i=1; i<MAX_CURVES; i++)
    {
        for(int pnum=0; pnum<4; pnum++)
        {
            b[i].p[pnum].x = b[i-1].p[pnum].x+50;
            b[i].p[pnum].y = b[i-1].p[pnum].y+50;
        }
    }
    b[0].SetColor(redMaterial);
    b[1].SetColor(greenMaterial);
    b[2].SetColor(purpleMaterial);
    b[3].SetColor(blueMaterial);
    b[4].SetColor(grayMaterial);
}

void AddBezier()
{
    if(current_curves < MAX_CURVES)
    {
        current_curves++;
    }
}

void top_menu(int id) //line 380
{ 
    if(id==1)
        exit(0);
    else if(id ==2)
        AddBezier();
    glutPostRedisplay();
}

void color_menu(int id)
{
    if(last_selected_curve != -1)
    {
        if(id==1)
            b[last_selected_curve].SetColor(redMaterial);
        else if(id==2)
            b[last_selected_curve].SetColor(greenMaterial);
        else if(id==3)
            b[last_selected_curve].SetColor(brightGreenMaterial);
        else if(id==4)
            b[last_selected_curve].SetColor(blueMaterial);
        else if(id==5)
            b[last_selected_curve].SetColor(grayMaterial);
        else if(id==6)
            b[last_selected_curve].SetColor(pinkMaterial);
        else if(id==7)
            b[last_selected_curve].SetColor(yellowMaterial);
        else if(id==8)
            b[last_selected_curve].SetColor(purpleMaterial);
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screen_x, screen_y);
    glutInitWindowPosition(0,0);

    int fullscreen = 0;
    if (fullscreen)
    {
        glutGameModeString("800x600:32");
        glutEnterGameMode();
    }
    else
    {
        glutCreateWindow("Shapes");
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    int sub_menu = glutCreateMenu(color_menu);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Green", 2);
    glutAddMenuEntry("Bright Green", 3);
    glutAddMenuEntry("Blue", 4);
    glutAddMenuEntry("Gray", 5);
    glutAddMenuEntry("Pink", 6);
    glutAddMenuEntry("Yellow", 7);
    glutAddMenuEntry("Purple", 8); 

    glutCreateMenu(top_menu);
    glutAddMenuEntry("quit", 1);
    glutAddMenuEntry("Add Curve", 2);
    glutAddSubMenu("Set Color", sub_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glClearColor(1,1,1,1);
    InitializeMyStuff();

    glutMainLoop();
    return 0;
}


