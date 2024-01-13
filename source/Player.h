#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <queue>

#include "GNeck.h"
#include "midiloader.h"
#include "Font.h"
#include "ChartReader.h"

#define POINTS_PER_MS 0.1f
#define POINTS_PER_NOTE 50 

static int TOLERENCE = 100;
static float ZOOM = 25.0f;

#define GRID_RESOLUTION 2
#define TEXT_MOTION_VELOCITY 0.7f
#define STAR_ANIM_TIME 2000000
#define PING_FREQUENCY 1000000
#define SYCHRO_TOLERENCE 10
#define SHOWED_POINTS_STEP 29

/*
 * Check if a file exist using fopen() function
 * return 1 if the file exist otherwise return 0
 */
int cfileexists(const char* filename) {
	/* try to open file to read */
	FILE* file;
	if (file = fopen(filename, "r")) {
		fclose(file);
		return 1;
	}
	return 0;
}

class GNote : public Note {
private:
public:
	float getHScale(float zoom) {
		float ratio = zoom * 10;

		if (!released) {
			return (float)(fin - debut) / (ratio);
		}
		else {
			return  (float)(releasetime - debut) / (ratio);
		}
	}

	glm::vec4 getColor(int type, float alfa = 1.0f) {
		if (missed)
			return glm::vec4(0.7f, 0.7f, 0.7f, 0.5f);

		switch (type) {
		case 0:
			return glm::vec4(0.0f, 1.0f, 0.0f, alfa);
		case 1:
			return glm::vec4(1.0f, 0.0f, 0.0f, alfa);
		case 2:
			return glm::vec4(1.0f, 1.0f, 0.0f, alfa);
		case 3:
			return glm::vec4(0.0f, 0.0f, 1.0f, alfa);
		case 4:
			return glm::vec4(1.0f, 0.0f, 1.0f, alfa);

		default:
			return glm::vec4(0, 0, 0, alfa);
		}
	}
};


/**
 * classe représentant la potition des étoiles a l'écran
 */
class Etoile {
public:
	int x, y;
	float timeStart;
};

class Particule {
public:
	float angle;
	int length;
	float timeStart;
};


class MockTimer {
private:
	float _fps;
	float _ticks;
	bool _started;
	bool _pause;
public:

	MockTimer(float fps) {
		_fps = fps;
		_started = false;
		_pause = false;
		_ticks = 0;
	}

	void Update() {
		if (_started && !_pause)
			_ticks += _fps;
	}

	void Pause() {
		_pause = true;
		audio1::OGG_Pause();
		audio2::OGG_Pause();
		
		audio3::OGG_Pause();
		audio4::OGG_Pause();
	}

	void Stop() {
		_pause = true;
		audio1::OGG_Stop();
		audio2::OGG_Stop();
		
		audio3::OGG_Stop();
		audio4::OGG_Stop();
		
		_started = false;
		_pause = false;
		_ticks = 0;
	}

	void Start() {
		if (!_pause)
			_ticks = 0;

		_started = true;
		_pause = false;

		audio1::OGG_Play();
		audio2::OGG_Play();
		
		audio3::OGG_Play();
		audio4::OGG_Play();
	}

	Uint32 Ticks()
	{
		return _ticks;
	}

};



/**
 * les différents etat du jeu
 */
enum PLAYER_STATES {
	play,
	inPause,
	inDebug,
	score,
	inChangeDificulty,
	finished
};

class Player {

public:

	//etat actuel de jeu
	PLAYER_STATES state;

	//variables pour le score
	int mult;//coeff multiplicateur des points
	int points;// le score de joueur
	int showedPoints;// le score de joueur affiché a l'écran
	int expectedPoints;// les prochains points qui vont etre gagné
	int suite;//nombre de notes jouée de suite actuellement
	int accuracy;//utilisé pour le score final, précision en pourcent
	int longestStreak;//utilisé pour le score final, la plus longe suite

