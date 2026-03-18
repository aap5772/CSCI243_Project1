//
// File: brace_topia.h
// Description: contains the headers and documentation for the functions used in brace_topia_c
//
// @author Alex Polo aap5772
//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#define NEWLINE 'n'
#define ENDLINE 'e'
#define VACANT '.'

#include <stdio.h>
#include <stdlib.h>

// Default simulation values
#define DEFAULT_DIM 15
#define DEFAULT_STRENGTH 50
#define DEFAULT_VACANCY 20
#define DEFAULT_ENDLINE 60
#define DEFAULT_DELAY 900000
#define DEFAULT_CYCLE_COUNT -1  // Infinite mode

/*
* the agent struct stores the individual characteristics of the Agent like the type (Vacant, Endline, and Newline) as well as their happiness
*/
typedef struct Agent_S{
    char type;
    int happy;
} Agent;

/*
* the struct Grid has a dimention and a 2D array of Agents to track locations
*/
typedef struct Grid_S{
    int dim;
    Agent **cells;
} Grid;

/*
* @breif: Help command
*
* Description: the help command required from the instructions
*/
void print_usage();

/*
* @breif: parse commandline statements
*
* Description: uses the flag system and switch cases to test each commmandline argument then if it passes initialize it
*
* @param: int argc, the amount of commantline args
* @param: char *argv[], the commandline arguments
*/
void parse_arguments(int argc, char *argv[]);

/*
* @breif: frees the grid
*
* Description: goes through every element and frees it before freeing the grid struct
*
* @param: Grid *grid, the grid to be freed
*/
void free_grid(Grid *grid);

/*
* @breif: Initializes the Grid when defining a grid
*
* Description: Takes the parameters that arent global and makes a square grid of all the agents all in random shuffled spots
*              then returns the grid
*
* @param: int size, dimention of one side of the square grid
* @param: int end_percent, uses the percent of endlines and vacancies in unision to decide how many of each agent type goes ther    e
* @param: int vacancy_percent, similar as endpercent but with the vacancies
* @return: returns a fully initialized grid
*/
Grid * init_grid(int size, int end_percent, int vacancy_percent);

/*
* @breif: Print function
*
* Description: prints the grid and then the description as exemplified in the instructions
*
* @param: const Grid *grid, the grid we are printing
* @param: int cycle, cycle isnt global but we need to track it outside of print and so we pass it
* @param: int moves, same as cycle
*/
void print_grid(const Grid *grid, int cycle, int moves);

/*
* @breif: Calculates the happiness of all the Agents
*
* Description: loops through every Agent in the grid and checks the happiness of said agent without going through the sides of t    he grid. then it finds the average happiness with the new information
*
* @param: const Grid *grid, the grid we search through
* @return: returns a double that is the average happiness of the whole grid
*/
double calculate_happiness(const Grid *grid);

/*
* @breif: moves the Unhappy agents
*
* Description: Checks if an agent is happy, moves it to its own array then randomly reassigns them to the grid all while trackin    g the nessisary parts
*
* @param: Grid *grid, the grid we utilize
* @param: int moves, a tracker for the moves so we can print it later
* @return: returns the counted moves
*/
int move_agents(Grid *grid, int moves);

/*
* @breif: Infinite print
*
* Description: If run without a -c flag it will run infinitly
*
* @param: Grid *grid, we initilize the grid outside of it so we need to pass it
*/
void infinite_mode(Grid *grid);

/*
* @breif: prints each cycle
*
* Description: If run with a -c flag it will run that many times
*
* @param: Grid *grid, we initilize the grid outside of it so we need to pass it
* @param: int max_cycles, the amount of cycles to print
*/
void print_mode(Grid *grid, int max_cycles);

/*
* @breif: Shuffles the array
*
* Description: uses fisher yates shuffle for a given array
*
* @param: char * agents, the array we shuffle
* @param: int size, the size of the array
*/
void shuffle(char * agents, int size);
