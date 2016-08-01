//=========================================================
//
// CS 148 (Summer 2016) - Assignment 5 - Mesh Smoothing
//
//=========================================================
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

// Includes all libst classes
#include "lib/libst/include/st.h"

#define DEBUG 0

const int g_k_window_width = 768;
const int g_k_window_height = 512;

// The default mesh
STShape * g_shape;
STShape * golden_shape;

bool taubinFlag = false;
bool HCFlag = false;

// How many iterations of smoothing
unsigned int g_k_num_iterations = 1;

// Inspection controls
bool g_just_clicked = false;
double g_mouse_x = 0.0;
double g_mouse_y = 0.0;
double g_mouse_x_prev = 0.0;
double g_mouse_y_prev = 0.0;
double g_rot_angle_x = 0.0;
double g_rot_angle_y = 0.0;

//--------------------------------------------------------- 
// Your task is to implement this method by making a copy 
// of "g_shape" which models "g_k_num_iterations" of 
// Laplacian smoothing.
//---------------------------------------------------------

void HC_algorithm()
{
  float alpha = 0.1;
  float beta = 0.6;
  //float lamda = 0;
  STShape *pingpong;
  pingpong = new STShape(golden_shape);
  STShape *bShape = new STShape(golden_shape);

  int nVertices = golden_shape->GetNumVertices();

  // For each vertex in pingpong, perform Laplacian interations
  for(int it = 0; it < g_k_num_iterations; it++) {

      for(int j = 0; j < nVertices; j++) {
          // get Vertices neighbor from pingpong
          STShape::VertexIndexSet vSet = pingpong->GetNeighboringVertices(j);
          STShape::Vertex centerVertex = pingpong->GetVertex(j);

          // calculate update position
          STPoint3 position(0, 0, 0);

          std::set<STShape::Index>::iterator iSet;
          for(iSet = vSet.begin(); iSet != vSet.end(); ++iSet) {
              position.x += pingpong->GetVertex(*iSet).position.x;
              position.y += pingpong->GetVertex(*iSet).position.y;
              position.z += pingpong->GetVertex(*iSet).position.z;
          }

          position.x = position.x / vSet.size();
          position.y = position.y / vSet.size();
          position.z = position.z / vSet.size();

          // calculate b = position - (alpha * origin + (1 - alpha) * q(previous))
          STPoint3 b(0, 0, 0);
          STShape::Vertex originVertex = golden_shape->GetVertex(j);

          b.x = position.x - (alpha*originVertex.position.x + (1-alpha) * centerVertex.position.x);
          b.y = position.y - (alpha*originVertex.position.y + (1-alpha) * centerVertex.position.y);
          b.z = position.z - (alpha*originVertex.position.z + (1-alpha) * centerVertex.position.z);         

          // update b to bShape
          centerVertex.position.x = b.x;
          centerVertex.position.y = b.y;
          centerVertex.position.z = b.z;

          bShape->SetVertex(j, centerVertex);
      }

      // Second update
      for(int j = 0; j < nVertices; j++) {
          // get Vertices neighbor from pingpong
          STShape::VertexIndexSet vSet = pingpong->GetNeighboringVertices(j);
          STShape::Vertex centerVertex = pingpong->GetVertex(j);
          STShape::Vertex bShapeVertex = bShape->GetVertex(j);

          // calculate update position
          STPoint3 bPosition(0, 0, 0);

          std::set<STShape::Index>::iterator iSet;
          for(iSet = vSet.begin(); iSet != vSet.end(); ++iSet) {
              bPosition.x += bShape->GetVertex(*iSet).position.x;
              bPosition.y += bShape->GetVertex(*iSet).position.y;
              bPosition.z += bShape->GetVertex(*iSet).position.z;            
          }

          bPosition.x = bPosition.x / vSet.size();
          bPosition.y = bPosition.y / vSet.size();
          bPosition.z = bPosition.z / vSet.size();         

          centerVertex.position.x -= (beta*bShapeVertex.position.x + (1-beta)*bPosition.x);
          centerVertex.position.y -= (beta*bShapeVertex.position.y + (1-beta)*bPosition.y);
          centerVertex.position.z -= (beta*bShapeVertex.position.z + (1-beta)*bPosition.z);

          // set Vertex in g_shape
          g_shape->SetVertex(j, centerVertex);     
      }

      delete pingpong;
      pingpong = new STShape(g_shape);
  }

  if(pingpong) {
      delete pingpong;
  }

  if(bShape)
      delete bShape;

  g_shape->GenerateNormals();
}

