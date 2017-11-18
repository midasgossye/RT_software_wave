/*
Wave Generator created by Midas Gosye, Jan Verheyen, Benyamin de Leeuw and Frederic Dupon

Part of the course MA4830: Realtime software for mechatronic systems
Professor Seet Gim Lee, Gerald
Nanyang Technological University, Singapore
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include "pcicard.h"

#define SOFT_VERSION 5.4
#define MAX_LEN 80
#define FREQ_LOWER_BOUND 2
#define FREQ_UPPER_BOUND 25000
#define AMPL_LOWER_BOUND 0
#define AMPL_UPPER_BOUND 5
#define MEAN_LOWER_BOUND 0
#define MEAN_UPPER_BOUND 5

#ifdef _WIN32                               //"cls" for win/DOS
#define CLEARSTR "cls"                      //char *CLEARSTR = "cls"
#define DIRLIST "dir config_files /b /a-d"
#else
#define CLEARSTR "clear"                    //"clear" for Unix
#define DIRLIST "ls -1 config_files"
#endif

// ============ GLOBAL VARIABLES ==================
int menu_choice;                    //variable used to switch between menu entries
int cal_ADC_val = 0;                //ADC-value read from the calibrate potentiometer
static int out_ADC = 0;             //used to store which ADC is selected to use
unsigned int data[100000];          //data array for generation of waves (data is stored in this)
bool quit_wave_gen = false;         //bool operator to go out of wave generating loop
static double ampl_val = 2.5;       //standard value
static double mean_val = 2.5;       //standard value
static double freq_val = 10.0;      //standard value
static long resolution = 50;        //resolution of created wave
static void *hdl;                   //MUST be global variable
uintptr_t iobase[6];                //MUST be global variable
void INThandler(int);               //interrupt handler to capture Ctrl+c

// ============= PROGRAM FUNCTIONS ================

//function to simply clear stdinvoid flush_stdin(void){
	int c;
	fseek(stdin,0,SEEK_END);
}

//function to print the nice welcoming message
void print_image(FILE *fptr){
    char read_string[MAX_LEN];
    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);
}

//used to remove the space behind string input
void remove_space(char *d, const char *s){
    for(;*s;++s){
        if(*s != ' ')
            *d++ = *s;
    }
    *d = *s;
}

//program checks if the choice of the user is in the range of choices and if there are no wrong inputs
int choice_checker(int lower_bound, int upper_bound){
    char valid_in = 1;
    int choice;
    printf("\nChoice: ");
    fflush(stdout);
    do{
        choice = getchar();
        if(choice != '\n'){
        	choice = choice - '0';
        	if(choice < lower_bound || choice > upper_bound){
                printf("Invalid choice!\n");
            	printf("\nChoice: ");
            	fflush(stdout);
                flush_stdin();
            	valid_in = 0;
       		}
        	else valid_in = 1;
        	}
    }while(!valid_in);
	flush_stdin();
    return choice;
}

char main_menu(void){

    //Clear the console to prepare for printing the main menu	system(CLEARSTR);
	//print the main menu
    printf("\t\t** Waveform generator V4.3 **\n");
    printf("\t\t=============================\n\n");
    printf("1)\tConfigure DAC\n");
    printf("2)\tGenerate Square Wave\n");
    printf("3)\tGenerate Sine Wave\n");
    printf("4)\tGenerate Triangle Wave\n");
    printf("5)\tLoad config file\n");
    printf("6)\tCalibrate potentiometer\n");
    printf("7)\tHelp\n\n");
    printf("8)\tQuit\n");

    return choice_checker(1,8);
}

//this is the HELP menu, just some plain text explaining the various functions.
void load_help_menu(void){
    char dummy;
	char *filename = "welcome.txt"; // Filename of .txt file containing ASCII art
    FILE *fptr = NULL;

    //Try to open file
    if((fptr = fopen(filename, "r")) == NULL) {
    	fprintf(stderr, "error opening file %s\n", filename);

    }
    system(CLEARSTR);
    //prompt user to run help menu in full screen mode to make everything readable
    printf("PLEASE RUN HELP MENU IN FULL SCREEN WINDOW FOR THE BEST EXPERIENCE\n");
    printf("Press ENTER to continue...");
    fflush(stdout);
    dummy = getchar();
    //Clear screen and display ASCII Art
    system(CLEARSTR);
    print_image(fptr);
    fflush(stdout);
    //Wait for 3 s
    sleep(3);
    //Clear screen and print actual help text
	system(CLEARSTR);

	printf("\t\t** HELP MENU **\n");
	printf("\t\t===============\n\n");
	printf("\tPROGRAM DESCRIPTION\n");
	printf("This program uses the PCI-DAS1602/16 ADC/DAC Card to generate sine, square and triangle signals.\n");
	printf("This program also serves as the second Continuous Assessment for the NTU R/T software course \nto prove the authors proficiency in the C programming language and QNX OS.\n\n");

	printf("\tBASIC PROGRAM USAGE\n");
	printf("The user can configure the DAC parameters by setting them in the DAC configuration menu.\n");
	printf("Care must be taken to make sure the signal will not exceed the 0-5 V boundary, otherwise the signal\nwill clip to the supply rail(s).\n");
	printf("Apart from setting the signals amplitude, mean value and frequency, the user can also select which \nDAC channel he would like to use.\n\n");

	printf("\tCHANGE DAC-SETTINGS BY USING SWITCHES AND POTENTIOMETER\n");
	printf("The amplitude, mean and frequency values can also be changed by flipping the right switches\nand then changing them with the potentiometer.\n");
	printf("On the board you can see the switches that are connected to the Card.\nFor more detailed information refer to the MANUAL.\n");
	printf("The switches are ordered accordingly: the left-most is number 1, the right-most is number 4.\n");
	printf("To change amplitude, flip SWITCH 1; to change the mean, flip SWITCH 2; to change the frequency,\nflip SWITCH 3.Once they are flipped, their value can be changed with the potentiometer\n");
	printf("Activating SWITCH 4 will terminate the generation of the respective wave form.\n\n");

	printf("\tCALIBRATE THE POTENTIOMETER\n");
	printf("Due to some inaccuracy in the reading of the potentiometer values, it is advised to calibrate the potentiometer.\n");
	printf("The user will be asked to turn the potentiometer to the minimum position and to press ENTER.\n");
	printf("This procedure will provide more accurate results (especially in the lower Hz range).\n\n");

	printf("\tSAVING AND LOADING CONFIG FILES\n");
	printf("If the user has set his/her desired DAC configuration parameters, he/she can choose to save this configuration\nto a config file. This is very helpful if the user needs the same parameters at another time.\n");
	printf("You can save the current configuration by choosing the 'save configuration in config file' option in the \nDAC configuration menu. To save a file, type in a file name followed by the .txt extension (e.g. config1.txt)\n");
	printf("To load a previously saved config file, select the 'Load config file' option in the main menu. A list of all\nthe available config files will be shown. To choose a config file, \ntype in the name of the file as shown in the listing (including the file extension).\n");
	printf("When the file is loaded in successfully, all of the loaded parameters will be shown. If an error occurred, \ntry again and double check if you spelled the desired file correctly and included the file extension.\n\n");
	printf("Press ENTER to go back to the main menu...");
	fflush(stdout);
	dummy = getchar();
}


//set the value for the amplitude manually
double config_amplitude(void){
   double ampl_value;
   char valid_in = 1;
   float lower_bound = AMPL_LOWER_BOUND;
   float upper_bound = AMPL_UPPER_BOUND;

   system(CLEARSTR);

    do{
        printf("Amplitude value (%.1f-%.1f V): ",lower_bound, upper_bound );
        fflush(stdout);
        ampl_value = 0;
        if(scanf("%lf", &ampl_value)!=1){
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(ampl_value <= AMPL_LOWER_BOUND || ampl_value > AMPL_UPPER_BOUND){
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    }while(!valid_in);
    return ampl_value;
}

//set the value for the mean value manually
double config_mean(void){
    double mean_value;
    char valid_in = 1;
    float lower_bound = MEAN_LOWER_BOUND;
    float upper_bound = MEAN_UPPER_BOUND;

    system(CLEARSTR);

    do{
        printf("Mean Value (%.0f-%.0f V): ", lower_bound, upper_bound);
        fflush(stdout);
        if(scanf("%lf", &mean_value)!=1){
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(mean_value <= MEAN_LOWER_BOUND || mean_value > MEAN_UPPER_BOUND){
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    }while(!valid_in);

    return mean_value;
}

//configure the frequency of the wave manually
double config_freq(void){
    double freq_value;
    char valid_in = 1;
    float lower_bound = FREQ_LOWER_BOUND;
    float upper_bound = FREQ_UPPER_BOUND;

    system(CLEARSTR);

    do{
        printf("Frequency (%.1f-%.1f Hz): ", lower_bound, upper_bound);
        fflush(stdout);
        if(scanf("%lf", &freq_value)!=1){
           printf("\nInvalid Input!\n");
        }
        getchar();
        if(freq_value < FREQ_LOWER_BOUND || freq_value > FREQ_UPPER_BOUND){
            printf("\nValue out of range!\n");
            valid_in = 0;
        }
        else valid_in = 1;
    }while(!valid_in);

    return freq_value;
}

//make sure no clipping occurs while changing amplitude or mean of the wave
char check_mean_amp_val(double amp_value, double mean_value){
    char compatible = 1;
    if(amp_value + mean_value > 5){
        compatible = 0;
    }
    else if(mean_value - amp_value < 0){
        compatible = 0;
    }
    if(!compatible){
        printf("\nWarning: Waveform will clip with current waveform settings.\n         Please change the amplitude and/or the mean value of the signal before exiting this menu.\n");
    }

    return compatible;
}

//this functions pulls the right values for amp, mean,... and saves them in a .txt file
void generate_config_file(void){
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

    if(strcmp(file_name, "\n") == 0){
    	printf("Saving file cancelled, press ENTER to return to menu...");
    	fflush(stdout);
    	dummy = getchar();
    }
    else{
    	strtok(file_name, "\n");
    	file_path = malloc(strlen(path_name)+strlen(file_name)+1);
    	file_path[0] = '\0';
    	strcat(file_path, path_name);
    	strcat(file_path, file_name);
    	fp = fopen(file_path, "w+");
    	fprintf(fp, "AMPL=%lf MEAN=%lf FREQ=%lf ADC=%d",ampl_val, mean_val, freq_val, out_ADC);
    	fclose(fp);
    	printf("Config file created with filename %s in folder %s\nPress ENTER to continue...",file_name, path_name);
    	fflush(stdout);
    	dummy = getchar();
    }
}

//submenu called DAC configuration menu that gives the user the choice to setup variables and possibly save them
void configure_DAC(void){
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
        switch(choice){
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

//thread that gets the keyboard input during the wave-generation so the user can quit
void *keyboard_check(void *arg){
	char *buffer = "a\n";
	char in_c;
	quit_wave_gen = false;

	delay(50);      //is needed to give pc time, otherwise it would skip the flush commands
	printf("\nPress 'q+ENTER' or turn the 4th switch on to quit: ");
	fflush(stdout);
	flush_stdin();

	do{
		in_c = getchar();
		if(in_c == 'q'){
			quit_wave_gen = true;
		}
		else{
			printf("\nInvalid Choice!");
			printf("\nPress 'q+ENTER' or turn the 4th switch on to quit: ");
			fflush(stdout);
		}
	}while(quit_wave_gen == false);

	flush_stdin();
}

//thread that lets the user configure the wave variables with the switches and potentiometer
//on the board
void *pot_DAC_config(void *_wave){    unsigned char * wave = (unsigned char *) _wave;
    //setup variables
	bool recalculate = false;
	float delta, dummy;
	double max_ampl_val;
	double new_ampl_val;
	double min_mean_val;
	double max_mean_val;
	double new_mean_val;
	double min_freq_val = FREQ_LOWER_BOUND;
	double max_freq_val = FREQ_UPPER_BOUND;
	double new_freq_val;
	uintptr_t switch_status;
	unsigned int i;
	//keeps checking as long as the thread is running
	while(1){
		switch_status = read_switches();
		//Checks if a switch has changed except the quit one
		if(!(switch_status == 0xff ||switch_status == 0xfe)){
            //activates when switch 1 is flipped
			if(switch_status == 0xf8){
				max_ampl_val = fmin(mean_val, 5.0-mean_val);
				new_ampl_val = (fmax(read_ADC(0) - cal_ADC_val, 0))* (max_ampl_val/(float)0xFFFF);
                //value only recalculated if treshold is met
				if(fabs(new_ampl_val - ampl_val) > 0.1){
					ampl_val = new_ampl_val;
					recalculate = true;
				}
			}
            //activates when switch 2 is flipped
            else if(switch_status == 0xf4){
			  	max_mean_val = 5.0-ampl_val;
			  	min_mean_val = ampl_val;;
			  	new_mean_val = (max_mean_val-min_mean_val)/0xFFFF *(fmax(read_ADC(0) - cal_ADC_val, 0)) + min_mean_val;
			  	if(fabs(new_mean_val - mean_val) > 0.1){
			  		mean_val = new_mean_val;
			  		recalculate = true;
			  	}
            }
            //activates when switch 3 is flipped
            else if(switch_status == 0xf2){
                delay(5);
                new_freq_val = (max_freq_val-min_freq_val)/(float)0xFFFF *(fmax(read_ADC(0) - cal_ADC_val, 0)) + min_freq_val;
	   			if((fabs(new_freq_val - freq_val) > 4)){
			  		freq_val = new_freq_val;
			  		recalculate = true;
			  	}
            }
            //when a value has been changed behind the treshold it will be recalculated and pushed to the data array
            if(recalculate){
                resolution = (174000/(freq_val));
                delta=(2.0*3.1415926535897932384626)/(float)resolution;					// increment
                for(i=0;i<resolution;i++){
                    //square wave
                    if(*wave == 0){
                        if(i<(resolution/2)){
                            dummy = (mean_val - ampl_val)/5.0*0xFFFF;
                        }
                        else{
                            dummy = (mean_val + ampl_val)/5.0*0xFFFF;
                        }
                    }
                    //sine wave
                    else if(*wave == 1){
                        dummy= (ampl_val*(sinf((float)(i*delta))) + mean_val)/5.0 * 0xFFFF ; // 0x8000 is a scaling value to scale a 0-2 V signal between 0x0000 and 0xFFFF
                    }
                    //triangle wave
                    else if(*wave == 2){
                        if(i<(resolution/2)){
                             dummy = fmin((4.0*ampl_val*(1.0/resolution)*i+mean_val-ampl_val)/5.0 * 0xFFFE, (float)0xFFFE);
                        }
                        else{
                             dummy = fmin((-4.0*ampl_val*(1.0/resolution)*i+3.0*ampl_val + mean_val)/5.0 * 0xFFFF, (float)0xFFFE);
                        }
                    }
                    data[i]= (unsigned) dummy;			// add offset + scale
                }
                recalculate = false;
            }

        }
        delay(10);
    }
}

//generate the square wave
void gen_square(void){
	int i;
	pthread_t tid;
	pthread_t tid2;
	int wave = 0;
	int *wave_ptr;
	float delta, dummy;
	wave_ptr = &wave;
	//start threads for keyboard input and board input
	pthread_create(&tid, NULL, pot_DAC_config, &wave);
	pthread_create(&tid2, NULL, keyboard_check, NULL);

    resolution = (174000/(freq_val));
    system(CLEARSTR);
    LED_out(0x01);

    printf("\t\t** Generating square wave **\n");
    printf("\t\t============================\n\n");
    //half the time value = min other half value = max
    for(i=0;i<resolution;i++){
        if(i<(resolution/2)){
            dummy = (mean_val - ampl_val)/5.0*0xFFFF;
        }
        else{
        dummy = (mean_val + ampl_val)/5.0*0xFFFF;
        }

        data[i]= (unsigned) dummy;
    }
    //write to data stream    do{
        for(i = 0; i<resolution; i++){
            write_DAC(data[i], out_ADC);
        }

    }while(((read_switches() & 0x01) == 0) && (quit_wave_gen == false));

    //kill threads
    pthread_cancel(tid);
    pthread_cancel(tid2);
}

//generate sine wave, idem comments of square
void gen_sine(void){
	int i;
	pthread_t tid;
	pthread_t tid2;
	int wave = 1;
	int *wave_ptr;
	float delta,dummy;
	wave_ptr = &wave;
	pthread_create(&tid, NULL, pot_DAC_config, &wave);
	pthread_create(&tid2, NULL, keyboard_check, NULL);
	system(CLEARSTR);
	LED_out(0x02);

	resolution = (174000/(freq_val));
    printf("\t\t** Generating sine wave **\n");
    printf("\t\t==========================\n\n");
    delta=(2.0*3.1415926535897932384626)/(float)resolution;					// increment

	for(i=0;i<resolution;i++){
        dummy= (ampl_val*(sinf((float)(i*delta))) + mean_val)/5.0 * 0xFFFF ;   // 0x8000 is a scaling value to scale a 0-2 V signal between 0x0000 and 0xFFFF
        data[i]= (unsigned) dummy;			                                   // add offset +  scale
    }

    do{
        for(i = 0; i<resolution; i++){
            write_DAC(data[i], out_ADC);
		}

    }while(((read_switches() & 0x01) == 0) && (quit_wave_gen == false));
    pthread_cancel(tid);
    pthread_cancel(tid2);
}

//create triangle wave, idem comments as square wave
void gen_triangle(void){
	int i;
    pthread_t tid;
    pthread_t tid2;
	int wave = 2;
	int *wave_ptr;
	float delta,dummy;
	wave_ptr = &wave;
	pthread_create(&tid, NULL, pot_DAC_config, &wave);
	pthread_create(&tid2, NULL, keyboard_check, NULL);
	resolution = (174000/(freq_val));
	system(CLEARSTR);
	LED_out(0x04);

    printf("\t\t** Generating triangle wave **\n");
    printf("\t\t==============================\n\n");
    for(i=0;i<resolution;i++){
        if(i<(resolution/2)){
            dummy = fmin((4.0*ampl_val*(1.0/resolution)*i+mean_val-ampl_val)/5.0 * 0xFFFE, (float)0xFFFE);
        }
        else{
            dummy = fmin((-4.0*ampl_val*(1.0/resolution)*i+3.0*ampl_val + mean_val)/5.0 * 0xFFFF, (float)0xFFFE);
        }
        data[i]= (unsigned) dummy;
    }

    do{
        for(i = 0; i<resolution; i++) {
            write_DAC(data[i], out_ADC);
        }

    }while(((read_switches() & 0x01) == 0) && (quit_wave_gen == false));
	pthread_cancel(tid);
	pthread_cancel(tid2);
}

// load the configuration file that was previously saved
void load_config(void){
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
    if(fp == NULL){
        perror("Error opening file");
        printf("File not found, press ENTER to continue...\n");
        dummy = getchar();
    }
    else{
        fscanf(fp,"AMPL=%lf MEAN=%lf FREQ=%lf ADC=%d",&ampl_val, &mean_val, &freq_val, &out_ADC);
        fclose(fp);
        printf("\nConfiguration file loaded successfully:\n\n");
        printf("Amplitude: \t%lf V\n",ampl_val);
        printf("Mean value: \t%lf V\n", mean_val);
        printf("Frequency: \t%lf Hz\n", freq_val);
        printf("Output DAC: \tDAC%d\n", out_ADC);
        printf("Press ENTER to continue...");
        fflush(stdout);
        dummy = getchar();	}
}

//load the right values and check whether they are within the limitations otherwise set default value
void load_values_from_arguments(int argc, char **argv){
	int i;
	char dummy;
	system(CLEARSTR);
    for(i=1; i < argc; i+=2){
        if(strcmp(argv[i],"-a") == 0){
            if(sscanf(argv[i+1], "%lf", &ampl_val) != 1) printf("Invalid amplitude value!\n");
            else {
                if(ampl_val > AMPL_UPPER_BOUND || ampl_val < AMPL_LOWER_BOUND){
                    printf("Amplitude value out of range! Default value loaded...\n");
                    ampl_val = 2.5;
                }
            }
        }
        else if(strcmp(argv[i],"-m") == 0){
            if(sscanf(argv[i+1], "%lf", &mean_val) != 1)  printf("Invalid mean value!\n");
            else {
                if(mean_val > MEAN_UPPER_BOUND || mean_val < MEAN_LOWER_BOUND){
                    printf("Mean value out of range! Default value loaded...\n");
                    mean_val = 2.5;
                }
            }
        }
        else if(strcmp(argv[i],"-f") == 0){
            if(sscanf(argv[i+1], "%lf", &freq_val) != 1) printf("Invalid frequency value!\n");
            else{
                if(freq_val > FREQ_UPPER_BOUND || freq_val < FREQ_LOWER_BOUND){
                    printf("Frequency value out of range! Default value loaded...\n");
                    freq_val = 10.0;
                }
            }
        }
        else if(strcmp(argv[i],"-DAC") == 0){
            if(sscanf(argv[i+1], "%d", &out_ADC) != 1) printf("Invalid DAC selection!\n") ;
            else{
                if(out_ADC < 0 || out_ADC > 1){
                    printf("Invalid DAC selection! Default value loaded...\n");
                    out_ADC = 0;
                }
            }
        }
        else{
            printf("Invalid argument(s) given!\n");
        }
    }
	if(!check_mean_amp_val(ampl_val, mean_val)){
		printf("Default values loaded for amplitude and mean value...\n");
		ampl_val = 2.5;
		mean_val = 2.5;
	}
	printf("\nValues loaded:\n\n");
    printf("Amplitude: \t%lf V\n",ampl_val);
    printf("Mean value: \t%lf V\n", mean_val);
    printf("Frequency: \t%lf Hz\n", freq_val);
    printf("Output DAC: \tDAC%d\n", out_ADC);
    printf("Press ENTER to continue...");
	fflush(stdout);
    dummy = getchar();
}

//calibration of the potentiometer
void cal_ADC(void){
	char dummy;
	bool cal_finish = true;

	system(CLEARSTR);
	printf("\t\t** ADC CALIBRATION **\n\n");
	printf("Please ensure the potentiometer is in the lowest position (i.e. clockwise position)\nto ensure proper calibration.\n");
	do{
        printf("Press ENTER when ready...");
		fflush(stdout);
		dummy = getchar();
		delay(100);
		cal_ADC_val = read_ADC(0)+30; //+30 added for safety margin
		if(cal_ADC_val > 500){
            printf("Calibration failed! Please ensure the potentiometer is in the correct position.\n");
			cal_finish = false;
		}
		else cal_finish = true;
	}while(!cal_finish);
	printf("Calibration complete.\n");
	sleep(2);
}


// ================== MAIN PROGRAM  ==================
int main(int argc, char **argv){
    //set variable to start the main while loop
	char running = 1;

	//initialise the board (load the right adresses etc.)
	init_PCI_device();

	//set LEDs to off
	LED_out(0x00);
	if(argc > 1){
		load_values_from_arguments(argc, argv);
	}
	//for capturing Ctrl+c to correctly shut down program
	signal(SIGINT, INThandler);
    while(running){
        menu_choice = main_menu();
        //depending on choice in the menu different functions are called
        switch(menu_choice){
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
            case 6: cal_ADC();
            		 break;
            case 7: load_help_menu();
            		break;
            case 8: running = 0;        //quit
        }
        LED_out(0x00);
    }
    //reset the PCI
    clean_up_PCI_device();
    return 0;
}

//when Ctrl+c is pressed the program gives a  prompt and then quits after 1 sec
void INThandler(int sig){
	char c;

	signal(sig, SIG_IGN);
	system(CLEARSTR);
	printf("Received ctrl-c signal\n");
	printf("Shutting down program...\n");
	LED_out(0xf);
	clean_up_PCI_device();
	sleep(1);
	LED_out(0x00);
	exit(0);
}