	//utilisé pour le menu de pause
	int motion; //utilisé pour l'equation différentielle responsable du mouvement de texte
	int pauseSelection; //entrée de munu séléctionné en pause

	//variables temporelles
	float oggTime;//position actuel dans la musique en ms
	float timeNow, timeLast;// timeNow-timeLast = temps en micro secondes de la boucle de rendu

	//listes des notes
	std::vector<Note*> notes[5];

	//float velocity;//TODO supprimer si non utilisé
	std::deque<Etoile*> etoileList;
	std::deque<Particule*> particulesList[5];
	bool songPresent;
	bool rhythmPresent;

	//SceKernelUtilsMt19937Context ctx;

	//les chemins vers les fichiers
	char songPath[100];
	char guitarPath[100];
	char rhythmPath[100];
	char notesPath[100];
	char temp[100];

	bool touches[5];//pour savoir si les touches sont pressée a la dèrnière boucle
	bool firetouches[5]; //pour savoir si les touches sont pressée dans la boucle boucle et pas avant


	//new 
	const char* skin;
	Renderer* renderer;
	Shader* texShader;
	Shader* colShader;
	GNeck* neck;
	TexturedModel button;
	RawModel squarel;
	MockTimer timer;

	std::vector<unsigned int> buttonTexts;
	unsigned int flammeImg[5];

	Font* font;

	bool stopChangeDificulty;
	MusicEntry* loadedMusic;
	char dificutyText[15];

