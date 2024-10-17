#include <iostream>
#include <omp.h>

int sudoku[9][9];
int main(){
    #pragma omp parallel for num_threads(4) schedule(static)
    for (int box = 0; box < 9; ++box) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                sudoku[3 * (box / 3) + row][3 * (box % 3) + col] = omp_get_thread_num();
            }
        }
    }

    // Imprimir el sudoku con el formato solicitado
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            printf("%d ", sudoku[row][col]);
        }
        printf("\n");  
    }

    printf("\n");
    return 0;
}