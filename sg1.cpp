/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
//#include "glut.h" //MSVC local library install
#include <GL/glut.h> //system-wide install (or compiler default path)

double circle = atan(1) * 8; 
double halfCircle = atan(1) * 4;
double tau = circle; // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI

//How often should the drawing algorithm sample the function.
double step = 0.05;


int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

/*
   Nicomedes' Conchoid
   $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. or 
   $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. where
   $t \in (-\pi / 2, \pi / 2)$
*/
void Display1() {
   double xmax, ymax, xmin, ymin;
   /*
     Nicomedes' Conchoid is a family of functions. These two parameters,
     a and b, choose a specific 2D function from that family.
     It has two branches, so, for each y, we have two x values.
     Therefore, we're actually drawing two lines, not one.
    */
   double a = 1, b = 2;

   /*
     We shadow the global variable with a different step size,
     as values different from 0.05
     would yield a significantly different plot.
   */
   double step = 0.05;
   
   /*
     First, we compute the points of the function, so we can determine
     the maximal extend of the drawing.
    */
   xmax = a - b - 1;
   xmin = a + b + 1;
   ymax = ymin = 0;
   for (double t = - pi/2 + step; t < pi / 2; t += step) {
      double x1, y1, x2, y2;
      x1 = a + b * cos(t);
      xmax = (xmax < x1) ? x1 : xmax;
      xmin = (xmin > x1) ? x1 : xmin;

      x2 = a - b * cos(t);
      xmax = (xmax < x2) ? x2 : xmax;
      xmin = (xmin > x2) ? x2 : xmin;

      y1 = a * tan(t) + b * sin(t);
      ymax = (ymax < y1) ? y1 : ymax;
      ymin = (ymin > y1) ? y1 : ymin;

      y2 = a * tan(t) - b * sin(t);
      ymax = (ymax < y2) ? y2 : ymax;
      ymin = (ymin > y2) ? y2 : ymin;
   }
   //We care about the maximal extent on each axis (from the origin).
   xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
   ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

   /*
     Since we have, for x and for y, the maximal absolute values,
     dividing the coordinates of the points by these values will
     ensure we draw the whole function inside [-1, 1]^2, the default
     OpenGL screen.
    */

   glColor3f(1,0.1,0.1);
   glBegin(GL_LINE_STRIP); 
   for (double t = - pi/2 + step; t < pi / 2; t += step) {
     double x1, y1, x2, y2; //You might get some warnings in your IDE. Why?
     x1 = (a + b * cos(t)) / xmax;
     x2 = (a - b * cos(t)) / xmax;
     y1 = (a * tan(t) + b * sin(t)) / ymax;
     y2 = (a * tan(t) - b * sin(t)) / ymax;
     
     glVertex2d(x1,y1);
   }
   glEnd();
   
   glBegin(GL_LINE_STRIP); 
   for (double t = - pi/2 + step; t < pi / 2; t += step) {
     double x1, y1, x2, y2;
     x1 = (a + b * cos(t)) / xmax;
     x2 = (a - b * cos(t)) / xmax;
     y1 = (a * tan(t) + b * sin(t)) / ymax;
     y2 = (a * tan(t) - b * sin(t)) / ymax;
     
     glVertex2d(x2,y2);
   }
   glEnd();
}

// $f(x) = \left| sin(x) \right| \cdot e^{-sin(x)}, x \in \left[ 0, 8 \cdot \pi \right]$, 
void Display2() {
  /*
    We can determine how far the function extends
    (and thus the needed scaling factors)
    by looking at the function and doing a bit of Calculus.
  */
  double xmax = 8 * pi;
  double ymax = exp(1.1); //Why 1.1?
  
  glColor3f(1,0.1,0.1);
  glBegin(GL_LINE_STRIP); 
  for (double x = 0; x < xmax; x += step) {
    double x1, y1;
    x1 = x / xmax;
    y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;
    glVertex2d(x1,y1);
  }
  glEnd();
}

