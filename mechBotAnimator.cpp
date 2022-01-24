#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include "mechBotAnimator.h"
#include "subdivcurve.h"
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "QuadMesh.h"
#include <ctime>

//#define STB_IMAGE_IMPLEMENTATION
//#define STBI_FAILURE_USERMSG
//#include "stb_image.h"


enum BotType { CUBE, SPHERE, WHEEL};
BotType botType = WHEEL;
 
int numCirclePoints = 30;
double circleRadius = 0.2;
int hoveredCircle = -1;
int curveIndex = 0;
int currentCurvePoint = 0;
float angle = 0;
int delay = 15; // milliseconds

GLfloat cannon = -8;
bool stop = false;

// Forward and Movement Vectors
//float accumz[0] = 0.0;
//float accumx[0] = 0.0;
int p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0;
int ap0 = 0, ap1 = 0, ap2 = 0, ap3 = 0, ap4 = 0;
float anglewheel = 0;

float x0 = 0.0, x1 = -6.0, x2 = 6.0, x3 = 10.0, x4 = -10.0;
float z0 = 0.0, z1 = 0.0, z2 = 0.0, z3 = 0.0, z4 = 0.0;

// List of subcurves
SubdivisionCurve subcurves[5];
int curvesize = sizeof(subcurves) / sizeof(subcurves[0]);

//Movement Paths for Bots
bool botAlive[5] = { true, true, true, true, true};
int current_curve[5] = { 0,1,2,3,4 };
float accumx[5] = {x0, x1, x2, x3, x4};
float accumz[5] = {z0, z1, z2, z3, z4};
int point[5] = { p0,p1,p2,p3,p4 };
int a_point[5] = { ap0,ap1,ap2,ap3,ap4 };
//robot bullets
GLfloat botbulletx[5] = { 0,0,0,0,0 };
GLfloat botbulletz[5] = { 37,37,37,37,37 };
bool robotFiring = false;

GLdouble worldLeft = -12;
GLdouble worldRight = 12;
GLdouble worldBottom = -9;
GLdouble worldTop = 9;
GLdouble worldCenterX = 0.0;
GLdouble worldCenterY = 0.0;
GLdouble wvLeft = -12;
GLdouble wvRight = 12;
GLdouble wvBottom = -9;
GLdouble wvTop = 9;

GLint glutWindowWidth = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;

// Ground Mesh material
GLfloat groundMat_ambient[]    = {0.11, 0.55, 0.11, 1.0};
GLfloat groundMat_specular[]   = {0.01, 0.01, 0.01, 1.0};
GLfloat groundMat_diffuse[]   = {0.4, 0.4, 0.4, 1.0};
GLfloat groundMat_shininess[]  = {0.001};

GLfloat light_position0[] = {4.0, 8.0, 8.0, 1.0};
GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1};

GLfloat light_position1[] = {-4.0, 8.0, 8.0, 1.0};
GLfloat light_diffuse1[] = { 0.25, 0.25, 0.25, 0.25 };

GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat model_ambient[]  = {0.65, 0.35, 0.25, 0.25};

// 
GLdouble spin  = 0.0;

// Use circles to **draw** (i.e. visualize) subdivision curve control points
Circle circles[MAXCONTROLPOINTS];

int lastMouseX;
int lastMouseY;
int window2D, window3D;
int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;
GLdouble eyeX = 0.0, eyeY = 6.0, eyeZ = 22.0;
GLdouble centerX = 0.0, centerY = 0.0, centerZ = 0.0;
GLdouble zNear = 0.1, zFar = 40.0;
GLdouble fov = 60.0;
bool firstperson = true;

//Robot Variables
float botScale = 0.1;
float robotBodyWidth = 3.0;
float robotBodyLength = 6.0;
float robotBodyDepth = 10.0;
float headWidth = 0.4*robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength / 2;
float upperArmWidth = 0.3*robotBodyWidth;
float gunLength = 3;
float gunWidth = upperArmWidth * 0.9;
float gunDepth = upperArmWidth;
float wheelLength = robotBodyLength / 1.1;
float wheelRadius = robotBodyLength / 1.5;
float baseWidth = 2 * robotBodyWidth;
float baseLength = 0.25*wheelLength;

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = -40.0;
float gunAngle = -25.0;

// Control Wheel Spinning
float wheelAngle = 0.0;

