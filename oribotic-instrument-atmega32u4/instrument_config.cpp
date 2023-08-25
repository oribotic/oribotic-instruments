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
#include "ORICORD.h"

uint8_t intervaldelay = INTERVALDELAY;

#if ORIGAMI == YOSHIMURA
    // --------------------------------------------------------------------    BUTTONS
    struct pressbuttons buttons[BTN_COUNT] =
    {
        {.pin = CAL_BTN, .led = LED1, .state = 0},
        {.pin = HI_BTN,  .led = LED1, .state = 0},
        {.pin = LO_BTN,  .led = LED1, .state = 0},
        {.pin = FN1_BTN, .led = LED1, .state = 0},
        {.pin = FN2_BTN, .led = LED1, .state = 0}
    };

    #if TEXTILE == 0
        struct orikey orikeys[PANELCOUNT] = {
            // numbered clockwise, starting from usb cable
            // [0 / 1][2 / 3]
            // [6 / 7][4 / 5]
            // top left (closest to usb)
            {.pin = 5, .panel = 0, .note = 4, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  0
            {.pin = 4, .panel = 0, .note = 5, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  1
            // top right
            {.pin = 0, .panel = 0, .note = 8, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  2
            {.pin = 1, .panel = 0, .note = 9, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  3
            // bottom right
            {.pin = 7, .panel = 0, .note = 7, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  4
            {.pin = 6, .panel = 0, .note = 6, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  5
            // bottom left
            {.pin = 3, .panel = 0, .note = 3, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  6
            {.pin = 2, .panel = 0, .note = 2, .hard = 1023, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  7 
        };  
    #endif
    // same for midi and osc
    #if TEXTILE == 1
        struct orikey orikeys[PANELCOUNT] = {
            {.pin = 4, .panel = 0, .note = 2, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  0
            {.pin = 5, .panel = 0, .note = 3, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  1
            {.pin = 2, .panel = 0, .note = 8, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  6
            {.pin = 3, .panel = 0, .note = 9, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  7
            {.pin = 7, .panel = 0, .note = 4, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  2
            {.pin = 6, .panel = 0, .note = 5, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  3
            {.pin = 9, .panel = 0, .note = 7, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  5
            {.pin = 8, .panel = 0, .note = 6, .hard = 70, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  //  4
    };
    #endif
    

    // LINEAR BEND TABLE
    #if OSC == 1
        uint8_t bendLinear[256] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 80, 81, 82, 84, 86, 87, 89, 91, 93, 95, 97, 99, 102, 105, 108, 111, 115, 119, 123, 128, 134, 142, 151, 165, 186, 190, 195, 200, 205, 210, 220, 255};
    #endif 
    
    #if MIDI == 1
    uint8_t bendLinear[128] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 38, 39, 40, 42, 43, 45, 47, 49, 52, 55, 59, 64, 71, 82, 95, 100, 105, 127};
    #endif

    #ifdef PANELGROUPS
    // 11111111 = 255
    // 11001100 = 204
    // 00110011 = 51
    bool usePanelGroups = true;
    
    struct panelgroup
    {
        char *title;
        uint8_t value;
        byte pattern[1];
    };

    // 00000011 = 3
    // 00001100 = 12
    // 00110000 = 48
    // 11000000 = 192
    // 00110011 = 51

    struct panelgroup panelgroups[8] = {
        {.title = "pp01", .value = 0, .pattern = {0b00000011}},
        {.title = "pp23", .value = 0, .pattern = {0b00001100}},
        {.title = "pp45", .value = 0, .pattern = {0b00110000}},
        {.title = "pp67", .value = 0, .pattern = {0b11000000}},
        {.title = "dia1", .value = 0, .pattern = {0b00110011}},
        {.title = "dia2", .value = 0, .pattern = {0b11001100}},
    };
    #endif  // ------------------------- PANELGROUPS

#endif // ------------------------- YOSHIMURA

#if ORIGAMI == KRESLING

    const uint8_t MPR121_INT = 4; // pin 4 is the MPR121 interrupt on the Bare Touch Board

    // --------------------------------------------------------------------    BUTTONS

    struct pressbuttons buttons[BTN_COUNT] =
    {
        {.pin = CAL_BTN, .led = LED1, .state = 0},
        {.pin = HI_BTN, .led = LED2, .state = 0},
        {.pin = LO_BTN, .led = LED3, .state = 0}
    };

    // LINEAR BEND TABLE
    uint8_t bendLinear[256] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 80, 81, 82, 84, 86, 87, 89, 91, 93, 95, 97, 99, 102, 105, 108, 111, 115, 119, 123, 128, 134, 142, 151, 165, 186, 190, 195, 200, 205, 210, 220, 255};

    struct orikey orikeys[48] = {
        // row 1  
        { .pin = 0,  .panel = 0, .note =  3, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  0
        { .pin = 7,  .panel = 0, .note =  2, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  1
        { .pin = 6,  .panel = 0, .note = 12, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  2
        { .pin = 1,  .panel = 0, .note = 24, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  3
        { .pin = 2,  .panel = 0, .note = 24, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  4
        { .pin = 5,  .panel = 0, .note = 12, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  5
        { .pin = 4,  .panel = 0, .note =  5, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  6
        { .pin = 3,  .panel = 0, .note =  0, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  7
        // row 2
        { .pin = 8,  .panel = 0, .note =  5, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  8
        { .pin = 3,  .panel = 1, .note =  4, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, //  9
        { .pin = 2,  .panel = 1, .note = 14, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 10
        { .pin = 9,  .panel = 0, .note = 26, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 11
        { .pin = 10, .panel = 0, .note = 26, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 12
        { .pin = 11, .panel = 1, .note = 14, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 13
        { .pin = 0,  .panel = 1, .note =  7, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 14
        { .pin = 11, .panel = 0, .note =  2, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 15
        // row 3
        { .pin = 4,  .panel = 1, .note =  7, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 16
        { .pin = 11, .panel = 1, .note =  6, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 17
        { .pin = 10, .panel = 1, .note = 16, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 18
        { .pin = 5,  .panel = 1, .note = 28, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 19
        { .pin = 6,  .panel = 1, .note = 28, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 20
        { .pin = 9,  .panel = 1, .note = 16, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 21
        { .pin = 8,  .panel = 1, .note =  9, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 22
        { .pin = 7,  .panel = 1, .note =  4, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 23
        // row 4
        { .pin = 0,  .panel = 2, .note =  8, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 24
        { .pin = 7,  .panel = 2, .note =  7, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 25
        { .pin = 6,  .panel = 2, .note = 17, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 26
        { .pin = 1,  .panel = 2, .note = 29, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 27
        { .pin = 2,  .panel = 2, .note = 29, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 28
        { .pin = 5,  .panel = 2, .note = 17, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 29
        { .pin = 4,  .panel = 2, .note = 10, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 30
        { .pin = 3,  .panel = 2, .note =  5, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 31
        // row 5 
        { .pin = 8,  .panel = 2, .note = 10, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 32
        { .pin = 3,  .panel = 3, .note =  9, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 33
        { .pin = 2,  .panel = 3, .note = 19, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 34
        { .pin = 9,  .panel = 2, .note = 31, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 35
        { .pin = 10, .panel = 2, .note = 31, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 36
        { .pin = 1,  .panel = 3, .note = 19, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 37
        { .pin = 0,  .panel = 3, .note = 12, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 38
        { .pin = 11, .panel = 2, .note =  7, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 39
        // row 6 
        { .pin = 4,  .panel = 3, .note = 12, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 40
        { .pin = 11, .panel = 3, .note = 11, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 41
        { .pin = 10, .panel = 3, .note = 21, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 42
        { .pin = 5,  .panel = 3, .note = 33, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 43
        { .pin = 6,  .panel = 3, .note = 33, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 44
        { .pin = 9,  .panel = 3, .note = 21, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 45
        { .pin = 8,  .panel = 3, .note = 14, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }, // 46
        { .pin = 7,  .panel = 3, .note =  9, .hard = 70, .bendLO = 700, .bendHI = 750, .state = 0, .last = 0 }  // 47
    };

   #ifdef PANELGROUPS
    // 11111111 = 255
    // 11001100 = 204
    // 00110011 = 51
    bool usePanelGroups = true;

    struct panelgroup
    {
        char *title;
        uint8_t value;
        byte pattern[6];
    };

    // 11111111 = 255
    // 11001100 = 204
    // 00110011 = 51

    struct panelgroup panelgroups[8] = {
        {.title = "tlt0", .value = 0, .pattern = {0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}},
        {.title = "tlt1", .value = 0, .pattern = {0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000}},
        {.title = "tlt2", .value = 0, .pattern = {0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000}},
        {.title = "tlt3", .value = 0, .pattern = {0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b00000000}},
        {.title = "tlt4", .value = 0, .pattern = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b00000000}},
        {.title = "tlt5", .value = 0, .pattern = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111}},
        {.title = "twsL", .value = 0, .pattern = {0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100}},
        {.title = "twsR", .value = 0, .pattern = {0b00110011, 0b00110011, 0b00110011, 0b00110011, 0b00110011, 0b00110011}},
    };
    #endif  // ------------------------- PANELGROUPS

#endif // ------------------------- KRESLING

#if ORIGAMI == SUKI
    // --------------------------------------------------------------------    BUTTONS
    struct pressbuttons buttons[BTN_COUNT] =
    {
        {.pin = CAL_BTN, .led = LED1, .state = 0},
        {.pin = HI_BTN, .led = LED1, .state = 0},
        {.pin = LO_BTN, .led = LED1, .state = 0},
        {.pin = FN1_BTN, .led = LED1, .state = 0},
        {.pin = FN2_BTN, .led = LED1, .state = 0}
    };

    // LINEAR BEND TABLE
    uint8_t bendLinear[256] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 80, 81, 82, 84, 86, 87, 89, 91, 93, 95, 97, 99, 102, 105, 108, 111, 115, 119, 123, 128, 134, 142, 151, 165, 186, 190, 195, 200, 205, 210, 220, 255};

    struct orikey orikeys[PANELCOUNT] = {
        {.pin = 3,  .panel = 0, .note = 2, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  0
        {.pin = 4,  .panel = 0, .note = 3, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  1
        {.pin = 2,  .panel = 0, .note = 8, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  2
        {.pin = 0,  .panel = 0, .note = 9, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  3
        {.pin = 11, .panel = 0, .note = 4, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  4
        {.pin = 8,  .panel = 0, .note = 5, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  5
        {.pin = 5,  .panel = 0, .note = 7, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  6
        {.pin = 1,  .panel = 0, .note = 6, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  7
        {.pin = 10, .panel = 0, .note = 2, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  8
        {.pin = 9,  .panel = 0, .note = 3, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},   //  9
        {.pin = 7,  .panel = 0, .note = 8, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0},  // 10
        {.pin = 6,  .panel = 0, .note = 9, .hard = 1024, .soft = 0, .bendLO = 100, .bendHI = 1023, .state = 0, .last = 0}  // 11
    };

#endif 
