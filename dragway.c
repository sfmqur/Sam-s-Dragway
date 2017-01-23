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
#define pi 3.14159265358979

//structures
struct Engine {
	char name[bufferSize];
	double hp; //horsepower
	double redline;
};
struct Transmission {
	char name[bufferSize];
	double numGears;
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

//TODO: opponents time rename this function to return a double(time) and create new racing function.
double dragCalc(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire) {
	double time = 0.0;
	double myTime = 0;
	double distance = 5280 / 4; //quarter mile drag
	double myDistance = 0;
	double mySpeed = 0;
	double rpm = 0;
	double rpmPerSecond = 5.156271577; //RPM per second per horsepower, default vehicle 0-60 is 7.9 seconds(Chevy Trailblazer)
	double drdt = rpmPerSecond * engine.hp;
	double tempdrdt = 0; //used when drdt + rpm exceeds redline
	double tempstep = 0; //used when drdt + rpm exceeds redline
	int gear = 1;
	double step = 0.1;

	while (myDistance < distance) {
		if (gear != trans.numGears && rpm != engine.redline) { //check to make sure engine and trans are not maxed out
			if (rpm + (drdt * step) > engine.redline) { //does little time step
				tempdrdt = engine.redline - rpm;
				rpm += tempdrdt;
				time += 1 / (rpmPerSecond * engine.hp / tempdrdt);
				mySpeed = rpm / trans.ratios[gear - 1] / diff.ratio * pi
						* tire.size * 60 / 12; //feet per second
				myDistance += mySpeed * step;
				if (gear != trans.numGears) { //switches gears if not in max gear
					gear++;
				}
				continue;
			}
			time += step;
			rpm = drdt * time; //eliminates roundoff error
			mySpeed = rpm / trans.ratios[gear - 1] / diff.ratio * pi * tire.size
					* 60 / 12; //feet per second
			myDistance += mySpeed * step; // TODO: may exceed quarter mile
		} else {
			myDistance += mySpeed * step;
		}
	}
	return mySpeed;
}

void drag(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire) {
	double myTime = dragCalc(engine, trans, diff, tire);
	printf("My time is %g seconds\n", myTime);
}

void specs(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire) {
	int i;

	printf("Engine: %s %g horsepower %g RPM redline\n", engine.name, engine.hp,
			engine.redline);
	printf("Transmission: %s, %d gears ",trans.name,trans.numGears);
	for (i = 0; i < trans.numGears; i++){
		printf("%g ",trans.ratios[i]);
		if (i == trans.numGears - 1){
			printf("\n");
		}
	}
	printf("Differential: %g\n",diff.ratio);
	printf("Tires: %g inches\n",tire.size);
}

void test(struct Transmission trans) {
	int i;
	for (i = 0; i < trans.numGears; i++) {
		printf("%g ",trans.ratios[i]);
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

	int numEngines = 4; // could replace with end of file line detection
	int numTrans = 7;	//and increment these variables....this works fine
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
		fscanf(fp, "%lf %lf\n", &engines[i].hp, &engines[i].redline);
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
		fscanf(fp, "%d %lf %lf %lf %lf %lf %lf\n", &trannies[i].numGears,
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
		fscanf(fp, "%lf\n", &diffs[i].ratio);
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
		fscanf(fp, "%lf\n", &tires[i].size);
	}
	fclose(fp);

//assign default parts
	struct Engine myEngine = engines[0];
	struct Transmission myTrans = trannies[0];
	struct Differential myDiff = diffs[0];
	struct Tire myTire = tires[0];

	while (!done) { //game loop
		printf("What would you like to do?\n");
		fgets(choice, bufferSize, stdin);
		choice[strlen(choice) - 1] = '\0'; //removes newline

		if (!strcmp(choice, "help")) {
			printf("Your choices are help, quit, drag, specs\n");
		} else if (!strcmp(choice, "quit")) {
			quit(&done);
		} else if (!strcmp(choice, "drag")) {
			drag(myEngine, myTrans, myDiff, myTire);
		} else if (!strcmp(choice, "specs")) {
			specs(myEngine, myTrans, myDiff, myTire);
		} else if (!strcmp(choice, "test") && DEBUG) {
			test(myTrans);
		} else {
			printf(
					"Please enter a valid choice.\n Type help for your options\n");
		}
	} //game loop
	return 0;
}
