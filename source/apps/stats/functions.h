#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#define N (325) // using wc -l
#define N_PARAMS (18) // number of parameters in the matrix

#include <stdio.h>

#include "../../nseos/empirical.h"

int read_table_of_sets(FILE *, struct parameters *satdata, float *m, float *dm);

struct transition_qtt
{
    double nt;
    double pt;
};
void eval_transition_qtt(struct parameters, double p,
        struct transition_qtt *tqtt);

void calc_weights_for_masses_filter(float chi2[N], float w[N]);

struct stats
{
    float average[N_PARAMS];
    float variance[N_PARAMS];
    float deviation[N_PARAMS];
    float correlation[N_PARAMS][N_PARAMS];
};
struct stats calc_stats(float data[N_PARAMS][N], float w[N]);

#endif // H_FUNCTIONS
