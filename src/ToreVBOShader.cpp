/********************************************************/
/*                     CubeVBOShader.cpp                         */
/********************************************************/
/* Premiers pas avec OpenGL.                            */
/* Objectif : afficher a l'ecran uncube avec ou sans shader    */
/********************************************************/

/* inclusion des fichiers d'en-tete Glut */
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include "shader.hpp"
#include <string.h>
// #include "../utilstexture/sdlglutils.h"

// Include GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
using namespace glm;
using namespace std;

#define P_SIZE 3
#define N_SIZE 3 // necessarily 3
#define C_SIZE 3

#define N_VERTS 8
#define N_VERTS_BY_FACE 3
#define N_FACES 12

#define NB_R 40
#define NB_r 20
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLfloat vertices[(NB_R + 1) * (NB_r + 1) * 3];     // x 3 coordonnées (+1 acr on double les dernierspoints pour avoir des coord de textures <> pour les points de jonctions)
GLuint indexes[NB_R * NB_r * 6];                   // x6 car pour chaque face quadrangulaire on a 6 indexes (2 triangles=2x 3 indexes)
GLfloat coordTexture[(NB_R + 1) * (NB_r + 1) * 2]; // x 2 car U+V par vertices
GLfloat normals[(NB_R + 1) * (NB_r + 1) * 3];

// initializations

void genVBO();
void deleteVBO();
void drawObject();

// glut callback functions
void display();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);
void reshape(int, int);
// misc
void drawString(const char *str, int x, int y, float color[4], void *font);
void showInfo();
void *font = GLUT_BITMAP_8_BY_13; // to display 2D texts on the screen
// global variables for OpenGL
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance = 0.;

// Handle variables of opengl
//--------------------------
GLuint programID;                                                     // handle for the shader
GLuint MatrixIDMVP, MatrixIDView, MatrixIDModel, MatrixIDPerspective; // handle for the MVP Matrix
GLuint VBO_vertices, VBO_normals, VBO_indexes, VBO_UVtext, VAO;
GLuint locCameraPosition;
GLuint locmaterialShininess;
GLuint locmaterialSpecularColor;
GLuint locLightPosition;
GLuint locLightIntensities; // a.k.a the color of the light
GLuint locLightAttenuation;
GLuint locLightAmbientCoefficient;

// VBO's location
//------------------
GLuint indexVertex = 0, indexUVTexture = 2, indexNormale = 3;

// variable for lighting param
//--------------------------------------
vec3 cameraPosition(0., 0., 3.);
// material
//---------------
GLfloat materialShininess = 3.;
vec3 materialSpecularColor(1., .1, 1); // material color

// light
//-----------
vec3 LightPosition(1., 0., .5);
vec3 LightIntensities(1., 1., 1.); // light color
GLfloat LightAttenuation = 1.;
GLfloat LightAmbientCoefficient = .1;

glm::mat4 MVP;                     
glm::mat4 Model, View, Projection; // Matrixes building MVP

int screenHeight = 500;
int screenWidth = 500;

// for the texture
//-------------------
GLuint image;
GLuint bufTexture, bufNormalMap;
GLuint locationTexture, locationNormalMap;
//-------------------------
void createTorus(float R, float r)
{
  float theta, phi;
  theta = ((float)radians(360.f)) / ((float)NB_R);
  phi = ((float)(radians(360.f))) / ((float)NB_r);

  float pasU, pasV;
  pasU = 1. / NB_R;
  pasV = 1. / NB_r;
  for (int i = 0; i <= NB_R; i++)
    for (int j = 0; j <= NB_r; j++)
    {
      float a, b, c;
      vertices[(i * (NB_r + 1) * 3) + (j * 3)] = (R + r * cos((float)j * phi)) * cos((float)i * theta);     // x
      vertices[(i * (NB_r + 1) * 3) + (j * 3) + 1] = (R + r * cos((float)j * phi)) * sin((float)i * theta); // y
      vertices[(i * (NB_r + 1) * 3) + (j * 3) + 2] = r * sin((float)j * phi);

      normals[(i * (NB_r + 1) * 3) + (j * 3)] = cos((float)j * phi) * cos((float)i * theta);     // x
      normals[(i * (NB_r + 1) * 3) + (j * 3) + 1] = cos((float)j * phi) * sin((float)i * theta); // y
      normals[(i * (NB_r + 1) * 3) + (j * 3) + 2] = sin((float)j * phi);

      coordTexture[(i * (NB_r + 1) * 2) + (j * 2)] = ((float)i) * pasV;
      coordTexture[(i * (NB_r + 1) * 2) + (j * 2) + 1] = ((float)j) * pasV;
    }

  int indexMaxI = ((NB_R + 1) * (NB_r)) - 1;
  int indexMaxJ = (NB_r + 1);

  for (int i = 0; i < NB_R; i++)
    for (int j = 0; j < NB_r; j++)
    {
      int i0, i1, i2, i3, i4, i5;
      indexes[(i * NB_r * 6) + (j * 6)] = (unsigned int)((i * (NB_r + 1)) + j);
      indexes[(i * NB_r * 6) + (j * 6) + 1] = (unsigned int)((i + 1) * (NB_r + 1) + (j));
      indexes[(i * NB_r * 6) + (j * 6) + 2] = (unsigned int)(((i + 1) * (NB_r + 1)) + (j + 1));
      indexes[(i * NB_r * 6) + (j * 6) + 3] = (unsigned int)((i * (NB_r + 1)) + j);
      indexes[(i * NB_r * 6) + (j * 6) + 4] = (unsigned int)(((i + 1) * (NB_r + 1)) + (j + 1));
      indexes[(i * NB_r * 6) + (j * 6) + 5] = (unsigned int)(((i) * (NB_r + 1)) + (j + 1));
    }
}

