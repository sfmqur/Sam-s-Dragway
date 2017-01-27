/*      Sam's Dragway
 * 
 *
 *  Created: January 2017
 *      Author: Sam
 *
 * A text based drag racing simulator with an emphasis on vehicle builds versus the driving of the vehicle. 
 *   The program is built more for entertainment than for physical realism as the only factors that are currently
 *   in the drag calculation are, horsepower, transmission ratios, differential ratio, and tire size.  
 */

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define DEBUG 0 //debug allows you to give yourself money using the test command
#define bufferSize 50
#define pi 3.14159265358979
#define numOpponents 5

//number of each part in their files
const int numEngines = 10;
const int numTrans = 10;
const int numDiffs = 8;
const int numTires = 9;

double money = 0;
double bestTime = 99999; //99999 because new times need to be less than old best time. no time is less than 0
double lastTime = 0;

//structures
struct Engine {
	char name[bufferSize];
	double hp; //horsepower
	double redline;
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

double randPercent() { //returns random decimal from 0 to 1;
	return (double) rand() / (double) RAND_MAX;
}

//action functions
void quit(int *done) {
	char choice[bufferSize];
	printf("Are you sure you want to quit (yes or no)\n");
	fgets(choice, bufferSize, stdin);
	choice[strlen(choice) - 1] = '\0'; //removes newline
	if (!strcmp(choice, "yes") || !strcmp(choice, "y")) {
		*done = 1;
	}
}

//results is a 2 element array, element 0 is time, 1 is speed
double dragCalc(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire, int isPlayer) {
	//isPlayer used to create live feed of drag
	double time = 0.0;
	double myTime = 0;
	double lastShift = 0; //used for live feed to time it.
	double distance = 0.25; //quarter mile drag in mi
	double myDistance = 0;
	double mySpeed = 0;
	double rpm = 0;
	double rpmPerSecond = 4.156271577; //RPM per second per horsepower, default vehicle 0-60 is 7.9 seconds(Chevy Trailblazer), this 5.15 is default
	double drdt = rpmPerSecond * engine.hp;
	double tempdrdt = 0; //used when drdt + rpm exceeds redline
	double tempstep = 0; //used when drdt + rpm exceeds redline
	int gear = 1;
	double step = 0.01; //seconds

	if (isPlayer) { //isPlayer outputs only when it is the player
		printf("\nREADY\n");
		sleep(1);
		printf("SET\n");
		sleep(1);
		printf("GO!!!\n");
	}

	while (myDistance < distance) {
		if (gear == trans.numGears && rpm == engine.redline) {
			time += step;
			myDistance += mySpeed * step / 3600;
		} else {
			if (rpm + (drdt * step) > engine.redline) { //does little time step
				tempdrdt = engine.redline - rpm;
				rpm += tempdrdt;
				tempstep = 1 / (rpmPerSecond * engine.hp / tempdrdt); //calculates time to get from beginning rpm to the redline
				time += tempstep;
				mySpeed = rpm / trans.ratios[gear - 1] / diff.ratio * pi
						* tire.size / 12 / 5280 * 60; //miles an hour
				myDistance += mySpeed * tempstep / 3600;
				if (gear < trans.numGears) { //switches gears if not in max gear
					rpm = 0;
					gear++;
					if (isPlayer) {
						sleep(round(time - lastShift));
						printf(
								"You shift into gear %d at %g seconds at %g MPH\n",
								gear, time, mySpeed);
						lastShift = time;
					}
				}
				continue;
			} //end little time step
			time += step;
			rpm += drdt * step; //eliminates roundoff error
			mySpeed = rpm / trans.ratios[gear - 1] / diff.ratio * pi * tire.size
					/ 12 / 5280 * 60; //feet per second
			myDistance += mySpeed * step / 3600;
		} //end of else ie acceleration period
	} //end of while loop

	if (isPlayer) {
		sleep(round(time - lastShift));
		printf("FINISHED!!!\n\nYou finished in %g seconds\n", time);
	}

	myTime = time;
	return myTime;
}

void drag(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire, struct Engine *engines,
		struct Transmission *trannies, struct Differential *diffs,
		struct Tire *tires) {

