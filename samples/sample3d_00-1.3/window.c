/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin d'un quadrilat�re
 * plus des transformations spaciales projection/mod�lisation
 * utulisant les fonction gl4duXXX  
 * \author Far�s BELHADJ,
 * amsi@ai.univ-paris8.fr \date February 11 2018
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
 * Exercice compliqu� (corrig� en 1.4), faire dans l'ordre : (1)
 * revenir � un triangle d�passant l�g�rement du cube unitaire (par
 * exemple ses coordonn�es varient entre -1.5 et +1.5) et rajouter la
 * coordonn�e z � 0.0f (modifier glVertexAttribPointer et le vertex
 * shader en cons�quence). (2) Ajouter dans data 6x4 sommets (tous
 * couleur noire) repr�sentant les 6 faces du cube unitaire o� chaque
 * face sera dessin�e avec une LINE_LOOP de 4 sommets (voir doc de
 * glDrawArrays). Le triangle continue � tourner mais le cube ne doit
 * pas tourner. (3) Modifier la vue (il sera plus facile d'utiliser
 * gl4duLookAtf) de la matrice mod�lisation-vue afin de voir le cube
 * et le triangle depuis une position en haut � droite. Plus tard, si
 * vous le souhaiter vous pourrez s�parer la vue de la mod�lisation en
 * utilisant deux matrices disjointes (par exemple viewMatrix et
 * modelMatrix).
 */
static void init(void) {
  /* donn�es envoy�es par tranches sommet-attributs dans le VBO */
  GLfloat data[] = {
    /* 4 coordonn�es de sommets en 2D chacune suivie de sa couleur */
    -1.0f, -1.0f, 
    1.0f, 0.0f, 0.0f, 
    1.0f, -1.0f,
    0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f, 0.0f
  };
  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
   /* Cr�ation des matrices GL4Dummies, une pour la projection, une pour la mod�lisation-vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* on active la matrice de projection cr��e pr�c�demment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours re�oit identit� (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* d�commenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
  /* dans quelle partie de l'�cran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* G�n�ration d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL � l'identifiant VAO g�n�r� */
  glBindVertexArray(_vao);
  /* Activation des 3 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  /* G�n�ration d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL � l'identifiant VBO g�n�r� */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des donn�es VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Param�trage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof *data, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof *data, (const void *)(2 * sizeof *data));
  /* d�-lier le VBO et VAO */
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* une variable d'angle, maintenant elle passe ne degr�s */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice mod�lisation-vue
   * cr��e dans init */
  gl4duBindMatrix("modelViewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice courante avec une translation (voir la
   * projection perspective dans le manuel pour comprendre pourquoi
   * nous devons �loigner de -3 en z les objets dessin�s) */
  gl4duTranslatef(0, 0, -3);
  /* on transmet la variable d'angle en l'int�grant dans la matrice de
   * mod�lisation-vue. Soit composer la matrice courante avec une
   * rotation "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incr�mente angle d'un 1/60 de tour soit (360� x 1/60). Tester
   * l'application en activant/d�sactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que r�sultat soit toujours le m�me. */
  angle += (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* Lier le VAO-machine-GL � l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande de deux triangles avec 4 sommets commen�ant � 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
