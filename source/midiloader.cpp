#include "midiloader.h"

#define DIFFICULTY_BASE 60
#define DIFFICULTY_STEP 12

int accuracyPerCent(std::vector<Note*>* notes) {
	int nbnotes = 0;
	int total = 0;

	for (int j = 0; j < 5; j++) {
		for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
			Note* note = *it;
			if (note->played) nbnotes++;
			total++;
		}
	}

	return (nbnotes * 100) / total;
}

int numberOfNotes(char* filename, int difficulty) {
	int nbNotes = 0;

	//difficultyCode = index de la note 0 a la difficulté 'difficulty' dans le fichier midi
	int difficultyCode = DIFFICULTY_BASE + difficulty * DIFFICULTY_STEP;

	jdkmidi::MIDIFileReadStreamFile rs(filename);
	jdkmidi::MIDIMultiTrack tracks;
	jdkmidi::MIDIFileReadMultiTrack track_loader(&tracks);
	jdkmidi::MIDIFileRead reader(&rs, &track_loader);
	reader.Parse();

	jdkmidi::MIDIMultiTrackIterator i(&tracks);
	jdkmidi::MIDITimedBigMessage* msg;
	int trk_num;
	bool finished = false;
	i.GoToTime(0);
	i.GetCurEvent(&trk_num, &msg);
	{
		while (!finished) {
			if (i.GetCurEvent(&trk_num, &msg)) {
				if (msg) {
					if (msg->IsNoteOn()) {

					}
					else if (msg->IsNoteOff()) {
						if (msg->GetNote() >= difficultyCode && msg->GetNote() <= difficultyCode + 4) {
							nbNotes++;
						}
					}

					finished = !i.GoToNextEvent();
				}
			}
		}
	}
	return nbNotes;
}

void loadmidifile(char* filename, std::vector<Note*>* notes, int difficulty) {

	//difficultyCode = index de la note 0 a la difficulté 'difficulty' dans le fichier midi
	int difficultyCode = DIFFICULTY_BASE + difficulty * DIFFICULTY_STEP;

	jdkmidi::MIDIFileReadStreamFile rs(filename);
	jdkmidi::MIDIMultiTrack tracks;
	jdkmidi::MIDIFileReadMultiTrack track_loader(&tracks);
	jdkmidi::MIDIFileRead reader(&rs, &track_loader);
	reader.Parse();

	jdkmidi::MIDISequencer seq(&tracks, 0);

	jdkmidi::MIDIMultiTrackIterator i(&tracks);
	jdkmidi::MIDITimedBigMessage* msg;
	int trk_num;
	bool finished = false;
	i.GoToTime(0);
	i.GetCurEvent(&trk_num, &msg);
	seq.GetTrackState(trk_num)->Process(msg);
	{
		Note* t[200];
		for (int j = 0; j < 200; j++) {
			t[j] = 0;
		}
		while (!finished) {
			if (i.GetCurEvent(&trk_num, &msg)) {
				if (msg) {
					seq.GetTrackState(trk_num)->Process(msg);
					if ((strcmp("PART GUITAR", seq.GetTrackState(trk_num)->track_name) == 0) || trk_num == 1) {
						if (msg->IsNoteOn()) {
							Note* note = new Note();
							note->debut = msg->GetTime();
							note->note = msg->GetNote();
							note->played = false;
							note->pressed = false;
							note->missed = false;
							note->released = false;
							note->releasetime = 0;
							if (t[msg->GetNote()]) {
								delete t[msg->GetNote()];
							}
							t[msg->GetNote()] = note;
						}
						else if (msg->IsNoteOff()) {
							if (t[msg->GetNote()]) {
								Note* note = t[msg->GetNote()];
								note->fin = msg->GetTime();
								if (msg->GetNote() >= difficultyCode && msg->GetNote() <= difficultyCode + 4) {
									notes[msg->GetNote() - difficultyCode].push_back(note);
								}
								t[msg->GetNote()] = 0;
							}
						}
					}

					finished = !i.GoToNextEvent();
				}
			}
		}
		for (int j = 0; j < 200; j++) {
			if (t[j]) {
				delete t[j];
			}
		}
	}

	// ----- correction de la vitesse -----

	float velocity = getVelocity(filename);
	for (int j = 0; j < 5; j++) {
		for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
			Note* note = *it;
#ifdef DEBUG
			note->baseDebut = note->debut;
			note->baseFin = note->fin;
#endif
			note->debut = (int)(note->debut * velocity);
			note->fin = (int)(note->fin * velocity);
		}
	}
}

float getVelocity(char* filename) {
	FILE* pFile;
	unsigned char buff = 0;
	unsigned char buff2[3];// un cache de 3 char
	for (int j = 0; j < 3; j++) buff2[j] = 0;

	pFile = fopen(filename, "r");
	float timedivision = 1;
	float tempo = 0;
	if (pFile != NULL) {
		int i = 0;//position dans le buffer
		while (!feof(pFile)) {
			fread(&buff, sizeof buff, 1, pFile);

			//la variable de division temporelle se trouve sur les offsets 12 et 13
			if (i == 12) {
				timedivision = buff * 256;
			}
			else if (i == 13) {
				timedivision += buff;
			}

			buff2[0] = buff2[1];
			buff2[1] = buff2[2];
			buff2[2] = buff;
			// meta event = 255; type = 81 et length = 3 : c'est un event de tempo qui est codé sur 3 bytes
			if (buff2[0] == 255 && buff2[1] == 81 && buff2[2] == 3) {
				fread(&buff, sizeof buff, 1, pFile);
				tempo = buff * 256 * 256;
				fread(&buff, sizeof buff, 1, pFile);
				tempo += buff * 256;
				fread(&buff, sizeof buff, 1, pFile);
				tempo += buff;
				i += 3;
			}

			i++;
		}
		fclose(pFile);
	}
	//free(buff2);

	return tempo / (timedivision * 1000);
}