	double winnings = 0;
	double maxWinnings = 500; //winnings if you get 1st place

//calculate your time
	double myTime = dragCalc(engine, trans, diff, tire, 1);
	if (myTime < bestTime) {
		bestTime = myTime;
		printf("That is your new personal best!!!\n");
	} else {
		printf("Your personnal best is %g seconds.\n", bestTime);
	}
	if (lastTime != 0) {
		printf("Your last time was %g seconds\n", lastTime);
	}
	printf("\n");
	lastTime = myTime;
//Randomly Generate Opponents times
	int oppParts[numOpponents][4]; //4 is number of parts on car
	double scores[numOpponents + 1]; //room for player in scores array
	int i; //for loop variables

//generate opponents specs
	for (i = 0; i < numOpponents; i++) {
		oppParts[i][0] = rand() % numEngines;
		oppParts[i][1] = rand() % numTrans;
		oppParts[i][2] = rand() % numDiffs;
		oppParts[i][3] = rand() % numTires;
		scores[i] = dragCalc(engines[oppParts[i][0]], trannies[oppParts[i][1]],
				diffs[oppParts[i][2]], tires[oppParts[i][3]], 0); //opponent time calculation
	}

	for (i = 0; i < numOpponents; i++) { //output opponent scores
		printf("Opponent %d: %g seconds\n", i + 1, scores[i]);
	}
	printf("\n");

//creates the standings
	scores[numOpponents] = myTime; // adds my time in the array
	int sortComplete;
	double temp;
	int place;

//bubble sort the scores
	for (sortComplete = numOpponents; sortComplete > 0; sortComplete--) {
		for (i = 0; i < sortComplete; i++) {
			if (scores[i] > scores[i + 1]) {
				temp = scores[i];
				scores[i] = scores[i + 1];
				scores[i + 1] = temp;
			}
		}
	} //end bubble sort

	printf("The standings are:\n");
	for (i = 0; i <= numOpponents; i++) {
		printf("%g seconds\n", scores[i]);
		if (myTime == scores[i]) { //assign place
			place = i + 1;
		}
	}

//give winnings based on time differential between you and top score
	winnings = scores[0] / myTime * maxWinnings;
	money += winnings;

	printf("\nYou got place %d and earned $%g\n\n", place, winnings);
}

void shop(struct Engine *myEngine, struct Transmission *myTrans,
		struct Differential *myDiff, struct Tire *myTires,
		struct Engine *engines, struct Transmission *trannies,
		struct Differential *diffs, struct Tire *tires) {

//initialized display parts, part num and price
	double dispEngines[3][2];
	double costEngine = 7000;
	double dispTrannies[3][2];
	double costTrans = 5000;
	double dispDiffs[3][2];
	double costDiff = 2000;
	double dispTires[3][2];
	double costTires = 1500;

	int i, k;
	char choice[bufferSize];

//chooses parts in store and the price for them
	for (i = 0; i < 3; i++) {
		dispEngines[i][0] = rand() % numEngines; //stores integer versus structure, it saves memory
		dispEngines[i][1] = randPercent() * costEngine; //cost is percentage of teh max cost for each part
		dispTrannies[i][0] = rand() % numTrans;
		dispTrannies[i][1] = randPercent() * costTrans;
		dispDiffs[i][0] = rand() % numDiffs;
		dispDiffs[i][1] = randPercent() * costDiff;
		dispTires[i][0] = rand() % numTires;
		dispTires[i][1] = randPercent() * costTires;
	}

	//next 4 print then for loops print the parts on sale on the screen
	printf("\nEngines\n");
	for (i = 0; i < 3; i++) {
		printf("%d $%7g %30s: %6g HP %6g RPM redline\n", i, dispEngines[i][1],
				engines[dispEngines[i][0]].name, engines[dispEngines[i][0]].hp,
				engines[dispEngines[i][0]].redline);
	}

	printf("\nTrannies\n");
	for (i = 0; i < 3; i++) {
		printf("%d $%7g %20s: %d gears ", i, dispTrannies[i][1],
				trannies[dispTrannies[i][0]].name,
				trannies[dispTrannies[i][0]].numGears);
		for (k = 0; k < trannies[dispTrannies[i][0]].numGears; k++) {
			printf("%g ", trannies[dispTrannies[i][0]].ratios[k]);
		}
		printf("\n");
	}

