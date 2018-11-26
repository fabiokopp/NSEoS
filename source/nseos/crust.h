#ifndef H_ICRUST
#define H_ICRUST

#include <gsl/gsl_vector.h>

#include "empirical.h"
#include "nuclear_surface_en.h"

struct crust_fun_4d
{
    double f_stability;
    double f_beta;
    double f_muneq;
    double f_presseq;
};

struct compo
{
    double aa;
    double del;
    double n0;
    double ng;
}; 

// zero point quantum vibration
double calc_zp_en(struct parameters satdata, struct sf_params sparams, 
        double aa_, double ii_, double n0_, double np_);
double calc_ion_en(struct parameters satdata, struct sf_params sparams,
        double aa_, double del_, double n0_, double np_);
struct crust_fun_4d calc_crust_fun_4d(struct parameters satdata, 
        struct sf_params sparams, 
        double aa_, double del_, double n0_, double np_, double ng_);

struct rparams_crust
{
    double np;

    struct parameters satdata;
    struct sf_params sparams;
};

int assign_ocrust_fun_3d(const gsl_vector * x, void *params, gsl_vector * f);
struct compo calc_ocrust3d_composition(double nb_, double *guess,
        struct parameters satdata, struct sf_params sparams);

int assign_icrust_fun_4d(const gsl_vector * x, void *params, gsl_vector * f);
struct compo calc_icrust4d_composition(double nb_, double *guess,
        struct parameters satdata, struct sf_params sparams);

double calc_muncl(struct parameters satdata, struct sf_params sparams, 
        struct compo eq, double nb_);
double calc_crust_ws_cell_energy_density(struct parameters satdata, 
        struct sf_params sparams, 
        struct compo eq, double nb_);
double calc_ngas_pressure(struct parameters satdata, double ng_);
double calc_ion_pressure(struct parameters satdata, struct sf_params sparams, 
        double aa_, double del_, double n0_, double np_);
double calc_crust_ws_cell_pressure(struct parameters satdata, 
        struct sf_params sparams, 
        struct compo eq, double nb_);
void print_state_crust(struct parameters satdata, struct sf_params sparams, 
        struct compo eq, double nb_, 
        FILE *compo, FILE *eos);

#endif // H_ICRUST
