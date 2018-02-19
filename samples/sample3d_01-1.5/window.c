/*!\file window.c
 *
 * \brief introduction aux textures (plus ce qui a �t� vu avant) 
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr 
 * \date February 19 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
/* pour les g�om�tries GL4Dummies */
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fen�tre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant de la g�om�trie quadrilat�re de GL4Dummies */
static GLuint _quad = 0;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant d'une texture */
static GLuint _texId = 0;

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

static void init(void) {
  const GLuint B = RGB(255, 255, 255), N = 0;
  GLuint tex[] = { B, N, N, B };
  /* G�n�ration du quadrilat�re GL4Dummies */
  _quad = gl4dgGenQuadf();
  /* g�n�rer un identifiant de texture */
  glGenTextures(1, &_texId);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* param�trer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* bloc filtre nearest, par d�faut */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  /* envoi de la donn�e texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  /* on peut maintenant choisir comment GL filtre le rendu de textures
   * en fonction de l'�loignement */
  const int filtre = 2; /* 0 nearest, 1 mipmap, 2 anisotropic */
  switch(filtre) {
    GLfloat max;
  case 1:
    /* bloc filtre mipmap */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    break;
  case 2:
    /* bloc filtre anisotropique */
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max);
#endif
    break;
  default: break;
  }

  /* d�-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derri�re. */
  glEnable(GL_DEPTH_TEST);
  /* Cr�ation des matrices GL4Dummies, une pour la projection, une
   * pour la mod�lisation et une pour la vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
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
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derri�re. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la diff�rence. Nous avons ajout�
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice vue cr��e dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "cam�ra" �
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0, 2, 10, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice mod�le cr��e dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* on scale le quadrilat�re d'un facteur 50 en x et z*/
  gl4duScalef(50, 1, 50);
  /* on place le quadrilat�re horizontalement */
  gl4duRotatef(-90, 1, 0, 0);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* activer l'�tage de textures 0, plusieurs �tages sont disponibles,
   * nous pouvons lier une texture par type et par �tage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texId comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'�tage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* envoi d'un bool�en pour inverser l'axe y des coordonn�es de
   * textures (plus efficace � faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pId, "inv"), 0); 
  /* envoi d'un facteur permettant de multiplier la coordonn�e de
   * texture afin de la faire r�p�ter */
  glUniform1f(glGetUniformLocation(_pId, "tcScale"), 50.0f); 
  /* Dessiner une g�om�trie g�n�r�e par GL4Dummies */
  gl4dgDraw(_quad);
  /* d�sactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
static void quit(void) {
  /* suppression de la texture _texId en GPU */
  if(_texId)
    glDeleteTextures(1, &_texId);
  /* nettoyage des �l�ments utilis�s et g�n�r�s par la biblioth�que
   * GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