	printf("\nDifferentials\n");
	for (i = 0; i < 3; i++) {
		printf("%d $%7g %s\n", i, dispDiffs[i][1], diffs[dispDiffs[i][0]].name);
	}

	printf("\nTires\n");
	for (i = 0; i < 3; i++) {
		printf("%d $%7g %s\n", i, dispTires[i][1], tires[dispTires[i][0]].name);
	}

	printf("\n");

	char selectionStr[bufferSize];
	int selection;

	while (1) { //shop loop
		selection = 0; //clears selection so you can enter into the boy specific part while loop
		printf("You have $%6g\n", money);
		printf("What would you like to buy?(exit to leave shop)\n");
		printf("Your choices are: engine, trans, diff, tire\n");
		fgets(choice, bufferSize, stdin);
		choice[strlen(choice) - 1] = '\0'; //removes newline
		if (!strcmp(choice, "exit")) {
			break;
		} else if (!strcmp(choice, "engine")) {
			printf("Which engine do you want?(3 to exit)\n");
			fgets(selectionStr, bufferSize, stdin);
			selectionStr[strlen(selectionStr) - 1] = '\0'; //removes newline
			if (strcmp(selectionStr, "0") && strcmp(selectionStr, "1")
					&& strcmp(selectionStr, "2") && strcmp(selectionStr, "3")) {
				printf("Please Enter either 0, 1, or 2\n");
				continue;
			}
			selection = atoi(selectionStr); //converts the string to an integer

			if (selection == 3) {
				break;
			}

			if (dispEngines[selection][1] > money) {
				printf("You can't afford that\n");
				break;
			} else {
				money -= dispEngines[selection][1]; //subtracts cost of part from your money
				*myEngine = engines[dispEngines[selection][0]]; //assigns the engine you bought as your engine
				printf("You bought %s\n\n",
						engines[dispEngines[selection][0]].name);
			}
		} else if (!strcmp(choice, "trans")) {
			printf("Which transmission do you want?(3 to exit)\n");
			fgets(selectionStr, bufferSize, stdin);
			selectionStr[strlen(selectionStr) - 1] = '\0'; //removes newline
			if (strcmp(selectionStr, "0") && strcmp(selectionStr, "1")
					&& strcmp(selectionStr, "2") && strcmp(selectionStr, "3")) {
				printf("Please Enter either 0, 1, or 2\n");
				continue;
			}
			selection = atoi(selectionStr); //string to integer

			if (selection == 3) {
				break;
			}

			if (dispTrannies[selection][1] > money) {
				printf("You can't afford that\n");
				break;
			} else {
				money -= dispTrannies[selection][1]; //subtracts cost of part bought from user's money
				*myTrans = trannies[dispTrannies[selection][0]]; //assigns transmission as user's part
				printf("You bought %s\n\n",
						trannies[dispTrannies[selection][0]].name);
			}
		} else if (!strcmp(choice, "diff")) {
			printf("DIFFS!\n");
			printf("Which differential do you want?(3 to exit)\n");
			fgets(selectionStr, bufferSize, stdin);
			selectionStr[strlen(selectionStr) - 1] = '\0'; //removes newline
			if (strcmp(selectionStr, "0") && strcmp(selectionStr, "1")
					&& strcmp(selectionStr, "2") && strcmp(selectionStr, "3")) {
				printf("Please Enter either 0, 1, or 2\n");
				continue;
			}
			selection = atoi(selectionStr); //string to integer

			if (selection == 3) {
				break;
			}

			if (dispDiffs[selection][1] > money) {
				printf("You can't afford that\n");
				break;
			} else {
				money -= dispDiffs[selection][1]; //subtracts cost of part bought from user's money
				*myDiff = diffs[dispDiffs[selection][0]]; //assigns new diff to user
				printf("You bought %s\n", diffs[dispDiffs[selection][0]].name);
			}
		} else if (!strcmp(choice, "tire")) {
			printf("Which tire do you want?(3 to exit)\n");
			fgets(selectionStr, bufferSize, stdin);
			selectionStr[strlen(selectionStr) - 1] = '\0'; //removes newline
			if (strcmp(selectionStr, "0") && strcmp(selectionStr, "1")
					&& strcmp(selectionStr, "2") && strcmp(selectionStr, "3")) {
				printf("Please Enter either 0, 1, or 2\n");
				continue;
			}
			selection = atoi(selectionStr);

			if (selection == 3) {
				break;
			}

			if (dispTires[selection][1] > money) {
				printf("You can't afford that\n");
				break;
			} else {
				money -= dispTires[selection][1];
				*myTires = tires[dispTires[selection][0]];
				printf("You bought %s tires\n",
						tires[dispTires[selection][0]].name);
			}
		} else {
			printf("Please enter a valid option.\n");
		}
	} //end shop loop
}

void specs(struct Engine engine, struct Transmission trans,
		struct Differential diff, struct Tire tire) {
	int i;
	//prints out current money and current parts on vehicle
	printf("\nYou have $%g\n", money);
	printf("\nEngine: %s %g horsepower %g RPM redline\n", engine.name,
			engine.hp, engine.redline);
	printf("Transmission: %s %d gears ", trans.name, trans.numGears);
	for (i = 0; i < trans.numGears; i++) {
		printf("%g ", trans.ratios[i]);
		if (i == trans.numGears - 1) {
			printf("\n");
		}
	}
	printf("Differential: %g\n", diff.ratio);
	printf("Tires: %g inches\n\n", tire.size);
}

//!!!!!WARNING!!!!!
//Test has potential to break and crash the program, it is a debug feature meant for
//developer use
void test() { //can be used only if DEBUG it 1
	printf("Money?? I got you covered\n");
	scanf("%lf", &money); //sets the user to specific money value,
}

int main() {
	srand(time(NULL)); //seeds rand with the current time
//variables
	int done = 0;
	char choice[bufferSize];
	int i = 0;
	FILE *fp;

//parts arrays
	struct Engine engines[bufferSize];
	struct Transmission trannies[bufferSize];
	struct Differential diffs[bufferSize];
	struct Tire tires[bufferSize];

//import parts data from files
//import engine data
	fp = fopen("engines.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: engines.txt");
	}
	for (i = 0; i < numEngines; i++) {
		fgets(engines[i].name, bufferSize, fp); //reads name
		engines[i].name[strlen(engines[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%lf %lf\n", &engines[i].hp, &engines[i].redline); //reads numbers
	}
	fclose(fp);

//import transmission data
	fp = fopen("transmissions.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: transmissions.txt");
	}
	for (i = 0; i < numTrans; i++) {
		fgets(trannies[i].name, bufferSize, fp); //reads name
		trannies[i].name[strlen(trannies[i].name) - 1] = '\0'; //removes newline
		//reads numbers
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
		fgets(diffs[i].name, bufferSize, fp); //reads name
		diffs[i].name[strlen(diffs[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%lf\n", &diffs[i].ratio); //reads numbers
	}
	fclose(fp);

//import tire data
	fp = fopen("tires.txt", "r");
	if (fp == NULL) { //ends program if there is a file reading error
		printf("Error from reading from file: tires.txt");
	}
	for (i = 0; i < numTires; i++) {
		fgets(tires[i].name, bufferSize, fp); //reads name
		tires[i].name[strlen(tires[i].name) - 1] = '\0'; //removes newline
		fscanf(fp, "%lf\n", &tires[i].size); //reads numbers
	}
	fclose(fp);

//assign default parts
	struct Engine myEngine = engines[0];
	struct Transmission myTrans = trannies[7];
	struct Differential myDiff = diffs[0];
	struct Tire myTire = tires[0];

	while (!done) { //game loop
		printf("What would you like to do?\n");
		fgets(choice, bufferSize, stdin);
		choice[strlen(choice) - 1] = '\0'; //removes newline

		if (!strcmp(choice, "help")) {
			printf("Your choices are help, quit, drag, shop, specs, money\n");
		} else if (!strcmp(choice, "quit")) {
			quit(&done);
		} else if (!strcmp(choice, "drag")) {
			drag(myEngine, myTrans, myDiff, myTire, engines, trannies, diffs,
					tires);
		} else if (!strcmp(choice, "shop")) {
			shop(&myEngine, &myTrans, &myDiff, &myTire, engines, trannies,
					diffs, tires);
		} else if (!strcmp(choice, "specs") || !strcmp(choice, "money")) {
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
