#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// === GLOBAL VARIABLES ===
double ampl_val = 2.5;
double mean_val = 2.5;
double freq_val = 10.0;
char out_ADC = 0;




char choice_checker(char lower_bound, char upper_bound) {
    bool valid_in = true;
    char choice = 0;
    do {
        printf("\nChoice: ");
        fflush(stdin);
        choice = getchar()-'0';
        if(choice < lower_bound || choice > upper_bound) {
            printf("Invalid choice!\n");
            valid_in = false;
        }
        else valid_in = true;
    } while(!valid_in);
    //printf("%d",choice);
    return choice;

}
char main_menu(void) {
    system("cls");

    printf("\t\t** Waveform generator V1.0 **\n");
    printf("\t\t=============================\n\n");
    printf("1)\tConfigure DAC\n");
    printf("2)\tGenerate Square Wave\n");
    printf("3)\tGenerate Sine Wave\n");
    printf("4)\tGenerate Triangle Wave\n");
    printf("5)\tLoad config file\n\n");
    printf("6)\tQuit\n");
    return choice_checker(1,6);

}

double config_amplitude(void) {
    system("cls");
    double ampl_value;
    bool valid_in = true;
    do {
        printf("Amplitude value (0-5 V): ");
        fflush(stdin);
        if(scanf("%lf", &ampl_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        if(ampl_value <= 0 || ampl_value > 5) {
            printf("\nValue out of range!\n");
            valid_in = false;
        }
        else valid_in = true;
    } while(!valid_in);
    //printf("%lf",ampl_value);
    return ampl_value;
}

double config_mean(void) {
    system("cls");
    double mean_value;
    bool valid_in = true;
    do {
        printf("Mean Value (0-5 V): ");
        fflush(stdin);
        if(scanf("%lf", &mean_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        if(mean_value <= 0 || mean_value > 5) {
            printf("\nValue out of range!\n");
            valid_in = false;
        }
        else valid_in = true;
    } while(!valid_in);
    //printf("%lf",mean_val);
    return mean_value;
}

double config_freq(void) {
    system("cls");
    double freq_value;
    bool valid_in = true;
    do {
        printf("Frequency (0.1- 10 Hz): ");
        fflush(stdin);
        if(scanf("%lf", &freq_value)!=1) {
           printf("\nInvalid Input!\n");
        }
        if(freq_value < 0.1 || freq_value > 10) {
            printf("\nValue out of range!\n");
            valid_in = false;
        }
        else valid_in = true;
    } while(!valid_in);
    //printf("%lf",freq_value);
    return freq_value;
}

bool check_mean_amp_val(double amp_value, double mean_value) {
    bool compatible = true;
    if(amp_value + mean_value > 5) {
        compatible = false;
    }
    else if(mean_value - amp_value < 0) {
        compatible = false;
    }

    if(!compatible) {
        printf("\nWarning: Waveform will clip with current waveform settings.\n         Please change the amplitude and/or the mean value of the signal before exiting this menu.\n");
    }
    return compatible;

}

void configure_DAC(void) {
    bool running = true;
    bool compatible = true;
    char choice = 0;
    while(running) {
        system("cls");
        printf("\t\t** DAC Configuration Menu **\n");
        printf("\t\t============================\n\n");
        printf("1)\tSet Amplitude (Set value: %.2f V)\n",ampl_val);
        printf("2)\tSet Mean Value (Set value: %.2f V)\n",mean_val);
        printf("3)\tSet Frequency (Set value: %.2f Hz)\n",freq_val);
        printf("4)\tSelect Output DAC (Set output DAC: ADC%d)\n\n",out_ADC);
        printf("5)\tReturn to main menu\n");
        compatible = check_mean_amp_val(ampl_val, mean_val);
        choice = choice_checker(1,5);

        switch(choice) {
            case 1: ampl_val = config_amplitude();
                    break;
            case 2: mean_val = config_mean();
                    break;
            case 3: freq_val = config_freq();
                    break;
            case 4: system("cls");
                    printf("Select output DAC (0-1): \n");
                    out_ADC = choice_checker(0,1);
                    break;
            case 5: if(compatible) running = false;
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
    printf("Load config file");

}

int main() {
    bool running = true;
    while(running) {
        char choice = main_menu();
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
            case 6: running = false;



        }
    }
    return 0;
}
