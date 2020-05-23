#include <iostream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>

#include "audio/pogg.h"
#include "Player.h"
#include "PCamera.h"

#include "Window.h"
#include "Shader.h"
#include "Renderer.h"
#include "RawModel.h"
#include "Entry.h" 
#include "Font.h"

// BG credits https://unsplash.com/@narrowedge
/**
 * liste des états possibles du menu principale
 */
enum GAME_STATES {
	intro,
	mainMenu,
	credits,
	musicSelection,
	difficultySelection,
	gameLoading,
	gaming,
	error,
	aide,
	configMenu
};
 

int main(int argc, char* argv[])
{
	Window window;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init_audio();


	const char* skin = "data/skins/hero/";

	 
	Shader _shader("data/shader/vert.txt", "data/shader/sha.txt");
	_shader.use();
	_shader.setVec4("color", glm::vec4(1, 1, 1, 1));
	Shader _shaderColor("data/shader/vertcolor.txt", "data/shader/shacolor.txt");
	 
	 
	//audio1::setVolume(0);
	//audio2::setVolume(0);

	float Bgvertices[] = {
		1.0f,  1.0f, 0.0f,   // top right
		1.0f, 0.0f, 0.0f,   // bottom right
		0.0f, 0.0f, 0.0f,   // bottom left
		0.0f,  1.0f, 0.0f,   // top left 
	};

	int indices[] = {
			0,1,2,
			0,2,3
	};

	float textureCoords[] = {
			1,1,
			1,0,
			0,0,
			0,1,
	};

	auto bgnraw = Loader::loadToVAO(Bgvertices, sizeof(Bgvertices), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
	auto logoraw = Loader::loadToVAO(Bgvertices, sizeof(Bgvertices), textureCoords, sizeof(textureCoords), indices, sizeof(indices));
	auto bgImage = TexturedModel(bgnraw, Loader::loadTexture(skin, "fond.png"));
	auto logoImage = TexturedModel(logoraw, Loader::loadTexture(skin, "logo.png"));
	bgImage.getModel().scale(glm::vec3(SCREEN_WIDTH, SCREEN_HEIGHT, 1));
	logoImage.getModel().scale(glm::vec3(250,100, 1));

	PCamera cam;
	cam.translateZ(-1.7f);
	//cam.translateY(-10);
	//cam.translateZ(-10);

	cam.rotate(glm::vec3(33.0f, 0, 180.f));
	//cam.rotate(glm::vec3(0.0f, 0, 180.f));

	glm::mat4 ortview = glm::ortho<float>(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);

	Renderer renderer;
	

	Font arial(renderer, ortview);
	arial.loadFont("data/Adigiana Ultra.ttf", 0, 58.0f);
	auto lview = TextPrinter(&arial, &arial);


	char tmp[100];
	int difficulty;//numero de la difficulté séléctionné
	int possiblesDifficulties[4];//possiblesDifficulties[i] = nombre de notes pour la difficulté i
	std::vector<char*>  difficultyMenuTxt;

	int mainMenuSelection = 0;
	std::vector<char*> mainMenuTxt;
	mainMenuTxt.push_back((char*)"play");
	mainMenuTxt.push_back((char*)"config");
	mainMenuTxt.push_back((char*)"help");
	mainMenuTxt.push_back((char*)"credits");
	mainMenuTxt.push_back((char*)"quit");

	std::vector<FolderEntry*> musicListMenu;
	FolderEntry* musicList = loadFileList();
	musicListMenu.push_back(musicList);

	GAME_STATES state = intro;

  

	Player* p = nullptr;
	 
	Uint32 introStart = SDL_GetTicks();

	while (window.isClosedRequested()) {
		_shader.use();

		renderer.prepare();

		_shader.use();
		_shader.setInt("textureSampler", 0);
		_shader.setMat4("pmv", ortview * bgImage.getModel().getTransform());
		renderer.render(bgImage);

		if (state == intro) {
			float t2 = ((float)SDL_GetTicks() - introStart)/1000.0f; 
			auto text = "press + to continue"; 
			arial.Printf((SCREEN_WIDTH/2) -  (arial.mesureText(text)/2), 30 * 3.22f, glm::vec4(1,1, 1, (sin(t2*4) + 1) / 4) + 0.3f, text);
			if (Pad::start()) {
				state = mainMenu;
			} 
			
			logoImage.getModel().translate(glm::vec3(2,2, 0));
			_shader.setMat4("pmv", ortview * logoImage.getModel().getTransform());
			renderer.render(logoImage);
		}
		else if (state == mainMenu) {

			if (Pad::scroll_down()) {
				if (mainMenuSelection < 4) {
					mainMenuSelection++;
					lview.motionDown();
				}
			}
			if (Pad::scroll_up()) {
				if (mainMenuSelection > 0) {
					mainMenuSelection--;
					lview.motionUp();
				}
			}
			if (Pad::one_circle()) {
				if (mainMenuSelection == 0) {
					lview.resetMotionMenu();
					state = musicSelection;
				}
				else if (mainMenuSelection == 1) {
					lview.resetMotionMenu();
					state = configMenu;
				}
				else if (mainMenuSelection == 2) {
					lview.resetMotionMenu();
					state = aide;
				}
				else if (mainMenuSelection == 3) {
					lview.resetMotionMenu();
					state = credits;
				}
				else if (mainMenuSelection == 4) {
					break;
				}
			}


			lview.printMenu(mainMenuTxt, mainMenuSelection);
		}
		else if (state == credits) {

			if (Pad::one_circle()) {
				state = mainMenu;
			}

			arial.Printf(10 * 3.22, 30 * 3.22f, 1, "backgroud images by https://unsplash.com/@narrowedge");
			arial.Printf(10 * 3.22, 70 * 3.22f, 1, "https://github.com/tijeras94/GuitarNX");
			arial.Printf(10 * 3.22, 90 * 3.22f, 1,  "programed by Jaime Tijerina");
			arial.Printf(10 * 3.22, 130 * 3.22f,1, "GuitarNX v0.0.1");


		}
		else if (state == configMenu) {
			arial.Printf(10 * 3.22, 30 * 3.22f, 1, "Not Implemented :(");
			//if (config::configMenu() == false) state = mainMenu;
			if (Pad::one_circle()) {
				state = mainMenu;
			}

		}
		else if (state == aide) {

			if (Pad::one_circle()) {
				state = mainMenu;
			}
			arial.Printf(10 * 3.22f, 30 * 3.22f, 1,  "X : yellow + pink");
			arial.Printf(10 * 3.22f, 50 * 3.22f, 1, "A : pink");
			arial.Printf(10 * 3.22f, 70 * 3.22f, 1, "B : blue");
			arial.Printf(10 * 3.22f, 90 * 3.22f, 1, "Y : yellow");
			arial.Printf(10 * 3.22f, 110 * 3.22f, 1, "up: green + red");
			arial.Printf(10 * 3.22f, 130 * 3.22f, 1, "down: red");
			arial.Printf(10 * 3.22f, 150 * 3.22f, 1, "left: green");
			arial.Printf(10 * 3.22f, 170 * 3.22f, 1, "-- Controls -- ");

		}
		else if (state == musicSelection) {

			FolderEntry* fentry = musicListMenu.back();

			//if (fentry->list->at(fentry->selection)->type == MUSIC_ENTRY && onEntrySince + 1000000 < clock()) {
			//	startPreview((MusicEntry*)fentry->list->at(fentry->selection));
			//}

			if (Pad::scroll_down()) {
				if (fentry->selection < fentry->list->size() - 1) {
					fentry->selection++;
					lview.motionDown();
					//stopPreview();
				}
			}
			if (Pad::scroll_up()) {
				if (fentry->selection > 0) {
					fentry->selection--;
					lview.motionUp();
					//stopPreview();
				}
			}
			if (Pad::one_circle()) {
				if (fentry->list->at(fentry->selection)->type == MUSIC_ENTRY) {
					MusicEntry* music = (MusicEntry*)fentry->list->at(fentry->selection);
					sprintf(tmp, "%snotes.mid", music->dir);
					if (cfileexists(tmp)) {
						possiblesDifficulties[0] = numberOfNotes(tmp, 0);
						possiblesDifficulties[1] = numberOfNotes(tmp, 1);
						possiblesDifficulties[2] = numberOfNotes(tmp, 2);
						possiblesDifficulties[3] = numberOfNotes(tmp, 3);
						difficultyMenuTxt.clear();
						possiblesDifficulties[0] ? difficultyMenuTxt.push_back((char*)"super easy") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[1] ? difficultyMenuTxt.push_back((char*)"easy") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[2] ? difficultyMenuTxt.push_back((char*)"medium") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[3] ? difficultyMenuTxt.push_back((char*)"expert") : difficultyMenuTxt.push_back((char*)" ");
						state = difficultySelection;
						difficulty = 0;
						while (!possiblesDifficulties[difficulty]) {
							difficulty++;
						}
						
					}
					else
					{
						//check for char
						possiblesDifficulties[0] = 1;
						possiblesDifficulties[1] = 0;
						possiblesDifficulties[2] = 0;
						possiblesDifficulties[3] = 0;
						difficultyMenuTxt.clear();
						possiblesDifficulties[0] ? difficultyMenuTxt.push_back((char*)"Normal") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[1] ? difficultyMenuTxt.push_back((char*)"easy") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[2] ? difficultyMenuTxt.push_back((char*)"medium") : difficultyMenuTxt.push_back((char*)" ");
						possiblesDifficulties[3] ? difficultyMenuTxt.push_back((char*)"expert") : difficultyMenuTxt.push_back((char*)" ");
						state = difficultySelection;
						difficulty = 0;
						while (!possiblesDifficulties[difficulty]) {
							difficulty++;
						}
						
					}
				
					lview.resetMotionMenu();

				}
				else if (fentry->list->at(fentry->selection)->type == FOLDER_ENTRY) {
					FolderEntry* folder = (FolderEntry*)fentry->list->at(fentry->selection);
					musicListMenu.push_back(folder);
				}
				//stopPreview();
			}
			if (Pad::one_cross()) {
				lview.resetMotionMenu();
				if (musicListMenu.size() > 1) {
					musicListMenu.pop_back();
				}
				else {
					state = mainMenu;
				}
				//stopPreview();
			}

			lview.printMenuF(fentry);
		}
		else if (state == difficultySelection) {

			if (Pad::one_up()) {
				int nextD = difficulty;
				do {
					nextD--;
				} while (!possiblesDifficulties[nextD] && nextD >= 0);
				if (nextD >= 0) {
					lview.motionUp();
					difficulty = nextD;
				}
			}

			if (Pad::one_down()) {
				int nextD = difficulty;
				do {
					nextD++;
				} while (!possiblesDifficulties[nextD] && nextD <= 4);
				if (nextD < 4) {
					lview.motionDown();
					difficulty = nextD;
				}
			}
			if (Pad::one_cross()) {
				state = musicSelection;
				//onEntrySince = clock();
				lview.resetMotionMenu();
			}
			if (Pad::one_circle()) {
				auto text = "Loading Song....";
				arial.Printf(SCREEN_WIDTH - arial.mesureText(text)- 5, 30 * 3.22f, 1, text);
				state = gameLoading;
			}else{
				lview.printMenu(difficultyMenuTxt, difficulty);// only print when game not started
			}

		} else if (state == gameLoading){
			p = new Player(skin, &renderer, &_shader, &_shaderColor, &arial);

			FolderEntry* fentry = musicListMenu.back();
			MusicEntry* music = (MusicEntry*)fentry->list->at(fentry->selection);
			p->loadMusic(music->dir, difficulty);

			p->reset();
			state = gaming;
		}
		else if (state == gaming) {
			state = gaming;

			p->update(cam.getPV());

			if (!p->running()) {
				delete p;
				p = 0;
				state = mainMenu;
			}
		}
		else if (state == error) {
			arial.Printf(10 * 3.22, 30 * 3.22f, 1, "On Error State....");
			if (Pad::one_circle()) {
			}
		}

	
		window.updateDisplay();
	}
	 
//
//	audio1::OGG_End();
//	audio2::OGG_End();
	 
	end_audio();

	window.destroy();
	return 0;
}