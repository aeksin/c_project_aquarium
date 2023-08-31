#include <stdio.h>
#include <conio.h>
#include "core.c"

void print_aquarium() {
    printf("Step: %d | Plankton: %d | Fish: %d | Shark: %d\n", step, count_plankton, count_fish, count_shark);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j){
            switch (aquarium[i][j].type) {
                case 0:
                    printf(" ");
                    break;
                case 1:
                    printf(".");
                    break;
                case 2:
                    printf("-");
                    break;
                case 3:
                    printf("=");
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    /*
    100 50 300 300 150
    */
    int w, h, planktons, fishes, sharks;
    scanf("%d%d%d%d%d", &w, &h, &planktons, &fishes, &sharks);
    initialize_default();
    create(w, h, planktons, fishes, sharks);
    print_aquarium();
    getch();
    while (1) {
        update();
        print_aquarium();
        getch();
    }
    exit_();
    return 0;
}
