#pragma once
#include "../../libs/MinerConfig/minerConfig.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include "resource.h"

/*Wrapper of generateSettings for dialog*/
void generateSettingsForDIalog();

/*Reads/writes to/from GUI to/from settings struct*/
void getSetDialogValues(HWND hwnd, bool set);

/*Shows example arguments string in GUI*/
void showTestValue(HWND hwnd);

/*Creates miner installer binaries with new minerConfig*/
void configureSilentMinerInstaller(std::string binaryName);

/*Generates arguments same as miner(just as example)*/
void generateStartArguments(minerConfig* config, std::string* result);