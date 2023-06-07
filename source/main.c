#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <3ds.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LENGTH 255
#define SCREEN_TEXT_HEIGHT 27
#define DIRECTORY 0
#define DEFAULT 1
#define EXECUTABLE 2

#define CLRBLUE "\x1B[34m"
#define CLRGRN "\x1B[32m"
#define CLRWHT "\x1B[37m"
#define CLRYLW "\x1b[33m"

struct dirent *de; // Pointer for directory entry

char pointerDir[MAX_LENGTH+1];
int userPointer = 0;
int maxPointer;
int minPointer = 0;

struct {
	char filename[MAX_LENGTH + 1];
	int type;
} filecache[500];


void listDir(bool useCache) {
	printf("\x1B[0m");//reset color
	if (useCache == false) {//generate dir & cache
		memset(filecache->filename, 0, 25600*sizeof(char));//reset cachev
		char *cwd = getcwd(NULL, 0);

		// opendir() returns a pointer of DIR type. 
		DIR *dr = opendir(cwd);
		if (dr == NULL) {  // opendir returns NULL if couldn't open directory
			printf(CLRYLW"Could not open directory\n" );
		}
		printf("%s\n", cwd);//your current directory
		
		int i = 0;
		
		while ((de = readdir(dr)) != NULL) {//set cache
			struct stat st = {0};
			stat(de->d_name, &st);
			int scroll = 0;
			int currentType = 0;
			if (userPointer >= SCREEN_TEXT_HEIGHT) {
				scroll = userPointer - SCREEN_TEXT_HEIGHT + 1;
			}
			if (S_ISDIR(st.st_mode)) {
				currentType = DIRECTORY;
			} else if (S_ISREG(st.st_mode)) {
				char *s = strrchr(de->d_name, '.');
				if (s != NULL && (strcasecmp(s, ".3dsx") == 0)) {//exe
					currentType = EXECUTABLE;
				} else {
					currentType = DEFAULT;
				}
			} else {
				currentType = DEFAULT;
			}
			snprintf(filecache[i].filename, sizeof(filecache[i].filename), "%s", de->d_name);
			filecache[i].type = currentType;
			++i;
		}

		maxPointer = i - 1;
		//now read cache
		
		i = 0;//reset i
		
		if (userPointer == -1 && strcasecmp(cwd, "/") != 0) {
			printf(CLRWHT">"CLRBLUE"..\n\n");
			strcpy(pointerDir, "..");
			minPointer = -1;
		} else if (strcasecmp(cwd, "/") != 0) {
			printf(CLRBLUE"..\n\n");
			minPointer = -1;
		} else {
			minPointer = 0;
		}
		while (filecache[i].filename[0] != 0) {
			int scroll = 0;
			if (userPointer >= SCREEN_TEXT_HEIGHT) {
				scroll = userPointer - SCREEN_TEXT_HEIGHT + 1;
			}
			if (i < (SCREEN_TEXT_HEIGHT + scroll) && i > scroll) {
				if (filecache[i].type == DIRECTORY) {
					if (userPointer == i) {
						printf(CLRBLUE">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRBLUE"%s\n", filecache[i].filename);
					}
				} else if (filecache[i].type == EXECUTABLE) {
					if (userPointer == i) {
						printf(CLRGRN">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRGRN"%s\n", filecache[i].filename);
					}
				} else if (filecache[i].type == DEFAULT) {
					if (userPointer == i) {
						printf(CLRWHT">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRWHT"%s\n", filecache[i].filename);
					}
				}
			}
			++i;
		}
		
		
		closedir(dr);
		free(cwd);
		
	} else {// read cached dir (NOT SETTING THE CACHE. THIS IS USED IF BOOL USECACHE IS SET TO TRUE)
		int i = 0;
		char *cwd = getcwd(NULL, 0);
		printf("%s\n", cwd);//your current directory
		if (userPointer == -1 && strcasecmp(cwd, "/") != 0) {
			printf(CLRWHT">"CLRBLUE"..\n\n");
			strcpy(pointerDir, "..");
			minPointer = -1;
		} else if (strcasecmp(cwd, "/") != 0) {
			printf(CLRBLUE"..\n\n");
			minPointer = -1;
		} else {
			minPointer = 0;
		}
		while (filecache[i].filename[0] != 0) {
			int scroll = 0;
			if (userPointer >= SCREEN_TEXT_HEIGHT) {
				scroll = userPointer - SCREEN_TEXT_HEIGHT + 1;
			}
			if (i < (SCREEN_TEXT_HEIGHT + scroll) && i > scroll) {
				if (filecache[i].type == DIRECTORY) {
					if (userPointer == i) {
						printf(CLRBLUE">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRBLUE"%s\n", filecache[i].filename);
					}
				} else if (filecache[i].type == EXECUTABLE) {
					if (userPointer == i) {
						printf(CLRGRN">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRGRN"%s\n", filecache[i].filename);
					}
				} else if (filecache[i].type == DEFAULT) {
					if (userPointer == i) {
						printf(CLRWHT">%s\n", filecache[i].filename);
						snprintf(pointerDir, sizeof(pointerDir), "%s", filecache[i].filename);
					} else {
						printf(CLRWHT"%s\n", filecache[i].filename);
					}
				}
			}
			++i;
		}
	}
}

int main() {
	//set pointer array to spaces.
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	chdir("sdmc:/");
	listDir(false);//false is to reload the directory. true would be to use the cache.
	
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		
		if (kDown & KEY_UP && userPointer > minPointer) {
			--userPointer;
			consoleClear();
			listDir(true);
		}
		
		if (kDown & KEY_DOWN && userPointer < maxPointer) {
			++userPointer;
			consoleClear();
			listDir(true);
		}
		
		if (kDown & KEY_RIGHT && userPointer < maxPointer) {
			userPointer += 5;
			consoleClear();
			listDir(true);
		}
		
		if (kDown & KEY_LEFT && userPointer > minPointer) {
			userPointer -= 5;
			consoleClear();
			listDir(true);
		}
		
		if (kDown & KEY_A) {
			chdir(pointerDir);
			consoleClear();
			userPointer = 0;
			listDir(false);
		}
		
		if (kDown & KEY_B) {
			chdir("..");
			consoleClear();
			userPointer = 0;
			listDir(false);
		}
	}

	gfxExit();
	return 0;
}