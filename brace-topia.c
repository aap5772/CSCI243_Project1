#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "brace-topia.h"
#include "display.h"
#include <getopt.h>

extern char *optarg;

int dim=DEFAULT_DIM;
int strength =DEFAULT_STRENGTH;
int vacancy= DEFAULT_VACANCY;
int endline =DEFAULT_ENDLINE;
int delay=DEFAULT_DELAY;
int cycle_count = DEFAULT_CYCLE_COUNT;
double ave_happiness=1;

void shuffle(char *agents,int size){
    for(int i=size-1;i>0;i--){
        int j = rand()%(i+1);
        char tmp=agents[i];
        agents[i]=agents[j];
        agents[j]=tmp;
    }
}

Grid *init_grid(int size,int end_percent,int vacancy_percent){
    Grid *grid=malloc(sizeof(Grid));
    grid->dim=size;
    grid->cells= malloc(size*sizeof(Agent*));

    for(int i=0;i<size;i++){
        grid->cells[i]=malloc(size*sizeof(Agent));
        for(int j=0;j<size;j++){
            grid->cells[i][j].type=VACANT;
            grid->cells[i][j].happy =1;
        }
    }

    int total=size*size;
    int num_end=(total*end_percent)/100;
    int num_vacant = (total*vacancy_percent)/100;
    int num_newline=total-num_end-num_vacant;

    char *arr= malloc(total*sizeof(char));
    int k=0;

    for(int i=0;i<num_end;i++){
        arr[k++]=ENDLINE;
    }
    for(int i=0;i<num_newline;i++){
        arr[k++]=NEWLINE;
    }
    for(int i=0;i<num_vacant;i++){
        arr[k++]=VACANT;
    }

    srand(41);
    shuffle(arr,total);

    k=0;
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            grid->cells[i][j].type=arr[k++];
            grid->cells[i][j].happy=1;
        }
    }

    free(arr);
    return grid;
}

void free_grid(Grid *grid){
    assert(grid);
    for(int i=0;i<grid->dim;i++){
        free(grid->cells[i]);
    }
    free(grid->cells);
    free(grid);
}

void print_grid(const Grid *grid,int cycle,int moves){
    for(int i=0;i<grid->dim;i++){
        for(int j=0;j<grid->dim;j++){
            put(grid->cells[i][j].type);
        }
        put('\n');
    }

    printf("cycle: %d\nmoves this cycle: %d\nteams' \"happiness\": %f\n",cycle,moves,ave_happiness);
    printf("dim: %d, %%strength of preference: %d%%, %%vacancy: %d%%, %%end: %d%%\n",grid->dim,strength,vacancy,endline);
    printf("Use Control-C to quit.\n");
}

