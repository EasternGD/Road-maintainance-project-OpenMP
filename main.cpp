#include "inc/function.h"

int main(int argc, char *argv[])
{
    //cout << "stage 1 ---------------------" << endl;
    double tmp, tmp2 = omp_get_wtime();
    double t[8] = {0};
    srand(time(NULL));
    //srand(atoi(argv[9]));

    tmp = omp_get_wtime();
    PopulationType population(argv[1], 1); //1 mean for stage 1 ,2 mean for stage 2
    population.CROSSOVER_RATE = atof(argv[3]);
    population.MUTATION_RATE = atof(argv[4]);
    population.ALPHA = atof(argv[5]);
    population.BETA = atof(argv[6]);
    population.GAMMA = atof(argv[7]);
    population.budget = atoi(argv[8]);
    t[0] += omp_get_wtime() - tmp;
    //cout << "budget: " << population.budget / 1000000.0 << " M" << endl;

    for (size_t i = 0; i < ITERA_CNT; i++)
    {
        //cout << "(" << i + 1 << "/" << ITERA_CNT << ")" << endl;
        tmp = omp_get_wtime();
        population.reproduction();
        t[1] += omp_get_wtime() - tmp;
        tmp = omp_get_wtime();
        population.crossover();
        t[2] += omp_get_wtime() - tmp;
        tmp = omp_get_wtime();
        population.mutation();
        t[3] += omp_get_wtime() - tmp;
        tmp = omp_get_wtime();
        population.computeFitness(i);
        t[4] += omp_get_wtime() - tmp;
    }
    tmp = omp_get_wtime();
    population.printBestOne();
    t[5] += omp_get_wtime() - tmp;
    tmp = omp_get_wtime();
    saveJSON(population, argv[2]);
    t[6] += omp_get_wtime() - tmp;
    t[7] += omp_get_wtime() - tmp2;
    cout << "\nTiming: ";
    for (auto &i : t)
    {
        cout << i << ", ";
    }
    cout << endl;
    return (0);
}
