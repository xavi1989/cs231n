#include <stdio.h>
#include "GL/glut.h"

void setup()
{
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, w/(float)h, 1, 100);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, -5, 0, 0, 0, 0, 1, 0);
    glRotatef(-30, 1, 0, 0);

    glColor3f(1, 0, 0);
    float color[] = {1, 0, 0, 1};
    float specular[] = {1, 1, 1, 1};
    float shine = 128.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shine);

    glutSolidTeapot(1);

    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(512, 512);
    glutCreateWindow("Hello");
    glutInitWindowPosition(50, 50);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float lightDir[] = {1, 1, 1, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);

    setup();

    glutMainLoop();
}
