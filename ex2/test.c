/*
==========================================================================
File:        ex2.c (skeleton)
Authors:     Toby Howard
==========================================================================
*/

/* The following ratios are not to scale: */
/* Moon orbit : planet orbit */
/* Orbit radius : body radius */
/* Sun radius : planet radius */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BODIES 20
#define MAX_STARS 1000
#define TOP_VIEW 1
#define ECLIPTIC_VIEW 2
#define SHIP_VIEW 3
#define EARTH_VIEW 4
#define PI 3.14159
#define DEG_TO_RAD 0.017453293
#define ORBIT_POLY_SIDES 40
#define TIME_STEP 0.5   /* days per frame */

typedef struct {
  char    name[20];       /* name */
  GLfloat r, g, b;        /* colour */
  GLfloat orbital_radius; /* distance to parent body (km) */
  GLfloat orbital_tilt;   /* angle of orbit wrt ecliptic (deg) */
  GLfloat orbital_period; /* time taken to orbit (days) */
  GLfloat radius;         /* radius of body (km) */
  GLfloat axis_tilt;      /* tilt of axis wrt body's orbital plane (deg) */
  GLfloat rot_period;     /* body's period of rotation (days) */
  GLint   orbits_body;    /* identifier of parent body */

  GLfloat spin;           /* current spin value (deg) */
  GLfloat orbit;          /* current orbit value (deg) */

 } body;

typedef struct {
  int name;
  int xpos;
  int ypos;
  int zpos;
 } star;

body  bodies[MAX_BODIES];
star  stars[MAX_STARS];
int  numBodies, current_view, draw_labels, draw_orbits, draw_starfield;

float date;

/*****************************/

float myRand (void)
{
  /* return a random float in the range [0,1] */

  return (float) rand() / RAND_MAX;
}

/*****************************/
float randX;
float randY;
float randZ;
int starNumber;
int starsMade = 0;

void drawStarfield (void)
{
  if(starsMade == 0)
  {
    starNumber = 0;
    while(starNumber != 1000)
    {
      randX = 600000000 * (myRand() - 0.5);
      randY = 600000000 * (myRand() - 0.5);
      randZ = 600000000 * (myRand() - 0.5);
      glColor3f(1, 1, 1);
      glBegin (GL_POINTS);
      glVertex3f (randX, randY, randZ);
      glEnd();
      starNumber += 1;
      stars[starNumber].name = starNumber;
      stars[starNumber].xpos = randX;
      stars[starNumber].ypos = randY;
      stars[starNumber].zpos = randZ;
    }
    starsMade = 1;
  }
  else
  {
    starNumber = 0;
    while(starNumber != 1000)
    {
      glColor3f(1, 1, 1);
      glBegin (GL_POINTS);
      glVertex3f (stars[starNumber].xpos, stars[starNumber].ypos,
                  stars[starNumber].zpos);
      starNumber += 1;
      glEnd();
    }
  }
}

/*****************************/

void readSystem(void)
{
  /* reads in the description of the solar system */

  FILE *f;
  int i;

  f= fopen("sys", "r");
  if (f == NULL) {
     printf("ex2.c: Couldn't open the datafile 'sys'\n");
     printf("To get this file, use the following command:\n");
     printf("  cp /opt/info/courses/COMP27112/ex2/sys .\n");
     exit(0);
  }
  fscanf(f, "%d", &numBodies);
  for (i= 0; i < numBodies; i++)
  {
    fscanf(f, "%s %f %f %f %f %f %f %f %f %f %d",
      bodies[i].name,
      &bodies[i].r, &bodies[i].g, &bodies[i].b,
      &bodies[i].orbital_radius,
      &bodies[i].orbital_tilt,
      &bodies[i].orbital_period,
      &bodies[i].radius,
      &bodies[i].axis_tilt,
      &bodies[i].rot_period,
      &bodies[i].orbits_body);

    /* Initialise the body's state */
    bodies[i].spin= 0.0;
    bodies[i].orbit= myRand() * 360.0; /* Start each body's orbit at a
                                          random angle */
    bodies[i].radius*= 1000.0; /* Magnify the radii to make them visible */
  }
  fclose(f);
}

/*****************************/

void drawString (void *font, float x, float y, char *str)
{ /* Displays the string "str" at (x,y,0), using font "font" */

  /* This is for you to complete. */

}

/*****************************/

void showDate(void)
{ /* Displays the current date */

 /* This is for you to complete. */
}

/*****************************/
float spaceshipX;
float spaceshipY;
float spaceshipZ;
int spaceViewSet = 0;