void print_usage(){
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

void parse_arguments(int argc,char *argv[]){
    int opt;
    while((opt=getopt(argc,argv,"ht:c:d:s:v:e:"))!=-1){
        switch(opt){
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case 't':
                if(optarg==NULL){
                    fprintf(stderr,"Missing delay value after '-t'.\n");
                    exit(EXIT_FAILURE+1);
                }
                delay=atoi(optarg);
                if(delay<=0){
                    fprintf(stderr,"Invalid delay value. Using default: %d\n",DEFAULT_DELAY);
                    delay=DEFAULT_DELAY;
                }
                break;
            case 'c':
                if(optarg==NULL){
                    fprintf(stderr,"Missing cycle value after '-c'.\n");
                    exit(EXIT_FAILURE+1);
                }
                cycle_count=atoi(optarg);
                if(cycle_count<0){
                    fprintf(stderr,"Invalid cycle count.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'd':
                if(optarg==NULL){
                    fprintf(stderr,"Missing dimension value.\n");
                    exit(EXIT_FAILURE+1);
                }
                dim=atoi(optarg);
                if(dim<5||dim>39){
                    fprintf(stderr,"Invalid dimension size.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if(optarg==NULL){
                    fprintf(stderr,"Missing strength value.\n");
                    exit(EXIT_FAILURE+1);
                }
                strength=atoi(optarg);
                break;
            case 'v':
                if(optarg==NULL){
                    fprintf(stderr,"Missing vacancy value.\n");
                    exit(EXIT_FAILURE+1);
                }
                vacancy=atoi(optarg);
                break;
            case 'e':
                if(optarg==NULL){
                    fprintf(stderr,"Missing endline value.\n");
                    exit(EXIT_FAILURE+1);
                }
                endline=atoi(optarg);
                break;
            default:
                fprintf(stderr,"usage:\n");
                exit(EXIT_FAILURE);
        }
    }
}

double calculate_happiness(const Grid *grid){
    double total_happy=0;
    int count=0;

    for(int row=0;row<grid->dim;row++){
        for(int col=0;col<grid->dim;col++){
            int same=0;
            int neighbors=0;
            char t=grid->cells[row][col].type;

            if(t==VACANT){
                grid->cells[row][col].happy=1.0;
                continue;
            }

            count++;

            if(row>0){
                if(grid->cells[row-1][col].type!=VACANT){
                    neighbors++;
                    if(grid->cells[row-1][col].type==t) same++;
                }
            }
            if(row<grid->dim-1){
                if(grid->cells[row+1][col].type!=VACANT){
                    neighbors++;
                    if(grid->cells[row+1][col].type==t) same++;
                }
            }
            if(col>0){
                if(grid->cells[row][col-1].type!=VACANT){
                    neighbors++;
                    if(grid->cells[row][col-1].type==t) same++;
                }
            }
            if(col<grid->dim-1){
                if(grid->cells[row][col+1].type!=VACANT){
                    neighbors++;
                    if(grid->cells[row][col+1].type==t) same++;
                }
            }

            if(neighbors==0){
                grid->cells[row][col].happy=1.0;
            } else{
                grid->cells[row][col].happy=(double)same/neighbors;
            }

            total_happy+=grid->cells[row][col].happy;
        }
    }

    return total_happy/(grid->dim*grid->dim);
}

int move_agents(Grid *grid,int moves){
    typedef struct{
        int r;
        int c;
        Agent a;
    }TempAgent;

    int max=grid->dim*grid->dim;
    TempAgent *bad=malloc(max*sizeof(TempAgent));
    int bad_count=0;

    for(int i=0;i<grid->dim;i++){
        for(int j=0;j<grid->dim;j++){
            if(grid->cells[i][j].type==VACANT) continue;

            if((grid->cells[i][j].happy*100)<=strength){
                bad[bad_count].r=i;
                bad[bad_count].c=j;
                bad[bad_count].a=grid->cells[i][j];
                bad_count++;

                grid->cells[i][j].type=VACANT;
                grid->cells[i][j].happy=1;
            }
        }
    }

    int (*spots)[2]=malloc(max*sizeof(*spots));
    int spot_count=0;

    for(int i=0;i<grid->dim;i++){
        for(int j=0;j<grid->dim;j++){
            if(grid->cells[i][j].type==VACANT){
                spots[spot_count][0]=i;
                spots[spot_count][1]=j;
                spot_count++;
            }
        }
    }

    for(int i=spot_count-1;i>0;i--){
        int j=rand()%(i+1);
        int r=spots[i][0];
        int c=spots[i][1];
        spots[i][0]=spots[j][0];
        spots[i][1]=spots[j][1];
        spots[j][0]=r;
        spots[j][1]=c;
    }

    for(int i=0;i<bad_count;i++){
        int found=-1;

        for(int j=0;j<spot_count;j++){
            if(spots[j][0]!=bad[i].r||spots[j][1]!=bad[i].c){
                found=j;
                break;
            }
        }

        if(found==-1) continue;

        int nr=spots[found][0];
        int nc=spots[found][1];

        grid->cells[nr][nc]=bad[i].a;

        spots[found][0]=spots[spot_count-1][0];
        spots[found][1]=spots[spot_count-1][1];
        spot_count--;

        moves++;
    }

    free(bad);
    free(spots);
    return moves;
}

void infinite_mode(Grid *grid){
    int cycle=0;
    int moves=0;

    while(1){
        ave_happiness=calculate_happiness(grid);
        moves=0;

        if(cycle!=0){
            moves=move_agents(grid,moves);
        }

        clear();
        set_cur_pos(1,1);
        print_grid(grid,cycle,moves);
        usleep(delay);
        cycle++;
    }
}

void print_mode(Grid *grid,int max_cycles){
    int cycle=0;
    int moves=0;

    while(cycle<=max_cycles){
        ave_happiness=calculate_happiness(grid);
        moves=0;

        if(cycle!=0){
            moves=move_agents(grid,moves);
        }

        print_grid(grid,cycle,moves);
        usleep(delay);
        cycle++;
    }
}

int main(int argc,char *argv[]){
    parse_arguments(argc,argv);

    Grid *grid=init_grid(dim,endline,vacancy);

    if(cycle_count==-1){
        infinite_mode(grid);
    } else{
        print_mode(grid,cycle_count);
    }

    free_grid(grid);
    return 0;
