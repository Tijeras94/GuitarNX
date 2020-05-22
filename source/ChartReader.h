#ifndef CHART_READER
#define CHART_READER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OBJ_LIMIT 5000

struct chart_obj {
	int value; //0~4: notas; 5: rítmo
	int beat; //  time in relation to the hit on the chart
	int extra; // duration for notes, bpm * 1000 for rhythm
} song[OBJ_LIMIT];
int num_objects;
int last_note;

enum noteStat { STD, HIT, MISS, RELEASED };

struct time_info {
	int value; //0~4: notas; 5: linhas
	float time; //tempo da música em que ocorre
	bool long_note; //indica se é uma nota longa (se for linha indica que é "grande" (larga))
	float dur_time; //duração da nota longa (caso seja longa)
	enum noteStat status; //indica se a nota foi tocada, perdida ou nenhuma delas
} times[OBJ_LIMIT + 1000];
int num_times = 0;
int last_note_time;

int resolution;

int getResolution(FILE* file)
{
	rewind(file);
	char token[100];
	int result;
	while (strcmp(token, "Resolution") && strcmp(token, "}"))
		fscanf(file, "%s", &token);
	if (!strcmp(token, "Resolution")) {
		fscanf(file, "%s", &token);
		fscanf(file, "%d", &result);
	}
	else
		result = -1;
	return(result);
}


struct chart_obj getRhythmObj(FILE* file, int pointer_position)
{
	rewind(file);
	char line[100];
	while (strcmp(line, "[SyncTrack]\n"))
		fgets(line, 100, file);

	fgets(line, 100, file);
	fgets(line, 100, file);
	int beat, bpm;
	int pointer_counter = pointer_position;
	char type[5];
	do {
		sscanf(line, "%d = %s %d", &beat, &type, &bpm);
		fgets(line, 100, file);
		pointer_counter--;
	} while (strcmp(line, "}\n") && pointer_counter >= 0);

	struct chart_obj result;

	if (pointer_counter < 0) {
		if (!strcmp(type, "B")) {
			result.beat = beat;
			result.value = 5;
			result.extra = bpm;
		}
		else
			result.beat = -2;
	}
	else
		result.beat = -1;
	return(result);
}

struct chart_obj getNoteObj(FILE* file, int pointer_position)
{
	rewind(file);
	char line[100];
	while (strcmp(line, "[ExpertSingle]\n"))
		fgets(line, 100, file);

	fgets(line, 100, file);
	fgets(line, 100, file);
	int beat, value, duration;
	int pointer_counter = pointer_position;
	char type[5];
	do {
		sscanf(line, "%d = %s %d %d", &beat, type, &value, &duration);
		fgets(line, 100, file);
		pointer_counter--;
	} while (strcmp(line, "}\n") && pointer_counter >= 0);

	struct chart_obj result;

	if (pointer_counter < 0) {
		if (!strcmp(type, "N")) {
			result.beat = beat;
			result.value = value;
			result.extra = duration;
		}
		else
			result.beat = -2;
	}
	else
		result.beat = -1;
	return(result);
}

int getSongObjects(char* FileName, struct chart_obj result[OBJ_LIMIT])
{
	FILE* fp = fopen(FileName, "r");
	if (fp == NULL) {
		printf(".chart not found\n");
		exit(1);
	}

	int pointer_notes = 0;
	int pointer_rhythm = 0;
	struct chart_obj note_obj = {};
	struct chart_obj rhythm_obj = {};

	int desl_counter_r = 0;
	int desl_counter_n = 0;

	resolution = getResolution(fp);
	if (resolution == -1)
		resolution = 192;

	while (note_obj.beat != -1) {
		rhythm_obj.beat = -2;
		while (rhythm_obj.beat == -2) {
			rhythm_obj = getRhythmObj(fp, pointer_rhythm + desl_counter_r);
			if (rhythm_obj.beat == -2)
				desl_counter_r++;
		}
		do {
			note_obj.beat = -2;
			while (note_obj.beat == -2) {
				note_obj = getNoteObj(fp, pointer_notes + desl_counter_n);
				if (note_obj.beat == -2)
					desl_counter_n++;
			}
			if ((note_obj.beat < rhythm_obj.beat || rhythm_obj.beat == -1) && note_obj.beat != -1) {
				result[pointer_notes + pointer_rhythm].value = note_obj.value;
				result[pointer_notes + pointer_rhythm].beat = note_obj.beat;
				result[pointer_notes + pointer_rhythm].extra = note_obj.extra;
				last_note = pointer_notes + pointer_rhythm;
				pointer_notes++;
			}
			else {
				if (note_obj.beat != -1) {
					result[pointer_notes + pointer_rhythm].value = rhythm_obj.value;
					result[pointer_notes + pointer_rhythm].beat = rhythm_obj.beat;
					result[pointer_notes + pointer_rhythm].extra = rhythm_obj.extra;
					pointer_rhythm++;
				}
			}
		} while ((note_obj.beat < rhythm_obj.beat || rhythm_obj.beat == -1) && note_obj.beat != -1);
	}
	return(pointer_notes + pointer_rhythm);
}

