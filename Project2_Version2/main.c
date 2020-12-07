/*
 * main.c
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>

#include "apex_cpu.h"

int
main(int argc, char const *argv[])
{
    APEX_CPU *cpu;

    fprintf(stderr, "APEX CPU Pipeline Simulator v%0.1lf\n", VERSION);

    argc = 4; /*Shweta ::: remove this line before submitting*/
    if (argc != 4)
    {
        fprintf(stderr, "APEX_Help: Usage %s <input_file> <operation> <Number_Of_Cycles>\n", argv[0]);
        exit(1);
    }

    int numberOfCycles = atoi(argv[3]);

    /*Shweta : Pass input file, simulate/display/single_step, number of cycles*/
    cpu = APEX_cpu_init(argv[1], argv[2], numberOfCycles);

    if (!cpu)
    {
        fprintf(stderr, "APEX_Error: Unable to initialize CPU\n");
        exit(1);
    }

    APEX_cpu_run(cpu);
    APEX_cpu_stop(cpu);
    return 0;
}