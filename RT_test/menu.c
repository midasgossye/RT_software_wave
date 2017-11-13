#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>

#define FREQ_LOWER_BOUND 0.1
#define FREQ_UPPER_BOUND 10
#define AMPL_LOWER_BOUND 0
#define AMPL_UPPER_BOUND 5
#define MEAN_LOWER_BOUND 0
#define MEAN_UPPER_BOUND 5

#ifdef _WIN32
#define CLEARSTR "cls"
#define DIRLIST "dir config_files /b /a-d"
#else
#define CLEARSTR "clear"
#define DIRLIST "ls -1 config_files"
#endif

// === GLOBAL VARIABLES ===
static double ampl_val = 2.5;
static double mean_val = 2.5;
static double freq_val = 10.0;
static int out_ADC = 0;
//"cls" for win/DOS
//"clear" for Unix
//char *CLEARSTR = "cls";


void flush_stdin(void) {
	int c;
	while((c = getchar()) != '\n' && c != EOF);
}

void remove_space(char *d, const char *s){
    for(;*s;++s){
        if(*s != ' ')
            *d++ = *s;
    }
    *d = *s;
}

char switch_check(void) {
	return 0;
}

int choice_checker(int lower_bound, int upper_bound) {
    char valid_in = 1;
    int choice;
    //flush_stdin();
    do {
        printf("\nChoice: ");
        fflush(stdout);
       do {
       	choice = getch();
       }  while(choice==ERR && switch_check() == 0);
        choice = choice - '0';
        if(choice < lower_bound || choice > upper_bound) {
            printf("Invalid choice!\n");
            valid_in = 0;
        }
        else valid_in = 1;


    } while(!valid_in);
    //printf("%d",choice);
    flush_stdin();
    return choice;

}
char main_menu(void) {
    system(CLEARSTR);

    printf("\t\t** Waveform generator V1.0 **\n");
    printf("\t\t=============================\n\n");
    printf("1)\tConfigure DAC\n");
    printf("2)\tGenerate Square Wave\n");
    printf("3)\tGenerate Sine Wave\n");
    printf("4)\tGenerate Triangle Wave\n");
    printf("5)\tLoad config file\n");
    printf("6)\tHelp\n\n");
    printf("7)\tQuit\n");
    return choice_checker(1,7);

}

void load_help_menu(void) {
	char dummy;
	system(CLEARSTR);

	printf("\t\t** HELP MENU **\n");
	printf("\t\t===============\n\n");
	printf("\tPROGRAM DESCRIPTION\n");
	printf("This program uses the PCI-DAS1602/16 ADC/DAC Card to generate sine, square and triangle signals.\n");
	printf("This program also serves as the second Continuous Assesment for the NTU R/T software course \nto prove the authors proficiency in the C programming lamguage and QNX OS.\n\n");

	printf("\tBASIC PROGRAM USAGE\n");
	printf("The user can setup the DAC paramters by setting them in the DAC configuration menu.\n");
	printf("Care must be taken to make sure the signal will not exceed the 0-5 V boundary, otherwise the signal\nwill clip to the supply rail(s).\n");
	printf("Apart from setting the signals amplitude, mean value and frequency, the user can also select which \nDAC channel he would like to use.\n\n");

	printf("\tSAVING AND LOADING CONFIG FILES\n");
	printf("If the user has set his desired DAC configuration parameters, he can choose to save this configuration\nto a config file. This is very helpful if the user needs the same paramters at another time.\n");
	printf("You can save the current configuration by choosing the 'save configuration in config file' option in the \nDAC configuration menu. To save a file, type in a file name followed by the .txt extension (e.g. config1.txt)\n");
	printf("To load a previously saved config file, select the 'Load config file' option in the main menu. A list of all\nthe available config files will be shown. To choose a config file, \ntype in the name of the file as shown in the listing (including the file extension).\n");
	printf("When the file is loaded in succesfully, all of the loaded parameters will be shown. If an error occured, \ntry again and double check if you spelled the desired file correctly and included the file extension.\n\n");
	printf("Press enter to go back to the main menu...");
	fflush(stdout);
	dummy = getchar();

}