//----------------------------------------
GLubyte *glmReadPPM(char *filename, int *width, int *height)
//----------------------------------------
{
  FILE *fp;
  int i, w, h, d;
  unsigned char *image;
  char head[70]; /* max line <= 70 in PPM (per spec). */

  fp = fopen(filename, "rb");
  if (!fp)
  {
    perror(filename);
    return NULL;
  }

  /* grab first two chars of the file and make sure that it has the
     correct magic cookie for a raw PPM file. */
  fgets(head, 70, fp);
  if (strncmp(head, "P6", 2))
  {
    fprintf(stderr, "%s: Not a raw PPM file\n", filename);
    return NULL;
  }

  /* grab the three elements in the header (width, height, maxval). */
  i = 0;
  while (i < 3)
  {
    fgets(head, 70, fp);
    if (head[0] == '#') /* skip comments. */
      continue;
    if (i == 0)
      i += sscanf(head, "%d %d %d", &w, &h, &d);
    else if (i == 1)
      i += sscanf(head, "%d %d", &h, &d);
    else if (i == 2)
      i += sscanf(head, "%d", &d);
  }

  /* grab all the image data in one fell swoop. */
  image = new unsigned char[w * h * 3];
  fread(image, sizeof(unsigned char), w * h * 3, fp);
  fclose(fp);

  *width = w;
  *height = h;
  return image;
}

//----------------------------------------
void initTexture(void)
//-----------------------------------------
{
  int iwidth, iheight;
  GLubyte *image = NULL;

  image = glmReadPPM("../texture/Metalcolor.ppm", &iwidth, &iheight);
  glGenTextures(1, &bufTexture);
  glBindTexture(GL_TEXTURE_2D, bufTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  locationTexture = glGetUniformLocation(programID, "myTextureSampler"); // et il y a la texture elle même
                                                                         //   glBindAttribLocation(programID,indexUVTexture,"vertexUV");	// il y a les coord UV
}
//----------------------------------------
void initOpenGL(void)
//----------------------------------------
{
  glCullFace(GL_BACK);    // to remove the rear faces
  glEnable(GL_CULL_FACE); // activate face illumination
  glEnable(GL_DEPTH_TEST);
  // the shader
  programID = LoadShaders("PhongShader.vert", "PhongShader.frag");

  // Get  handles for our matrix transformations "MVP" VIEW  MODELuniform
  MatrixIDMVP = glGetUniformLocation(programID, "MVP");
  // MatrixIDView = glGetUniformLocation(programID, "VIEW");
  // MatrixIDModel = glGetUniformLocation(programID, "MODEL");
  // MatrixIDPerspective = glGetUniformLocation(programID, "PERSPECTIVE");

  // Projection matrix : 65 Field of View, 1:1 ratio, display range : 1 unit <-> 1000 units
  // ATTENTIOn l'angle est donné en radians si f GLM_FORCE_RADIANS est défini sinon en degré
  Projection = glm::perspective(glm::radians(60.f), 1.0f, 1.0f, 1000.0f);

  /* on recupere l'ID */
  locCameraPosition = glGetUniformLocation(programID, "cameraPosition");
  /*
  locmaterialShininess = glGetUniformLocation(programID, "materialShininess");
  locmaterialSpecularColor = glGetUniformLocation(programID, "materialSpecularColor");
  locLightPosition = glGetUniformLocation(programID, "light.position");
  locLightIntensities = glGetUniformLocation(programID, "light.intensities");//a.k.a the color of the light
  locLightAttenuation = glGetUniformLocation(programID, "light.attenuation");
  locLightAmbientCoefficient = glGetUniformLocation(programID, "light.ambientCoefficient");
  */
}
//----------------------------------------
int main(int argc, char **argv)
//----------------------------------------
{

  /* initialisation de glut et creation
     de la fenetre */

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(screenWidth, screenHeight);
  glutCreateWindow("CUBE VBO SHADER ");

  // Initialize GLEW
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  // info version GLSL
  std::cout << "***** Info GPU *****" << std::endl;
  std::cout << "Fabricant : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Carte graphique: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Version GLSL : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
            << std::endl;

  initOpenGL();

  createTorus(1., .3);

  // construction des VBO a partir des tableaux du cube deja construit
  genVBO();
  initTexture();

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);

  /* Entree dans la boucle principale glut */
  glutMainLoop();

  glDeleteProgram(programID);
  deleteVBO();
  return 0;
}

