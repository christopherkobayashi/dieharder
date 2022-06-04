/*
 *========================================================================
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include <dieharder/dieharder.h>

 /* #define GSL_VAR */
 /* List new rng types to be added. */
GSL_VAR const gsl_rng_type *gsl_rng_stdin_input_raw;   /* rgb Aug 2008 */
GSL_VAR const gsl_rng_type *gsl_rng_file_input_raw;
GSL_VAR const gsl_rng_type *gsl_rng_file_input;

GSL_VAR const gsl_rng_type *gsl_rng_dev_random;
GSL_VAR const gsl_rng_type *gsl_rng_dev_arandom;
GSL_VAR const gsl_rng_type *gsl_rng_dev_urandom;

GSL_VAR const gsl_rng_type *gsl_rng_r_wichmann_hill;	/* edd May 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_r_marsaglia_mc;	/* edd May 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_r_super_duper;	/* edd May 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_r_mersenne_twister;/* edd May 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_r_knuth_taocp;	/* edd May 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_r_knuth_taocp2;	/* edd May 2007 */

GSL_VAR const gsl_rng_type *gsl_rng_ca;
GSL_VAR const gsl_rng_type *gsl_rng_uvag;	        /* rgb Jun 2007 */
GSL_VAR const gsl_rng_type *gsl_rng_aes;	        /* bauer Oct 2009 */
GSL_VAR const gsl_rng_type *gsl_rng_threefish;	        /* bauer Oct 2009 */
GSL_VAR const gsl_rng_type *gsl_rng_kiss;	        /* rgb Jan 2011 */
GSL_VAR const gsl_rng_type *gsl_rng_superkiss;	        /* rgb Jan 2011 */
GSL_VAR const gsl_rng_type *gsl_rng_XOR;	        /* rgb Jan 2011 */

 /*
  * rng global vectors and variables for setup and tests.
  */
#define MAXRNGS 1000

const gsl_rng_type *dh_rng_types[MAXRNGS];
const gsl_rng_type **gsl_types;    /* where all the rng types go */

#define ADD(t) {if (i==MAXRNGS) abort(); dh_rng_types[i] = (t); i++; };

 /*
  * Global shared counters for the new types of rngs in the organization
  * defined above.
  */
unsigned int dh_num_rngs;           /* dh rngs available in dieharder */
unsigned int dh_num_gsl_rngs;       /* GSL rngs available in dieharder */
unsigned int dh_num_dieharder_rngs; /* dh rngs available in libdieharder */
unsigned int dh_num_R_rngs;         /* R-derived rngs available in libdieharder */
unsigned int dh_num_hardware_rngs;  /* hardware rngs supported in libdieharder */
unsigned int dh_num_user_rngs;      /* user-added rngs */
unsigned int dh_num_reserved_rngs;  /* ngs added in reserved space by new UI */

gsl_rng *rng;                  /* global gsl random number generator */

/* To enable large file support */
#define _FILE_OFFSET_BITS 64

int binary_rank(unsigned int **mtx,int mrows,int ncols);
    
unsigned int all;              /* Flag to do all tests on selected generator */
unsigned int binary;           /* Flag to output rands in binary (with -o -f) */
unsigned int bits;             /* bitstring size (in bits) */
unsigned int diehard;          /* Diehard test number */
unsigned int generator;        /* GSL generator id number to be tested */

