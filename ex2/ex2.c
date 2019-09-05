/*
==========================================================================
File:        ex2.c (skeleton)
Version:     5, 19/12/2017
Author:     Toby Howard
==========================================================================
*/

/* The following ratios are not to scale: */
/* Moon orbit : planet orbit */
/* Orbit radius : body radius */
/* Sun radius : planet radius */

#ifdef MACOS
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BODIES 25
#define TOP_VIEW 1
#define ECLIPTIC_VIEW 2
#define SHIP_VIEW 3
#define EARTH_VIEW 4
#define PI 3.141593
#define DEG_TO_RAD 0.01745329
#define ORBIT_POLY_SIDES 50
#define TIME_STEP 0.5   /* days per frame */
#define RUN_SPEED  2000
#define TURN_ANGLE 10.0
typedef struct {
  char    name[25];       /* name */
  GLfloat r,g,b;          /* colour */
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

body  bodies[MAX_BODIES];
int   numBodies, current_view, draw_orbits, draw_labels, draw_starfield,axe1,axe2;
float  date,phi;
GLfloat eyex,eyey,eyez;              /* View angles (degrees)    */
GLfloat lat,     lon;
GLfloat  centerx, centery, centerz; /* Look point               */
/*****************************/

float myRand (void)
{
  /* return a random float in the range [0,1] */

  return (float) rand() / RAND_MAX;
}

int RandSign(void)
{
        return random() & 1 ? 1 : -1;
}
/*****************************/

void drawStarfield (void)
{
  /* This is for you to complete. */

  int i;
  srand(1);
  for(i=0;i<1000;i++)
  {

    glBegin (GL_POINTS);
    glColor3f(255,255,255);   //WHITE
      glVertex3f(myRand()*300000*RandSign(), myRand()*300000*RandSign(), myRand()*300000*RandSign());
    glEnd ();
  }

    /* and all the other points */

}
void polarView(GLdouble distance,GLdouble RotateAngle)
{
      glTranslatef(0.0, 0.0, -distance);
      glRotatef(bodies[3].orbital_tilt,0,0,-1); //orbital_tilt
      glRotatef(RotateAngle,0,-1,0);
      glRotatef(bodies[3].orbit,0,-1,0);//orbit

}

void calculate_lookpoint(void)
{
  centerx=eyex+cos(DEG_TO_RAD*lat)*sin(DEG_TO_RAD*lon);
  centery=eyey+sin(DEG_TO_RAD*lat);
  centerz=eyez+cos(DEG_TO_RAD*lat)*cos(DEG_TO_RAD*lon);
  /* To be completed */

} // calculate_lookpoint()
/*****************************/

void drawAxes(void)
{
  float len=10000000000.0;
  glLineWidth(1.0);
  glBegin(GL_LINES);

  glColor3f(0,1,0);      //green
    glVertex3f(0,0,0);
    glVertex3f(len,0,0);   //x

  glColor3f(1,0,0);    //red
    glVertex3f(0,0,0);
    glVertex3f(0,len,0);   //y

  glColor3f(0,0,1);       //blue
    glVertex3f(0,0,0);
    glVertex3f(0,0,len);   //z

  glEnd();
}
void clearAxes(void)
{
  float len=10000000000.0;
  glLineWidth(1.0);
  glBegin(GL_LINES);

  glColor3f(0,0,0);
    glVertex3f(0,0,0);
    glVertex3f(len,0,0);   //x

  glColor3f(0,0,0);    //red
    glVertex3f(0,0,0);
    glVertex3f(0,len,0);   //y

  glColor3f(0,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,len);   //z

  glEnd();
}

void readSystem(void)
{
  /* reads in the description of the solar system */

  FILE *f;
  int i;

  f= fopen("sys", "r");
  if (f == NULL) {
     printf("ex2.c: Couldn't open 'sys'\n");
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
char * ch;
glRasterPos3f(x,y,0.0);
for(ch=str;*ch;ch++)
  glutBitmapCharacter(font,(int)*ch);
/* This is for you to complete. */
  /* This is for you to complete. */

}

/*****************************/

void setView (void) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (current_view) {
  case TOP_VIEW:
    /* This is for you to complete. */
    gluLookAt(
      0,500000,0, 0,0,0 ,0,0,1 );
    break;
  case ECLIPTIC_VIEW:
    gluLookAt(
      0,0,500000, 0,0,0 ,0,1,0 );
    /* This is for you to complete. */
    break;
  case SHIP_VIEW:
  calculate_lookpoint(); /* Compute the centre of interest   */
  gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0, 1, 0);
    /* This is for you to complete. */
    break;
  case EARTH_VIEW:
    polarView(bodies[3].orbital_radius/1000+0.3*bodies[3].radius/100,90);
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
  axe1=0;
  lat= 0.0;   /* Look horizontally ...  */
  lon= 0.0;   /* ... along the +Z axis  */
  eyex=5000;
  eyey=50000;
  eyez=0;
  axe2=0;

}

/*****************************/

void animate(void)
{
  int i;
  for(phi=-PI;phi<PI;phi+=(2*PI/ORBIT_POLY_SIDES)){
      gluLookAt(bodies[3].radius/1000*sin(phi),0,bodies[3].radius/1000*cos(phi), 0,0,0,0,1,0 );
      glutPostRedisplay();
  }
    date+=0.1* TIME_STEP;
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
      float angle = 2*PI/ORBIT_POLY_SIDES;
      float r = bodies[n].orbital_radius/1000;
      float i;
      glPushMatrix();
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glBegin(GL_POLYGON);
          glColor3f(bodies[n].r,bodies[n].g,bodies[n].b);
          for (i = -1*PI; i < PI; i+=angle){
              glVertex3f(r*cos(i),0,r*sin(i));
          }
      glEnd();
      glPopMatrix();
      /* This is for you to complete. */
    /* This is for you to complete. */
}

/*****************************/

void drawLabel(int n)
{ /* Draws the name of body "n" */

    /* This is for you to complete. */
}

void drawBody(int n)
{
 /* Draws body "n" */
  if(bodies[n].orbits_body==0)   //orbit around sun
  {
    glPushMatrix();
    glRotatef(bodies[n].orbital_tilt,0,0,1); //orbital_tilt
    drawOrbit(n);
    glRotatef(bodies[n].orbit,0,1,0);//orbit
    glTranslatef(bodies[n].orbital_radius/1000,0,0);

    glRotatef(bodies[n].axis_tilt,0,0,1);// self axis tilt

    /* rotate on its own */
    glPushMatrix();
        glRotatef(bodies[n].spin,0,1,0); //self-spin
        glRotatef(90,1,0,0); //vert
        float length=bodies[n].radius/800;
        glBegin(GL_LINES);
          glVertex3f(0,0,-length);
          glVertex3f(0,0,length);
        glEnd();
        glColor3f(bodies[n].r,bodies[n].g,bodies[n].b);
        glLineWidth(1.0);
        glutWireSphere(bodies[n].radius/1000,15,15);
    glPopMatrix();
    glPopMatrix();
  }
  //moon
  else{
    int parentIndex=bodies[n].orbits_body;
    //parent orbit
    glRotatef(bodies[parentIndex].orbital_tilt,0,0,1); //orbital_tilt
    glRotatef(bodies[parentIndex].orbit,0,1,0);//orbit
    glTranslatef(bodies[parentIndex].orbital_radius/1000,0,0);

    glRotatef(bodies[n].orbital_tilt,0,0,1); //orbital_tilt
    drawOrbit(n);
    glRotatef(bodies[n].orbit,0,1,0);//orbit
    glTranslatef(bodies[n].orbital_radius/1000,0,0);
    // self axis tilt
    glRotatef(bodies[n].axis_tilt,0,0,1);//atilt

    /* rotate on its own */
    glPushMatrix();
        glRotatef(bodies[n].spin,0,1,0); //self-spin
        glRotatef(90,1,0,0); //vert
        float length=bodies[n].radius/800;
        glBegin(GL_LINES);
          glVertex3f(0,0,-length);
          glVertex3f(0,0,length);
        glEnd();
        glColor3f(bodies[n].r,bodies[n].g,bodies[n].b);
        glLineWidth(1.0);
        glutWireSphere(bodies[n].radius/1000,15,15);
    glPopMatrix();
  }

    /* This is for you to complete. */
}

/*****************************/

void display(void)
{
  int i;
  glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
  /* set the camera */
  setView();
  if (draw_starfield)
  {
    glPushMatrix();
    glRotatef(bodies[1].spin/100,0,1,0);
    drawStarfield();

    glPopMatrix();
  }

  glColor3f(1.0,1.0,1.0);
  for (i= 0; i < numBodies; i++)
  {
      glPushMatrix();
      drawBody (i);
      glPopMatrix();
  }
  glPushMatrix();
  glRotatef(bodies[1].spin/30,0,1,0);

  glPopMatrix();
  if(axe1==1) drawAxes();
  if(axe2==1)
  {
    glColor3f(1.0,1.0,1.0);
  }

  calculate_lookpoint(); /* Compute the centre of interest   */
  gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0, 1, 0);
   glutSwapBuffers();

}

