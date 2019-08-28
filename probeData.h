/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: probeData.h
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 13-Nov-2018 00:34:03
 */

#ifndef PROBEDATA_H
#define PROBEDATA_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "rtwtypes.h"
#include "initSettings.h"

/* Type Definitions */
#include <stdio.h>

/* Function Declarations */
void probeData4bit(int8_t *data, int msTake);
void probeData8bit(int8_t *data, int msTake, int cursor);
void probeDataSampleBySample_8bit(int8_t *data, int sampleSizeToTake, int cursor);
void probeData(int *data, int msTake);
void probeBits(int8_t *data, int msTake, int cursor);

#endif

/*
 * File trailer for probeData.h
 *
 * [EOF]
 */