// Robot RGBA material properties
GLfloat robotBody_mat_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat robotBody_mat_specular[] = { 0.45f,0.55f,0.95f,0.8f };
GLfloat robotBody_mat_diffuse[] = { 0.15f,0.25f,0.3f,1.0f };
GLfloat robotBody_mat_shininess[] = { 45.0F };

GLfloat robotArm_mat_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat robotArm_mat_diffuse[] = { 0.145f,0.155f,0.195f,0.8f };
GLfloat robotArm_mat_specular[] = { 0.15f,0.25f,0.3f,1.0f };
GLfloat robotArm_mat_shininess[] = { 100.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat gun_mat_diffuse[] = { 0.65f,0.25f,0.15f,0.8f };
GLfloat gun_mat_specular[] = { 0.15f,0.25f,0.3f,1.0f };
GLfloat gun_mat_shininess[] = { 5.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.1f, 0.1f, 0.1f, 0.4f };
GLfloat robotLowerBody_mat_specular[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotLowerBody_mat_shininess[] = { 22.8F };

//Bullet RGBA
GLfloat bullet_mat_ambient[] = { 1.0f, 0.5f, 0.0f, 1.0f };
GLfloat bullet_mat_diffuse[] = { 0.6f, 0.2f, 0.1f, 1.0f };
GLfloat bullet_mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat bullet_mat_shininess[] = { 225.8F };

//Cannon Variables
bool firing = false;
bool playerLive = true;
GLfloat bulletx = cannon;
GLfloat bulletz = 10;

GLuint texture;
GLfloat lightPos[4] = { 10.0, 20.0, 10.0, 1.0 };

int main(int argc, char* argv[])
{
	//Unable to open any of the ground images
	//int width, height, channels;
	//unsigned char *img = stbi_load("ground.bmp", &width, &height, &channels, 0);
	//if (stbi_failure_reason) {
	//	std::cout << stbi_failure_reason();
	//}
	//printf("\nLoaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
	//stbi_image_free(img);

	glutInit(&argc, (char **)argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	glutInitWindowPosition(50, 100);

	InitSubCurves();
	window3D = glutCreateWindow("Mech Bot");
	glutPositionWindow(900,100);  
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouseButtonHandler3D);
	glutMotionFunc(mouseMotionHandler3D);
	glutKeyboardFunc(keyboardHandler);
	glutSpecialFunc(specialKeyHandler);

	// Initialize the 3D system
	init3DSurfaceWindow();

	// Annnd... ACTION!!
	glutMainLoop(); 

	return 0;
}

void InitSubCurves() 
{ 
	/*
	glLineWidth(3.0);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);
	*/
	for (int i = 0; i <= 4; i++)
	{
		subcurves[i] = initSubdivisionCurve(subcurves[i],i);
		subcurves[i] = initControlPoints(subcurves[i]);
	}

	computeSubdivisionCurve(&subcurves[current_curve[0]]);
} 

SubdivisionCurve initSubdivisionCurve(SubdivisionCurve subcurve,int cur_curve) {
	// Initialize 3 control points of the subdivision curve
	
	GLdouble x, y;
	GLfloat angle[5] = {0.25,0.0,-0.25};
	srand((int) time(0) + cur_curve);
	subcurve.numControlPoints = 8;
	subcurve.subdivisionSteps = 5;

	x = -(rand() % 30) * cos(M_PI*0.5) + cur_curve;
	y = 15 * sin(M_PI*0.5) + cur_curve*5;

	subcurve.controlPoints[0].x = x;
	subcurve.controlPoints[0].y = y;

	for (int i=1; i <= subcurve.numControlPoints - 1;i++)
	{
		int val = rand() % 20;
		if (val % 2 == 0) { val = -val; }
		x = val * cos(M_PI*angle[i]);
		y = (rand() % 4) * sin(M_PI*angle[i]);

		subcurve.controlPoints[i].x = x;
		subcurve.controlPoints[i].y = y;
	}
	int val = rand() % 100;
	if (val % 2 == 0) { val = -val; }
	x = -(val) * cos(M_PI*0.5);
	y = -25 * sin(M_PI*0.5);

	subcurve.controlPoints[subcurve.numControlPoints - 1].x = x;
	subcurve.controlPoints[subcurve.numControlPoints - 1].y = y;
	return subcurve;
}

SubdivisionCurve initControlPoints(SubdivisionCurve subcurve){
	int i;
	int num = subcurve.numControlPoints;
	for (i=0; i < num; i++){
		constructCircle(circleRadius, numCirclePoints, circles[i].circlePoints);
		circles[i].circleCenter = subcurve.controlPoints[i];
	}

	return subcurve;
}

void screenToWorldCoordinates(int xScreen, int yScreen, GLdouble *xw, GLdouble *yw)
{
	GLdouble xView, yView;
	screenToCameraCoordinates(xScreen, yScreen, &xView, &yView);
	cameraToWorldCoordinates(xView, yView, xw, yw);
}

void screenToCameraCoordinates(int xScreen, int yScreen, GLdouble *xCamera, GLdouble *yCamera)
{
	*xCamera = ((wvRight-wvLeft)/glutWindowWidth)  * xScreen; 
	*yCamera = ((wvTop-wvBottom)/glutWindowHeight) * (glutWindowHeight-yScreen); 
}

void cameraToWorldCoordinates(GLdouble xcam, GLdouble ycam, GLdouble *xw, GLdouble *yw)
{
	*xw = xcam + wvLeft;
	*yw = ycam + wvBottom;
}

void worldToCameraCoordiantes(GLdouble xWorld, GLdouble yWorld, GLdouble *xcam, GLdouble *ycam)
{
	double wvCenterX = wvLeft   + (wvRight - wvLeft)/2.0;
	double wvCenterY = wvBottom + (wvTop   - wvBottom)/2.0;
	*xcam = worldCenterX - wvCenterX + xWorld;
	*ycam = worldCenterY - wvCenterY + yWorld;
}

int currentButton;

void mouseButtonHandler(int button, int state, int xMouse, int yMouse)
{
	int i;
	
	currentButton = button;
	if (button == GLUT_LEFT_BUTTON)
	{  
		switch (state) {      
		case GLUT_DOWN:
			if (hoveredCircle > -1) {
				screenToWorldCoordinates(xMouse, yMouse, &circles[hoveredCircle].circleCenter.x, &circles[hoveredCircle].circleCenter.y);
				screenToWorldCoordinates(xMouse, yMouse, &subcurves[current_curve[0]].controlPoints[hoveredCircle].x, &subcurves[current_curve[0]].controlPoints[hoveredCircle].y);
			}
			break;
		case GLUT_UP:
			glutSetWindow(window3D);
			glutPostRedisplay();
			break;
		}
	}    
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		switch (state) {      
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			if (hoveredCircle == -1 && subcurves[current_curve[0]].numControlPoints < MAXCONTROLPOINTS){ 
				GLdouble newPointX;
				GLdouble newPointY;
				screenToWorldCoordinates(xMouse, yMouse, &newPointX, &newPointY);
				subcurves[current_curve[0]].controlPoints[subcurves[current_curve[0]].numControlPoints].x = newPointX;
				subcurves[current_curve[0]].controlPoints[subcurves[current_curve[0]].numControlPoints].y = newPointY;			
				constructCircle(circleRadius, numCirclePoints, circles[subcurves[current_curve[0]].numControlPoints].circlePoints);
				circles[subcurves[current_curve[0]].numControlPoints].circleCenter = subcurves[current_curve[0]].controlPoints[subcurves[current_curve[0]].numControlPoints];
				subcurves[current_curve[0]].numControlPoints++;
			} else if (hoveredCircle > -1 && subcurves[current_curve[0]].numControlPoints > MINCONTROLPOINTS) {
				subcurves[current_curve[0]].numControlPoints--;
				for (i=hoveredCircle; i<subcurves[current_curve[0]].numControlPoints; i++){
					subcurves[current_curve[0]].controlPoints[i].x = subcurves[current_curve[0]].controlPoints[i+1].x;
					subcurves[current_curve[0]].controlPoints[i].y = subcurves[current_curve[0]].controlPoints[i+1].y;
					circles[i].circleCenter = circles[i+1].circleCenter;
				}
			}
			
			glutSetWindow(window3D);
			glutPostRedisplay();
			break;
		}
	}

	glutSetWindow(window2D);
	glutPostRedisplay();
}

void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON) {  
		if (hoveredCircle > -1) {
			screenToWorldCoordinates(xMouse, yMouse, &circles[hoveredCircle].circleCenter.x, &circles[hoveredCircle].circleCenter.y);
			screenToWorldCoordinates(xMouse, yMouse, &subcurves[current_curve[0]].controlPoints[hoveredCircle].x, &subcurves[current_curve[0]].controlPoints[hoveredCircle].y);
		}
	}    
	else if (currentButton == GLUT_MIDDLE_BUTTON) {
	}
	glutPostRedisplay();
}