/*****************************/

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (48.0, (GLfloat)w/(GLfloat)h,  10000.0, 800000000.0);
}

/*****************************/

void keyboard(unsigned char key, int x, int y)
{

  switch(key)
  {

    case 27:  /* Escape key */
      exit(0);
      break;
    case 'a':
        if(axe1==0)
       {
          axe1=1;
        }
        else
        {
         axe1=0;
        }

      break;
      case 'b':
      if(axe2==0)
     {
        drawAxes();
        axe2=1;
      }
      else
      {
       clearAxes();
       axe2=0;
      }
      case ',':
        eyex += (RUN_SPEED*sin(DEG_TO_RAD*(lon+90)));
        eyez += (RUN_SPEED * cos(DEG_TO_RAD*(lon+90)));
      break;
      case '.':
        eyex += (RUN_SPEED * sin(DEG_TO_RAD*(lon-90)));
        eyez += (RUN_SPEED * cos(DEG_TO_RAD*(lon-90)));
      break;

  }
}
void cursor_keys(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      eyex += RUN_SPEED * sin(lon*DEG_TO_RAD);
      eyez += RUN_SPEED * cos(lon*DEG_TO_RAD);
    break;
    case GLUT_KEY_DOWN:
      eyex -= RUN_SPEED * sin(lon*DEG_TO_RAD);
      eyez -= RUN_SPEED * cos(lon*DEG_TO_RAD);
    break;
    case GLUT_KEY_PAGE_UP:
      if(lat<=90)  lat+=TURN_ANGLE;
    break;
    case GLUT_KEY_PAGE_DOWN:
      if(lat>=-90)  lat-=TURN_ANGLE;
    break;
    case GLUT_KEY_LEFT:
      lon+=TURN_ANGLE;
    break;
    //downwards movement
    case GLUT_KEY_RIGHT:
      lon-=TURN_ANGLE;
    break;
    case  27: exit(0); // '27' is ASCII code for the escape key
    break;
  }
}


/*****************************/

int main(int argc, char** argv)
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow ("COMP27112 Exercise 2");
  glutInitWindowSize (2000, 2000);
  init();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (cursor_keys);
  glutIdleFunc (animate);
  readSystem();
  glutMainLoop();
  return 0;
}
/* end of ex2.c */
