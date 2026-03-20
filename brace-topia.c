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
* shuffle: randomizes the array order
*
* Uses Fisher-Yates shuffle on the array
*
* params: agents array, size of array
*/
void shuffle(char *agents, int size){
    for(int i = size - 1; i > 0; i--){
        int j = rand() % (i + 1);
        char temp = agents[i];
        agents[i] = agents[j];
        agents[j] = temp;
    }
}

/*
* init_grid: builds and fills the grid
*
* Allocates a square grid and distributes agent types randomly
*
* params: grid size, percent endline, percent vacancy
*/
Grid *init_grid(int size, int end_percent, int vacancy_percent){
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

    int total = size * size;
    int num_end = (total * end_percent) / 100;
    int num_vacant = (total * vacancy_percent) / 100;
    int num_newline = total - num_end - num_vacant;

    char *arr = malloc(total * sizeof(char));
    int k = 0;

    for(int i = 0; i < num_end; i++) arr[k++] = ENDLINE;
    for(int i = 0; i < num_newline; i++) arr[k++] = NEWLINE;
    for(int i = 0; i < num_vacant; i++) arr[k++] = VACANT;

    srand(41);
    shuffle(arr, total);

    k = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            grid->cells[i][j].type = arr[k++];
            grid->cells[i][j].happy = 1;
        }
    }

    free(arr);
    return grid;
}

/*
* free_grid: releases grid memory
*
* Frees all rows and the grid itself
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
* print_grid: outputs the grid state
*
* Prints grid contents and simulation stats
*/
void print_grid(const Grid *grid, int cycle, int moves){
    for(int i = 0; i < grid->dim; i++){
        for(int j = 0; j < grid->dim; j++){
            put(grid->cells[i][j].type);
        }
        put('\n');
    }

    printf("cycle: %d\nmoves this cycle: %d\nteams' \"happiness\": %f\n",
           cycle, moves, ave_happiness);
    printf("dim: %d, %%strength of preference: %d%%, %%vacancy: %d%%, %%end: %d%%\n",
           grid->dim, strength, vacancy, endline);
}

/*
* print_usage: prints usage help
*/
void print_usage(){
    fprintf(stderr,
        "usage: \n"
        "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");
}

/*
* parse_arguments: handles command-line flags
*/
void parse_arguments(int argc, char *argv[]){
    int opt;
    while((opt = getopt(argc, argv, "ht:c:d:s:v:e:")) != -1){
        switch(opt){
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case 't':
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
                exit(EXIT_FAILURE);
        }
    }
}

/*
* calculate_happiness: computes average happiness
*
* Checks neighbors and assigns happiness score
*/
double calculate_happiness(const Grid *grid){
    double total_happy = 0;
    int count = 0;

    for(int row = 0; row < grid->dim; row++){
        for(int col = 0; col < grid->dim; col++){
            int same = 0;
            int neighbors = 0;
            char t = grid->cells[row][col].type;

            if(t == VACANT){
                grid->cells[row][col].happy = 1.0;
                continue;
            }

            count++;

            if(row > 0 && grid->cells[row-1][col].type != VACANT){
                neighbors++;
                if(grid->cells[row-1][col].type == t) same++;
            }
            if(col > 0 && grid->cells[row][col-1].type != VACANT){
                neighbors++;
                if(grid->cells[row][col-1].type == t) same++;
            }

            if(neighbors == 0){
                grid->cells[row][col].happy = 1.0;
            } else {
                grid->cells[row][col].happy = (double)same / neighbors;
            }

            total_happy += grid->cells[row][col].happy;
        }
    }

    return total_happy / (grid->dim * grid->dim);
}

/*
* move_agents: relocates unhappy agents
*/
int move_agents(Grid *grid, int moves){
    int size = grid->dim * grid->dim;
    Agent *temp = malloc(size * sizeof(Agent));
    int count = 0;

    for(int i = 0; i < grid->dim; i++){
        for(int j = 0; j < grid->dim; j++){
            if(grid->cells[i][j].type != VACANT &&
               (grid->cells[i][j].happy * 100) <= strength){
                temp[count++] = grid->cells[i][j];
                grid->cells[i][j].type = VACANT;
            }
        }
    }

    for(int i = 0; i < count; i++){
        int r = rand() % grid->dim;
        int c = rand() % grid->dim;

        if(grid->cells[r][c].type == VACANT){
            grid->cells[r][c] = temp[i];
            moves++;
        }
    }

    free(temp);
    return moves;
}

void infinite_mode(Grid *grid){
    int cycle = 0;
    int moves = 0;

    while(1){
        ave_happiness = calculate_happiness(grid);
        moves = move_agents(grid, 0);
        print_grid(grid, cycle++, moves);
        usleep(delay);
    }
}

void print_mode(Grid *grid, int max_cycles){
    int cycle = 0;

    while(cycle <= max_cycles){
        ave_happiness = calculate_happiness(grid);
        int moves = move_agents(grid, 0);
        print_grid(grid, cycle++, moves);
    }
}

int main(int argc, char *argv[]){
    parse_arguments(argc, argv);
    Grid *grid = init_grid(dim, endline, vacancy);

    if(cycle_count == -1){
        infinite_mode(grid);
    } else {
        print_mode(grid, cycle_count);
    }

    free_grid(grid);
    return 0;
}