void mouseHoverHandler(int xMouse, int yMouse)
{
	hoveredCircle = -1;
	GLdouble worldMouseX, worldMouseY;
	screenToWorldCoordinates(xMouse, yMouse, &worldMouseX, &worldMouseY);
	int i;
	// see if we're hovering over a circle
	for (i=0; i<subcurves[current_curve[0]].numControlPoints; i++){
		GLdouble distToX = worldMouseX - circles[i].circleCenter.x;
		GLdouble distToY = worldMouseY - circles[i].circleCenter.y;
		GLdouble euclideanDist = sqrt(distToX*distToX + distToY*distToY);
		//printf("Dist from point %d is %.2f\n", i, euclideanDist);
		if (euclideanDist < 2.0){
			hoveredCircle = i;
		}
	}
	
	glutPostRedisplay();
}

void cannonFire()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, bullet_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bullet_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bullet_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, bullet_mat_shininess);
	if (firing) 
	{
		glPushMatrix();
		
		glTranslatef(bulletx, 0, bulletz);
		glScalef(1, 1, 1.2);
		gluSphere(gluNewQuadric(), 0.2, 4, 4);
		glPopMatrix();
	}
}

void robotFire() 
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, bullet_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bullet_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bullet_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, bullet_mat_shininess);
	if (robotFiring) 
	{
		for (int i = 0; i <= 4; i++) 
		{
			if (botAlive[i])
			{
				glPushMatrix();
				glTranslatef(botbulletx[i], 0, botbulletz[i]);
				glScalef(0.5, 0.5, 0.5);
				gluSphere(gluNewQuadric(), 0.4, 6, 6);
				glPopMatrix();
			}
		}
	}
}

