#include <math.h>
#include <gsl/gsl_rng.h>

#include "bayes.h"
#include "tov.h"
#include "eos.h"
#include "nuclear_surface_en.h"
#include "nuclear_matter.h"
#include "modeling.h"

void get_low_density_posterior(FILE *prior, FILE *posterior)
{
    struct parameters satdata;
    float m, dm;

    double nn[10] = {0.02, 0.04, 0.06, 0.08, 0.10, 
        0.12, 0.14, 0.16, 0.18, 0.20}; 
    double e_sm_min[10] = {-4.4921, -7.6542, -10.1682, -12.1990, -13.8226, 
        -15.0815, -16.0017, -16.6000, -16.8879, -16.8733};
    double e_sm_max[10] = {-3.8713, -6.9232, -9.2607, -11.0292, -12.3161, 
        -13.0155, -13.2269, -13.0000, -12.3471, -11.2774};
    double p_sm_min[10] = {-0.0720, -0.2238, -0.4060, -0.5818, -0.7180, 
        -0.7820, -0.7430, -0.5811, -0.2508, 0.2839};
    double p_sm_max[10] = {-0.0688, -0.2067, -0.3503, -0.4479, -0.4512, 
        -0.3138, 0.0099, 0.5654, 1.3974, 2.5511};
    double e_nm_min[10] = {4.2123, 6.0424, 7.4706, 8.8545, 10.3097, 
        11.8712, 13.5397, 15.2000, 16.8821, 18.5931};
    double e_nm_max[10] = {4.3001, 6.2685, 7.9385, 9.6124, 11.3935, 
        13.3080, 15.3503, 17.5000, 19.7299, 22.0096};
    double p_nm_min[10] = {0.0463, 0.1225, 0.2476, 0.4510, 0.7375, 
        1.1104, 1.5754, 2.1269, 2.7534, 3.4383};
    double p_nm_max[10] = {0.0489, 0.1395, 0.2959, 0.5501, 0.9235, 
        1.4262, 2.0581, 2.8092, 3.6612, 4.5881};
    struct hnm test_hnm_sm;
    struct hnm test_hnm_nm;
    int ld_check;

    while(read_table_of_sets(prior, &satdata, &m, &dm) == 0)
    {
        ld_check = 0;

        for(int i = 3; i < 10; i++)
        {
            test_hnm_sm = calc_meta_model_nuclear_matter(
                    satdata, TAYLOR_EXP_ORDER, nn[i], 0., 0.);
            test_hnm_nm = calc_meta_model_nuclear_matter(
                    satdata, TAYLOR_EXP_ORDER, nn[i], 1., 0.);

            if(test_hnm_sm.enpernuc < e_sm_min[i] 
                    || test_hnm_sm.enpernuc > e_sm_max[i]
                    || test_hnm_sm.p < p_sm_min[i] 
                    || test_hnm_sm.p > p_sm_max[i]
                    || test_hnm_nm.enpernuc < e_nm_min[i] 
                    || test_hnm_nm.enpernuc > e_nm_max[i]
                    || test_hnm_nm.p < p_nm_min[i] 
                    || test_hnm_nm.p > p_nm_max[i])
            {
                ld_check = 1;
                break;
            } 
        }

        if(ld_check == 0)
            fprintf(posterior, "%g %g %g %g %g %g %g %g %g %g %g %g %g\n",
                    satdata.rhosat0, satdata.lasat0, satdata.ksat0, 
                    satdata.qsat0, satdata.zsat0, 
                    satdata.jsym0, satdata.lsym0, satdata.ksym0, 
                    satdata.qsym0, satdata.zsym0, 
                    m, dm, satdata.b);
    }
}

