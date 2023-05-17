#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmshc.h>
#include <time.h>
#include <getopt.h>



#include "frequency.h"
#include "animation.h"
#include "sound.h"

void usage()
{
	printf("Usage: \n"
			"./project <k> <out> [-s -H -h] [-o outputFile]\n" 
			"---------------------------- \n\n"
			" - k is the number of frequencies to compute. \n "
			"- out is the output file to write the frequencies. \n "
			"[-H] enable Harmony design\n "
			"[-s] enable sound simultation\n "
			"[-h] print the usage\n "
			"[-o] output file for geometry /!\\ MUST END WITH .msh /!\\ \n "
			"\n");
}

int main (int argc, char *argv[]) {

	if (argc < 2){
		usage();
		return -1;
	}
	
	FILE* file = fopen(argv[2], "w");

	bool compute_harmony = false;
	bool compute_sound = false;
	char* geo_output = NULL;
	for(int i = 2; i < argc; i++){
		if(strcmp(argv[i], "-h") == 0){
			usage(); 
			return 0;
		}else if(strcmp(argv[i],"-H") == 0){
			compute_harmony = true;
		}else if(strcmp(argv[i],"-s") == 0 ){
			compute_sound = true;
		}else if(strcmp(argv[i],"-o") == 0){
			geo_output = argv[i+1];
			i++;
		}
	}


	int ierr;
	gmshInitialize(argc, argv, 0, 0, &ierr);

	gmshOptionSetNumber("General.RotationX", 0, &ierr); 	
	gmshOptionSetNumber("General.Antialiasing", 1, &ierr); 	
	gmshOptionSetNumber("Mesh.SurfaceEdges", 0, &ierr); 	
	gmshOptionSetNumber("Geometry.Points", 0, &ierr); 	
	gmshOptionSetNumber("Geometry.Curves", 0, &ierr); 	
	gmshOptionSetNumber("PostProcessing.AnimationDelay", 0.05, &ierr); 	
	
	
	int n_vibration_modes = atoi(argv[1]);
	double* animation_points;
	double * frequencies = calloc(sizeof *frequencies, n_vibration_modes);
	int n_nodes;


	clock_t timer;
	timer = clock();

	if(compute_harmony){
		double meshSizeFactor = 0.5;
		double handle_length = 38e-3;
		double space_between = 2e-2;
		double space_second_between = 1e-2 / 4.0f;

		//FIRST FIND THE SECOND LAYER THEN THE FIRST LAYER ....
		double second_layer_phong_length = bin_search_MTFS_second_l(handle_length, 6e-2, space_between, space_second_between, 0.3, meshSizeFactor, 1e-1);
		
		printf("second layer phong length is %.10lf\n", second_layer_phong_length);
		
		double first_layer_phong_length = bin_search_MTFS_first_l(handle_length,0.3, space_between, space_second_between, second_layer_phong_length, meshSizeFactor, 1e-1);
		
		printf("first layer phong length is %.10lf\n", first_layer_phong_length);
		
		get_k_frequency_MTFS(file, geo_output, handle_length, first_layer_phong_length,space_between, space_second_between, second_layer_phong_length, meshSizeFactor, n_vibration_modes, true, &animation_points, &n_nodes, frequencies);
	
	}else{
		double meshSizeFactor = 0.3;
		double iner_radius = 6e-3;
		double outer_radius = 11e-3;
		double handle_length = 38e-3;

		double correct_l = bin_search_l(iner_radius, outer_radius, handle_length, 0.1, meshSizeFactor, 1e-1);
		printf("phong length is: %.10lf\n", correct_l);
		get_k_frequency(file, geo_output, iner_radius, outer_radius, handle_length, correct_l, meshSizeFactor, n_vibration_modes, true, &animation_points, &n_nodes, frequencies);

	}

	fclose(file);

	for(int i = 0; i < n_vibration_modes+1; i++){
		if(i==0) continue;
		generate_animation(0, i, animation_points, n_nodes);
 	}
	
	timer = clock() - timer;

	double time_solve_naif = (double)((1000 * timer) / (double)CLOCKS_PER_SEC);
	printf("TIME USED %.20lf\n", time_solve_naif);
	

	gmshViewCombine("steps", "all", 1, 1, &ierr);
	gmshOptionSetNumber("View.AdaptVisualizationGrid", 1, &ierr); 
	gmshOptionSetNumber("View.LightLines", 0, &ierr); 
	gmshOptionSetNumber("View.SmoothNormals", 1, &ierr); 

	if(compute_sound){
		InitSoundSystem();
		PlayFrequency(frequencies[1], 4000);
		CloseSoundSystem();
	}


	gmshFltkRun(&ierr);


	free(frequencies);
	free(animation_points);
	
	return 0;
}