double config_amplitude(void) {
   double ampl_value;
   char valid_in = 1;
   float lower_bound = AMPL_LOWER_BOUND;
   float upper_bound = AMPL_UPPER_BOUND;

   system(CLEARSTR);



    do {
        printf("Amplitude value (%.1f-%.1f V): ",lower_bound, upper_bound );
        fflush(stdout);
        ampl_value = 0;
        if(scanf("%lf", &ampl_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(ampl_value <= AMPL_LOWER_BOUND || ampl_value > AMPL_UPPER_BOUND) {
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    } while(!valid_in);
    //printf("%lf",ampl_value);
    return ampl_value;
}

double config_mean(void) {
    double mean_value;
    char valid_in = 1;
    float lower_bound = MEAN_LOWER_BOUND;
    float upper_bound = MEAN_UPPER_BOUND;
    system(CLEARSTR);

    do {
        printf("Mean Value (%.0f-%.0f V): ", lower_bound, upper_bound);
        fflush(stdout);
        if(scanf("%lf", &mean_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(mean_value <= MEAN_LOWER_BOUND || mean_value > MEAN_UPPER_BOUND) {
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    } while(!valid_in);
    //printf("%lf",mean_val);
    return mean_value;
}

double config_freq(void) {
    double freq_value;
    char valid_in = 1;
    float lower_bound = FREQ_LOWER_BOUND;
    float upper_bound = FREQ_UPPER_BOUND;
    system(CLEARSTR);

    do {
        printf("Frequency (%.1f-%.1f Hz): ", lower_bound, upper_bound);
        fflush(stdout);
        if(scanf("%lf", &freq_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(freq_value < FREQ_LOWER_BOUND || freq_value > FREQ_UPPER_BOUND) {
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    } while(!valid_in);
    //printf("%lf",freq_value);
    return freq_value;
}

char check_mean_amp_val(double amp_value, double mean_value) {
    char compatible = 1;
    if(amp_value + mean_value > 5) {
        compatible = 0;
    }
    else if(mean_value - amp_value < 0) {
        compatible = 0;
    }

    if(!compatible) {
        printf("\nWarning: Waveform will clip with current waveform settings.\n         Please change the amplitude and/or the mean value of the signal before exiting this menu.\n");
    }
    return compatible;

}

void generate_config_file(void) {
    char file_name_buff[60];
    char *path_name = "config_files/";
    char * file_path;
    FILE *fp;
    char dummy;
    char *file_name;


    system(CLEARSTR);
    printf("\t\t** Configuration file creator **\n");
    printf("\t\t================================\n\n");
    printf("Config file name: ");
    fflush(stdout);

    fgets (file_name_buff, 60, stdin);
    file_name = malloc(strlen(file_name_buff));
    remove_space(file_name, file_name_buff);
    strtok(file_name, "\n");

    file_path = malloc(strlen(path_name)+strlen(file_name)+1);
    file_path[0] = '\0';
    strcat(file_path, path_name);
    strcat(file_path, file_name);

    //printf("%s, %d", file_path, strlen(file_path));

    fp = fopen(file_path, "w+");
    fprintf(fp, "AMPL=%lf MEAN=%lf FREQ=%lf ADC=%d",ampl_val, mean_val, freq_val, out_ADC);
    fclose(fp);
    printf("Config file created with filename %s in folder %s\nPress enter to continue...",file_name, path_name);
    fflush(stdout);
    dummy = getchar();


}

void configure_DAC(void) {
    char running = 1;
    char compatible = 1;
    int choice = 0;
    while(running) {
        system(CLEARSTR);
        printf("\t\t** DAC Configuration Menu **\n");
        printf("\t\t============================\n\n");
        printf("1)\tSet Amplitude (Set value: %.2f V)\n",ampl_val);
        printf("2)\tSet Mean Value (Set value: %.2f V)\n",mean_val);
        printf("3)\tSet Frequency (Set value: %.2f Hz)\n",freq_val);
        printf("4)\tSelect Output DAC (Set output DAC: DAC%d)\n",out_ADC);
        printf("5)\tSave configuration in config file\n\n");
        printf("6)\tReturn to main menu\n");
        compatible = check_mean_amp_val(ampl_val, mean_val);
        choice = choice_checker(1,6);

        switch(choice) {
            case 1: ampl_val = config_amplitude();
                    break;
            case 2: mean_val = config_mean();
                    break;
            case 3: freq_val = config_freq();
                    break;
            case 4: system(CLEARSTR);
                    printf("Select output DAC (0-1): \n");
                    out_ADC = choice_checker(0,1);
                    break;
            case 5: generate_config_file();
                    break;
            case 6: if(compatible) running = 0;
                    else check_mean_amp_val(ampl_val, mean_val);
                    break;

        }
    }
}

void gen_square(void) {
    printf("Square");

}

void gen_sine(void) {
    printf("Sine");

}

void gen_triangle(void) {
    printf("Triangle");

}

void load_config(void) {
    char file_name_buff[60];
    char *file_name;
    char *path_name = "config_files/";
    char * file_path;
    FILE *fp;
    char str[60];
    char dummy;

    system(CLEARSTR);
    printf("\t\t** Configuration file loader **\n");
    printf("\t\t===============================\n\n");
    printf("Saved config files:\n");
    //"dir config_files /b /a-d" for WIN/DOS
    //"ls -1" for UNIX
    system(DIRLIST);
    printf("\nType filename of desired config file: ");
    fflush(stdout);
    fgets (file_name_buff, 60, stdin);
    file_name = malloc(strlen(file_name_buff));
    remove_space(file_name, file_name_buff);
    strtok(file_name, "\n");

    file_path = malloc(strlen(path_name)+strlen(file_name)+1);
    file_path[0] = '\0';
    strcat(file_path, path_name);
    strcat(file_path, file_name);
    printf("Selected file: %s\n",file_path);



   /* opening file for reading */
   fp = fopen(file_path , "r");
   if(fp == NULL) {
      perror("Error opening file");
      printf("File not found, press enter to continue...\n");
      dummy = getchar();
   }
   else {
    fscanf(fp,"AMPL=%lf MEAN=%lf FREQ=%lf ADC=%d",&ampl_val, &mean_val, &freq_val, &out_ADC);
    fclose(fp);

    printf("\nConfiguration file loaded successfully:\n\n");
    printf("Amplitude: \t%lf V\n",ampl_val);
    printf("Mean value: \t%lf V\n", mean_val);
    printf("Frequency: \t%lf Hz\n", freq_val);
    printf("Output DAC: \tDAC%d\n", out_ADC);
    printf("Press enter to continue...");
	fflush(stdout);
    dummy = getchar();
	}


}

void load_values_from_arguments(int argc, char **argv) {
	int i;
	char dummy;
	system(CLEARSTR);

		for(i=1; i < argc; i+=2) {
			if(strcmp(argv[i],"-a") == 0) {
				if(sscanf(argv[i+1], "%lf", &ampl_val) != 1) printf("Invalid amplitude value!\n") ;
				else {
					if(ampl_val > AMPL_UPPER_BOUND || ampl_val < AMPL_LOWER_BOUND) {
						printf("Amplitude value out of range! Default value loaded...\n");
						ampl_val = 2.5;
						}
				}
			}
			else if(strcmp(argv[i],"-m") == 0) {
				if(sscanf(argv[i+1], "%lf", &mean_val) != 1)  printf("Invalid mean value!\n");
				else {
					if(mean_val > MEAN_UPPER_BOUND || mean_val < MEAN_LOWER_BOUND) {
						printf("Mean value out of range! Default value loaded...\n");
						mean_val = 2.5;
					}
				}
			}
			else if(strcmp(argv[i],"-f") == 0) {
				if(sscanf(argv[i+1], "%lf", &freq_val) != 1) printf("Invalid frequency value!\n");
				else {
					if(freq_val > FREQ_UPPER_BOUND || freq_val < FREQ_LOWER_BOUND) {
						printf("Frequency value out of range! Default value loaded...\n");
						freq_val = 10.0;
					}
				}
			}
			else if(strcmp(argv[i],"-DAC") == 0) {
				if(sscanf(argv[i+1], "%d", &out_ADC) != 1) printf("Invalid DAC selection!\n") ;
				else {
					if(out_ADC < 0 || out_ADC > 1) {
						printf("Invalid DAC selection! Default value loaded...\n");
						out_ADC = 0;
					}
				}
			}
			else {
				printf("Invalid argument(s) given!\n");

			}
		}
	if(!check_mean_amp_val(ampl_val, mean_val)) {
		printf("Default values loaded for amplitude and mean value...\n");
		ampl_val = 2.5;
		mean_val = 2.5;
	}
	printf("\nValues loaded:\n\n");
    printf("Amplitude: \t%lf V\n",ampl_val);
    printf("Mean value: \t%lf V\n", mean_val);
    printf("Frequency: \t%lf Hz\n", freq_val);
    printf("Output DAC: \tDAC%d\n", out_ADC);
    printf("Press enter to continue...");
	fflush(stdout);
    dummy = getchar();
}



int main(int argc, char **argv) {
	char running = 1;
	initscr();
	//timeout(-1);
	cbreak();
	noecho();
	keypad( stdscr, TRUE);
	move(0,0);
	if(argc > 1) {
		load_values_from_arguments(argc, argv);
	}
    while(running) {
        int choice = main_menu();
        switch(choice) {
            case 1: configure_DAC();
                    break;
            case 2: gen_square();
                    break;
            case 3: gen_sine();
                    break;
            case 4: gen_triangle();
                    break;
            case 5: load_config();
                    break;
            case 6: load_help_menu();
            		break;
            case 7: running = 0;
        }
    }
    endwin();
    return 0;
}