void get_high_density_posterior(FILE *prior, 
        FILE *posterior, FILE *observables)
{
    struct parameters satdata;
    float m, dm;

    double qsat_ld, zsat_ld;
    double qsym_ld, zsym_ld;
    double qsat_hd, zsat_hd;
    double qsym_hd, zsym_hd;
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T); 

    while(read_table_of_sets(prior, &satdata, &m, &dm) == 0)
    {
        qsat_ld = satdata.qsat0;
        zsat_ld = satdata.zsat0;
        qsym_ld = satdata.qsym0;
        zsym_ld = satdata.zsym0;

        for(int j = 0; j < 10; j++)
        {
            satdata.qsat0 = 2000.*gsl_rng_uniform (r) - 1000.;
            satdata.zsat0 = 6000.*gsl_rng_uniform (r) - 3000.;
            satdata.qsym0 = 4000.*gsl_rng_uniform (r) - 2000.;
            satdata.zsym0 = 10000.*gsl_rng_uniform (r) - 5000.;

            // CAUSALITY, STABILITY, SYM EN ===================================
            struct transition_qtt tqtt_hd;
            double epst_hd;
            int hd_checker = 0;
            FILE *mycrust = fopen("crust.out", "w+");
            FILE *mycore = fopen("core.out", "w+");
            FILE *myeos = fopen("eos.out", "w+");
            int lines = calc_zero_temperature_equation_of_state(satdata, 3., 
                    &tqtt_hd, &epst_hd, &hd_checker, 
                    mycrust, mycore, myeos);
            fclose(mycrust);
            fclose(mycore);
            fclose(myeos);

            // TEST MAX MASS, OBSERVABLES =====================================
            if (hd_checker == 0)
            {
                qsat_hd = satdata.qsat0;
                zsat_hd = satdata.zsat0;
                qsym_hd = satdata.qsym0;
                zsym_hd = satdata.zsym0;

                satdata.qsat0 = qsat_ld;
                satdata.zsat0 = zsat_ld;
                satdata.qsym0 = qsym_ld;
                satdata.zsym0 = zsym_ld;
                struct transition_qtt tqtt_ld;
                tqtt_ld.nt = 0.0005;
                double epst_ld;

                eval_transition_qtt(satdata, 3., &tqtt_ld, &epst_ld);

                if (tqtt_ld.nt > 0.0005)
                {
                    myeos = fopen("eos.out", "r");
                    struct tov_solution tovs14;
                    FILE *mytov = fopen("tov.out", "w+");
                    double mmax  = solve_tov_equation(
                            lines, tqtt_ld.pt, epst_ld, myeos, 
                            &tovs14, 1.4, mytov);
                    if (mmax < 2.0)
                    {
                        hd_checker = 1;
                        fprintf(stderr, "Mmax = %g < 2 Msun\n", mmax);
                    }
                    else
                    {
                        fprintf(posterior, 
                                "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
                                satdata.rhosat0, satdata.lasat0, satdata.ksat0, 
                                qsat_ld, qsat_hd, zsat_ld, zsat_hd, 
                                satdata.jsym0, satdata.lsym0, satdata.ksym0, 
                                qsym_ld, qsym_hd, zsym_ld, zsym_hd, 
                                m, dm, satdata.b);
                        fprintf(observables, 
                                "%g %g %g %g %g %g %g %g %g %g %g\n", 
                                mmax, tovs14.rhoc, tovs14.pc, tovs14.r, 
                                tqtt_ld.nt, tqtt_ld.pt, 
                                tovs14.rcore, tovs14.mcore,
                                tovs14.i_over_mr2, 
                                tovs14.icrust_over_mr2/tovs14.i_over_mr2,
                                tovs14.lambda_dimless);
                    }
                    fclose(myeos);
                    fclose(mytov);
                }
            }
        }
    }

    gsl_rng_free (r);
}

