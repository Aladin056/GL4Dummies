/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin de triangle color�
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 11 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
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
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les param�tres OpenGL et les donn�es. 
 *
 * Exercice (corrig� en 1.1) : modifier data, les lignes
 * glVertexAttribPointer et glDrawArrays (dans draw) afin de dessiner
 * un quadrilat�re recouvrant l'ensemble de l'�cran ; mettre une
 * couleur jaune pour le point ajout�.
 */
static void init(void) {
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
    0.0f, 0.0f, 1.0f
  };
  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* dans quelle partie de l'�cran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* G�n�ration d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL � l'identifiant VAO g�n�r� */
  glBindVertexArray(_vao);
  /* Activation des 2 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  /* G�n�ration d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL � l'identifiant VBO g�n�r� */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des donn�es VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Param�trage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(3 * 2 * sizeof *data));
  /* d�-lier le VAO et VBO */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* Lier le VAO-machine-GL � l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande de triangles � 3 sommets commen�ant � 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
  /* d�-lier le VAO */
  glBindVertexArray(0);
  /* d�sactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
static void quit(void) {
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU */
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  /* nettoyage des �l�ments utilis�s par la biblioth�que GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