void setView (void) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (current_view) {
  case TOP_VIEW:
    gluLookAt(0, 200000000, 0, 0, 0, 0, 1, 0, 1);
    break;
  case ECLIPTIC_VIEW:
    gluLookAt(0, 0, 200000000, 0, 0, 0, 0, 1, 0);
    break;
  case SHIP_VIEW:
    if (spaceViewSet == 0)
    {
      spaceshipX = 600000000 * (myRand() - 0.5);
      spaceshipY = 600000000 * (myRand() - 0.5);
      spaceshipZ = 600000000 * (myRand() - 0.5);
      spaceViewSet = 1;
    }
    gluLookAt(spaceshipX, spaceshipY, spaceshipZ, 0, 0, 0, 0, 1, 0);
    break;
  case EARTH_VIEW:
    /* This is for you to complete. */
    break;
  }
}

/*****************************/

void menu (int menuentry) {
  switch (menuentry) {
  case 1: current_view= TOP_VIEW;
          break;
  case 2: current_view= ECLIPTIC_VIEW;
          break;
  case 3: current_view= SHIP_VIEW;
          break;
  case 4: current_view= EARTH_VIEW;
          break;
  case 5: draw_labels= !draw_labels;
          break;
  case 6: draw_orbits= !draw_orbits;
          break;
  case 7: draw_starfield= !draw_starfield;
          break;
  case 8: exit(0);
  }
}

/*****************************/

void init(void)
{
  /* Define background colour */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glutCreateMenu (menu);
  glutAddMenuEntry ("Top view", 1);
  glutAddMenuEntry ("Ecliptic view", 2);
  glutAddMenuEntry ("Spaceship view", 3);
  glutAddMenuEntry ("Earth view", 4);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Toggle labels", 5);
  glutAddMenuEntry ("Toggle orbits", 6);
  glutAddMenuEntry ("Toggle starfield", 7);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Quit", 8);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  current_view= TOP_VIEW;
  draw_labels= 1;
  draw_orbits= 1;
  draw_starfield= 1;
}

/*****************************/

void animate(void)
{
  int i;

    date+= TIME_STEP;

    for (i= 0; i < numBodies; i++)  {
      bodies[i].spin += 360.0 * TIME_STEP / bodies[i].rot_period;
      bodies[i].orbit += 360.0 * TIME_STEP / bodies[i].orbital_period;
      glutPostRedisplay();
    }
}

/*****************************/

void drawOrbit (int n)
{ /* Draws a polygon to approximate the circular
     orbit of body "n" */

    /* This is for you to complete. */
}

/*****************************/

void drawLabel(int n)
{ /* Draws the name of body "n" */

    /* This is for you to complete. */
}

/*****************************/

void drawAxis(int n)
{
  /* Draws the axis for body "n" */
  glColor3f(1.0,0.0,0.0);
  glBegin(GL_LINES);
    glVertex3f(0, bodies[n].radius, 0);
    glVertex3f(0, bodies[n].radius*2, 0);
  glEnd();

  glColor3f(0.0,1.0,0.0);
  glBegin(GL_LINES);
    glVertex3f(0, -bodies[n].radius, 0);
    glVertex3f(0, -bodies[n].radius*2, 0);
  glEnd();
}

/*****************************/

void drawBody(int n)
{
  if( n != 0 )
    return;
  /* Draws body "n" */
  glPushMatrix();
  drawAxis(n);
  glRotatef(bodies[n].spin, 0, 1, 0);
  glRotatef(90, 1, 0, 0);
  glColor3f(bodies[n].r, bodies[n].g, bodies[n].b);
  glutWireSphere(bodies[0].radius, 10, 10);
  glPopMatrix();

}

/*****************************/

void display(void)
{
  int i;

  glClear(GL_COLOR_BUFFER_BIT);

  showDate();

  /* set the camera */
  setView();

  if (draw_starfield)
  {
    drawStarfield();
  }


  for (i= 0; i < numBodies; i++)
  {
    glPushMatrix();
      drawBody (i);
    glPopMatrix();
  }

  glutSwapBuffers();
}

/*****************************/

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (48.0, (GLfloat) w/(GLfloat) h, 10000.0, 800000000.0);
}

/*****************************/

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:  /* Escape key */
      exit(0);
  }
}

/*****************************/

int main(int argc, char** argv)
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow ("COMP27112 Exercise 2");
  glutInitWindowSize (2000, 2000);
  init ();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutIdleFunc (animate);
  readSystem();
  glutMainLoop ();
  return 0;
}
/* end of ex2.c */
