/*******************************************************
 * Homework 2: OpenGL                                  *
 * CS 148 (Summer 2016), Stanford University           *
 *-----------------------------------------------------*
 * First, you should fill in problem1(), problem2(),   *
 * and problem3() as instructed in the written part of *
 * the problem set.  Then, express your creativity     *
 * with problem4()!                                    *
 *                                                     *
 * Note: you will only need to add/modify code where   *
 * it says "TODO".                                     *
 *                                                     *
 * The left mouse button rotates, the right mouse      *
 * button zooms, and the keyboard controls which       *
 * problem to display.                                 *
 *                                                     *
 * For Linux/OS X:                                     *
 * To compile your program, just type "make" at the    *
 * command line.  Typing "make clean" will remove all  *
 * computer-generated files.  Run by typing "./hw2"    *
 *                                                     *
 * For Visual Studio:                                  *
 * You can create a project with this main.cpp and     *
 * build and run the executable as you normally would. *
 *******************************************************/

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#if __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;

bool leftDown = false, rightDown = false;
int lastPos[2];
float cameraPos[4] = {0,1,4,1};
int windowWidth = 640, windowHeight = 480;
double yRot = 0;
int curProblem = 1; // TODO: change this number to try different examples

float specular[] = { 1.0, 1.0, 1.0, 1.0 };
float shininess[] = { 50.0 };

void problem1() {
    // TODO: Your code here!
  for(int i=0; i<10; i++) {
			glPushMatrix();
      glRotatef(i*36,0,0,1);
      glTranslatef(1.0, 0, 0);
			glutSolidTeapot(0.2);
			glPopMatrix();
  }
}

void problem2() {
    // TODO: Your code here!
    float start = -0.75;
    float deltaScale = 0.08;
    float deltaX = 0.1;
    for(int i=0; i<15; i++) {
      glPushMatrix();
      glScalef(1.5, 2*(1+deltaScale*i), 1);
      glTranslatef(start+deltaX*i, 0.05, 0);
      glutSolidCube(0.1);
      glPopMatrix();
      deltaScale += 0.01;
    }
}

void problem3() {
    // TODO: Your code here!
  float delta = 1.0;
  float start = 0;
  float verDelta = 0.5;
  float verInitial = 1.0;
  for(int i=0; i<6; i++) {
    int num = i+1;
    start = -i*delta / 2;
    for(int j=0; j<num; j++) {
      glPushMatrix();
      glTranslatef(start+j*delta, verInitial-i*verDelta, 0);
      glutSolidTeapot(0.15);
			glPopMatrix();
    }
  }
}

void draw_cube() {
  GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */

  /* Setup cube vertex data. */
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

  GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0, 1.0} };

  GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

  for (int i = 0; i < 6; i++) {
    glBegin(GL_QUADS);
    glNormal3fv(&n[i][0]);
    glVertex3fv(&v[faces[i][0]][0]);
    glVertex3fv(&v[faces[i][1]][0]);
    glVertex3fv(&v[faces[i][2]][0]);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}

void problem4() {
    // TODO: Your code here!
    // Draw N
    glPushMatrix();
    glTranslatef(-0.5, 0, 0);
    glPushMatrix();
    glTranslatef(-0.25, 0, 0);
    glScalef(0.1, 0.5, 0.1);
    draw_cube();
    glPopMatrix();

    glPushMatrix();
    glRotatef(30, 0, 0, 1);
    glScalef(0.1, 0.5*2/1.732, 0.1);
    draw_cube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25, 0, 0);
    glScalef(0.1, 0.5, 0.1);
    draw_cube();
    glPopMatrix();
    glPopMatrix();

     // Draw V
    glPushMatrix();
    glTranslatef(0.5, 0, 0);

    glPushMatrix();
    glTranslatef(-0.13, 0, 0);
    glRotatef(15, 0, 0, 1);
    glScalef(0.1, 0.5, 0.1);
    draw_cube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.13, 0, 0);
    glRotatef(-15, 0, 0, 1);
    glScalef(0.1, 0.5, 0.1);
    draw_cube();
    glPopMatrix();
    glPopMatrix();
}

void display() {
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0); // x axis
		glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0); // y axis
		glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1); // z axis
	glEnd(/*GL_LINES*/);

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,windowWidth,windowHeight);

	float ratio = (float)windowWidth / (float)windowHeight;
	gluPerspective(50, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], 0, 0, 0, 0, 1, 0);

	glLightfv(GL_LIGHT0, GL_POSITION, cameraPos);

	glRotatef(yRot,0,1,0);

	if (curProblem == 1) problem1();
	if (curProblem == 2) problem2();
	if (curProblem == 3) problem3();
	if (curProblem == 4) problem4();

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) leftDown = (state == GLUT_DOWN);
	else if (button == GLUT_RIGHT_BUTTON) rightDown = (state == GLUT_DOWN);

	lastPos[0] = x;
	lastPos[1] = y;
}

void mouseMoved(int x, int y) {
	if (leftDown) yRot += (x - lastPos[0])*.1;
	if (rightDown) {
		for (int i = 0; i < 3; i++)
			cameraPos[i] *= pow(1.1,(y-lastPos[1])*.1);
	}


	lastPos[0] = x;
	lastPos[1] = y;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	curProblem = key-'0';
    if (key == 'q' || key == 'Q' || key == 27){
        exit(0);
    }
	glutPostRedisplay();
}

void reshape(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("HW2");

	glutDisplayFunc(display);
	glutMotionFunc(mouseMoved);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}
