/*!\file window.c
 *
 * \brief GL4Dummies, premier exemple 3D avec projection orthogonale
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 5 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fen�tre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du (futur) Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant de la texture */
static GLuint _tId = 0;

/*!\brief cr�� la fen�tre d'affichage, initialise GL et les donn�es,
 * affecte les fonctions d'�v�nements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les param�tres OpenGL. */
static void init(void) {
  int i;
  const int w = 200, h = 150;
  GLubyte pixels[w * h * 4];
  /* donn�es envoy�es par tranches (diff�rent du mode interleaved
   * array) dans le VBO */
  GLfloat data[] = {
    /* 3 coordonn�es de sommets en 2D */
    -1.0f, -1.0f, 
    1.0f, -1.0f,
     0.0f,  1.0f,
    /* 3 couleurs */
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    /* 3 coordonn�es de texture, une par sommet */
    0.0f, 0.0f, 
    1.0f, 0.0f,
    0.5f, 1.0f
  };
  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* Cr�ation des matrices GL4Dummies, une pour la projection, une pour la mod�lisation */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* Force l'appel du resize */
  resize(_wW, _wH);
  /* G�n�ration d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL � l'identifiant VAO g�n�r� */
  glBindVertexArray(_vao);
  /* Activation des 3 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  /* G�n�ration d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL � l'identifiant VBO g�n�r� */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des donn�es VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Param�trage 3 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(3 * 2 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(3 * 5 * sizeof *data));
  /* d�-lier le VBO et VAO */
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  /* G�n�ration d'un identifiant de texture */  
  glGenTextures(1, &_tId);
  /* Lier la texture-GL-courante � l'identifiant de texture g�n�r� */
  glBindTexture(GL_TEXTURE_2D, _tId);
  /* param�trage de la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  /* remplissage du tableau pixels, contenant la texture RGBA */
  for(i = 0; i < w * h; i++) {
    GLubyte p = 256.0 * (rand() / (RAND_MAX + 1.0));
    pixels[4 * i + 0] = p;
    pixels[4 * i + 1] = p;
    pixels[4 * i + 2] = p;
    pixels[4 * i + 3] = 255;
  }
  /* Transfert de la texture */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  /* d�-lier l'identifiant de texture */
  glBindTexture(GL_TEXTURE_2D, 0);
}
/*!\brief param�tre la vue (viewPort) OpenGL en fonction
 * des dimensions de la fen�tre \a w (largeur) et \a h (hauteur).
 *
 * \param w largeur de la fen�tre.
 * \param h hauteur de la fen�tre.
 */
static void resize(int w, int h) {
  glViewport(0, 0, w, h);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duOrthof(-1, 1, -1, 1, 0, 100);
  gl4duBindMatrix("modelViewMatrix");
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat a = 0.0;
  /* effacement du buffer de couleur et de profondeur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant) la matrice model view dans GL4Dummies */
  gl4duBindMatrix("modelViewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice courante avec une translation */
  gl4duTranslatef(0, 0, -2);
  /* Composer la matrice courante avec une rotation */
  gl4duRotatef(a * 180.0f / M_PI, 0, 1, 0);
  /* Activer l'�tage de textures 0 dans OpenGL */
  glActiveTexture(GL_TEXTURE0);
  /* Lier la texture-GL-courante � l'identifiant de texture _tId */
  glBindTexture(GL_TEXTURE_2D, _tId);
  /* Envoyer, au shader, la valeur 0 � la variable "myTexture" de port�e "uniform" et de type entier */
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  /* Envoyer, au shader, toutes les matrices connues dans GL4Dummies */
  gl4duSendMatrices();
  /* Lier le VAO-machine-GL � l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande de triangles � 3 sommets commen�ant � 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
  /* d�-lier le VAO */
  glBindVertexArray(0);
  /* d�sactiver le programme shader */
  glUseProgram(0);
  a += 0.005;
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
static void quit(void) {
  /* suppression de la texture _tId en GPU */
  if(_tId)
    glDeleteTextures(1, &_tId);
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU */
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  /* nettoyage des �l�ments utilis�s par la biblioth�que GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
