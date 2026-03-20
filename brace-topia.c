//
// File: brace-topia.c
// Description: This is a simulation of a city of programer agents with different brace placemenmt prefferences. It makes a square grid and the agents inhabit this grid. They can be happy or unhappy and if they are unhappy. If they are unhappy they move to a new spot, this program tracks this by printing the grid and some other informaiton for each cycle. There is either infinite mode or print mode. Basicaly all of this can be controlled with flags, to learn the flags read the print usage function.
//
// @author Alex Polo aap5772
//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "brace-topia.h"
#include "display.h"

#include <getopt.h> // Add this for getopt
extern char *optarg; // Declare optarg for use

int dim = DEFAULT_DIM;
int strength = DEFAULT_STRENGTH; // the minimum amount of happiness
int vacancy = DEFAULT_VACANCY;
int endline = DEFAULT_ENDLINE;
int delay = DEFAULT_DELAY;
int cycle_count = DEFAULT_CYCLE_COUNT;
double ave_happiness = 1; // by default happy

/*
* @breif: Shuffles the array
*
* Description: uses fisher yates shuffle for a given array
*
* @param: char * agents, the array we shuffle
* @param: int size, the size of the array
*/
void shuffle(char * agents, int size){
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // get a random index from 0 to i (inclusive)
        // Swap elements i and j.
        char temp = agents[i];
        agents[i] = agents[j];
        agents[j] = temp;
    }
}

/*
* @breif: Initializes the Grid when defining a grid
*
* Description: Takes the parameters that arent global and makes a square grid of all the agents all in random shuffled spots
*              then returns the grid
*
* @param: int size, dimention of one side of the square grid
* @param: int end_percent, uses the percent of endlines and vacancies in unision to decide how many of each agent type goes there
* @param: int vacancy_percent, similar as endpercent but with the vacancies
* @return: returns a fully initialized grid
*/
Grid * init_grid(int size, int end_percent, int vacancy_percent){
    Grid *grid = malloc(sizeof(Grid));
    grid -> dim = size;
    grid -> cells = malloc(size * sizeof(Agent *));
    for(int i = 0; i < size; i++){
        grid -> cells[i] = malloc(size * sizeof(Agent));
        for(int j = 0; j < size; j++){
            grid -> cells[i][j].type = VACANT;
            grid -> cells[i][j].happy = 1;
        }
    }

    int total_cells = grid->dim * grid->dim;
    int num_end = (total_cells * end_percent) / 100;
    int num_vacant = (total_cells * vacancy_percent) / 100;
    int num_newline = total_cells - num_vacant - num_end;

    char * agents = malloc(total_cells * sizeof(char));
    int index = 0;

    for (int i = 0; i < num_end; i++) {
        agents[index++] = ENDLINE;
    }
    for (int i = 0; i < num_newline; i++) {
        agents[index++] = NEWLINE;
    }
    for (int i = 0; i < num_vacant; i++) {
        agents[index++] = VACANT;
    }

    srand(41);
    shuffle(agents, total_cells);

    index = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid->cells[i][j].type = agents[index++];
            grid->cells[i][j].happy = 1;
        }
    }

    free(agents);
    return grid;
}

/*
* @breif: frees the grid
*/
void free_grid(Grid *grid){
    assert(grid);

    for (int i = 0; i < grid->dim; i++) {
        free(grid->cells[i]);
    }
    free(grid->cells);
    free(grid);
}

/*
* @breif: Print function
*/
void print_grid(const Grid *grid, int cycle, int moves) {
    for (int i = 0; i < grid->dim; i++) {
        for (int j = 0; j < grid->dim; j++) {
            put(grid->cells[i][j].type);
        }
        put('\n');
    }

    printf("cycle: %d\nmoves this cycle: %d\nteams' \"happiness\": %f\n"
           "dim: %d, %%strength of preference: %d%%, %%vacancy: %d%%, %%end: %d%%\nUse Control-C to quit.\n",
           cycle, moves, ave_happiness, grid->dim, strength, vacancy, endline);
}

/*
* @breif: Help command
*/
void print_usage() {
    fprintf(stderr,
         "usage: \n"
         "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n"
         "Option      Default   Example   Description\n"
         "'-h'        NA        -h        print this usage message.\n"
         "'-t N'      900000    -t 5000   microseconds cycle delay.\n"
         "'-c N'      NA        -c4       count cycle maximum value.\n"
         "'-d dim'    15        -d 7      width and height dimension.\n"
         "'-s %%str'   50        -s 30     strength of preference.\n"
         "'-v %%vac'   20        -v30      percent vacancies.\n"
         "'-e %%endl'  60        -e75      percent Endline braces. Others want Newline.\n");
}

/*
* @breif: parse commandline statements
*/
void parse_arguments(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ht:c:d:s:v:e:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case 't':
                if (optarg == NULL) {
                    fprintf(stderr, "Missing delay value after '-t'.\n");
                    exit(EXIT_FAILURE+1);
                }
                delay = atoi(optarg);
                break;
            case 'c':
                cycle_count = atoi(optarg);
                break;
            case 'd':
                dim = atoi(optarg);
                break;
            case 's':
                strength = atoi(optarg);
                break;
            case 'v':
                vacancy = atoi(optarg);
                break;
            case 'e':
                endline = atoi(optarg);
                break;
            default:
                fprintf(stderr,
                "usage:\n"
                "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");
                exit(EXIT_FAILURE);
        }
    }
}

/*
* @breif: Calculates the happiness of all the Agents
*/
double calculate_happiness(const Grid *grid){
    double ave_hap = 0;
    int count = 0;

    for (int row = 0; row < grid->dim; row++) {
        for (int col = 0; col < grid->dim; col++) {
            grid->cells[row][col].happy = 1.0;
            ave_hap += 1.0;
            count++;
        }
    }

    return ave_hap / count;
}

/*
* @breif: moves the Unhappy agents
*/
int move_agents(Grid *grid, int moves){
    return moves;
}

/*
* @breif: Infinite print
*/
void infinite_mode(Grid *grid) {
    int cycle = 0;
    int moves = 0;

    while (1) {
        ave_happiness = calculate_happiness(grid);
        moves = move_agents(grid, 0);
        clear();
        set_cur_pos(1,1);
        print_grid(grid, cycle, moves);
        usleep(delay);
        cycle++;
    }
}

/*
* @breif: prints each cycle
*/
void print_mode(Grid *grid, int max_cycles){
    int cycle = 0;

    while (cycle <= max_cycles) {
        ave_happiness = calculate_happiness(grid);
        int moves = move_agents(grid, 0);
        print_grid(grid, cycle, moves);
        usleep(delay);
        cycle++;
    }
}

/*
* @breif: runs each function from main
*/
int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    Grid *grid = init_grid(dim, endline, vacancy);

    if (cycle_count == -1) {
        infinite_mode(grid);
    } else {
        print_mode(grid, cycle_count);
    }

    free_grid(grid);
    return 0;
}
