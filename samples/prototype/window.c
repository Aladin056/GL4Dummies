/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 04 2018
 */

/* inclusion des ent�tes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * p�dagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des ent�tes de fonctions de cr�ation et de gestion de
 * fen�tres syst�me ouvrant un contexte favorable � GL4dummies. Cette
 * partie est d�pendante de la biblioth�que SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief cr�� la fen�tre, un screen 2D effac� en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de cr�ation d'une fen�tre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 320, 240, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* �tat visible */) {
    /* ici si �chec de la cr�ation souvent li� � un probl�me d'absence
     * de contexte graphique ou d'impossibilit� d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* cr�ation d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fen�tre */
  gl4dpInitScreen();
  /* effacement du screen en cours en utilisant la couleur par d�faut,
   * le noir */
  gl4dpClearScreen();
  gl4dpUpdateScreen(NULL);
  /* boucle infinie pour �viter que le programme ne s'arr�te et ferme
   * la fen�tre */
  gl4duwMainLoop();
  return 0;
}