/* 1)
   \( f(x) = 
     \left\{
       \begin{array}{cl}
         1              & x = 0   \\
         \frac{d(x)}{x} & x \gt 0 \\
       \end{array} 
     \right.
   \)
   \( d(x) = |x - \mathrm{round}(x)| \), domain \( x \in [0, 100] \).
 */
void Display3() {
  const double xmax = 100.0;
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_STRIP);
  for (double x = 0; x <= xmax; x += step) {
    double y = (x == 0.0) ? 1.0 : std::fabs(x - std::round(x)) / x;
    glVertex2d(x / xmax, y);
  }
  glEnd();
}

// Parametric curve: x(a,b,t), y(a,b,t); samples t, scales by scaleX/scaleY; breaks GL_LINE_STRIP at non-finite points (asymptotes).
void plot(double (*x)(double, double, double), double (*y)(double, double, double), double a, double b, double intervalStart, double intervalEnd, double step = 0.01, double scaleX = 1, double scaleY = 1) {
  bool drawing = false;
  for (double t = intervalStart; t <= intervalEnd; t += step) {
    const double px = x(a, b, t);
    const double py = y(a, b, t);



    if (!std::isfinite(px) || !std::isfinite(py)) {
      if (drawing) {
        glEnd();
        drawing = false;
      }
      continue;
    }
    if (!drawing) {
      glBegin(GL_LINE_STRIP);
      drawing = true;
    }
    glVertex2d(px / scaleX, py / scaleY);
  }
  if (drawing)
    glEnd();
}

double limaconX(double a, double b, double t) {
  return 2.0 * (a * cos(t) + b) * cos(t);
}

double limaconY(double a, double b, double t) {
  return 2.0 * (a * cos(t) + b) * sin(t);
}

double cycloidX(double a, double b, double t) {
  return a * t - b * sin(t);
}

double cycloidY(double a, double b, double t) {
  return a - b * cos(t);
}

double epicycloidX(double a, double b, double t) {
  return (a + b) * cos((b / a) * t) - b * cos(t + (b / a) * t);
}

double epicycloidY(double a, double b, double t) {
  return (a + b) * sin((b / a) * t) - b * sin(t + (b / a) * t);
}

double hypocycloidX(double a, double b, double t) {
  return (a - b) * cos((b / a) * t) - b * cos(t - (b / a) * t);
}

double hypocycloidY(double a, double b, double t) {
  return (a - b) * sin((b / a) * t) - b * sin(t - (b / a) * t);
}

double logSpiralX(double a, double, double t) {
  const double r = a * exp(1.0 + t);
  return r * cos(t);
}

double logSpiralY(double a, double, double t) {
  const double r = a * exp(1.0 + t);
  return r * sin(t);
}

double flowerX(double a, double, double t) {
  const double r = sin(a * t);
  return r * cos(t);
}

double flowerY(double a, double, double t) {
  const double r = sin(a * t);
  return r * sin(t);
}

/*
  2) Circle Concoid (Limaçon, Pascal's Snail):
  \(x = 2 \cdot (a \cdot cos(t) + b) \cdot cos(t), \; y = 2 \cdot (a \cdot cos(t) + b) \cdot sin(t), \; t \in (-\pi, \pi)\) .
  For this plot, \(a = 0.3, \; b = 0.2\) .
*/
void Display4() {
  glColor3f(1, 0.1, 0.1);
  plot(limaconX, limaconY, 0.3, 0.2, -pi, pi, 0.001, 1.0, 1.0);
}

/*
  2) Cicloid:
  \(x = a \cdot t - b \cdot sin(t), \; y = a - b \cdot cos(t), \; t \in \mathbb{R} \) .
  For this plot, \(a = 0.1, \; b = 0.2\) .
*/
void Display5() {
  glColor3f(1, 0.1, 0.1);
  // 3 complete periods -> 3 visible loops
  plot(cycloidX, cycloidY, 0.1, 0.2, 0.0, 6.0 * pi, 0.001, 2, 2.5);
}