void computeLaplacianSmoothedMesh()
{
  // Do it!
  float lamda = 1;
  //float lamda = 0;
  STShape *pingpong;
  pingpong = new STShape(golden_shape);

  int nVertices = golden_shape->GetNumVertices();

  int mul = 1;
  if(taubinFlag)
      mul = 2;
  // For each vertex in pingpong, perform Laplacian interations
  for(int it = 0; it < g_k_num_iterations * mul; it++) {
      if(taubinFlag) {
          lamda = 0.9;
          if(it % 2 == 1) {
              lamda = -1;
          }
      }

      for(int j = 0; j < nVertices; j++) {
          // get Vertices neighbor from pingpong
          STShape::VertexIndexSet vSet = pingpong->GetNeighboringVertices(j);
          STShape::Vertex centerVertex = pingpong->GetVertex(j);

          // calculate update position
          STPoint3 position(0, 0, 0);

          std::set<STShape::Index>::iterator iSet;
          for(iSet = vSet.begin(); iSet != vSet.end(); ++iSet) {
              position.x += pingpong->GetVertex(*iSet).position.x;
              position.y += pingpong->GetVertex(*iSet).position.y;
              position.z += pingpong->GetVertex(*iSet).position.z;
          }

          position.x = position.x / vSet.size();
          position.y = position.y / vSet.size();
          position.z = position.z / vSet.size();

          // dp
          STPoint3 dp(0, 0, 0);
          dp.x = position.x - centerVertex.position.x;
          dp.y = position.y - centerVertex.position.y;
          dp.z = position.z - centerVertex.position.z;

          // update centerVertex
#if DEBUG
          printf("before centerVertex %f %f %f \n", centerVertex.position.x, centerVertex.position.y, centerVertex.position.z);
#endif

          centerVertex.position.x += lamda * dp.x;
          centerVertex.position.y += lamda * dp.y;
          centerVertex.position.z += lamda * dp.z;

#if DEBUG
          printf("end centerVertex %f %f %f \n", centerVertex.position.x, centerVertex.position.y, centerVertex.position.z);
#endif

          // set Vertex in g_shape
          g_shape->SetVertex(j, centerVertex);
      }

      delete pingpong;
      pingpong = new STShape(g_shape);
  }

  if(pingpong) {
      delete pingpong;
  }

  g_shape->GenerateNormals();
}

//--------------------------------------------------------- 
// Render the scene
//---------------------------------------------------------
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // setup lighting
  float mat_amb_diff[] = { 0.6, 0.6, 0.6, 1.0 };
  float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  float mat_shininess[] = { 10.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  // draw
  glPushMatrix();
    glRotatef(g_rot_angle_x, 0.0, 0.0, 1.0);
    glRotatef(g_rot_angle_y, 1.0, 0.0, 0.0);
    glutSolidSphere(0.04, 20, 20);
    if (g_shape) {
      g_shape->Draw();
    }
  glPopMatrix();

  glutSwapBuffers();
  glFlush();
}

