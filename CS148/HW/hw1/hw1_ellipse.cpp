/*******************************************************
 * Homework 1: Rasterization                           *
 * CS 148 (Summer 2016), Stanford University           *
 *-----------------------------------------------------*
 * Here you will implement the circle rasterization    *
 * method you derived in the written portion of the    *
 * homework.                                           *
 * To compile this in linux:                           *
 *        g++ hw1.cpp                                  *
 * Then, run the program as follows:                   *
 *        ./a.out 200                                  *
 * to generate a 200x200 image containing a circular   *
 * arc.  Note that the coordinate system we're using   *
 * places pixel centers at integer coordinates and     *
 * has the origin at the lower left.                   *
 * Your code will generate a .ppm file containing the  *
 * final image. These images can be viewed using       *
 * "display" in Linux or Irfanview in Mac/Windows.     *
 *******************************************************/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
using namespace std;

// We'll store image info as globals; not great programming practice
// but ok for this short program.
int a;
int b;
bool **image;

void renderPixel(int x, int y) {
	assert(x >= 0 && y >= 0 && x <= 2*a && y <= 2*b);
	image[y][x] = 1;

	// TODO:  light up the pixel's symmetric counterpart
  image[2*b - y][x] = 1;
  image[2*b - y][2*a - x] = 1;
  image[y][2*a - x] = 1;
}

void rasterizeArc() {
	// TODO:  rasterize the arc using renderPixel to light up pixels
  int x = a;
  int y = 2*b;
  int d = 4*b*b+a*a-4*a*a*b;

  renderPixel(x, y);

  while((x-a)*b*b <= (y-b)*a*a) {
    if(d < 0) {
      d += (8*(x-a)+12)*b*b; // select E
    } else {
      d += (8*(x-a) + 12)*b*b - (8*(y-b)-12)*a*a; // select SE
      y--;
    }
    x++;
    renderPixel(x, y);
  }

  while((x-a)*b*b > (y-b)*a*a) {
    if(d < 0) {
      d += (8*(x-a)+8)*b*b - (8*(y-b)-12)*a*a; // select SE2
      x++;
    } else {
      d += -(8*(y-b)-12)*a*a; // select SE
    }
    if(y > b+1)
      y--;

    if(x >= 2*a && y >= b)
      break;
    renderPixel(x, y);
  }

}

// You shouldn't need to change anything below this point.

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " a  b\n";
		return 0;
	}
	
#ifdef _WIN32
	sscanf_s(argv[1], "%d", &a);
	sscanf_s(argv[2], "%d", &b);
#else
	sscanf(argv[1], "%d", &a);
	sscanf(argv[2], "%d", &b);
#endif
	if (a <= 0 || b<=0) {
		cout << "Image must be of positive size.\n";
		return 0;
	}

  if(a <= b) {
		cout << "specify ellipse with a > b\n";
		return 0;    
  }
	
	// reserve image as 2d array
	image = new bool*[2*a+1];
	for (int i = 0; i <= 2*b; i++) image[i] = new bool[2*a+1];
	
	rasterizeArc();
	
	char filename[50];
#ifdef _WIN32
	sprintf_s(filename, 50, "ellispe%dx%d.ppm", a, b);
#else
	sprintf(filename, "circle%dx%d.ppm", a, b);
#endif
	
	ofstream outfile(filename);
	outfile << "P3\n# " << filename << "\n";
	outfile << 2*a+1 << ' ' << 2*b+1 << ' ' << 1 << endl;

	for (int i = 0; i <= 2*b; i++)
	for (int j = 0; j <= 2*a; j++) {
		outfile << image[2*b-i][j] << " 0 0\n";
  }

	// delete image data
	for (int i = 0; i <= 2*b; i++) delete [] image[i];
	delete [] image;
	
	return 0;
}