void genVBO()
{
  glGenBuffers(1, &VAO);
  glBindVertexArray(VAO); // ici on bind le VAO , c'est lui qui recupèrera les configurations des VBO glVertexAttribPointer , glEnableVertexAttribArray...

  if (glIsBuffer(VBO_vertices) == GL_TRUE)
    glDeleteBuffers(1, &VBO_vertices);
  glGenBuffers(1, &VBO_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(indexVertex, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  if (glIsBuffer(VBO_normals) == GL_TRUE)
    glDeleteBuffers(1, &VBO_normals);
  glGenBuffers(1, &VBO_normals);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
  glVertexAttribPointer(indexNormale, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  if (glIsBuffer(VBO_indexes) == GL_TRUE)
    glDeleteBuffers(1, &VBO_indexes);
  glGenBuffers(1, &VBO_indexes); // ATTENTIOn IBO doit etre un GL_ELEMENT_ARRAY_BUFFER
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indexes);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

  if (glIsBuffer(VBO_UVtext) == GL_TRUE)
    glDeleteBuffers(1, &VBO_UVtext);
  glGenBuffers(1, &VBO_UVtext);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_UVtext);
  glBufferData(GL_ARRAY_BUFFER, sizeof(coordTexture), coordTexture, GL_STATIC_DRAW);
  glVertexAttribPointer(indexUVTexture, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glEnableVertexAttribArray(indexVertex);
  glEnableVertexAttribArray(indexNormale);
  glEnableVertexAttribArray(indexUVTexture);

  // une fois la config terminée
  // on désactive le dernier VBO et le VAO pour qu'ils ne soit pas accidentellement modifié
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
//-----------------
void deleteVBO()
//-----------------
{
  glDeleteBuffers(1, &VBO_vertices);
  glDeleteBuffers(1, &VBO_normals);
  glDeleteBuffers(1, &VBO_indexes);
  glDeleteBuffers(1, &VBO_UVtext);
  glDeleteBuffers(1, &VAO);
}

/* display function */
void display()
{

  /* effacement de l'image avec la couleur de fond */
  /* Initialisation d'OpenGL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(10.0f); // 0 is near, >0 is far
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(2.0);

  View = glm::lookAt(cameraPosition,     // Camera is at (0,0,3), in World Space
                     glm::vec3(0, 0, 0), // and looks at the origin
                     glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );
  Model = glm::mat4(1.0f);
  Model = glm::translate(Model, glm::vec3(0, 0, cameraDistance));
  Model = glm::rotate(Model, glm::radians(cameraAngleX), glm::vec3(1, 0, 0));
  Model = glm::rotate(Model, glm::radians(cameraAngleY), glm::vec3(0, 1, 0));
  Model = glm::scale(Model, glm::vec3(.8, .8, .8));
  MVP = Projection * View * Model;
  drawObject(); // trace VBO avec ou sans shader

  /* on force l'display du resultat */
  glutPostRedisplay();
  glutSwapBuffers();
}

//-------------------------------------
// Trace le tore 2 via le VAO
void drawObject()
//-------------------------------------
{
  // Use  shader & MVP matrix   MVP = Projection * View * Model;
  glUseProgram(programID);

  // on envoie les données necessaires aux shaders */
  glUniformMatrix4fv(MatrixIDMVP, 1, GL_FALSE, &MVP[0][0]);
  // glUniformMatrix4fv(MatrixIDView, 1, GL_FALSE,&View[0][0]);
  // glUniformMatrix4fv(MatrixIDModel, 1, GL_FALSE, &Model[0][0]);
  // glUniformMatrix4fv(MatrixIDPerspective, 1, GL_FALSE, &Projection[0][0]);

  glUniform3f(locCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);

  /*
   glUniform1f(locmaterialShininess,materialShininess);
   glUniform3f(locmaterialSpecularColor,materialSpecularColor.x,materialSpecularColor.y,materialSpecularColor.z);
   glUniform3f(locLightPosition,LightPosition.x,LightPosition.y,LightPosition.z);
   glUniform3f(locLightIntensities,LightIntensities.x,LightIntensities.y,LightIntensities.z);
   glUniform1f(locLightAttenuation,LightAttenuation);
   glUniform1f(locLightAmbientCoefficient,LightAmbientCoefficient);
  */

  // pour l'display
  glBindVertexArray(VAO);                                            // on active le VAO
  glDrawElements(GL_TRIANGLES, sizeof(indexes), GL_UNSIGNED_INT, 0); // on appelle la fonction dessin
  glBindVertexArray(0);                                              // on desactive les VAO
  glUseProgram(0);                                                   // et le pg
}

void reshape(int w, int h)
{
  // set viewport to be the entire window
  glViewport(0, 0, (GLsizei)w, (GLsizei)h); // ATTENTION GLsizei important - indique qu'il faut convertir en entier non négatif

  // set perspective viewing frustum
  float aspectRatio = (float)w / h;

  Projection = glm::perspective(glm::radians(60.0f), (float)(w) / (float)h, 1.0f, 1000.0f);
}

void keyboard(unsigned char touche, int x, int y)
{
  switch (touche)
  {
  case 'f': /* display du carre plein */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutPostRedisplay();
    break;
  case 'e': /* display en mode fil de fer */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutPostRedisplay();
    break;
  case 'v': /* display en mode vertices seuls */
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glutPostRedisplay();
    break;
  case 's': /* display en mode vertices seuls */
    materialShininess -= .5;
    glutPostRedisplay();
    break;
  case 'S': /* display en mode vertices seuls */
    materialShininess += .5;
    glutPostRedisplay();
    break;
  case 'x': /* display en mode vertices seuls */
    LightPosition.x -= .2;
    glutPostRedisplay();
    break;
  case 'X': /* display en mode vertices seuls */
    LightPosition.x += .2;
    glutPostRedisplay();
    break;
  case 'y': /* display en mode vertices seuls */
    LightPosition.y -= .2;
    glutPostRedisplay();
    break;
  case 'Y': /* display en mode vertices seuls */
    LightPosition.y += .2;
    glutPostRedisplay();
    break;
  case 'z': /* display en mode vertices seuls */
    LightPosition.z -= .2;
    glutPostRedisplay();
    break;
  case 'Z': /* display en mode vertices seuls */
    LightPosition.z += .2;
    glutPostRedisplay();
    break;
  case 'a': /* display en mode vertices seuls */
    LightAmbientCoefficient -= .1;
    glutPostRedisplay();
    break;
  case 'A': /* display en mode vertices seuls */
    LightAmbientCoefficient += .1;
    glutPostRedisplay();
    break;

  case 'q': /*la touche 'q' permet de quitter le programme */
    exit(0);
  }
}

void mouse(int button, int state, int x, int y)
{
  mouseX = x;
  mouseY = y;

  if (button == GLUT_LEFT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseLeftDown = true;
    }
    else if (state == GLUT_UP)
      mouseLeftDown = false;
  }

  else if (button == GLUT_RIGHT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseRightDown = true;
    }
    else if (state == GLUT_UP)
      mouseRightDown = false;
  }

  else if (button == GLUT_MIDDLE_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseMiddleDown = true;
    }
    else if (state == GLUT_UP)
      mouseMiddleDown = false;
  }
}

void mouseMotion(int x, int y)
{
  if (mouseLeftDown)
  {
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
  }
  if (mouseRightDown)
  {
    cameraDistance += (y - mouseY) * 0.2f;
    mouseY = y;
  }

  glutPostRedisplay();
}