void reshape(int w, int h)
{
	glutWindowWidth = (GLsizei) w;
	glutWindowHeight = (GLsizei) h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(wvLeft, wvRight, wvBottom, wvTop);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/************************************************************************************
 *
 *
 * 3D Window Code 
 *
 * Fill in the code in the empty functions
 ************************************************************************************/
void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	//glScalef(robotBodyWidth, robotBodyLength,robotBodyLength);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -5);

	GLUquadricObj *body = gluNewQuadric();
	gluQuadricDrawStyle(body, GLU_FILL);
	gluCylinder(body, robotBodyWidth, robotBodyWidth, robotBodyLength, 30, 30);
	glPopMatrix();

	// Build bottom Sphere
	glPushMatrix();
	glTranslatef(0, -robotBodyLength + 5, 0);
	glScalef(robotBodyWidth*0.94, robotBodyWidth, robotBodyWidth);
	GLUquadricObj *botsphere = gluNewQuadric();
	gluSphere(botsphere,robotBodyWidth / 3, 20, 20);
	//glutSolidSphere(robotBodyWidth / 3, 20, 20);
	glPopMatrix();

	//Build Legs
	glPushMatrix();
	//glScalef(robotBodyWidth, robotBodyLength,robotBodyLength);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 2.7, -robotBodyLength * 0.75);

	GLUquadricObj *leg = gluNewQuadric();
	gluQuadricDrawStyle(leg, GLU_FILL);
	gluCylinder(leg, robotBodyWidth / 1.5, robotBodyWidth / 1.5, robotBodyLength*1.5, 30, 30);
	glPopMatrix();

	// Build Arm Spheres
	glPushMatrix();
	glTranslatef(0, 2.7, 0);
	glScalef(0.7, 1.1, 1.1);

	glPushMatrix();
	glTranslatef(-robotBodyLength, 0, 0);
	GLUquadricObj *shoulder1 = gluNewQuadric();
	gluSphere(shoulder1, robotBodyWidth / 1.5, 20, 20);
	//glutSolidSphere(robotBodyWidth / 1.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(robotBodyLength, 0, 0);
	GLUquadricObj *shoulder2 = gluNewQuadric();
	gluSphere(shoulder2, robotBodyWidth / 1.5, 20, 20);
	//glutSolidSphere(robotBodyWidth / 1.5, 20, 20);
	glPopMatrix();

	glPopMatrix();

	//Build Wheel Arms--Upper
	glPushMatrix();
	glRotatef(-65, 1, 0, 0);
	glScalef(1, 1, robotBodyWidth*1.5);
	glTranslatef(0, 2, -0.4);

	glPushMatrix();
	glTranslatef(-robotBodyLength * 0.68, 0, 0);
	glutSolidCube(robotBodyWidth / 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(robotBodyLength * 0.68, 0, 0);
	glutSolidCube(robotBodyWidth / 2);
	glPopMatrix();
	glPopMatrix();

	//Build Wheel Arms--Lower
	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glScalef(1, 1, robotBodyWidth*1.4);
	glTranslatef(0, -5.8, 0.6);

	glPushMatrix();
	glTranslatef(-robotBodyLength * 0.68, 0, 0);
	glutSolidCube(robotBodyWidth / 2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(robotBodyLength * 0.68, 0, 0);
	glutSolidCube(robotBodyWidth / 2);
	glPopMatrix();

	glPopMatrix();
}
void drawHead()
{


	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	// Position head with respect to parent (body)
	glTranslatef(0, 0.5*robotBodyLength + headLength, 0); // this will be done last
	glRotatef(90, 1, 0, 0);

	// Build Head Border
	glPushMatrix();
	glScalef(1.2*robotBodyWidth, 1.2*robotBodyWidth, 0.4*robotBodyWidth);
	glTranslatef(0, 0, robotBodyLength - 6.65);
	GLUquadricObj *head = gluNewQuadric();
	gluQuadricDrawStyle(head, GLU_FILL);
	gluCylinder(head, 0.88, 0.88, 1, 30, 30);
	glPopMatrix();


	// Build Head Sphere
	glPushMatrix();
	glTranslatef(0, 0, robotBodyLength - 7);
	glScalef(robotBodyWidth*0.94, robotBodyWidth, robotBodyWidth);
	GLUquadricObj *head2 = gluNewQuadric();
	gluSphere(head2,robotBodyWidth / 3, 20, 20);
	//glutSolidSphere(robotBodyWidth / 3, 20, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawLowerBody()
{
	glPushMatrix();
	// Position stanchion and base with respect to body
	glTranslatef(0, -1.5*robotBodyLength, 0.0); // this will be done last

	// Wheel
	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glRotatef(-wheelAngle, 1, 0, 0);
	glPushMatrix();
	glTranslatef(-wheelLength / 2, 0, 0);
	glScalef(wheelLength, wheelRadius, wheelRadius);
	//glRotatef(wheelAngle, 0, 0, 1);
	glRotatef(90, 0.0, 1.0, 0.0);
	GLUquadricObj *wheel = gluNewQuadric();
	gluCylinder(wheel, 1.0, 1.0, 1.0, 10, 10);
	glPopMatrix();
	glPopMatrix();

	//Wheel Caps
	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	glScalef(wheelLength, wheelRadius*0.8, wheelRadius*0.8);
	glRotatef(90, 0.0, 1, 0.0);
	GLUquadricObj *cap1 = gluNewQuadric();
	gluSphere(cap1, 1.0, 10.0, 10.0);
	//glutSolidSphere(1.0, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.9, 0, 0);
	glScalef(wheelLength / 10, wheelRadius, wheelRadius);
	glRotatef(90, 0.0, 1, 0.0);
	GLUquadricObj *cap2 = gluNewQuadric();
	gluSphere(cap2, 1.0, 30.0, 30.0);
	//glutSolidSphere(1.0, 30.0, 30.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(wheelLength - 2.6, 0, 0);
	glScalef(wheelLength / 10, wheelRadius, wheelRadius);
	glRotatef(90, 0.0, 1, 0.0);
	GLUquadricObj *cap3 = gluNewQuadric();
	gluSphere(cap3, 1.0, 30.0, 30.0);
	//glutSolidSphere(1.0, 30.0, 30.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}
void drawLeftArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0.5*robotBodyWidth + 0.5*upperArmWidth, 0, 0.0); // this will be done last

	// build arm
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}
void drawRightArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();

	// Rotate arm at shoulder
	glTranslatef(-(robotBodyWidth + 0.5*upperArmWidth), 0.25*upperArmLength - 1, 0.0);
	glRotatef(shoulderAngle, 1.0, 0.0, 0.0);
	glTranslatef((0.5*robotBodyWidth + 0.8*upperArmWidth), -0.5*upperArmLength, 0.0);

	// Position arm and gun with respect to parent body
	glTranslatef(-(0.7*robotBodyWidth + 0.6*upperArmWidth), 1.8, 0.0);

	// build arm
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	//  Gun
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	glPushMatrix();
	// rotate gun
	glTranslatef(-(0.5*robotBodyWidth + 0.5*upperArmWidth), -(0.5*upperArmLength), 0.0);
	glRotatef(angle * 180 / 3.14159265359, 0.0, 0.0, 1.0);
	glTranslatef((0.5*robotBodyWidth + 0.5*upperArmWidth), (0.5*upperArmLength), 0.0);

	// Position gun with respect to parent arm 
	glTranslatef(0, -(0.5*upperArmLength + 0.5*gunLength), 0.0);

	// build gun
	glScalef(gunWidth, gunLength, gunDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();


}
void drawRobot()
{
	glPushMatrix();
	// spin robot on base.
	drawBody();
	drawHead();
	drawLeftArm();
	drawRightArm();
	drawLowerBody();
	glPopMatrix();


	glPopMatrix();
}

void init3DSurfaceWindow()
{
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//vec3 light = 
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	eyeX, eyeY, eyeZ, //eye
				centerX, centerY, centerZ,  //center
				0.0, 1.0, 0.0); //up
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

}
void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei) w;
	glutWindowHeight = (GLsizei) h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,aspect,zNear,zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	eyeX, eyeY, eyeZ, //eye
				centerX, centerY, centerZ,  //center
				0.0, 1.0, 0.0); //up
}

