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
	fgets(choice,bufferSize,stdin);
	choice[strlen(choice) - 1] = '\0'; //removes newline
	if(!strcmp(choice,"yes")){
		*done = 1;
	}
}

void drag() {
	printf("You are now racing!!!\n");
}

void specs(){
	printf("You have some nice specs there\n");
}

void test(){

}

int main() {
	//variables
	int done = 0;
	char choice[bufferSize];

	//import parts data from files

	//assign default parts

	while (!done) { //game loop
		printf("What would you like to do?\n");
		fgets(choice, bufferSize, stdin);
		choice[strlen(choice) - 1] = '\0'; //removes newline

		if       (!strcmp(choice, "help")) {
			printf("Your choices are help, quit, drag, specs\n");
		}else if (!strcmp(choice, "quit")){
			quit(&done);
		}else if (!strcmp(choice, "drag")) {
			drag();
		}else if(!strcmp(choice, "specs")){
			specs();
		}else if(!strcmp(choice, "test") && DEBUG){
			test();
		}else {
			printf("Please enter a valid choice.\n Type help for your options\n");
		}
	} //game loop
	return 0;
}
