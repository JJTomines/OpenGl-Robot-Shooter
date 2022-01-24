/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "cube.h"
#include "QuadMesh.h"

const int vWidth  = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Forward and Movement Vectors
VECTOR3D lookVector(0.0f,0.0f,1.0f);
float accum_forwardZ = 0.0;
float accum_forwardX = 0.0;
float working = 0.0;
float newX = 0;
float newZ = 0;

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 3.0;
float robotBodyLength = 6.0;
float robotBodyDepth = 10.0;
float headWidth = 0.4*robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength/2;
float upperArmWidth = 0.3*robotBodyWidth;
float gunLength = 3;
float gunWidth = upperArmWidth*0.9;
float gunDepth = upperArmWidth;
float wheelLength = robotBodyLength/1.1;
float wheelRadius = robotBodyLength/1.5;
float baseWidth = 2 * robotBodyWidth;
float baseLength = 0.25*wheelLength;

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = -40.0;
float gunAngle = -25.0;

// Spin Cube Mesh
float cubeAngle = 0.0;
float cubeScale = 1.0;
// Control Wheel Spinning
float wheelAngle = 0.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
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

// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Mouse button
int currentButton;

// A template cube mesh
CubeMesh *cubeMesh = createCubeMesh();

// A flat open mesh
QuadMesh *groundMesh = NULL;

// Default Mesh Size
int meshSize = 64;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void drawRobot();
void drawBody();
void drawHead();
void drawLowerBody();
void drawLeftArm();
void drawRightArm();

