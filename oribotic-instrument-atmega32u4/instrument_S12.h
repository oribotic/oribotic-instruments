/*
 * Copyright (c) 2023 Matthew Gardiner
 *
 * MIT License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/oribotic/oribotic-instruments for documentation
 *
 */

// instrument definion for S12 
// Waterbomb 12 panel instrument

#include "ORICORD.h"
#include <Arduino.h>

#define SOFT_LOW_OFFSET 5
#define LO_HARD_OFFSET 50
#define PINCOUNT 12
#define MPRCOUNT 1
//#define PANELGROUPS 1

#define CAL_BTN 5
#define HI_BTN 10
#define LO_BTN 9
#define FN1_BTN 8
#define FN2_BTN 6
#define LED1 13
#define BTN_COUNT 5
#define PANELCOUNT 12
