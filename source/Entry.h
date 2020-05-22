#ifndef ENTRY_H
#define ENTRY_H

#include <vector> 
#include<stdio.h> 
#include <ctype.h> // contains tolower() function in switch
#include <dirent.h>

typedef struct MenuEntry {
	int type;
};

#define FOLDER_ENTRY 1
typedef struct FolderEntry {
	int type;
	int selection;
	char name[128];
	std::vector<MenuEntry*>* list;
};

#define MUSIC_ENTRY 2
typedef struct MusicEntry {
	int type;
	char dir[128];
	char name[128];
	char artist[128];
};


void iniInfo(char* filename, char* name, char* artist) {
	FILE* file = fopen(filename, "r");
	if (!file)return;

	char temp[101];
	for (int j = 0; j < 100; j++)temp[j] = 0;
	char c;
	int i = 0;
	int n = 0;
	int a = 0;
	while (fscanf(file, "%c", &c) > 0) {
		if (i > 100 || c == '\n') {
			for (int j = 0; j < 100; j++)temp[j] = 0;
			i = 0;
		}
		else {
			temp[i] = c;
			i++;
		}

		if (temp[0] == 'n' &&
			temp[1] == 'a' &&
			temp[2] == 'm' &&
			temp[3] == 'e' &&
			temp[4] == ' ' &&
			temp[5] == '=' &&
			temp[6] == ' ' &&
			i > 7) {
			name[n] = c;
			n++;
		}
		if (temp[0] == 'a' &&
			temp[1] == 'r' &&
			temp[2] == 't' &&
			temp[3] == 'i' &&
			temp[4] == 's' &&
			temp[5] == 't' &&
			temp[6] == ' ' &&
			temp[7] == '=' &&
			temp[8] == ' ' &&
			i > 9) {
			artist[a] = c;
			a++;
		}

	}


	fclose(file);

}

bool isMusicDir(char* dirname) {
	DIR* dfd = opendir(dirname);
	if (dfd > 0)
	{
		dirent* dir;

		while ((dir = readdir(dfd)) > 0)
		{ 
			if (dir->d_type & DT_REG)
			{
				//strlwr
#ifdef __SWITCH__
				strlwr(dir->d_name);  
#else
				_strlwr(dir->d_name); // for windows
#endif 

				if (strcmp(dir->d_name, "song.ini") == 0) {
					closedir(dfd);
					return true;
				}
			}
		}
		closedir(dfd);
	}
	return false;
}

void loadFileList(std::vector<MenuEntry*>* musicList, char* folder) {
	musicList->clear();

	char tmp[512];
	DIR* dfd;
	dfd = opendir(folder);
	if (dfd > 0)
	{
		dirent* dir;

		while ((dir = readdir(dfd)) > 0)
		{
			if (dir->d_type & DT_DIR)
			{
				if (dir->d_name[0] != '.')
				{
					/*Music* music = new Music();
					sprintf(music->dir,"music/%s/",dir.d_name);
					sprintf(tmp, "%ssong.ini", music->dir);
					iniInfo(tmp,music->name,music->artist);
					musicList->push_back(music);*/
					sprintf(tmp, "%s/%s", folder, dir->d_name);
					if (isMusicDir(tmp)) {
						MusicEntry* music = new MusicEntry();
						music->type = MUSIC_ENTRY;
						sprintf(music->dir, "%s/%s/", folder, dir->d_name);
						sprintf(tmp, "%ssong.ini", music->dir);
						iniInfo(tmp, music->name, music->artist);
						musicList->push_back((MenuEntry*)music);
					}
					else {
						std::vector<MenuEntry*>* subEntry = new std::vector<MenuEntry*>();
						loadFileList(subEntry, tmp);

						FolderEntry* entry = new FolderEntry();
						entry->type = FOLDER_ENTRY;
						sprintf(entry->name, dir->d_name);
						entry->list = subEntry;
						entry->selection = 0;

						musicList->push_back((MenuEntry*)entry);
					}
				}
			}
		}
		closedir(dfd);
	}
}

//TODO not working!!!!!!!!!!!!!!!!
void removeEmpty(FolderEntry* folder) {
	for (std::vector<MenuEntry*>::iterator it = folder->list->begin(); it < folder->list->end(); it++) {
		MenuEntry* entry = (MenuEntry*)*it;
		if (entry->type == FOLDER_ENTRY) {
			FolderEntry* fentry = (FolderEntry*)*it;

			if (fentry->list->empty()) {
				folder->list->erase(it);
				it--;
			}
			else {
				removeEmpty(fentry);
			}
		}
	}
}

FolderEntry* loadFileList() {
	FolderEntry* entry = new FolderEntry();
	entry->type = FOLDER_ENTRY;
	sprintf(entry->name, "music list");

	std::vector<MenuEntry*>* subEntry = new std::vector<MenuEntry*>();
	loadFileList(subEntry, (char*)"data/songs"); 
	entry->list = subEntry;
	entry->selection = 0;

	removeEmpty(entry);

	return entry;
}
#endif