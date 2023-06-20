#include <Windows.h>
#include <string>
#include "resource.h"
#include "dialog.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow) {
    MSG  msg;
    HWND hwnd;

    hwnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, NULL);

    ShowWindow(hwnd, SW_SHOW);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


