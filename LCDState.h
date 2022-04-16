#ifndef LCDSTATE_H
#define LCDSTATE_H

#include "Arduino.h"
#include "LiquidCrystal.h"

#define NUM_MENU 6
#define NUM_PROGRAM 7
#define NUM_SPEED 141 // 15 rps
#define NUM_POWER 100
#define NUM_MOTOR_MODE 2
#define NUM_MODE 3

char const* MenuItems[]=
{
    "Select Menu Item",
    "   Prg_Start  Mt",
    "t  Mtr_Start  Pr",
    "rt  Mtr_Stop  Pr",
    "rt  Program  Spe",
    "op  Program  Spe",
    "rt  Program  Pow",
    "op  Program  Pow",
    "ram  Speed  Moto",
    "ram  Power  Moto",
    "eed  Motor  Mode",
    "wer  Motor  Mode",
    "otor  Mode      "
};
char const* ProgramItems[]=
{
    "Select Program  ",
    "     Test1  Test", // Program 1
    "st1  Test2  Test",
    "st2  Test3  Glob",
    "st3  Globe  Voxe",
    "obe  Voxel  Sinc",
    "oxel  Sinc  Teap",
    "inc  Teapot     ",
};
char const* MotorItems[]=
{
    "Motor Mode      ",
    "     Speed  Powe",
    "eed  Power      ",
};
char const* ModeItems[]=
{
    "Select Mode     ",
    "    Run_Once  Hl",
    "e  Hold_frame  L",
    "rame  Loop      ",
};
enum MenuState{
    topLevel = 0,
    programStart = 1,
    motorStart = 2,
    programSelect = 3,
    speedSelect = 4,
    motorModeSelect = 5,
    programModeSelect = 6,
    powerSelect = 7,
};

class LCDState {
private:
    LiquidCrystal* _lcd;

    bool program_start = 0;
    bool show_start = 1;
    bool show_speed;

    float speed;
    uint8_t to_print;

    // 0 -> top level menu
    // 3 -> program select
    // 4 -> speed select
    // 5 -> motor mode select
    // 6 -> program mode select
    // 7 -> power select
    MenuState menu_state = 0;
    uint8_t selectors[8];

public:
    LCDState(LiquidCrystal* lcd) : _lcd(lcd), selectors({1, 0, 0, 1, 80, 2, 3, 10}) {} // Set defaults
    
    bool is_started(){
        return !show_start;
    }

    bool is_program_start(){
        if (program_start){
            program_start = 0;
            return true;
        }
    }

    uint8_t get_program(){ // returns number from 1-NUM_PROGRAM
        return selectors[programSelect];
    }

    uint8_t get_program_mode(){
        return selectors[programModeSelect];
    }

    uint8_t get_power(){ // returns as duty cycle from 1-255
        float percent = selectors[powerSelect];
        return percent * 255.0/100.0;
    }

    float get_speed(){ // returns as float in rps
        return selectors[speedSelect]/10.0 + 0.9;
    }

    void button_press(){
        if (menu_state != topLevel) {menu_state = topLevel; return;}
        if (selectors[topLevel] == programStart) {program_start = 1; return;}
        if (selectors[topLevel] == motorStart) {show_start = !show_start; return;}
        menu_state = selectors[topLevel];
        if (menu_state == speedSelect) menu_state = show_speed ? speedSelect : powerSelect;
    }

    void update_selector(int8_t move){
        if (move == 0) return;
        selectors[menu_state] += (move > 0) ? 1 : -1;
        selectors[menu_state] = max(1, selectors[menu_state]);
        if (menu_state == topLevel) selectors[menu_state] = min(NUM_MENU, selectors[menu_state]);
        if (menu_state == programSelect) selectors[menu_state] = min(NUM_PROGRAM, selectors[menu_state]);
        if (menu_state == speedSelect) selectors[menu_state] = min(NUM_SPEED, selectors[menu_state]);
        if (menu_state == motorModeSelect) selectors[menu_state] = min(NUM_MOTOR_MODE, selectors[menu_state]);
        if (menu_state == programModeSelect) selectors[menu_state] = min(NUM_MODE, selectors[menu_state]);
        if (menu_state == powerSelect) selectors[menu_state] = min(NUM_POWER, selectors[menu_state]);
    }