#define GVECMAX 100
char gnames[GVECMAX][128];  /* VECTOR of names to be XOR'd into a "super" generator */
unsigned int gseeds[GVECMAX];       /* VECTOR of unsigned int seeds used for the "super" generators */
unsigned int gnumbs[GVECMAX];       /* VECTOR of GSL generators to be XOR'd into a "super" generator */
unsigned int gvcount;               /* Number of generators to be XOR'd into a "super" generator */
unsigned int gscount;               /* Number of seeds entered on the CL in XOR mode */
unsigned int help_flag;        /* Help flag */
unsigned int hist_flag;        /* Histogram display flag */
unsigned int iterations;	/* For timing loop, set iterations to be timed */
unsigned int ks_test;          /* Selects the KS test to be used, 0 = Kuiper 1 = Anderson-Darling */
unsigned int list;             /* List all tests flag */
unsigned int List;             /* List all generators flag */
double multiply_p;	/* multiplier for default # of psamples in -a(ll) */
unsigned int ntuple;           /* n-tuple size for n-tuple tests */
unsigned int num_randoms;      /* the number of randoms stored into memory and usable */
unsigned int output_file;      /* equals 1 if you output to file, otherwise 0. */
unsigned int output_format;    /* equals 0 (binary), 1 (unsigned int), 2 (decimal) output */
unsigned int overlap;          /* 1 use overlapping samples, 0 don't (for tests with the option) */
unsigned int psamples;         /* Number of test runs in final KS test */
unsigned int quiet;            /* quiet flag -- surpresses full output report */
unsigned int rgb;              /* rgb test number */
unsigned int sts;              /* sts test number */
unsigned int Seed;             /* user selected seed.  Surpresses reseeding per sample.*/
off_t tsamples;        /* Generally should be "a lot".  off_t is u_int64_t. */
unsigned int user;             /* user defined test number */
unsigned int verbose;          /* Default is not to be verbose. */
double Xweak;          /* "Weak" generator cut-off (one sided) */
double Xfail;          /* "Unambiguous Fail" generator cut-off (one sided) */
unsigned int Xtrategy;         /* Strategy used in TTD mode */
unsigned int Xstep;            /* Number of additional psamples in TTD/RA mode */
unsigned int Xoff;             /* Max number of psamples in TTD/RA mode */
double x_user;         /* Reserved general purpose command line inputs for */
double y_user;         /* use in any new user test. */
double z_user;
  
#define KS_SAMPLES_PER_TEST_MAX 256
 /* We need two of these to do diehard_craps.  Sigh. */
double *ks_pvalue,*ks_pvalue2;
unsigned int kspi;
struct timeval tv_start,tv_stop;
int dummy,idiot;
FILE *fp;
#define MAXFIELDNUMBER 8
char **fields;

unsigned int file_input_get_rewind_cnt(gsl_rng *rng);
off_t file_input_get_rtot(gsl_rng *rng);
void file_input_set_rtot(gsl_rng *rng,unsigned int value);

char filename[K];      /* Input file name */
int fromfile;		/* set true if file is used for rands */
int filenumbits;	/* number of bits per integer */

off_t filecount;	/* number of rands in file */
char filetype;         /* file type */

const gsl_rng_type **types;       /* where all the rng types go */
gsl_rng *rng;               /* global gsl random number generator */

gsl_matrix *A,*V;
gsl_vector *S,*svdwork;

unsigned long int seed;             /* rng seed of run (?) */
unsigned int random_max;       /* maximum rng returned by generator */
unsigned int rmax;             /* scratch space for random_max manipulation */
unsigned int rmax_bits;        /* Number of valid bits in rng */
unsigned int rmax_mask;        /* Mask for valid section of unsigned int */

#define MAXTESTS 1000



Dtest *dh_test_types[MAXTESTS];

#define ADD_TEST(t) {if (i==MAXTESTS) abort(); dh_test_types[i] = (t); i++; };

unsigned int dh_num_diehard_tests;  /* diehard tests available in dieharder */
unsigned int dh_num_sts_tests;      /* STS tests available in dieharder */
unsigned int dh_num_other_tests;    /* other tests available in dieharder */
unsigned int dh_num_user_tests;     /* user tests added in ui segment */
unsigned int dh_num_tests;          /* total tests available in dieharder */


Dtest *dh_test;             /* global pointer to the current test */

char splitbuf[PK][PBUF];

unsigned int *rgb_persist_rand_uint;
