/////////////////////////////////////////////////////////////////////////////////////////////////
// manel

#include <stdio.h>

int min_num;
int max_num;
int resultado;


int main(void)
{
    //printf("Min : 1 Max : 30 %d\n", random_number(0,5));
    //printf("Min : 100 Max : 1000 %d\n",random_number(100,1000));
    //return 0;
    int result = 0, low_num = 0, hi_num = 0;
    min_num=15;
	max_num=25;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    resultado = (rand() % (hi_num - low_num)) + low_num;
    printf("Min=15 Max=25 TEMP=%d\n",resultado);
    return resultado;
}