int main(int argc, char **argv)
{
	
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Jeffrey Tomines Assignment 1");
	printf("PRESS F1 FOR CONTROLS");
	// Initialize GL
	initOpenGL(vWidth, vHeight);
	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	
	//VBO ID
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(quadNormals) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(quadNormals), quadNormals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(quadNormals), sizeof(colours), colours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//IBO ID
	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	glDeleteBuffers(1, &vboId);
	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{

	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw Robot
	drawRobot();

	// Example of drawing a mesh (closed cube mesh)
	glPushMatrix();
	// spin cube
	glTranslatef(10.0, 0, 6.0);
	glRotatef(-cubeAngle, 1.5, 1.0, 1.0);
	glScalef(sin(cubeScale), sin(cubeScale), sin(cubeScale));
	glTranslatef(-10.0, 0, -6.0);
	// position and draw cube
	glTranslatef(10.0, 0, 6.0); 
	drawCubeMesh(cubeMesh);
	glPopMatrix();
	
	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -20.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawRobot()
{
	glPushMatrix();
	 // spin robot on base.
	 glTranslatef(accum_forwardX,0,accum_forwardZ);
	 glRotatef(robotAngle, 0.0, 1.0, 0.0);
	 drawBody();
	 drawHead();
	 drawLeftArm();
	 drawRightArm();
	 drawLowerBody();
	glPopMatrix();
	

	glPopMatrix();
}


void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	//glScalef(robotBodyWidth, robotBodyLength,robotBodyLength);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0,0,-5);

	GLUquadricObj *body;
	body = gluNewQuadric();
	gluQuadricDrawStyle(body, GLU_FILL);
	gluCylinder(body, robotBodyWidth, robotBodyWidth, robotBodyLength, 30, 30);
	glPopMatrix();

	// Build bottom Sphere
	glPushMatrix();
	glTranslatef(0, -robotBodyLength + 5, 0);
	glScalef(robotBodyWidth*0.94, robotBodyWidth, robotBodyWidth);
	glutSolidSphere(robotBodyWidth/3, 20, 20);
	glPopMatrix();

	//Build Legs
	glPushMatrix();
	//glScalef(robotBodyWidth, robotBodyLength,robotBodyLength);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 2.7, -robotBodyLength*0.75);

	GLUquadricObj *leg;
	leg = gluNewQuadric();
	gluQuadricDrawStyle(leg, GLU_FILL);
	gluCylinder(leg, robotBodyWidth/1.5, robotBodyWidth/1.5, robotBodyLength*1.5, 30, 30);
	glPopMatrix();
	  
	// Build Arm Spheres
	glPushMatrix();
	glTranslatef(0,2.7,0);
	glScalef(0.7, 1.1, 1.1);

	glPushMatrix();
	glTranslatef(-robotBodyLength,0,0);
	glutSolidSphere(robotBodyWidth / 1.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(robotBodyLength, 0, 0);
	glutSolidSphere(robotBodyWidth / 1.5, 20, 20);
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
	glTranslatef(0, 0.5*robotBodyLength+headLength, 0); // this will be done last
	glRotatef(90, 1, 0, 0);
	
	// Build Head Border
	glPushMatrix();
	glScalef(1.2*robotBodyWidth, 1.2*robotBodyWidth, 0.4*robotBodyWidth);
	glTranslatef(0, 0,robotBodyLength-6.65);
	GLUquadricObj *head;
	head = gluNewQuadric();
	gluQuadricDrawStyle(head, GLU_FILL);
	gluCylinder(head,0.88,0.88,1,30,30);
	glPopMatrix();
	

	// Build Head Sphere
	glPushMatrix();
	glTranslatef(0,0, robotBodyLength-7);
	glScalef(robotBodyWidth*0.94, robotBodyWidth, robotBodyWidth);
	glutSolidSphere(robotBodyWidth/3,20,20);
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
	glTranslatef(-wheelLength/2, 0, 0);
	glScalef(wheelLength, wheelRadius, wheelRadius);
	//glRotatef(wheelAngle, 0, 0, 1);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 10, 10);
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
	glutSolidSphere( 1.0, 10.0, 10.0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-2.9, 0, 0);
	glScalef(wheelLength/10, wheelRadius, wheelRadius);
	glRotatef(90, 0.0, 1, 0.0);
	glutSolidSphere(1.0, 30.0, 30.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(wheelLength-2.6, 0, 0);
	glScalef(wheelLength / 10, wheelRadius, wheelRadius);
	glRotatef(90, 0.0, 1, 0.0);
	glutSolidSphere(1.0, 30.0, 30.0);
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
	glRotatef(gunAngle, 1.0, 0.0, 0.0);
	glTranslatef((0.5*robotBodyWidth + 0.5*upperArmWidth), (0.5*upperArmLength ), 0.0);
	
	// Position gun with respect to parent arm 
	glTranslatef(0, -(0.5*upperArmLength + 0.5*gunLength), 0.0);

	// build gun
	glScalef(gunWidth, gunLength, gunDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();


}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'd':
		robotAngle += 5.5;
		working = (-5.5 * 3.14159265 /180) ;

		newX = lookVector.GetX() * cos(working) - lookVector.GetZ() * sin(working);
		newZ = lookVector.GetX() * sin(working) + lookVector.GetZ() * cos(working);
		lookVector.SetX(newX);
		lookVector.SetZ(newZ);

		break;
	case 'a':
		robotAngle -= 5.5;
		working = (5.5 * 3.14159265 / 180);

		newX = lookVector.GetX() * cos(working) - lookVector.GetZ() * sin(working);
		newZ = lookVector.GetX() * sin(working) + lookVector.GetZ() * cos(working);
		lookVector.SetX(newX);
		lookVector.SetZ(newZ);

		break;
	case 'q':
		if (shoulderAngle > 10) 
			shoulderAngle = 10.0;
		else
			shoulderAngle += 1.0;
		break;
	case 'e':
		if (shoulderAngle < -60)
			shoulderAngle = -60.0;
		else
			shoulderAngle -= 1.0;
		break;
	case 'g':
		if (gunAngle > -10)
			gunAngle = -10.0;
		else
			gunAngle += 2.0;
		break;
	case 'G':
		if (gunAngle < -35)
			gunAngle = -35.0;
		else
			gunAngle -= 2.0;
		break;
	case 'v':
		stop = false;
		glutTimerFunc(10, animationHandler, 0);
		break;
	case 'V':
		stop = true;
		break;
	case 'w':
		wheelAngle -= 6.0;
		accum_forwardZ += lookVector.GetZ()*0.3;
		accum_forwardX += lookVector.GetX()*0.3;
		break;
	case 's':
		wheelAngle += 6.0;
		accum_forwardZ -= lookVector.GetZ()*0.3;
		accum_forwardX -= lookVector.GetX()*0.3;
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


void animationHandler(int param)
{
	if (!stop)
	{
		cubeAngle += 2.0;
		cubeScale += 0.02;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("CPS511 Assignment 1\nJeffrey Tomines 500960477\n-------------\nCONTROLS\n");
		printf("w: MOVE FORWARD\ns: MOVE BACKWARD\nd: TURN CCW\na: TURN CW\n");
		printf("q: ARM DOWN\ne: ARM UP\ng: GUN DOWN\nG: GUN UP\n");
		printf("v: ANIMATE/SPEED UP CUBE\nV: STOP CUBE");

	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