//--------------------------------------------------------- 
// Number keys 1-7 are used to toggle the iteration count
//---------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
  if (key == 'q' || key == 'Q' || key == 27) {
      if(g_shape) {
          delete g_shape;
      }

      if(golden_shape) {
          delete golden_shape;
      }

      exit(0);
   }

  switch(key) {
    case '1':
      g_k_num_iterations = 1;
      break;
    case '2':
      g_k_num_iterations = 2;
      break;
    case '3':
      g_k_num_iterations = 4;
      break;
    case '4':
      g_k_num_iterations = 8;
      break;
    case '5':
      g_k_num_iterations = 16;
      break;
    case '6':
      g_k_num_iterations = 32;
      break;
    case '7':
      g_k_num_iterations = 64;
      break;
    case 't':
    case 'T':
      taubinFlag = !taubinFlag;
      if(taubinFlag) {
          HCFlag = false;
          printf("taubin smoothing enabled \n");
      } else {
          printf("taubin smoothing disabled \n");
      }
      break;
    case 'h':
    case 'H':
      HCFlag = !HCFlag;
      if(HCFlag) {
          taubinFlag = false;
          printf("HC smoothing enabled \n");
      } else {
          printf("HC smoothing disabled \n");
      }
      break;
    default:
      break;
  }

  cout << "Smoothing with " << g_k_num_iterations << " iterations." << endl;

  if(HCFlag) {
      HC_algorithm();
  } else {
      computeLaplacianSmoothedMesh();
  }

  glutPostRedisplay();
}

//--------------------------------------------------------- 
// Mouse click handler
//---------------------------------------------------------
void mouseFunc(int button, int state, int x, int y)
{
  //printf("mouse func: (%d, %d, %d, %d)\n", button, state, x, y);

  bool clicked = (state == 0);
  bool released = (state == 1);

  if (clicked) {
    if (!g_just_clicked) {
      g_mouse_x_prev = (float)x;
      g_mouse_y_prev = (float)y;
      g_just_clicked = true;
    }
  } else if (released) {
    g_just_clicked = false;
  }
}

//--------------------------------------------------------- 
// Mouse motion handler
//---------------------------------------------------------
void motionFunc(int x, int y)
{
  if (!g_just_clicked) {
    g_mouse_x_prev = g_mouse_x;
    g_mouse_y_prev = g_mouse_y;
  } else {
    g_just_clicked = false;
  }

  g_mouse_x = (float)x;
  g_mouse_y = (float)y;

  float delta_x = -0.25 * (g_mouse_x - g_mouse_x_prev);
  float delta_y =  0.25 * (g_mouse_y - g_mouse_y_prev);

  g_rot_angle_x += delta_x;
  g_rot_angle_y += delta_y;

  if (g_rot_angle_x < -360.0) g_rot_angle_x += 360.0;
  if (g_rot_angle_x > 360.0) g_rot_angle_x -= 360.0;
  if (g_rot_angle_y < -360.0) g_rot_angle_y += 360.0;
  if (g_rot_angle_y > 360.0) g_rot_angle_y -= 360.0;

  glutPostRedisplay();
}

//--------------------------------------------------------- 
// Create the mesh and initialize OpenGL state
//---------------------------------------------------------
void setupScene()
{
  g_shape = new STShape("bunny.obj");
  //g_shape = STShapes::CreateThickCylinder(0.02, 0.02, 0.05);
  golden_shape = new STShape(g_shape);

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);

  float light_pos[] = { 2.0, 3.0, 4.0, 1.0 };
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

  glEnable(GL_DEPTH_TEST);

  // Setup the view of the mesh
  glMatrixMode(GL_PROJECTION);
  gluPerspective( 60.0, /* field of view in degrees */
                  double(g_k_window_width)/double(g_k_window_height), // aspect ratio
                  0.01,  /* z near */ 
                  10.0); /* z far */ 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(-0.1, 0.16, 0.25,  /* eye is at (x,y,z) */
             0.0,  0.1,  0.0,  /* center of world */
             0.0,  1.0,  0.0);  /* up is in positive Y direction */
}

//--------------------------------------------------------- 
// Entry point
//---------------------------------------------------------
int main(int argc, char ** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(g_k_window_width, g_k_window_height);

  glutCreateWindow("CS 148 - HW 5 - Laplace Smoothing");

  glutDisplayFunc(display);
  glutMouseFunc(mouseFunc);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(motionFunc);

  setupScene();

  glutMainLoop();

  return 0;
}