void fillTimes(void)
{
	float note_time = 0, screen_z_value = 0;

	int current_bpm = song[0].extra;

	int num_lines = 0;

	for (int i = 1; i < num_objects; i++) {
		note_time += (float)(song[i].beat - song[i - 1].beat) / (float)current_bpm * 60000.0f / (float)resolution;
		while (song[i].beat >= num_lines * resolution) {
			float line_time = note_time - (float)(song[i].beat - num_lines * resolution) / (float)current_bpm * 60000.0f / (float)resolution;
			times[num_times].value = 5;
			times[num_times].time = line_time;
			if (num_lines % 4 == 0)
				times[num_times].long_note = true;
			else
				times[num_times].long_note = false;
			times[num_times].status = STD;
			num_times++;
			num_lines++;
		}
		if (song[i].value == 5)
			current_bpm = song[i].extra;
		else {
			if (song[i].extra > 0) {
				float dur_time = 0.0f;
				int count_obj = i + 1;
				int dur_bpm = current_bpm;
				if (i != last_note)
					while (song[count_obj].beat < song[i].beat + song[i].extra) {
						dur_time += (float)(song[count_obj].beat - song[count_obj - 1].beat) / (float)dur_bpm * 60000.0f / (float)resolution;
						if (song[count_obj].value == 5)
							dur_bpm = song[count_obj].extra;
						count_obj++;
					}
				dur_time += (float)(song[i].beat + song[i].extra - song[count_obj - 1].beat) / (float)dur_bpm * 60000.0f / (float)resolution;
				times[num_times].value = song[i].value;
				times[num_times].time = note_time;
				times[num_times].long_note = true;
				times[num_times].dur_time = dur_time;
				times[num_times].status = STD;
				last_note_time = num_times;
			}
			else {
				times[num_times].value = song[i].value;
				times[num_times].time = note_time;
				times[num_times].long_note = false;
				times[num_times].status = STD;
				last_note_time = num_times;
			}
			num_times++;
		}
	}
	while (song[num_objects - 1].beat + song[num_objects - 1].extra > (num_lines - 1)* resolution) {
		float line_time = note_time + (float)(num_lines * resolution - song[num_objects - 1].beat) / (float)current_bpm * 60000.0f / (float)resolution;
		times[num_times].value = 5;
		times[num_times].time = line_time;
		if (num_lines % 4 == 0)
			times[num_times].long_note = true;
		else
			times[num_times].long_note = false;
		num_times++;
		num_lines++;
	}
}


/*
 * charge le fichier midi dans la variable notes
 *
 * @param filename le chemin vers le fichier midi
 * @param notes un pointeur vers un tableau de 5 vecteur de Note qui sera chargé
 * @param difficulty le niveau de difficulté (compris entre 0 pour très facile et 3 pour expert)
 */
void loadcharfile(char* filename, std::vector<Note*>* notes, int difficulty)
{

	printf("Loading chart...\n");
	num_objects = getSongObjects(filename, song);
	printf("Charging complete!\n");

	if (num_objects > OBJ_LIMIT) {
		printf("Chart contains too many notes. Can't load.");
		exit(1);
	}

	fillTimes();

	for (size_t i = 0; i < num_times; i++)
	{
		Note* note = new Note;
		note->debut = (times[i].time * 1000) - (480) ;
		note->fin = note->debut + (times[i].dur_time * 1000);
		note->note = times[i].value;
		note->played = false;
		note->pressed = false;
		note->missed = false;
		note->released = false;
		note->releasetime = 0;

		printf("adding note: %d\n", note->note);
		if(note->note < 5)
			notes[note->note].push_back(note);
	}

}
#endif