void animationHandler(int param)
{
		//Cannon Fire
		if (firing)
		{
			bulletz -= 1;
			//Bullet has gone far enough
			if (bulletz <= -15)
			{
				firing = false;
				bulletz = 10;
			}
		}
		//Bullet Fire
		if (robotFiring)
		{
			for (int i = 0; i <= 4; i++) 
			{
				botbulletz[i] += 0.5;
				//Bullet Off screen
				if (botbulletz[i] >= 40)
				{
					botbulletx[i] = accumx[i];
					botbulletz[i] = accumz[i];
				}
			}
		}
		//Moving Bot
		if (!stop)
		{
			//Move all Bots
			for (int i = 0; i <= 4; i++)
			{
				robotFiring = true;
				computeSubdivisionCurve(&subcurves[current_curve[i]]);
				if ((subcurves[current_curve[i]].curvePoints[point[i]].x == subcurves[current_curve[i]].controlPoints[a_point[i]].x) && (subcurves[current_curve[i]].curvePoints[point[i]].y == subcurves[current_curve[i]].controlPoints[a_point[i]].y)) 
				{
					a_point[i] += 1; 
				}


				if (point[i] < subcurves[current_curve[i]].numCurvePoints - 1) {
					point[i] += 1;
					anglewheel += 12;
					angle = -atan((subcurves[current_curve[i]].curvePoints[point[i] + 1].x - subcurves[current_curve[i]].curvePoints[point[i]].x) / (subcurves[current_curve[i]].curvePoints[point[i] + 1].y - subcurves[current_curve[i]].curvePoints[point[i]].y));
				}

				accumx[i] = subcurves[current_curve[i]].curvePoints[point[i]].x;
				accumz[i] = subcurves[current_curve[i]].curvePoints[point[i]].y;
			}
			//Give new paths to all robots when all are done moving
			for (int i = 0; i <= 4; i++)
			{
				if (point[i] == subcurves[current_curve[i]].numCurvePoints - 1)
				{
					point[i] = 0;
					a_point[i] = 0;
					//Determine next curves
					
					subcurves[i] = initSubdivisionCurve(subcurves[i], i);
					subcurves[i] = initControlPoints(subcurves[i]);
			
			}
		}
	}
		if (firing || !stop) 
		{
			glutPostRedisplay();
			glutTimerFunc(75, animationHandler, 75);
		}
}

