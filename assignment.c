/*
 * projectile-motion.c
 *
 * This program shows how to compute projectile motion using both
 * analytical and numerical integration of the equations of motion.
 *
 * $Id: projectile-motion.c,v 1.6 2016/03/15 00:15:26 gl Exp gl $
 *
 */
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

typedef struct { float x, y; } vec2f;

typedef struct { vec2f r0, v0, r, v; } state;
state projectile = { 
  { 0.0, 0.0 },  
  { 1.0, 2.0 }, 
  { 0.0, 0.0 }, 
  { 1.0, 2.0 } 
};

const float g = -9.8;
const int milli = 1000;

typedef enum { analytical, numerical } integrationMode;

typedef struct {
	bool cartesianFlag;
  bool debug;
  bool go;
  float startTime;
  integrationMode integrateMode;
  bool OSD;
  int frames;
  float frameRate;
  float frameRateInterval;
  float lastFrameRateT;
} global_t;

int segments = 8;
global_t global = { true, true, false, 0.0, numerical, true, 0, 0.0, 0.2, 0.0 };

void drawAxes(float length)
{
	glBegin(GL_LINES);
	glColor3f(0,1,0);
    glVertex3f(0,0,0);
	glVertex3f(0,length,0);
	glColor3f(1,0,0);
    glVertex3f(0,0,0);
	glVertex3f(length,0,0);	
	glColor3f(0,0,1);
    glVertex3f(0,0,0);
	glVertex3f(0,0,length);
	glEnd();
}

void updateProjectileStateAnalytical(float t)
{
  vec2f r0, v0;

  r0 = projectile.r0;
  v0 = projectile.v0;

  projectile.r.x = v0.x * t + r0.x;
  projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
  /* 
   * The above statement can alternatively be written as below, saving
   * a few arithmetic operations, which may or may not be worthwhile
   * and which can obscure code and introduce hard to find bugs:
   *
   * 1) Precalculate 1.0 / 2.0 as 0.5 to save a division
   * projectileState.r.y = 0.5 * g * t * t + v0.y * t + r0.y;
   *
   * 2) Factorise to save a multiplication
   * projectileState.r.y = t * (0.5 * g * t + v0.y) + r0.y;
  */
}

void updateProjectileStateNumerical(float dt)
{
  // Euler integration

  // Position
  projectile.r.x += projectile.v.x * dt;
  projectile.r.y += projectile.v.y * dt;
	if (projectile.r.y < 0) {
		projectile.r.y = 0;
		global.go = false;
	}

  // Velocity
  projectile.v.y += g * dt;
}

void updateProjectileState(float t, float dt)
{
  if (global.debug)
    printf("global.integrateMode: %d\n", global.integrateMode);
  if (global.integrateMode == analytical)
    updateProjectileStateAnalytical(t);
  else
    updateProjectileStateNumerical(dt);
}

void drawParametricParabola(int segments)
{
	float x,y,angle;
	float left = -1.0;
	float right = 1.0;
	float range = right - left;
	float stepSize = range/segments;
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < projectile.v.y + 1; i++)
	{
	  x = 2 + i;
		y = 4 + i;
		glVertex3f(x,y,0);
/*printf("%f,%f \n",x,y);*/
	}

	glEnd();
}

void drawCartesianParabola(int segments)
{
	float x,y,angle;
	float left = -1.0;
	float right = 1.0;
	float range = right - left;
	float stepSize = range/segments;
	glBegin(GL_LINE);
	for(int i = 0; i < projectile.v.y + 1; i++)
	{
		x = sin(angle)*sin(angle)*(pow(projectile.v.y,2))/g;
		y = tan(angle)*x-g/(2*pow(projectile.v.y ,2)*cos(angle)*cos(angle))*pow(x,2);
		glVertex3f(x,y,0);
/*printf("%f,%f \n",x,y);*/
	}

	glEnd();
}
void drawCartesianCircle(float a, float b,int segments) {

 /* old equasion: x = a + sqrt( (2*b*y) - (b*b) + (r*r) - (y*y) ); */
    float r = 0.1;
    glBegin(GL_LINE_STRIP);
    glColor3f(1,1,1);
    float y = r,x;
    for (int i = 0;i <= segments;i++) {
      if (i < (segments/2) ){
		    x = a + sqrt( (2*b*y) - (b*b) + (r*r) - (y*y) );
		    x = a + sqrt( pow(r,2) - pow(y-b,2) );
		    glVertex3f(x,y,0);
		    y = y-(r/((float)segments/4));
      }
     else if ((2*i + 1) == segments) {
		   x = a + sqrt( (2*b*y) - (b*b) + (r*r) - (y*y) );
		   x = a + sqrt( pow(r,2) - pow(y-b,2) );
		   glVertex3f(x,y,0);
      }
      else {
		    x = a - sqrt( (2*b*y) - (b*b) + (r*r) - (y*y) );
		    x = a - sqrt( pow(r,2) - pow(y-b,2) );
		    glVertex3f(x,y,0);
		    y = y+(r/((float)segments/4));
	}
}
	glEnd();

}

