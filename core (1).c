#ifndef CORE
#define CORE
#include <stdlib.h>

enum ENTITIES {NONE, PLANKTON, FISH, SHARK};

typedef struct entity {
    char type;
    int age;
    int food;
    int rtoreproduce;
} ENTITY;

typedef struct pos {
    int i, j;
} POS;

const int di[] = {-1,-1,-1, 0, 0, 1, 1, 1};
const int dj[] = {-1, 0, 1,-1, 1,-1, 0, 1};

int height, width;
ENTITY** aquarium;
int step = 0;
int count_plankton = 0;
int count_fish = 0;
int count_shark = 0;

int max_age_plankton = 10;
int max_age_fish = 20;
int max_age_shark = 30;
int max_food_fish = 10;
int max_food_shark = 10;
int visibility_fish = 3;
int visibility_shark = 4;
int dist_move_fish = 3;
int dist_move_shark = 2;
int chance_duplication_plankton = 15;
int food_to_reproduce_fish = 1;
int food_to_reproduce_shark = 2;

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

int get_dist(POS a, POS b) {
    if (a.i < b.i)
        swap(&a.i, &b.i);
    if (a.j < b.j)
        swap(&a.j, &b.j);
    return max(min(a.i-b.i, b.i-a.i+height), min(a.j-b.j, b.j-a.j+width));
}

int get_dist1(POS a, int bi, int bj) {
    POS b = {bi,bj};
    return get_dist(a,b);
}

int get_dist2(int ai, int aj, int bi, int bj) {
    POS a = {ai,aj};
    POS b = {bi,bj};
    return get_dist(a,b);
}

void init_aquarium(ENTITY*** aquarium) {
    *aquarium = (ENTITY**)malloc(sizeof(ENTITY*)*height);
    for (int i = 0; i < height; ++i) {
        (*aquarium)[i] = (ENTITY*)malloc(sizeof(ENTITY)*width);
        for (int j = 0; j < width; ++j)
            (*aquarium)[i][j].type = 0;
    }
}

void free_aquarium(ENTITY*** aquarium) {
    for (int i = 0; i < height; ++i)
        free((*aquarium)[i]);
    free(*aquarium);
}

void create(int w, int h, int planktons, int fishes, int sharks) {
    width = w;
    height = h;
    count_plankton = planktons;
    count_fish = fishes;
    count_shark = sharks;
    init_aquarium(&aquarium);
    while (planktons--) {
        int i, j;
        do {
            i = rand()%height;
            j = rand()%width;
        } while (aquarium[i][j].type);
        aquarium[i][j].type = PLANKTON;
        aquarium[i][j].age = 0;
    }
    while (fishes--) {
        int i, j;
        do {
            i = rand()%height;
            j = rand()%width;
        } while (aquarium[i][j].type);
        aquarium[i][j].type = FISH;
        aquarium[i][j].age = 0;
        aquarium[i][j].food = max_food_fish;
        aquarium[i][j].rtoreproduce=0;
    }
    while (sharks--) {
        int i, j;
        do {
            i = rand()%height;
            j = rand()%width;
        } while (aquarium[i][j].type);
        aquarium[i][j].type = SHARK;
        aquarium[i][j].age = 0;
        aquarium[i][j].food = max_food_shark;
        aquarium[i][j].rtoreproduce=0;
    }
}

