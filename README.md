NSEoS
=====

NSEoS (Neutron Star Equation of State) is a library that aims to provide the 
useful tools to calculate the composition, the equation of state, and 
observables of neutron stars according to different nuclear models. 
Please contact carreau@lpccaen.in2p3.fr if you have any questions.

Contents
--------

1. [Getting the code](#getting-the-code)
2. [Requirements](#requirements)
3. [Usage](#usage)
    1. [Apps](#apps)
    2. [Nuclear models](#nuclear-models)

### Getting the code

    git clone https://github.com/thomascarreau/NSEoS

### Requirements

* [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/)

### Usage

#### Apps

You can find three example apps in `NSEoS/source/apps/`:

* `nseos`, to calculate the composition, the equation of state, and 
    other observables of cold isolated neutron stars for a given nuclear model.

* `crust_crystallization`, to calculate the crust composition and the 
    equation of state at the melting temperature for a given nuclear model.

* `gs_ocrust`, to calculate the ground state of the outer crust for a given 
    nuclear mass table.

Feel free to write new apps using the functions of the library and to contact 
carreau@lpccaen.in2p3.fr if you wish to contribute.

##### nseos

In `NSEoS/source/apps/nseos`:

    make
    ./nseos set.in crust.out core.out eos.out tov.out

The first output file gives you the crust composition (number density, mass of 
the cluster, global asymmetry in the cluster, number of charges, cluster 
density, gas density, and radius of the cell). The second output file gives you 
the core composition (number density, fraction of protons, electrons, and 
muons). The third output file gives you the EoS (mass density and pressure). 
Finally, the last output file gives you the TOV solution (central density, 
central pressure, radius, mass, core radius, core mass, normalized moment of 
inertia, fraction of moment of inertia residing in the crust, tidal love 
number, and dimensionless tidal deformability).

##### crust_crystallization

In `NSEoS/source/apps/crust_crystallization`:

    make
    ./crust_crystallization set.in crust.out eos.out

The first output file gives you the crust composition at the melting 
temperature(number density, melting temperature, mass of the cluster, global 
asymmetry in the cluster, number of charges, cluster density, gas density, 
and radius of the cell). The second output file gives you the EoS at the 
melting temperature (mass density and pressure).

##### gs_ocrust

In `NSEoS/source/apps/gs_ocrust`:

    make
    ./gs_ocrust mass_table.data outfile

Nuclear mass tables can be found in `NSEoS/source/input/mass_tables`. The 
format of the tables is Z, N, mass excess. You can use 
the script `mergeTables.py` to merge two mass tables. For example, if you want 
to complete AME2012 data with HFB-24 theoretical masses then run:

    python3 mergeTables.py ame2012.data hfb24.data ame2012_plus_hfb24.data

Note that it requires [NumPy](https://numpy.org) library.

#### Nuclear models

##### Skyrme-type interactions

* BSk14: `bsk14.in`

* BSk16: `bsk16.in`

* BSk17: `bsk17.in`

* BSk22: `bsk22.in`

* BSk24: `bsk24.in`

* BSk25: `bsk25.in`

* BSk26: `bsk26.in`

* NRAPR: `nrapr.in`

* RATP: `ratp.in`

* SkO: `sko.in`

* SLy230a: `sly230a.in`

* SLy230b: `sly230b.in`

* SLy4: `sly4.in`

##### RMF models

* DDME2: `ddme2.in`

* DDMEd: `ddmed.in`

* NL3: `nl3.in`

* PKDD: `pkdd.in`

* TM1: `tm1.in`

* TW99: `tw99.in`

##### More

We use the [meta-modeling technique](https://arxiv.org/abs/1708.06894) to 
mimic existing nuclear models with very good accuracy. The input parameters are 
the successive density derivatives of nuclear matter energy (`lasat0`, 
`ksat0`, `qsat0`, `zsat0`, `jsym0`, `lsym0`, `ksym0`, `qsym0`, and `zsym0`) and 
effective masses (`effm` and `isosplit`) at saturation density (`rhosat0`).

If you wish to add new models in `NSEoS/source/input/satdata`, here is the 
format:

    rhosat0  lasat0  ksat0  qsat0  zsat0
    jsym0  lsym0  ksym0  qsym0  zsym0
    effm  isosplit
