/*
 * dragway.c
 *
 *  Created on: Jan 18, 2017
 *      Author: Sam
 */

#include<stdio.h>
#include<string.h>

#define DEBUG 1
#define bufferSize 50

//structures
struct Engine {
	char name[bufferSize];
	int hp; //horsepower
	int redline;
};
struct Transmission {
	char name[bufferSize];
	int numGears;
	double ratios[6]; //6 speed transmissions max
};
struct Differential {
	char name[bufferSize];
	double ratio;
};
struct Tire {
	char name[bufferSize];
	double size; //in inches
};

//action functions
void quit(int *done) {
	char choice[bufferSize];
	printf("Are you sure you want to quit (yes or no)\n");
	fgets(choice, bufferSize, stdin);
	choice[strlen(choice) - 1] = '\0'; //removes newline
	if (!strcmp(choice, "yes")) {
		*done = 1;
	}
}

void drag() {
	printf("You are now racing!!!\n");
}

void specs() {
	printf("You have some nice specs there\n");
}

void test(struct Engine *engines, int numEngines) {
	int i;
	for (i = 0; i < numEngines; i++) {
		if (engines[i].name[0] != '\0') {
			printf("%s\n", engines[i].name);
		} else {
			printf("Engine %d does not have a name\n", i);
		}
	}
}

int main() {
	//variables
	int done = 0;
	char choice[bufferSize];
	int i = 0;
	FILE *fp;
	int switcher = 0; //used to alternate reading strings and numbers from files each line

	//parts arrays
	struct Engine engines[bufferSize];
	struct Transmission trannies[bufferSize];
	struct Differential diffs[bufferSize];
	struct Tire tires[bufferSize];

	int numEngines = 2; // could replace with end of file line detection
	int numTrans = 4;	//and increment these variables....this works fine
	int numDiffs = 3;
	int numTires = 2;

	//import parts data from files
	//import engine data
	fp = fopen("engines.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: engines.txt");
	}
	for (i = 0; i < numEngines; i++) {
		fgets(engines[i].name, bufferSize, fp);
		engines[i].name[strlen(engines[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%d %d\n", &engines[i].hp, &engines[i].redline);
	}
	fclose(fp);

	//import transmission data
	fp = fopen("transmissions.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: transmissions.txt");
	}
	for (i = 0; i < numTrans; i++) {
		fgets(trannies[i].name, bufferSize, fp);
		trannies[i].name[strlen(trannies[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%d %g %g %g %g %g %g\n", &trannies[i].numGears,
				&trannies[i].ratios[0], &trannies[i].ratios[1],
				&trannies[i].ratios[2], &trannies[i].ratios[3],
				&trannies[i].ratios[4], &trannies[i].ratios[5]);
	}
	fclose(fp);

	//import diff data
	fp = fopen("differentials.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: differentials.txt");
	}
	for (i = 0; i < numDiffs; i++) {
		fgets(diffs[i].name, bufferSize, fp);
		diffs[i].name[strlen(diffs[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%g\n", &diffs[i].ratio);
	}
	fclose(fp);

	//import tire data
	fp = fopen("tires.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: tires.txt");
	}
	for (i = 0; i < numTires; i++) {
		fgets(tires[i].name, bufferSize, fp);
		tires[i].name[strlen(tires[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%g\n", &tires[i].size);
	}
	fclose(fp);

	//assign default parts

	while (!done) { //game loop
		printf("What would you like to do?\n");
		fgets(choice, bufferSize, stdin);
		choice[strlen(choice) - 1] = '\0'; //removes newline

		if (!strcmp(choice, "help")) {
			printf("Your choices are help, quit, drag, specs\n");
		} else if (!strcmp(choice, "quit")) {
			quit(&done);
		} else if (!strcmp(choice, "drag")) {
			drag();
		} else if (!strcmp(choice, "specs")) {
			specs();
		} else if (!strcmp(choice, "test") && DEBUG) {
			test(engines, numEngines);
		} else {
			printf(
					"Please enter a valid choice.\n Type help for your options\n");
		}
	} //game loop
	return 0;
}