void display3D()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_LIGHT0);

	if (firstperson) {
		gluLookAt(0, 6, 22, //eye
			0, 0, 0,   //center
			0.0, 1.0, 0.0); //up
	}
	else {
		gluLookAt(cannon, 0.5, 10.5, //eye
			cannon, 0, -11,   //center
			0.0, 1.0, 0.0); //up	
	}

	drawGround();
	//draw3DSubdivisionCurve();
	//draw3DControlPoints();
	botHit();
	cannonHit();
	drawBot();
	drawCannon();
	cannonFire();
	robotFire();

	char text[] = "Help:h Movement:Left/Right Arrow Start:a Reset:r Shoot:f Camera:v";
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	glColor3f(0, 0, 0);
	glRasterPos2f(-20, -13);
	int len, i;
	len = (int)strlen(text);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glutSwapBuffers();
}

GLfloat line_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat line_mat_specular[] = { 0.55f,0.45f,0.45f,1.0f };
GLfloat line_mat_diffuse[] = { 0.35f,0.1f,0.1f,1.0f };

void draw3DSubdivisionCurve() 
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	// Subdivide the given curve
	computeSubdivisionCurve(&subcurves[current_curve[0]]);
	computeRunningDistances(&subcurves[current_curve[0]]);
	computeNormalVectors(&subcurves[current_curve[0]]);
	int i = 0;
	glPushMatrix();
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, line_mat_ambient);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, line_mat_diffuse);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < subcurves[current_curve[0]].numCurvePoints; i++) {
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(subcurves[current_curve[0]].curvePoints[i].x, 0.1 ,-(subcurves[current_curve[0]].curvePoints[i].y)) ;
	}
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void draw3DControlPoints()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	int i, j;
	for (i = 0; i < subcurves[current_curve[0]].numControlPoints; i++) {
		glPushMatrix();
		glColor3f(0.0f, 0.0f, 1.0f);
		glTranslatef(circles[i].circleCenter.x, 0.1 , -(circles[i].circleCenter.y));
		glBegin(GL_LINE_LOOP);
		for (j = 0; j < numCirclePoints; j++) {
			glVertex3f(circles[i].circlePoints[j].x, circles[i].circlePoints[j].y, 0);
		}
		glColor3f(0.0f, 0.0f, 1.0f);
		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void drawBot() 
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, robotBody_mat_shininess);
	if (botType == WHEEL)
	{
		for (int i = 0; i <= 4; i++)
		{
			if (botAlive[i]) 
			{
				glPushMatrix();
				glTranslatef(accumx[i], 1, -(accumz[i]));
				glRotatef(angle, 0.0, 1.0, 0.0);

				glScalef(botScale, botScale, botScale);
				drawRobot();
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

void drawCannon() 
{
	if (playerLive) 
	{
		glPushMatrix();
		glTranslatef(cannon, -1, 11);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(1, 1, 20, 20);

		glPushMatrix();
		glScalef(1,4,1);
		glTranslatef(0, 0.2, 1.2);
		gluSphere(gluNewQuadric(),0.2,10,10);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0, 1.3);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

		glPushMatrix();
		gluCylinder(gluNewQuadric(), 0.5, 0.5, 1.2, 10, 10);
		glPopMatrix();

		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glTranslatef(cannon, -1, 11);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(1, 0.5, 10, 10);

		glPushMatrix();
		glRotatef(45, 0, 1, -2);
		glScalef(1, 4, 1);
		glTranslatef(0, 0.2, 0.5);
		gluSphere(gluNewQuadric(), 0.2, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0, 0.7);
		glScalef(1, 1, 0.8);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

		glPushMatrix();
		gluCylinder(gluNewQuadric(), 0.8, 0.7, 0.7, 5, 5);
		glPopMatrix();

		glPopMatrix();
	}
}

void botHit()
{
	for (int i = 0; i <= 4; i++) 
	{
		if (accumx[i] + robotBodyWidth / 20 >= bulletx && accumx[i] - robotBodyWidth / 20 <= bulletx*botScale && firing)
		{
			if (-accumz[i] + robotBodyWidth / 4 >= bulletz && -accumz[i] - robotBodyWidth / 4 <= bulletz)
			{
				botAlive[i] = false;
			}
		}
	}
}

void cannonHit() 
{
	for (int i = 0; i <= 4; i++)
	{
		if ((cannon - 0.5 <= botbulletx[i]) && (cannon + 0.5 >= botbulletx[i]) && botAlive[i])
		{
			if (botbulletz[i] > 10.5 && botbulletz[i] < 11.5) 
			{
				playerLive = false;
			}
		}
	}
}

void drawGround() {

	glEnable(GL_TEXTURE_2D);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0, 1, 0);
	glVertex3f(-26.0f, -1.0f, -26.0f);
	glVertex3f(-26.0f, -1.0f, 26.0f);
	glVertex3f(26.0f, -1.0f, 26.0f);
	glVertex3f(26.0f, -1.0f, -26.0f);
	glEnd();
	glPopMatrix();
}

void keyboardHandler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 
		exit(0);
		break;
	case 'a':
		if (stop){
		stop = false;
		for (int i = 0; i <= 4; i++)
		{
			point[i] = 0;
			a_point[i] = 0;
		}
		glutTimerFunc(120, animationHandler, 120);
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
		}
	case 'r':
		if (!stop)
		{
			playerLive = true;
			stop = true;
			for (int i = 0; i <= 4; i++)
			{
				robotFiring = false;
				botAlive[i] = true;
				accumx[i] = subcurves[current_curve[0]].controlPoints[0].x;
				accumz[i] = subcurves[current_curve[0]].controlPoints[0].y;
				angle = -atan((subcurves[current_curve[0]].curvePoints[1].y - subcurves[current_curve[0]].curvePoints[0].y) / (subcurves[current_curve[0]].curvePoints[1].x - subcurves[current_curve[0]].curvePoints[0].x));
				botbulletx[i] = accumx[i];
				botbulletz[i] = accumz[i];
				point[i] = 0;
				a_point[i] = 0;
			}

			botType = WHEEL;
			glutSetWindow(window3D);
			glutPostRedisplay();
			break;
		}
	/*
	case 'c':
		botType = CUBE;
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	case 's':
		botType = SPHERE;
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	*/
	case 'w':
		botType = WHEEL;
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	default:
		break;
	case 'f':
		if (!firing && !stop && playerLive)
		{
			firing = true;
			bulletx = cannon;
		}
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	case 'v':
		firstperson = !firstperson;
		break;
	case 'h':
		printf("CPS511 ASSIGNMENT 3\nJEFFREY TOMINES 500960477\n-----------------------------------------");
		printf("CONTROLS:\nMove Left/Right: Left/Right Arrow Keys\nStart Animation:a\nReset Animation:r\n");
		printf("Shoot Cannon: f\nChange Camera: v\n Quit Game:q,Q,esc");
	}
	glutPostRedisplay();
}

