
GLfloat vertices[] = {
	 2.5f, 2.5f, 2.5f,  -2.5f, 2.5f, 2.5f,  -2.5f,-2.5f, 2.5f,  2.5f,-2.5f, 2.5f,   // v0,v1,v2,v3 (front)
	 2.5f, 2.5f, 2.5f,   2.5f,-2.5f, 2.5f,   2.5f,-2.5f,-2.5f,  2.5f, 2.5f,-2.5f,   // v0,v3,v4,v5 (right)
	 2.5f, 2.5f, 2.5f,   2.5f, 2.5f,-2.5f,  -2.5f, 2.5f,-2.5f, -2.5f, 2.5f, 2.5f,   // v0,v5,v6,v1 (top)
	-2.5f, 2.5f, 2.5f,  -2.5f, 2.5f,-2.5f,  -2.5f,-2.5f,-2.5f, -2.5f,-2.5f, 2.5f,   // v1,v6,v7,v2 (left)
	-2.5f,-2.5f,-2.5f,   2.5f,-2.5f,-2.5f,   2.5f,-2.5f, 2.5f, -2.5f,-2.5f, 2.5f,   // v7,v4,v3,v2 (bottom)
	 2.5f,-2.5f,-2.5f,  -2.5f,-2.5f,-2.5f,  -2.5f, 2.5f,-2.5f,  2.5f, 2.5f,-2.5f    // v4,v7,v6,v5 (back)
};

GLfloat quadNormals[] = {
	 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,  // v0,v1,v2,v3 (front)
	 1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
	 0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,  // v0,v5,v6,v1 (top)
	-1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  // v1,v6,v7,v2 (left)
	 0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,  // v7,v4,v3,v2 (bottom)
	 0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1   // v4,v7,v6,v5 (back)
};

GLfloat colours[] = {
	 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,  // v0,v1,v2,v3 (front)
	 1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,  // v0,v3,v4,v5 (right)
	 1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,  // v0,v5,v6,v1 (top)
	 1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,  // v1,v6,v7,v2 (left)
	 0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,  // v7,v4,v3,v2 (bottom)
	 0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1   // v4,v7,v6,v5 (back)
};

GLuint indices[] = {
	 0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
	 4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
	 8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
	12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
	16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
	20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
};

//VBO ID
GLuint vboId;
GLuint iboId;

typedef struct CubeMesh
{
	VECTOR3D center;
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of cube coordinate system
} CubeMesh;

CubeMesh *createCubeMesh()
{
  CubeMesh *newCube = (CubeMesh*)calloc(1, sizeof(CubeMesh));
  
  newCube->angle = 0.0;
  newCube->sfx = newCube->sfy = newCube->sfz = 1.0;
  newCube->tx = 0.0;
  newCube->ty = 0.0;
  newCube->tz = 0.0;
 
  return newCube;
}

void drawCubeMesh(CubeMesh *cube)
{
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);

  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  glNormalPointer(GL_FLOAT, 0, (char*)sizeof(vertices));
  glColorPointer(3, GL_FLOAT, 0, (char*)(sizeof(vertices) + sizeof(quadNormals)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  /*

  // Setup the material and lights used for the cube
  // you may want to use the alternate material if this cube is selected
  glMaterialfv(GL_FRONT, GL_AMBIENT, cube->mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, cube->mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, cube->mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, cube->mat_shininess);
  
  // Transform Cube
  // you need to add code here
 
  // Draw Cube using simple immediate mode rendering
  glBegin(GL_QUADS);
  // Back Face
  glNormal3f(quadNormals[0][0],quadNormals[0][1],quadNormals[0][2]); 
  glVertex3f(vertices[quads[0]][0],vertices[quads[0]][1],vertices[quads[0]][2]);
  glVertex3f(vertices[quads[1]][0],vertices[quads[1]][1],vertices[quads[1]][2]);
  glVertex3f(vertices[quads[2]][0],vertices[quads[2]][1],vertices[quads[2]][2]);
  glVertex3f(vertices[quads[3]][0],vertices[quads[3]][1],vertices[quads[3]][2]);
  // Top Face
  glNormal3f(quadNormals[1][0],quadNormals[1][1],quadNormals[1][2]);
  glVertex3f(vertices[quads[4]][0],vertices[quads[4]][1],vertices[quads[4]][2]);
  glVertex3f(vertices[quads[5]][0],vertices[quads[5]][1],vertices[quads[5]][2]);
  glVertex3f(vertices[quads[6]][0],vertices[quads[6]][1],vertices[quads[6]][2]);
  glVertex3f(vertices[quads[7]][0],vertices[quads[7]][1],vertices[quads[7]][2]);
  // Left Face
  glNormal3f(quadNormals[2][0],quadNormals[2][1],quadNormals[2][2]);
  glVertex3f(vertices[quads[8]][0],vertices[quads[8]][1],vertices[quads[8]][2]);
  glVertex3f(vertices[quads[9]][0],vertices[quads[9]][1],vertices[quads[9]][2]);
  glVertex3f(vertices[quads[10]][0],vertices[quads[10]][1],vertices[quads[10]][2]);
  glVertex3f(vertices[quads[11]][0],vertices[quads[11]][1],vertices[quads[11]][2]);
  // Right Face
  glNormal3f(quadNormals[3][0],quadNormals[3][1],quadNormals[3][2]);
  glVertex3f(vertices[quads[12]][0],vertices[quads[12]][1],vertices[quads[12]][2]);
  glVertex3f(vertices[quads[13]][0],vertices[quads[13]][1],vertices[quads[13]][2]);
  glVertex3f(vertices[quads[14]][0],vertices[quads[14]][1],vertices[quads[14]][2]);
  glVertex3f(vertices[quads[15]][0],vertices[quads[15]][1],vertices[quads[15]][2]);
  // Front Face
  glNormal3f(quadNormals[4][0],quadNormals[4][1],quadNormals[4][2]);
  glVertex3f(vertices[quads[16]][0],vertices[quads[16]][1],vertices[quads[16]][2]);
  glVertex3f(vertices[quads[17]][0],vertices[quads[17]][1],vertices[quads[17]][2]);
  glVertex3f(vertices[quads[18]][0],vertices[quads[18]][1],vertices[quads[18]][2]);
  glVertex3f(vertices[quads[19]][0],vertices[quads[19]][1],vertices[quads[19]][2]);
  // Bottom Face
  glNormal3f(quadNormals[5][0],quadNormals[5][1],quadNormals[5][2]);
  glVertex3f(vertices[quads[20]][0],vertices[quads[20]][1],vertices[quads[20]][2]);
  glVertex3f(vertices[quads[21]][0],vertices[quads[21]][1],vertices[quads[21]][2]);
  glVertex3f(vertices[quads[22]][0],vertices[quads[22]][1],vertices[quads[22]][2]);
  glVertex3f(vertices[quads[23]][0],vertices[quads[23]][1],vertices[quads[23]][2]);
  glEnd();
 */
}
	