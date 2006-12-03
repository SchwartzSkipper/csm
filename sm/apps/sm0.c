#include <time.h>
#include "../src/math_utils.h"
#include "../src/sm.h"
#include "../src/laser_data.h"

extern int distance_counter;
int main(int argc, const char*argv[]) {
	FILE * file;
	if(argc==1) {
		file = stdin;
	} else {
		const char * filename = argv[1];
		file = fopen(filename,"r");
		if(file==NULL) {
			fprintf(stderr, "Could not open '%s'.\n", filename); 
			return -1;
		}
	}
	
	struct sm_params params;
	struct sm_result result;
	
	params.maxAngularCorrectionDeg = 5;
	params.maxLinearCorrection = 0.2;
	params.maxIterations = 30;
	params.epsilon_xy = 0.001;
	params.epsilon_theta = 0.001;
	params.maxCorrespondenceDist = 2;
	params.sigma = 0.01;
	params.restart = 1;
	params.restart_threshold_mean_error = 3.0 / 300.0;
	params.restart_dt= 0.1;
	params.restart_dtheta=    1.5 * 3.14 /180;

	params.clusteringThreshold = 0.05;
	params.orientationNeighbourhood = 3;
	params.useCorrTricks = 1;

	params.doAlphaTest = 0;
	params.outliers_maxPerc = 0.85;

	params.outliers_adaptive_order =0.7;
	params.outliers_adaptive_mult=2;
	params.doVisibilityTest = 1;
	params.doComputeCovariance = 0;

	int num_matchings = 0;
	int num_iterations = 0;
	clock_t start = clock();
	
	if(ld_read_next_laser_carmen(file, &params.laser_ref)) {
		printf("Could not read first scan.\n");
		return -1;
	}
	
	gsl_vector *u = gsl_vector_alloc(3);
	gsl_vector *x_old = gsl_vector_alloc(3);
	gsl_vector *x_new = gsl_vector_alloc(3);
	while(!ld_read_next_laser_carmen(file, &params.laser_sens)) {
		copy_from_array(x_old, params.laser_ref.odometry);
		copy_from_array(x_new, params.laser_sens.odometry);
		pose_diff(x_new,x_old,u);
		vector_to_array(u, params.odometry);
	
	 	sm_gpm(&params,&result);
	 	//sm_icp(&params,&result);
		
		num_matchings++;
		num_iterations += result.iterations;
	
		ld_free(&(params.laser_ref));
		params.laser_ref = params.laser_sens;
	}
	ld_free(&(params.laser_ref));

	clock_t end = clock();
	float seconds = (end-start)/((float)CLOCKS_PER_SEC);
	
	printf("sm0: CPU time = %f (seconds) (start=%d end=%d)\n", seconds,(int)start,(int)end);
	printf("sm0: Total number of matchings = %d\n", num_matchings);
	printf("sm0: Total number of iterations = %d\n", num_iterations);
	printf("sm0: Avg. iterations per matching = %f\n", num_iterations/((float)num_matchings));
	printf("sm0: Avg. seconds per matching = %f\n", seconds/num_matchings);
	printf("sm0:   that is, %d matchings per second\n", (int)floor(num_matchings/seconds));
	printf("sm0: Avg. seconds per iteration = %f (note: very imprecise)\n", seconds/num_iterations);
	printf("sm0: Number of comparisons = %d \n", distance_counter);
	printf("sm0: Avg. comparisons per ray = %f \n", 
		(distance_counter/((float)num_iterations*params.laser_ref.nrays)));
	
	gsl_vector_free(u);
	gsl_vector_free(x_old);
	gsl_vector_free(x_new);
	return 0;
}