/*
  2) Epicicloid:
  \(x = (a + b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t + \frac{b}{a}\cdot t \right) \)
  \(y = (a + b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t + \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
*/
void Display6() {
  glColor3f(1, 0.1, 0.1);
  plot(epicycloidX, epicycloidY, 0.1, 0.3, 0.0, 2.0 * pi, 0.001, 0.8, 0.8);
}

/*
  2) Hipocicloid:
  \(x = (a - b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t - \frac{b}{a}\cdot t \right) \)
  \(y = (a - b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t - \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
 */
void Display7() {
  glColor3f(1, 0.1, 0.1);
  plot(hypocycloidX, hypocycloidY, 0.1, 0.3, 0.0, 2.0 * pi, 0.001, 0.6, 0.6);
}

/*
 4) Logarithmic spiral (in polar coordinates):
 \( r = a \cdot e^{1+t}, \; t \in (0, \infty) \) .
 For this plot, \(a = 0.02\) .
*/
void Display8() {
  glColor3f(1, 0.1, 0.1);
  plot(logSpiralX, logSpiralY, 0.02, 0.0, 0.0, 4.0, 0.001, 3.2, 3.2);
}

/*
  4) Sine polar plot flower:
  \( r = sin(a \cdot t), \; t \in (0, \infty)  \) .
  For this plot, \(a = 10\), and the number 'petals' is \( 2 \cdot a \). Think about why.
*/
void Display9() {
  glColor3f(1, 0.1, 0.1);
  plot(flowerX, flowerY, 10.0, 0.0, 0.0, 2.0 * pi, 0.001, 1.0, 1.0);
}

/*
5) Longchamps' Trisectrix:
\(
x = \frac{a}{4 \cdot cos^2(t) - 3}, \;
y = \frac{a \cdot tg(t)}{4 \cdot cos^2(t) - 3}, \;
t \in (-\pi/2, \pi/2) \setminus \{ -\pi/6, \pi/6 \} \) .
For this plot, \(a = 0.2\) .
 */
void Display10() {
}

void init(void) {
  glClearColor(1.0,1.0,1.0,1.0);
  glLineWidth(2);
  glPointSize(1);
  //glPolygonMode(GL_FRONT, GL_LINE);
  //Enabling blending and smoothing
  glEnable(GL_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
  glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
  glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
}

void Display(void) {
  std::cout<<("Call Display")<<std::endl;
  // Clear the buffer. See init();
  glClear(GL_COLOR_BUFFER_BIT);

  switch(prevKey) {
  case '1':
    Display1();
    break;
  case '2':
    Display2();
    break;
  case '3':
    Display3();
    break;
  case '4':
    Display4();
    break;
  case '5':
    Display5();
    break;
  case '6':
    Display6();
    break;
  case '7':
    Display7();
    break;
  case '8':
    Display8();
    break;
  case '9':
    Display9();
    break;
  case '0':
    Display10();
    break;
  default:
    break;
  }
  glFlush();
}

void Reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}
void KeyboardFunc(unsigned char key, int x, int y) {
  prevKey = key;
  if (key == 27) // escape
    exit(0);
  //The proper way to ask glut to redraw the window.
  glutPostRedisplay();
}

/*
  Callback upon mouse press or release.
  The button can be:
  GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
  (and further for mousewheel and other mouse buttons)
  The state can be either GLUT_DOWN or  GLUT_UP, for
  a pressed or released button.
  (x, y) are the coordinates of the mouse.
*/
void MouseFunc(int button, int state, int x, int y) {
  std::cout<< "Mouse button ";
  std::cout<<( (button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right": "middle") ) << " ";
  std::cout<< ( (state == GLUT_DOWN) ? "pressed" : "released" );
  std::cout<< " at coordinates: " << x <<" x " << y << std::endl;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(defaultW, defaultH);
  glutInitWindowPosition(-1, -1);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA); 
  glutCreateWindow (argv[0]);
  init();
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(KeyboardFunc);
  glutMouseFunc(MouseFunc);
  glutDisplayFunc(Display);
  //glutIdleFunc(Display);
  glutMainLoop();

  return 0;
}