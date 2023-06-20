#pragma once
#include <Windows.h>
#include <string>
#include "resource.h"

LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
void getSetStringInEditControl(HWND hwnd, std::string *itemToSetGet, int nIDDlgItem, BOOL set);
void getSetNumberInEditControl(HWND hwnd, int* itemToSetGet, int nIDDlgItem, BOOL set);
void getSetBoolInCheckBox(HWND hwnd, bool* itemToSetGet, int nIDDlgItem, BOOL set);