void drawParametricCircle(float x, float y, int segments) {
	float angle,r = 0.1;
	
	glBegin(GL_LINE_STRIP);
	glColor3f(1,1,1);
	for (int i=0;i<=segments;i++) {
		angle = (float)i/(float)segments *2*M_PI;
		glVertex3f(x+r*cos(angle),y+r*sin(angle),0.0);
	}
	glEnd();

}

void drawParabola(bool cartesianFlag) {
	if (cartesianFlag) {
		drawCartesianParabola(segments);
	}
	else {
		drawParametricParabola(segments);
	}
}

void drawCircle(bool cartesianFlag) {
	if (cartesianFlag) {
		drawCartesianCircle(projectile.r.x, projectile.r.y,segments);
	}
	else {
		drawParametricCircle(projectile.r.x, projectile.r.y,segments);
	}
}
void displayParabola(void)
{
	drawParabola(global.cartesianFlag);
}

void displayProjectile(void)
{
  /*glPointSize(5.0);
  glBegin(GL_POINTS);
  glVertex2f(projectile.r.x, projectile.r.y);
  glEnd();*/
	drawCircle(global.cartesianFlag);
	
}

// Idle callback for animation
void update(void)
{
  static float lastT = -1.0;
  float t, dt;

  if (!global.go) 
    return;

  t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;

  if (lastT < 0.0) {
    lastT = t;
    return;
  }

  dt = t - lastT;
  if (global.debug)
    printf("%f %f\n", t, dt);
  updateProjectileState(t, dt);
  lastT = t;

  /* Frame rate */
  dt = t - global.lastFrameRateT;
  if (dt > global.frameRateInterval) {
    global.frameRate = global.frames / dt;
    global.lastFrameRateT = t;
    global.frames = 0;
  }

  glutPostRedisplay();
}

void displayOSD()
{
  char buffer[30];
  char *bufp;
  int w, h;
    
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  /* Set up orthographic coordinate system to match the 
     window, i.e. (0,0)-(w,h) */
  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  /* Frame rate */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 60);
  snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frameRate);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Time per frame */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 40);
  snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frameRate * 1000.0);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Pop modelview */
  glPopMatrix();  
  glMatrixMode(GL_PROJECTION);

  /* Pop projection */
  glPopMatrix();  
  glMatrixMode(GL_MODELVIEW);

  /* Pop attributes */
  glPopAttrib();
}

void display(void)
{
  float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0 - global.startTime;

  GLenum err;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f (0.8, 0.8, 0.8);

	drawAxes(1.0);

  // Display projectile
  displayProjectile();

	displayParabola();

  // Display OSD
  if (global.OSD)
    displayOSD();

  glPopMatrix();

  glutSwapBuffers();

  global.frames++;
  
  // Check for errors
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("%s\n",gluErrorString(err));
}

void myinit (void) 
{
}
void keyboardSpecial(int key, int x,int y)
{
	switch (key) {

	case GLUT_KEY_UP:
    segments = 2*segments;
    break;
  case GLUT_KEY_DOWN:
	  if(segments > 4)
    	segments = segments/2;
		else if(global.debug)
			printf("Can't draw a reasonable circle with less than 4 segments \n");
    break;
	
	default:
    break;
  }
  glutPostRedisplay();
}
void keyboardCB(unsigned char key, int x, int y)
{
  switch (key) {

  case 27:
  case 'q':
    exit(EXIT_SUCCESS);
    break;
  case 'd':
    global.debug = !global.debug;
    break;
  case 'i':
    if (global.integrateMode == analytical) 
      global.integrateMode = numerical;
    else 
      global.integrateMode = analytical;
    break;
  case 'o':
    global.OSD = !global.OSD;
    break;
	case 'f':
		global.cartesianFlag = !global.cartesianFlag;
		break;
  case ' ':
    if (!global.go) {
      global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
      global.go = true;
    }
    break;
	
  default:
    break;
  }
  glutPostRedisplay();
}

void myReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(500, 500);
  glutCreateWindow("Projectile Motion");
	glutKeyboardFunc(keyboardCB);
  glutSpecialFunc(keyboardSpecial);
  glutReshapeFunc(myReshape);
  glutDisplayFunc(display);
  glutIdleFunc(update);

  myinit();

  glutMainLoop();
}