void specialKeyHandler(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		if (playerLive) 
		{
			cannon -= 0.5;
			if (!firing) bulletx = cannon;
			if (cannon < -8) cannon = -8;
		}
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		if (playerLive)
		{
			cannon += 0.5;
			if (!firing) bulletx = cannon;
			if (cannon > 8) cannon = 8;
		}
		glutSetWindow(window3D);
		glutPostRedisplay();
		break;
	}
	glutPostRedisplay();
}

void mouseButtonHandler3D(int button, int state, int x, int y)
{

	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		
		break;
	case GLUT_MIDDLE_BUTTON:
		
		break;
	default:
		break;
	}
}

void mouseMotionHandler3D(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;
	if (currentButton == GLUT_LEFT_BUTTON) {
		;
	}
	if (currentButton == GLUT_RIGHT_BUTTON)
	{
		if (dx > 0 && eyeY > 0) {
			eyeY -= 1;
			eyeZ += 2;
		}
		else if (dx < 0 && eyeY < 10) {
			eyeY += 1;
			eyeZ -= 2;
		}
	}
	else if (currentButton == GLUT_MIDDLE_BUTTON) 
	{
	}
	lastMouseX = x;
	lastMouseY = y;
	glutPostRedisplay();
}

// Some Utility Functions

Vector3D crossProduct(Vector3D a, Vector3D b){
	Vector3D cross;
	
	cross.x = a.y * b.z - b.y * a.z;
	cross.y = a.x * b.z - b.x * a.z;
	cross.z = a.x * b.y - b.x * a.y;
	
	return cross;
}

Vector3D normalize(Vector3D a){
	GLdouble norm = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	Vector3D normalized;
	normalized.x = a.x/norm;
	normalized.y = a.y/norm;
	normalized.z = a.z/norm;
	return normalized;
}