	void erreur() {
		//pspAudioSetVolume(2, 0, 0);
		audio2::setVolume(0);
		suite = 0;
		mult = 1;

		for (int j = 0; j < 5; j++) {
			for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
				Note* note = *it;

				if (note->pressed && !note->released) {
					note->released = true;
					note->missed = true;
				}
			}
		}
	}

	void noerreur(int noteTime) {
		//pspAudioSetVolume(2, 0x8000, 0x8000);
		audio2::setVolume(100);
		suite++;
		if (suite > longestStreak) longestStreak = suite;
		mult = suite / 10 + 1;
		if (mult < 1) mult = 1;
		if (mult > 6) mult = 6;
		points += ((int)(noteTime * POINTS_PER_MS) + POINTS_PER_NOTE) * mult;
	}

	void touchesValSet() {
		for (int i = 0; i < 5; i++) {
			//controls
			switch (i) {
			case 0:
				touchesValSet(i, Pad::left() || Pad::up());
				break;
			case 1:
				touchesValSet(i, Pad::down() || Pad::up());
				break;
			case 2:
				touchesValSet(i, Pad::square() || Pad::triangle());
				break;
			case 3:
				touchesValSet(i, Pad::cross());
				break;
			case 4:
				touchesValSet(i, Pad::circle() || Pad::triangle());
				break;
			}
			/*
			touchesValSet(i, (config::touchesCircle[i] && pad::circle()) ||
				(config::touchesSquare[i] && pad::square()) ||
				(config::touchesCross[i] && pad::cross()) ||
				(config::touchesTriangle[i] && pad::triangle()) ||
				(config::touchesUp[i] && pad::up()) ||
				(config::touchesDown[i] && pad::down()) ||
				(config::touchesLeft[i] && pad::left()) ||
				(config::touchesRight[i] && pad::right()) ||
				(config::touchesR[i] && pad::R()) ||
				(config::touchesL[i] && pad::L()));
			*/
		}
	}

	void touchesValSet(int touchenb, bool pressed) {
		if (pressed) {
			if (!touches[touchenb]) {
				touches[touchenb] = true;
				firetouches[touchenb] = true;
			}
		}
		else {
			touches[touchenb] = false;
		}
	}

	void stop() {
		state = finished;
	}


	void renderButtons(glm::mat4 PV) {
		texShader->use();
		texShader->setInt("textureSampler", 0);

		for (size_t i = 0; i < 5; i++)
		{
			auto model = glm::identity<glm::mat4>();
			model = glm::translate(model, glm::vec3(((float)((int)i - 2)) / 2.0f, 0, 0.0f));
			//	model = glm::rotate(model, glm::radians(-180.0f / 2), glm::vec3(0, 0, 1));
			if (!touches[i]) { //  rendering if the key is not pressed 
				button.setTexture(buttonTexts.at(5));
			}
			else { //  if it is pressed

				for (float u = 1.5f; u > 1.0f; u -= 0.1f) {
					model = glm::identity<glm::mat4>();
					model = glm::translate(model, glm::vec3(((float)((int)i - 2)) / 2.0f, 0, 0.0f));
					model = glm::scale(model, glm::vec3(u, u, u));
					//model = glm::rotate(model, glm::radians(-180.0f / 2), glm::vec3(0, 0, 1));

				}

				button.setTexture(buttonTexts.at(i));
			}
			texShader->setMat4("pmv", PV * model);
			renderer->render(button);
		}
	}

	void renderScreen(glm::mat4 PV) {
		bool toucheActivated[5];
		for (int i = 0; i < 5; i++)toucheActivated[i] = false;


		neck->render(PV, oggTime);


		//notes 
		for (int j = 0; j < 5; j++)
		{
			for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it)
			{
				GNote* note = (GNote*)*it;
				squarel.resetTransform();

				int time = note->debut;
				int timeend = note->fin;
				int timedrop = note->releasetime;
				if (note->pressed && !note->released) toucheActivated[j] = true;
				if (timeend >= oggTime - 1000 && time <= (oggTime + 5000))
				{
					float x = (j - 2) / 2.0f;
					float y = (((time - oggTime)) / ZOOM);

					colShader->use();
					colShader->setVec4("color", note->getColor(j));
				
					squarel.scaleY(note->getHScale(ZOOM));
					// render note lenght
					{
						squarel.translateX(x/(.2f));
						squarel.translateY((y / (10 * note->getHScale(ZOOM))));
						squarel.scaleX(.2);
						colShader->setMat4("pmv", PV * squarel.getTransform());
						if (note->getHScale(ZOOM) > 0.7)
							renderer->render(squarel);
					}

					//render sustain note
					{
						//test
						//note->pressed = true;
						//note->released = false;

						if (note->pressed & !note->released) {
							int randValue = rand() % 10;
							for (float u = -randValue / 100.0f; u <= randValue / 100.0f; u += 0.02f) {
								int randValue2 = rand() % 10;
								for (float v = -randValue2 / 100.0f; v <= randValue2 / 100.0f; v += 0.02f) { 
									squarel.translateX((x + u) / (.2f));
									squarel.translateY(((y + v) / (10 * note->getHScale(ZOOM))));

									colShader->setMat4("pmv", PV * squarel.getTransform());
									if (note->getHScale(ZOOM) > 0.7)
										renderer->render(squarel);
								}
							}


							// add particles only on sustain notes
							if (toucheActivated[j] && (note->getHScale(ZOOM) > 0.7)) {

								float delta = timeNow - timeLast;
								for (int i = 0; i < delta / 20000; i++) {
									Particule* newP = new Particule();
									int randValue = rand();
									int randValue2 = rand();
									newP->angle = (randValue % 600) / 1000.0f;
									newP->length = (randValue2 % 200) + 200;
									newP->timeStart = timer.Ticks();
									particulesList[j].push_back(newP);
								}
							}
						}
					}


					//note
					texShader->use();
					texShader->setInt("textureSampler", 0);
					button.getModel().translate(glm::vec3(x, y / 10, 0));
					texShader->setMat4("pmv", PV * button.getTransform());
					if (note->missed)
						button.setTexture(buttonTexts.at(5));
					else
						button.setTexture(buttonTexts.at(j));

					renderer->render(button);
				}
			}
		}


		renderButtons(PV);

		texShader->use();
		texShader->setInt("textureSampler", 0);

		for (int i = 0; i < 5; i++) {//particle rendering
			TexturedModel starM(squarel, flammeImg[i]);

			for (std::deque<Particule*>::iterator it = particulesList[i].begin(); it != particulesList[i].end(); ++it) {
				Particule* p = *it;

				if (timeNow - p->timeStart < p->length) {

					texShader->setVec4("color", glm::vec4(1, 1, 1, 0.9f * (1 - (timeNow - p->timeStart) / ((float)p->length))));

					{
						auto model = starM.getTransform();
						model = glm::translate(model, glm::vec3((((float)(i - 2)) / 2.0f), 0, 0));
						model = glm::rotate(model, (3.14f / 6.0f), glm::vec3(1, 0, 0));
						model = glm::rotate(model, (p->angle), glm::vec3(0, 0, 1));
						model = glm::translate(model, glm::vec3(0.0f, p->length * (timeNow - p->timeStart) / 300000.0f, 0.0f));
						model = glm::scale(model, glm::vec3(0.4f, 0.8f, 0.4f));
						texShader->setMat4("pmv", PV * model);
					}
					renderer->render(starM);
				}
			}
		}
		texShader->setVec4("color", glm::vec4(1, 1, 1, 1));
	}

	Player(const char* skin, Renderer* renderer, Shader* texShader, Shader* colShader, Font* font) : skin(skin), renderer(renderer), texShader(texShader), colShader(colShader), timer(1000.0f / 60.0f), font(font) {
		//initialisation des variables
		points = 0;
		showedPoints = 0;
		expectedPoints = 0;
		suite = 0;
		mult = 1;
		pauseSelection = 0;
		motion = 0;
		state = play;
		for (int j = 0; j < 5; j++)touches[j] = false;
		for (int j = 0; j < 5; j++)firetouches[j] = false;
		songPresent = false;
		rhythmPresent = false;

		neck = new GNeck(skin, renderer, texShader, colShader);

		ZOOM = 36;
		neck->setZOOM(ZOOM);

		//sceKernelUtilsMt19937Init(&ctx, time(NULL));
	}

	~Player() {
		//if (songPresent)oggA::OGG_Stop();
		//if (rhythmPresent)oggC::OGG_Stop();
		//oggB::OGG_Stop();

		//if (songPresent)oggA::OGG_FreeTune();
		//if (rhythmPresent)oggC::OGG_FreeTune();
		//oggB::OGG_FreeTune();
	}

	void ShowScore() {
		font->Printf(10, SCREEN_HEIGHT - font->getFontHeight(), true, "%i", showedPoints);
		font->Printf(SCREEN_WIDTH - font->getFontHeight()*2, SCREEN_HEIGHT - font->getFontHeight(), true, "X%i", mult);


		//printf("Points: %i\n", points);
		//printf("LongestStreak: %i\n", longestStreak);
		//printf("Multiplier:  X%i\n", mult);
	}


	void loadMusic(MusicEntry* music, std::vector<char*> & listdif , int difficulty) {

		memset(dificutyText, 15, 10);
		sprintf(dificutyText, "%s", listdif[difficulty]);

		loadedMusic = music;
		stopChangeDificulty = false;
		audio1::OGG_End();
		audio2::OGG_End();
		audio3::OGG_End();
		audio4::OGG_End();

		const char* path = music->dir;
		//Chargement des ressources
		sprintf(songPath, "%s%s", path, "song.ogg");

		sprintf(rhythmPath, "%s%s", path, "rhythm.ogg");

		sprintf(guitarPath, "%s%s", path, "guitar.ogg");
		sprintf(notesPath, "%s%s", path, "notes.mid"); 

		if(cfileexists(notesPath))
			loadmidifile(notesPath, notes, difficulty);
		else
		{
			sprintf(notesPath, "%s%s", path, "notes.chart");
			loadcharfile(notesPath, notes, difficulty);
		}//velocity = getVelocity(notesPath);
		printf("Midi Loaded :)\n");


		//printf("Loading Audio\n");
		audio1::OGG_Load(0, path, "song.ogg");
		//printf("A2\n");
		audio2::OGG_Load(1, path, "guitar.ogg");
		
		// needs some sort of cheking if files exits, comment for now
		audio3::OGG_Load(2, path, "vocals.ogg");
		audio4::OGG_Load(3, path, "drums.ogg");
		



		//load graphics

		neck->load();

		{
			//button textures
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonvert.png"));
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonrouge.png"));
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonjaune.png"));
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonbleu.png"));
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonrose.png"));
			buttonTexts.push_back(Loader::loadTexture(skin, "boutonMissed.png"));

			//fire
			flammeImg[0] = Loader::loadTexture(skin, "flammevert.png");
			flammeImg[1] = Loader::loadTexture(skin, "flammerouge.png");
			flammeImg[2] = Loader::loadTexture(skin, "flammejaune.png");
			flammeImg[3] = Loader::loadTexture(skin, "flammebleu.png");
			flammeImg[4] = Loader::loadTexture(skin, "flammerose.png");

			float vertices[] = {
			-0.15f, 0.15f, 0.0f,
			 0.15f, 0.15f, 0.0f,
			 0.15f,-0.15f, 0.0f,
			-0.15f,-0.15f, 0.0f
			};

			int indices[] = {
					0,1,2,//top left triangle (v0, v1, v3)
					0,2,3//bottom right triangle (v3, v1, v2)
			};

			float textureCoords[] = {
				0,1,
				1,1,
				1,0,
				0,0,
			};

			auto buttonraw = Loader::loadToVAO(vertices, sizeof(vertices), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
			button = TexturedModel(buttonraw, buttonTexts.at(5));


			float squarelraw[] = {
				-0.1f,  1.0f, 0.0f,
				0.1f,  1.0f, 0.0f,
				0.1f,  0.0f, 0.0f,
				-0.1f, 0.0f, 0.0f ,
			};
			squarel = Loader::loadToVAO(squarelraw, sizeof(squarelraw), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
		}


	}

	void reset() {

		///if (songPresent)oggA::OGG_Stop();
		//if (rhythmPresent)oggC::OGG_Stop();
		//oggB::OGG_Stop();

		//if (songPresent)oggA::OGG_FreeTune();
		//if (rhythmPresent)oggC::OGG_FreeTune();
		//oggB::OGG_FreeTune();

		//oggB::OGG_Load(guitarPath);
		//songPresent = oggA::OGG_Load(songPath);
		//rhythmPresent = oggC::OGG_Load(rhythmPath);

		//if (songPresent)oggA::OGG_Play();
		//if (rhythmPresent)oggC::OGG_Play();
		//oggB::OGG_Play();
		//pspAudioSetVolume(2, 0x8000, 0x8000);
		audio2::setVolume(100);

		timer.Stop();
		timer.Start();



		points = 0;//on remet les compteurs a 0
		showedPoints = 0;
		expectedPoints = 0;
		suite = 0;
		longestStreak = 0;
		state = play;
		for (int j = 0; j < 5; j++) {
			for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
				Note* note = *it;
				note->played = false;
				note->missed = false;
				note->pressed = false;
				note->released = false;
				note->releasetime = 0;
			}
		}
	}

	bool running() {
		return state != finished;
	}

	void update(glm::mat4 PV)
	{
		stopChangeDificulty = false;
		// boucle
		timer.Update();
		timeNow = timer.Ticks();//clock();
		oggTime = timer.Ticks();//oggB::OGG_GetTime();

		if (audio1::OGG_EndOfStream()) {
			timer.Stop();
			//if (songPresent)audio1::OGG_Stop();
			//if (rhythmPresent)audio2::OGG_Stop();
			//audio2::OGG_Stop();
			state = score;
			accuracy = accuracyPerCent(notes);

		}


		if ((points + expectedPoints - showedPoints) > SHOWED_POINTS_STEP) {
			showedPoints += SHOWED_POINTS_STEP;
		}
		else if ((-points - expectedPoints + showedPoints) > SHOWED_POINTS_STEP) {
			showedPoints -= SHOWED_POINTS_STEP;
		}
		else {
			showedPoints = points + expectedPoints;
		}

		if (state == play) {

			if (Pad::one_start()) {
				//if (songPresent)audio1::OGG_Pause();
				//if (rhythmPresent)oggC::OGG_Pause();
				//audio2::OGG_Pause();
				timer.Pause();
				state = inPause;
				pauseSelection = 0;
				motion = 0;
			}


			touchesValSet();//on met a jour l'état des touches
			// gestions des notes
			{
				expectedPoints = 0;//on calcul dans cette variable les points qui vont etre gagné mais qui ne sont pas comptabilisé dans la variable points
				for (int j = 0; j < 5; j++) {
					for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
						Note* note = *it;
						int debut = note->debut;

						if (!note->missed && !note->played && note->pressed) {
							expectedPoints += POINTS_PER_NOTE + ((oggTime > debut) ? (oggTime - debut) * POINTS_PER_MS : 0);
						}

						if (oggTime < (debut + TOLERENCE) && oggTime >(debut - TOLERENCE)) {// we are at the beginning of the note
							if (!note->played && !note->missed && !note->pressed) {
								if (firetouches[j]) {
									note->pressed = true;
									firetouches[j] = false;
									//pspAudioSetVolume(2, 0x8000, 0x8000);
									audio2::setVolume(100);
								}
							}
						}

						if (oggTime > (debut - TOLERENCE)) { // we are on or after the start of the note
							if (!note->played && !note->missed && note->pressed) {
								if (!touches[j]) {
									note->played = true;
									note->released = true;
									note->releasetime = (int)oggTime;
									if (note->releasetime > note->fin) note->releasetime = note->fin;
									noerreur(note->releasetime - note->debut);
								}
							}
						}

						if (oggTime > (debut + TOLERENCE)) {//we are after the start of the note
							if (!note->played && !note->missed && !note->pressed) {
								note->missed = true;
								erreur();
							}
						}

						if (oggTime > (note->fin + TOLERENCE)) {// the note is completely passed
											//le +TOLERENCE sert a garder la note en mode pressée et non joué assez longtemps pour que ce soit affiché
							if (!note->played && !note->missed && note->pressed) {
								note->played = true;
								note->released = true;
								note->releasetime = (int)oggTime;
								if (note->releasetime > note->fin) note->releasetime = note->fin;
								noerreur(note->releasetime - note->debut);
							}
						}
					}
				}

				for (int j = 0; j < 5; j++) {
					if (firetouches[j]) erreur();//une touche a été pressé mais elle ne correspond a aucune note
					firetouches[j] = false;
				}
			}
			//gestion de la liste des étoiles
			{
				float delta = timeNow - timeLast;
				if (rand() % 800000 < delta * mult) {
					Etoile* newEtoile = new Etoile();
					newEtoile->x = rand() % (480 - 32);
					newEtoile->y = rand() % (272 - 32);
					newEtoile->timeStart = timer.Ticks();// clock();
					etoileList.push_back(newEtoile);
				}

				while (!etoileList.empty()) {
					float time = timer.Ticks() - etoileList.front()->timeStart;
					if (time > STAR_ANIM_TIME) {
						etoileList.pop_front();
					}
					else {
						break;
					}
				}
			}
			// particle list management
			{
				for (int i = 0; i < 5; i++) {
					while (!particulesList[i].empty()) {
						float time = timer.Ticks() - particulesList[i].front()->timeStart;
						if (time > particulesList[i].front()->length) {
							particulesList[i].pop_front();
						}
						else {
							break;
						}
					}
				}
			}



			//simple acuraci calulator
			{
				int nbnotes = 1;
				int total = 1;
				for (int j = 0; j < 5; j++) {
					for (std::vector<Note*>::iterator it = notes[j].begin(); it != notes[j].end(); ++it) {
						Note* note = *it;
						if (oggTime < (note->debut - TOLERENCE)) continue;
						if (note->played) nbnotes++;
						total++;
					}
				}
				neck->setPower(((nbnotes * 100) / total));
			}


			renderScreen(PV);

			ShowScore();
		}
		else if (state == inPause) {
			if (Pad::one_start() || Pad::one_cross()) {
				//if (songPresent)audio1::OGG_Pause();
				//if (rhythmPresent)audio3::OGG_Pause();
				//audio2::OGG_Pause();
				timer.Start();
				state = play;
			}
			if (Pad::scroll_up()) {
				if (pauseSelection > 0) {
					motion -= 18;
					pauseSelection--;
				}
			}
			if (Pad::scroll_down()) {
				if (pauseSelection < 3) {
					motion += 18;
					pauseSelection++;
				}
			}
			if (Pad::one_circle()) {
				if (pauseSelection == 0) {
					//if (songPresent)audio1::OGG_Pause();
					//if (rhythmPresent)audio3::OGG_Pause();
					//audio2::OGG_Pause();
					timer.Start();
					state = play;
				}
				else if (pauseSelection == 1) {
					reset();
				}
				else if (pauseSelection == 2) {
					state = inChangeDificulty;
				}
				else if (pauseSelection == 3) {
					stop();
				}

			}

			motion = (int)(motion * TEXT_MOTION_VELOCITY);

			renderScreen(PV);

			font->Printf(10, SCREEN_HEIGHT - font->getFontHeight(), true, "%s by %s (%s)", loadedMusic->name, loadedMusic->artist, dificutyText);
			
			

			font->Printf(10, (SCREEN_HEIGHT / 2) - 0 * 40 - (-1 * pauseSelection) * 40 + motion, pauseSelection == 0, "resume");
			font->Printf(10, (720 / 2) - 1 * 40 - (-1 * pauseSelection) * 40 + motion, pauseSelection == 1, "restart music");
			font->Printf(10, (SCREEN_HEIGHT / 2) - 2 * 40 - (-1 * pauseSelection) * 40 + motion, pauseSelection == 2, "change difficulty");
			font->Printf(10, (SCREEN_HEIGHT / 2) - 3 * 40 - (-1 * pauseSelection) * 40 + motion, pauseSelection == 3, "go to main menu");

		}else if (state == inChangeDificulty) {
			stop();
			stopChangeDificulty = true;

			/*
			renderScreen(PV);
			
			font->Printf(10 * 3.22, 184 * 3.22f, 1, "Not implemented yet.."); 

			//go back
			if (Pad::one_circle()) {
				state = inPause;
			}
			*/
		}
		else if (state == score) {

			font->Printf(10 * 3.22, SCREEN_HEIGHT - font->getFontHeight(), 1, "%s by %s (%s)", loadedMusic->name, loadedMusic->artist, dificutyText);
			font->Printf(10 * 3.22, 166 * 3.22f, 1, "points = %i", points);
			font->Printf(10 * 3.22, 148 * 3.22f, 1, "accuracy = %i percent", accuracy);
			font->Printf(10 * 3.22, 130 * 3.22f, 1, "longest streak = %i", longestStreak);
			font->Printf(10 * 3.22, 112 * 3.22f, 1, "press A to quit"); 

			if (Pad::one_circle()) {
				stop();
			}
		}

		timeLast = timeNow;
	}

};

#endif