    void update_display(){
        if (selectors[motorModeSelect] == 1) show_speed = 1;
        if (selectors[motorModeSelect] == 2) show_speed = 0;
        switch(menu_state) {
            case topLevel: // top level menu
                _lcd->setCursor(0, 0);
                _lcd->print(MenuItems[topLevel]);
                _lcd->setCursor(0, 1);
                switch(selectors[menu_state]){
                    case 1:
                        _lcd->print(MenuItems[1]); return;
                    case 2:
                        if (show_start) {_lcd->print(MenuItems[2]); return;}
                        _lcd->print(MenuItems[3]); return;
                    case 3:
                        if (show_start && show_speed) {_lcd->print(MenuItems[4]); return;}
                        if (!show_start && show_speed) {_lcd->print(MenuItems[5]); return;}
                        if (show_start && !show_speed) {_lcd->print(MenuItems[6]); return;}
                        _lcd->print(MenuItems[7]); return;
                    case 4:
                        if (show_speed) {_lcd->print(MenuItems[8]); return;}
                        _lcd->print(MenuItems[9]); return;
                    case 5:
                        if (show_speed) {_lcd->print(MenuItems[10]); return;}
                        _lcd->print(MenuItems[11]); return;
                    case 6:
                        _lcd->print(MenuItems[12]); return;
                }
                break;

            case programSelect: // program select
                _lcd->setCursor(0, 0);
                _lcd->print(ProgramItems[0]);
                _lcd->setCursor(0, 1);
                _lcd->print(ProgramItems[selectors[menu_state]]); 
                break;

            case speedSelect: // speed select
                _lcd->setCursor(0, 0);
                _lcd->print("Select Speed RPS");
                _lcd->setCursor(0, 1);
                speed = selectors[menu_state]/10.0 + 0.91; // Add 0.01 to avoid round-down
                if (selectors[menu_state] != 1){
                    to_print = speed - 0.1;
                    if (to_print < 10){
                        _lcd->print("0");
                    }
                    _lcd->print(to_print);
                    _lcd->print(".");
                    to_print = (int)((speed - 0.1) * 10.0) % 10;
                    _lcd->print(to_print);
                } else {
                    _lcd->print("    ");
                }
                _lcd->print("  ");
                to_print = speed;
                if (to_print < 10){
                    _lcd->print("0");
                }
                _lcd->print(to_print);
                _lcd->print(".");
                to_print = (int)((speed) * 10.0) % 10;
                _lcd->print(to_print);
                _lcd->print("  ");
                if (selectors[menu_state] != NUM_SPEED){
                    to_print = speed + 0.1;
                    if (to_print < 10){
                        _lcd->print("0");
                    }
                    _lcd->print(to_print);
                    _lcd->print(".");
                    to_print = (int)((speed + 0.1) * 10.0) % 10;
                    _lcd->print(to_print);
                } else {
                    _lcd->print("    ");
                }                
                break;

            case motorModeSelect: // mode select
                _lcd->setCursor(0, 0);
                _lcd->print(MotorItems[0]);
                _lcd->setCursor(0, 1);
                _lcd->print(MotorItems[selectors[menu_state]]);
                break;

            case programModeSelect: // mode select
                _lcd->setCursor(0, 0);
                _lcd->print(ModeItems[0]);
                _lcd->setCursor(0, 1);
                _lcd->print(ModeItems[selectors[menu_state]]);
                break;

            case powerSelect: // power select
                _lcd->setCursor(0, 0);
                _lcd->print("Select Power %  ");
                _lcd->setCursor(0, 1);
                _lcd->print(" ");
                if (selectors[menu_state] != 1){
                    if (selectors[menu_state] - 1< 10){
                        _lcd->print("0");
                    }
                    _lcd->print(selectors[menu_state] - 1);
                    _lcd->print("%  ");
                } else {
                    _lcd->print("     ");
                }
                if (selectors[menu_state] < 10){
                    _lcd->print("0");
                }
                _lcd->print(selectors[menu_state]);
                _lcd->print("%  ");
                if (selectors[menu_state] != 100){
                    if (selectors[menu_state] + 1< 10){
                        _lcd->print("0");
                    }
                    _lcd->print(selectors[menu_state] + 1);
                    _lcd->print("% ");
                    if (selectors[menu_state] + 1 != 100){
                        _lcd->print(" ");
                    }
                } else {
                    _lcd->print("     ");
                }                
                break;
        }
    }
};

#endif