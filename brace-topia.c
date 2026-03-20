//
// File: brace-topia.c
// Description: Simulation of a city of programmer agents with different brace placement preferences. It builds a square grid where agents live, tracks whether they are happy or unhappy, and moves unhappy agents. The grid and stats are printed each cycle. Supports infinite mode or fixed-cycle mode using flags.
//
// @author Alex Polo aap5772
//

#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "brace-topia.h"
#include "display.h"

#include <getopt.h>
extern char *optarg;

int dim = DEFAULT_DIM;
int strength = DEFAULT_STRENGTH;
int vacancy = DEFAULT_VACANCY;
int endline = DEFAULT_ENDLINE;
int delay = DEFAULT_DELAY;
int cycle_count = DEFAULT_CYCLE_COUNT;
double ave_happiness = 1;

/*
* @breif: Shuffles the array
*/
void shuffle(char * agents, int size){
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp = agents[i];
        agents[i] = agents[j];
        agents[j] = temp;
    }
}

/*
* @breif: Initializes the Grid
*/
Grid * init_grid(int size, int end_percent, int vacancy_percent){
    Grid *grid = malloc(sizeof(Grid));
    grid->dim = size;
    grid->cells = malloc(size * sizeof(Agent*));

    for(int i = 0; i < size; i++){
        grid->cells[i] = malloc(size * sizeof(Agent));
        for(int j = 0; j < size; j++){
            grid->cells[i][j].type = VACANT;
            grid->cells[i][j].happy = 1;
        }
    }

    int total_cells = size * size;
    int num_end = (total_cells * end_percent) / 100;
    int num_vacant = (total_cells * vacancy_percent) / 100;
    int num_newline = total_cells - num_vacant - num_end;

    char *agents = malloc(total_cells);
    int index = 0;

    for(int i = 0; i < num_end; i++) agents[index++] = ENDLINE;
    for(int i = 0; i < num_newline; i++) agents[index++] = NEWLINE;
    for(int i = 0; i < num_vacant; i++) agents[index++] = VACANT;

    srand(41);
    shuffle(agents, total_cells);

    index = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
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
    for(int i = 0; i < grid->dim; i++){
        free(grid->cells[i]);
    }
    free(grid->cells);
    free(grid);
}

/*
* @breif: Print function
*/
void print_grid(const Grid *grid, int cycle, int moves){
    for(int i = 0; i < grid->dim; i++){
        for(int j = 0; j < grid->dim; j++){
            put(grid->cells[i][j].type);
        }
        put('\n');
    }

    printf("cycle: %d\nmoves this cycle: %d\nteams' \"happiness\": %f\n"
           "dim: %d, %%strength of preference: %d%%, %%vacancy: %d%%, %%end: %d%%\n",
           cycle, moves, ave_happiness, grid->dim, strength, vacancy, endline);
}

/*
* @breif: Help command
*/
void print_usage() {
    fprintf(stderr,
         "usage:\n"
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
                    fprintf(stderr, "brace-topia: option requires an argument -- 't'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                delay = atoi(optarg);
                break;

            case 'c':
                if (optarg == NULL) {
                    fprintf(stderr, "brace-topia: option requires an argument -- 'c'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                cycle_count = atoi(optarg);
                break;

            case 'd':
                if (optarg == NULL) {
                    fprintf(stderr, "brace-topia: option requires an argument -- 'd'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                dim = atoi(optarg);
                break;

            case 's':
                if (optarg == NULL) {
                    fprintf(stderr, "brace-topia: option requires an argument -- 's'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                strength = atoi(optarg);
                break;

            case 'v':
                if (optarg == NULL) {
                    fprintf(stderr, "brace-topia: option requires an argument -- 'v'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                vacancy = atoi(optarg);
                break;

            case 'e':
                if (optarg == NULL) {
                    fprintf(stderr, "brace-topia: option requires an argument -- 'e'\n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                endline = atoi(optarg);
                break;

            default:
                fprintf(stderr, "brace-topia: invalid option -- '%c'\n", optopt);
                print_usage();
                exit(EXIT_FAILURE);
        }
    }
}