void update() {
    step++;
    ENTITY** temp;
    init_aquarium(&temp);
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            if (aquarium[i][j].type != SHARK)
                continue;
            if (aquarium[i][j].age == max_age_shark || !aquarium[i][j].food) {
                count_shark--;
                continue;
            }
            //a
            int moved=0;
            if (aquarium[i][j].rtoreproduce>=food_to_reproduce_shark) {
                POS new_pos = {i, j};
                POS nearest_shark = {-1, -1};
                int dist_nearest_shark = -1;
                for (int i2 = (i - visibility_shark + height) % height;
                     i2 != (i + visibility_shark + 1) % height; i2 = (i2 + 1) % height)
                    for (int j2 = (j - visibility_shark + width) % width;
                         j2 != (j + visibility_shark + 1) % width; j2 = (j2 + 1) % width)
                        if (aquarium[i2][j2].type == SHARK && (i2 != i || j2 != j)) {
                            int dist = get_dist2(i, j, i2, j2);
                            if (dist_nearest_shark == -1 || dist < dist_nearest_shark) {
                                nearest_shark.i = i2;
                                nearest_shark.j = j2;
                                dist_nearest_shark = dist;
                            }
                        }
                int dist = -1;
                if (dist_nearest_shark != -1)
                    for (int i2 = (i - dist_move_shark + height) % height;
                         i2 != (i + dist_move_shark + 1) % height; i2 = (i2 + 1) % height)
                        for (int j2 = (j - dist_move_shark + width) % width;
                             j2 != (j + dist_move_shark + 1) % width; j2 = (j2 + 1) % width) {
                            dist = get_dist1(nearest_shark, i2, j2);
                            if (temp[i2][j2].type == NONE &&
                                (aquarium[i2][j2].type == NONE || aquarium[i2][j2].type == SHARK) &&
                                dist < get_dist(new_pos, nearest_shark) && dist != 0) {
                                new_pos.i = i2;
                                new_pos.j = j2;
                            }
                        }
                if (new_pos.i != i || new_pos.j != j) {

                    moved = 1;
                }
                else{
                    moved=-1;
                }

                temp[new_pos.i][new_pos.j].type = SHARK;

                temp[new_pos.i][new_pos.j].age = aquarium[i][j].age + 1;
                dist = get_dist(new_pos, nearest_shark);

                if (dist == 1 && nearest_shark.i != -1 && nearest_shark.j != -1) {
                    for (int k = 0; k < 8; k++) {
                        int i2 = (new_pos.i + di[k] + height) % height;
                        int j2 = (new_pos.j + dj[k] + width) % width;
                        if (temp[i2][j2].type == NONE && aquarium[i2][j2].type == NONE) {
                            count_shark++;
                            temp[i2][j2].type = SHARK;
                            temp[i2][j2].food = max_food_shark;
                            temp[i2][j2].age = 0;
                            temp[i2][j2].rtoreproduce = 0;
                            break;
                        }
                    }
                    temp[new_pos.i][new_pos.j].rtoreproduce = 0;
                    temp[nearest_shark.i][nearest_shark.j].rtoreproduce = 0;
                }
                temp[new_pos.i][new_pos.j].food = aquarium[i][j].food - 1;

            }
            else{
                if (moved==-1){
                    temp[i][j].food++;
                    temp[i][j].age--;
                    moved=0;
                }
                POS new_pos = {i,j};
                POS nearest_fish = {-1,-1};
                int dist_nearest_fish = -1;
                for (int i2 = (i-visibility_shark+height)%height; i2 != (i+visibility_shark+1)%height; i2 = (i2+1)%height)
                    for (int j2 = (j-visibility_shark+width)%width; j2 != (j+visibility_shark+1)%width; j2 = (j2+1)%width)
                        if (aquarium[i2][j2].type == FISH) {
                            int dist = get_dist2(i, j, i2, j2);
                            if (dist_nearest_fish == -1 || dist < dist_nearest_fish) {
                                nearest_fish.i = i2;
                                nearest_fish.j = j2;
                                dist_nearest_fish = dist;
                            }
                        }
                if (dist_nearest_fish != -1)
                    for (int i2 = (i-dist_move_shark+height)%height; i2 != (i+dist_move_shark+1)%height; i2 = (i2+1)%height)
                        for (int j2 = (j-dist_move_shark+width)%width; j2 != (j+dist_move_shark+1)%width; j2 = (j2+1)%width) {
                            int dist = get_dist1(nearest_fish, i2, j2);
                            if (temp[i2][j2].type == NONE && (aquarium[i2][j2].type == NONE || aquarium[i2][j2].type == FISH) && dist < get_dist(new_pos, nearest_fish)) {
                                new_pos.i = i2;
                                new_pos.j = j2;
                            }
                        }
                if (new_pos.i!=i || new_pos.j!=j){
                    moved=1;
                }
                else{
                    moved=-1;
                }
                temp[new_pos.i][new_pos.j].type = SHARK;
                temp[new_pos.i][new_pos.j].age = aquarium[i][j].age + 1;
                temp[new_pos.i][new_pos.j].rtoreproduce=aquarium[i][j].rtoreproduce;
                if (aquarium[new_pos.i][new_pos.j].type == FISH) {
                    count_fish--;
                    aquarium[new_pos.i][new_pos.j].type = NONE;
                    temp[new_pos.i][new_pos.j].food = max_food_shark;
                    temp[new_pos.i][new_pos.j].rtoreproduce++;

                }
                else{
                    temp[new_pos.i][new_pos.j].food = aquarium[i][j].food - 1;
                }


            }
            if (moved==-1){
                temp[i][j].food++;
                temp[i][j].age--;
                moved=0;
            }

            if (moved==0){
                for (int k=0;k<dist_move_shark*dist_move_shark;k++){
                    int ri=(rand()%dist_move_shark*2) -dist_move_shark;
                    int rj=(rand()%dist_move_shark*2) -dist_move_shark;
                    int i2 = (i+ri+height)%height;
                    int j2 = (j+rj+width)%width;
                    if (temp[i2][j2].type == NONE && aquarium[i2][j2].type == NONE){
                        //еду и возраст уже поменяли

                        temp[i2][j2].type=SHARK;
                        temp[i2][j2].food=temp[i][j].food-1;
                        temp[i2][j2].age=temp[i][j].age+1;
                        temp[i2][j2].rtoreproduce=temp[i][j].rtoreproduce;
                        temp[i][j].type=NONE;
                        temp[i][j].food=0;
                        temp[i][j].age=0;
                        temp[i][j].rtoreproduce=0;
                        break;
                    }
                }
            }
//                printf("begin\n");
//                for (int i = 0; i < height; ++i) {
//                    for (int j = 0; j < width; ++j)
//                        printf("%d", temp[i][j].type);
//                    printf("\n");
//                }
//                printf("all\n");
        }
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            if (aquarium[i][j].type != FISH){
                continue;
            }

            if (aquarium[i][j].age == max_age_fish || !aquarium[i][j].food){
                count_fish--;
                continue;
            }

            int moved=0;
            //a
            if (aquarium[i][j].rtoreproduce>=food_to_reproduce_fish){ //a

                POS new_pos = {i,j};
                POS nearest_fish = {-1,-1};
                int dist_nearest_fish = -1;
                for (int i2 = (i-visibility_fish+height)%height; i2 != (i+visibility_fish+1)%height; i2 = (i2+1)%height)
                    for (int j2 = (j-visibility_fish+width)%width; j2 != (j+visibility_fish+1)%width; j2 = (j2+1)%width)
                        if (aquarium[i2][j2].type == FISH && (i2!=i || j2!=j)) {
                            int dist = get_dist2(i, j, i2, j2);
                            if (dist_nearest_fish == -1 || dist < dist_nearest_fish) {
                                nearest_fish.i = i2;
                                nearest_fish.j = j2;
                                dist_nearest_fish = dist;
                            }
                        }
                int dist=-1;

                if (dist_nearest_fish != -1)
                    for (int i2 = (i-dist_move_fish+height)%height; i2 != (i+dist_move_fish+1)%height; i2 = (i2+1)%height)
                        for (int j2 = (j-dist_move_shark+width)%width; j2 != (j+dist_move_fish+1)%width; j2 = (j2+1)%width) {
                            dist = get_dist1(nearest_fish, i2, j2);
                            //проверить второе условие, я чет туплю
                            //и зациклить условие
                            if (temp[i2][j2].type == NONE && (aquarium[i2][j2].type == NONE || aquarium[i2][j2].type == FISH) && dist < get_dist(new_pos, nearest_fish) && dist!=0) {
                                new_pos.i = i2;
                                new_pos.j = j2;
                            }
                        }
                if (new_pos.i!=i || new_pos.j!=j){
                    moved=1;
                }
                else{
                    moved=-1;
                }
                temp[new_pos.i][new_pos.j].type = FISH;
                temp[new_pos.i][new_pos.j].age = aquarium[i][j].age + 1;
                dist=get_dist(new_pos, nearest_fish);

                if (dist==1 && nearest_fish.i!=-1 && nearest_fish.j!=-1) {
                    for (int k=0;k<8;k++){
                        int i2 = (new_pos.i+di[k]+height)%height;
                        int j2 = (new_pos.j+dj[k]+width)%width;
                        if (temp[i2][j2].type == NONE && aquarium[i2][j2].type == NONE){
                            count_fish++;
                            temp[i2][j2].type=FISH;
                            temp[i2][j2].food=max_food_fish;
                            temp[i2][j2].age=0;
                            temp[i2][j2].rtoreproduce=0;
                            break;
                        }
                    }
                    temp[new_pos.i][new_pos.j].rtoreproduce=0;
                    temp[nearest_fish.i][nearest_fish.j].rtoreproduce=0;
                    aquarium[nearest_fish.i][nearest_fish.j].rtoreproduce=0;
                }
                temp[new_pos.i][new_pos.j].food = aquarium[i][j].food - 1;
            }
            else{
                if (moved==-1){
                    temp[i][j].food++;
                    temp[i][j].age--;
                    moved=0;
                }
                POS new_pos = {i,j};
                POS nearest_plankton = {-1,-1};
                int dist_nearest_plankton = -1;
                POS nearest_shark = {-1,-1};
                int dist_nearest_shark = -1;
                for (int i2 = (i-visibility_fish+height)%height; i2 != (i+visibility_fish+1)%height; i2 = (i2+1)%height)
                    for (int j2 = (j-visibility_fish+width)%width; j2 != (j+visibility_fish+1)%width; j2 = (j2+1)%width) {
                        int dist = get_dist2(i, j, i2, j2);
                        switch (aquarium[i2][j2].type) {
                            case PLANKTON:
                                if (dist_nearest_shark != -1)
                                    break;
                                if (dist_nearest_plankton == -1 || dist < dist_nearest_plankton) {
                                    nearest_plankton.i = i2;
                                    nearest_plankton.j = j2;
                                    dist_nearest_plankton = dist;
                                }
                                break;
                            case SHARK:
                                if (dist_nearest_shark == -1 || dist < dist_nearest_shark) {
                                    nearest_shark.i = i2;
                                    nearest_shark.j = j2;
                                    dist_nearest_shark = dist;
                                }
                                break;
                        }
                    }
                if (dist_nearest_shark != -1)
                    for (int i2 = (i-dist_move_fish+height)%height; i2 != (i+dist_move_fish+1)%height; i2 = (i2+1)%height)
                        for (int j2 = (j-dist_move_fish+width)%width; j2 != (j+dist_move_fish+1)%width; j2 = (j2+1)%width) {
                            int dist = get_dist1(nearest_shark, i2, j2);
                            if (temp[i2][j2].type == NONE && (aquarium[i2][j2].type == NONE || aquarium[i2][j2].type == PLANKTON) && dist > get_dist(new_pos, nearest_shark)) {
                                new_pos.i = i2;
                                new_pos.j = j2;
                            }
                        }
                else if (dist_nearest_plankton != -1)
                    for (int i2 = (i-dist_move_fish+height)%height; i2 != (i+dist_move_fish+1)%height; i2 = (i2+1)%height)
                        for (int j2 = (j-dist_move_fish+width)%width; j2 != (j+dist_move_fish+1)%width; j2 = (j2+1)%width) {
                            int dist = get_dist1(nearest_plankton, i2, j2);
                            if (temp[i2][j2].type == NONE && (aquarium[i2][j2].type == NONE || aquarium[i2][j2].type == PLANKTON) && dist < get_dist(new_pos, nearest_plankton)) {
                                new_pos.i = i2;
                                new_pos.j = j2;
                            }
                        }

                if (new_pos.i!=i || new_pos.j!=j){
                    moved=1;
                }
                else{
                    moved=-1;
                }
                temp[new_pos.i][new_pos.j].type = FISH;
                temp[new_pos.i][new_pos.j].age = aquarium[i][j].age + 1;
                temp[new_pos.i][new_pos.j].rtoreproduce=aquarium[i][j].rtoreproduce;
                if (aquarium[new_pos.i][new_pos.j].type == PLANKTON) {
                    count_plankton--;
                    aquarium[new_pos.i][new_pos.j].type = NONE;
                    temp[new_pos.i][new_pos.j].food = max_food_fish;
                    temp[new_pos.i][new_pos.j].rtoreproduce++;
                }
                else{
                    temp[new_pos.i][new_pos.j].food = aquarium[i][j].food - 1;
                }


            }
            if (moved==-1){
                temp[i][j].food++;
                temp[i][j].age--;
                moved=0;
            }
            if (moved==0){
                for (int k=0;k<dist_move_fish*dist_move_fish;k++){
                    int ri=(rand()%dist_move_fish*2) -dist_move_fish;
                    int rj=(rand()%dist_move_fish*2) -dist_move_fish;
                    int i2 = (i+ri+height)%height;
                    int j2 = (j+rj+width)%width;
                    if (temp[i2][j2].type == NONE && (ri!=i || rj!=j) && aquarium[i2][j2].type == NONE){
                        //еду и возраст уже поменяли
                        temp[i2][j2].type=FISH;
                        temp[i2][j2].food=temp[i][j].food-1;
                        temp[i2][j2].age=temp[i][j].age+1;
                        temp[i2][j2].rtoreproduce=temp[i][j].rtoreproduce;
                        temp[i][j].type=NONE;
                        temp[i][j].food=0;
                        temp[i][j].age=0;
                        temp[i][j].rtoreproduce=0;
                        break;
                    }
                }
            }
        }
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            //printf("plank %d %d\n",i,j);
            if (aquarium[i][j].type != PLANKTON)
                continue;
            if (aquarium[i][j].age == max_age_plankton) {
                count_plankton--;
                continue;
            }
            char can_move = 0;
            for (int k = 0; k < 8; ++k) {
                int i2 = (i + di[k] + height)%height;
                int j2 = (j + dj[k] + width)%width;
                if (aquarium[i2][j2].type == NONE && temp[i2][j2].type == NONE) {
                    can_move = 1;
                    break;
                }
            }
            POS new_pos = {i,j};
            if (can_move && rand()%2){
                do {
                    int k = rand()%8;
                    new_pos.i = (i + di[k] + height)%height;
                    new_pos.j = (j + dj[k] + width)%width;
                } while (temp[new_pos.i][new_pos.j].type != NONE || aquarium[new_pos.i][new_pos.j].type != NONE);
            }
            temp[new_pos.i][new_pos.j].type = PLANKTON;
            temp[new_pos.i][new_pos.j].age = aquarium[i][j].age + 1;
            if (rand()%100<chance_duplication_plankton){
                for (int k=0;k<8;k++){
                    int i2 = (new_pos.i+di[k]+height)%height;
                    int j2 = (new_pos.j+dj[k]+width)%width;
                    if (temp[i2][j2].type == NONE && aquarium[i2][j2].type == NONE){
                        count_plankton++;
                        temp[i2][j2].type=PLANKTON;
                        temp[i2][j2].age=0;
                        break;
                    }
                }
            }
        }
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            aquarium[i][j] = temp[i][j];
    free_aquarium(&temp);
}

void exit_() {
    free_aquarium(&aquarium);
}

#endif // CORE
