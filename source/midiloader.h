#pragma once

#include <vector>
#include "jdkmidi/world.h"
#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/parser.h"
#include "jdkmidi/fileread.h"
#include "jdkmidi/track.h"
#include "jdkmidi/multitrack.h"
#include "jdkmidi/filereadmultitrack.h"
#include "jdkmidi/fileshow.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/manager.h"
#include "jdkmidi/driverdump.h"

typedef struct Note {
	int debut, fin, note;//   start and endtime of the note and the value of the note from 0 to 4
	bool played, missed, pressed, released;//indicates if the note has been played and missed; if it is pressed and released
	int releasetime;// indicates the time at which the note was released
     

#ifdef DEBUG
	int baseDebut, baseFin;
#endif
};

/**
 * fonction qui prend en argument le chemin vers un fichier midi et qui retourne le coefficient multiplicateur de la vitesse du fichier midi
 * spécifications midi utilisé: http://www.sonicspot.com/guide/midifiles.html
 *
 * @param filename le chemin vers le fichier
 * @return le coefficient multiplicateur de la vitesse de la musique
 */
float getVelocity(char* filename);

/*
 * charge le fichier midi dans la variable notes
 *
 * @param filename le chemin vers le fichier midi
 * @param notes un pointeur vers un tableau de 5 vecteur de Note qui sera chargé
 * @param difficulty le niveau de difficulté (compris entre 0 pour très facile et 3 pour expert)
 */
void loadmidifile(char* filename, std::vector<Note*>* notes, int difficulty);

/**
 * donne le nombre de notes dans le fichier midi 'filename' a la difficulté 'difficulty'
 *
 * @param filename le chemin vers le fichier midi
 * @param difficulty ne niveau de difficulté
 * @return le nombre de notes
 */
int numberOfNotes(char* filename, int difficulty);

/**
 * calcul en pourcent le rapport "nb de notes jouées"/"nb de notes totale"
 *
 * @param notes le tableau de 5 vecteur Note dans lequel il faut calculer
 * @return la précision en pourcent
 */
int accuracyPerCent(std::vector<Note*>* notes);