void calc_observables(FILE *posterior, int p_switch, 
        FILE *observables, FILE *new_posterior)
{
    struct parameters satdata;
    float m, dm;

    FILE *ftov = NULL;
    double mmax;

    if(p_switch == 0) // then p=3
    {
        while(read_table_of_sets(posterior, &satdata, &m, &dm) == 0)
        {
            struct transition_qtt tqtt;
            double epst;

            int hd_checker = 0;
            FILE *fcrust = fopen("crust.out", "w+");
            FILE *fcore = fopen("core.out", "w+");
            FILE *feos = fopen("eos.out", "w+");
            int lines = calc_zero_temperature_equation_of_state(satdata, 3., 
                    &tqtt, &epst, &hd_checker,
                    fcrust, fcore, feos);
            fclose(fcrust);
            fclose(fcore);
            fclose(feos);

            feos = fopen("eos.out", "r");
            struct tov_solution tovs14;
            ftov = fopen("tov.out", "w+");
            mmax = solve_tov_equation(lines, tqtt.pt, epst, feos, 
                    &tovs14, 1.4, ftov);
            fclose(feos);
            fclose(ftov);

            if(mmax > 1.4 && tqtt.nt > 0.0005 && tqtt.pt > 0.)
            {
                fprintf(observables, "%g %g %g %g %g %g %g %g %g %g %g\n", 
                        mmax, tovs14.rhoc, tovs14.pc, tovs14.r, 
                        tqtt.nt, tqtt.pt, tovs14.rcore, tovs14.mcore,
                        tovs14.i_over_mr2, 
                        tovs14.icrust_over_mr2/tovs14.i_over_mr2,
                        tovs14.lambda_dimless);
                struct sf_params prms;
                prms = fit_sf_params(satdata, 3.);
                fprintf(new_posterior, 
                        "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
                        satdata.rhosat0, satdata.lasat0, satdata.ksat0, 
                        satdata.qsat0, satdata.zsat0, 
                        satdata.jsym0, satdata.lsym0, satdata.ksym0, 
                        satdata.qsym0, satdata.zsym0,
                        m, dm, satdata.b, 
                        prms.p, prms.sigma0, prms.b, prms.chi2);
            }
        }
    }
    else if(p_switch == 1) // then p={2.5, 3.0, 3.5}
    {
        double p[3] = {2.5, 3., 3.5};

        while(read_table_of_sets(posterior, &satdata, &m, &dm) == 0)
        {
            for(int i = 0; i < 3; i++)
            {
                struct transition_qtt tqtt;
                double epst;

                int hd_checker = 0;
                FILE *fcrust = fopen("crust.out", "w+");
                FILE *fcore = fopen("core.out", "w+");
                FILE *feos = fopen("eos.out", "w+");
                int lines = calc_zero_temperature_equation_of_state(
                        satdata, p[i], 
                        &tqtt, &epst, &hd_checker,
                        fcrust, fcore, feos);
                fclose(fcrust);
                fclose(fcore);
                fclose(feos);

                feos = fopen("eos.out", "r");
                struct tov_solution tovs14;
                ftov = fopen("tov.out", "w+");
                mmax = solve_tov_equation(lines, tqtt.pt, epst, feos, 
                        &tovs14, 1.4, ftov);
                fclose(feos);
                fclose(ftov);

                if(mmax > 1.4 && tqtt.nt > 0.0005 && tqtt.pt > 0.)
                {
                    fprintf(observables, "%g %g %g %g %g %g %g %g %g %g %g\n", 
                            mmax, tovs14.rhoc, tovs14.pc, tovs14.r, 
                            tqtt.nt, tqtt.pt, tovs14.rcore, tovs14.mcore,
                            tovs14.i_over_mr2, 
                            tovs14.icrust_over_mr2/tovs14.i_over_mr2,
                            tovs14.lambda_dimless);
                    struct sf_params prms;
                    prms = fit_sf_params(satdata, p[i]);
                    fprintf(new_posterior, 
                            "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
                            satdata.rhosat0, satdata.lasat0, satdata.ksat0, 
                            satdata.qsat0, satdata.zsat0, 
                            satdata.jsym0, satdata.lsym0, satdata.ksym0, 
                            satdata.qsym0, satdata.zsym0,
                            m, dm, satdata.b, 
                            prms.p, prms.sigma0, prms.b, prms.chi2);
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "ERROR: p_switch should be either 0 or 1");
        return;
    }
}