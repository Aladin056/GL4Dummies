/*!\file window.c
 * \brief affichage du spectre sonore
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void initAudio(const char * file);
static void quit(void);
/*!\brief nombre d'�chantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des �chantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
/*!\brief dimensions de la fen�tre */
static int _windowWidth = ECHANTILLONS, _windowHeight = 256;
/*!\brief id du screen � cr�er */
static GLuint _screen = 0;
/*!\brief pointeur vers la musique charg�e par SDL_Mixer */
static Mix_Music * _mmusic = NULL;

/*!\brief La fonction principale cr�� la fen�tre d'affichage,
 * initialise GL et les donn�es, affecte les fonctions d'�v�nements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10, 
			 _windowWidth, _windowHeight, GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les param�tres OpenGL */
static void init(void) {
  glViewport(0, 0, _windowWidth, _windowHeight);
  _screen = gl4dpInitScreen();
  initAudio("takeonme.mod");
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  int i;
  gl4dpSetScreen(_screen);
  gl4dpClearScreen();
  for(i = 0; i < ECHANTILLONS - 1; i++) {
    int x0, y0, x1, y1;
    x0 = (i * (_windowWidth - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    x1 = ((i + 1) * (_windowWidth - 1)) / (ECHANTILLONS - 1);
    y1 = _hauteurs[i + 1];
    gl4dpSetColor(rand());
    gl4dpLine(x0, y0, x1, y1);
  }
  gl4dpUpdateScreen(NULL);
}

/*!\brief Cette fonction est appel�e quand l'audio est jou� et met 
 * dans \a stream les donn�es audio de longueur \a len.
 * \param udata pour user data, donn�es pass�es par l'utilisateur, ici NULL.
 * \param stream flux de donn�es audio.
 * \param len longueur de \a stream. */
static void mixCallback(void *udata, Uint8 *stream, int len) {
  int i;
  Sint16 *s = (Sint16 *)stream;
  if(len >= 2 * ECHANTILLONS)
    for(i = 0; i < ECHANTILLONS; i++)
      _hauteurs[i] = _windowHeight / 2 + (_windowHeight / 2) * s[i] / ((1 << 15) - 1.0);
  return;
}

/*!\brief Cette fonction initialise les param�tres SDL_Mixer et charge
 *  le fichier audio.*/
static void initAudio(const char * file) {
  int mixFlags = MIX_INIT_OGG, res;
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
    fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    exit(-3);
  }
  if(Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024) < 0)
    exit(-4);
  if(!(_mmusic = Mix_LoadMUS(file))) {
    fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
    exit(-5);
  }
  Mix_SetPostMix(mixCallback, NULL);
  if(!Mix_PlayingMusic())
    Mix_PlayMusic(_mmusic, 1);
}

/*!\brief appel�e au moment de sortir du programme (atexit), lib�re les �l�ments utilis�s */
static void quit(void) {
  if(_mmusic) {
    if(Mix_PlayingMusic())
      Mix_HaltMusic();
    Mix_FreeMusic(_mmusic);
    _mmusic = NULL;
  }
  Mix_CloseAudio();
  Mix_Quit();
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
  gl4duClean(GL4DU_ALL);
}
