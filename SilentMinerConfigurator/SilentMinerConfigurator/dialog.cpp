#include "dialog.h"
#include "settings.h"


LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        //on dialog start
    case WM_INITDIALOG:
    {
        getSetDialogValues(hwnd, true);
        generateSettingsForDIalog();
    }
    return TRUE;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {


            case IDC_BUTTON1:
            {
                getSetDialogValues(hwnd, false);
                generateSettingsForDIalog();
                showTestValue(hwnd);
                break;
            }


            //button create miner installer dll and exe
            case IDOK:
            {
                getSetDialogValues(hwnd, false);
                generateSettingsForDIalog();
                configureSilentMinerInstaller(std::string("InstallerEXE.exe"));
                configureSilentMinerInstaller(std::string("installerDLL.dll"));
                break;
            }
            //button cancel and exit
            case IDCANCEL:
            {
                PostQuitMessage(0);
                break;
            }
        }
        break;
    }

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    }

    return false;
}


void getSetBoolInCheckBox(HWND hwnd, bool* itemToSetGet, int nIDDlgItem, BOOL set)
{
    if (set == true)
    {
        if (*itemToSetGet == true)
        {
            CheckDlgButton(hwnd, nIDDlgItem, BST_CHECKED);
        }
        else
        {
            CheckDlgButton(hwnd, nIDDlgItem, BST_UNCHECKED);
        }
    }
    else
    {
        UINT res = IsDlgButtonChecked(hwnd, nIDDlgItem);
        if (res == BST_CHECKED)
        {
            *itemToSetGet = true;
        }
        else
        {
            *itemToSetGet = false;
        }
    }
}


void getSetNumberInEditControl(HWND hwnd, int* itemToSetGet, int nIDDlgItem, BOOL set)
{
    if (set == true)
    {
        std::string numberAsstring = std::to_string(*itemToSetGet);
        SetDlgItemText(hwnd, nIDDlgItem, numberAsstring.c_str());
    }
    else
    {
        char* wbuf;
        int len = SendDlgItemMessage(hwnd, nIDDlgItem, WM_GETTEXTLENGTH, 0, 0);

        if (len > 0)
        {
            wbuf = new char[len + 1];
            GetDlgItemText(hwnd, nIDDlgItem, wbuf, len + 1);
            *itemToSetGet = atoi(wbuf);
            delete[] wbuf;
        }
        else
        {
            *itemToSetGet = 0;
        }
    }
}

void getSetStringInEditControl(HWND hwnd, std::string *itemToSetGet, int nIDDlgItem, BOOL set)
{
    if (set == true)
    {
        SetDlgItemText(hwnd, nIDDlgItem, (*itemToSetGet).c_str());
    }
    else
    {
        char* wbuf;
        int len = SendDlgItemMessage(hwnd, nIDDlgItem, WM_GETTEXTLENGTH, 0, 0);

        if (len > 0)
        {
            if (len > 1023) 
            {
                len = 1023;
            }
            wbuf = new char[len + 1];
            GetDlgItemText(hwnd, nIDDlgItem, wbuf, len + 1);
            *itemToSetGet = wbuf;
            delete[] wbuf;
        }
        else
        {
            *itemToSetGet = "";
        }
    }
}