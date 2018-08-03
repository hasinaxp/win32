#include <windows.h>
#include <wchar.h>
#include "constants.h"

//globals
wchar_t className[] = L"Myclass";
HWND myDlg = NULL;

//wndProc functions
BOOL LoadTextfile(HWND hwnd, LPCTSTR flName);
BOOL saveTextFile(HWND textControl, LPCTSTR filename);

void saveFile(HWND h);
void currenrtSave(HWND hwnd);

void DoSelectFont(HWND hwnd);


LRESULT CALLBACK winProc(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdResult, int nCmdShow)
{
    HWND hwnd;
    WNDCLASSEX w;
    MSG msg;

    //window class resistration

    w.hInstance =hInstance;
    w.lpfnWndProc =winProc;
    w.hCursor = LoadCursor(hInstance, IDC_ARROW);
    w.hIcon =LoadIcon(NULL, IDI_APPLICATION);
    w.lpszClassName =className;
    w.cbClsExtra =NULL;
    w.cbWndExtra =NULL;
    w.hbrBackground =(HBRUSH)(COLOR_WINDOW + 1);
    w.lpszMenuName =NULL;
    w.style =NULL;
    w.cbSize = sizeof(WNDCLASSEX);
    w.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&w))
        MessageBox(NULL, L"Unable to resister window class!", L"error", MB_ICONERROR | MB_OK);
    
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, L"S-EDIT", WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT,
        640,380,NULL,NULL,hInstance, NULL);

    if(hwnd == NULL)
        MessageBox(NULL, L"Unable to create window!", L"error", MB_ICONERROR | MB_OK);
    
    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, hwnd, NULL, NULL) >0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK winProc(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
     HMENU hMenu, hSubMenu,hEditSubMenu;

    switch (msg)
    {
    case WM_CREATE:
        {
            //menu creation:
           

            hMenu = CreateMenu();
            hSubMenu = CreatePopupMenu();
            hEditSubMenu =CreatePopupMenu();

            //creating the file menu
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_NEW, L"&New");
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_LOAD, L"&Load");
            AppendMenu(hSubMenu,MF_STRING, ID_FILE_SAVE, L"&Save");
            AppendMenu(hSubMenu,MF_STRING, ID_FILE_SAVE_AS, L"&Save As");
            AppendMenu(hSubMenu, MF_MENUBREAK,NULL,NULL);
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, L"&Exit");

            //creating the edit menu
             AppendMenu(hEditSubMenu, MF_STRING, ID_EDIT_UNDO, L"Undo");
             AppendMenu(hEditSubMenu, MF_MENUBREAK,NULL,NULL);
             AppendMenu(hEditSubMenu, MF_STRING, ID_EDIT_CUT, L"Cut");
             AppendMenu(hEditSubMenu, MF_STRING, ID_EDIT_COPY, L"copy");
             AppendMenu(hEditSubMenu, MF_STRING, ID_EDIT_PASTE, L"paste");

            //creating the main menu
            AppendMenu(hMenu, MF_STRING |MF_POPUP, (UINT)hSubMenu, L"&File");
             AppendMenu(hMenu, MF_STRING |MF_POPUP, (UINT)hEditSubMenu, L"&Edit");
            AppendMenu(hMenu, MF_STRING, ID_FONT_DLG, L"&Font");
            AppendMenu(hMenu, MF_STRING,ID_ABOUT, L"&About");
       
            SetMenu(hwnd, hMenu);

            //creating main edit control...
            //HFONT hDefFont;
            HWND hEdit;
     
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD| WS_VISIBLE |WS_HSCROLL | WS_VSCROLL|ES_MULTILINE |ES_AUTOHSCROLL |ES_AUTOVSCROLL,  0,0, 500, 500, hwnd, (HMENU)ID_MY_TEXT_FIELD, GetModuleHandle(NULL),NULL); 
            if(hEdit == NULL)
                MessageBox(NULL, L"Unale to create edit control", L"error", MB_ICONERROR |MB_OK);

            SetFocus(hEdit);
        }
        break;
    case WM_SIZE:
        {
            HWND hEdit;
            RECT rcClient;

            GetClientRect(hwnd,&rcClient);
            
            hEdit =GetDlgItem(hwnd, ID_MY_TEXT_FIELD);
            SetWindowPos(hEdit,NULL, 0, 0, rcClient.right,rcClient.bottom, SWP_NOZORDER); 
        }
        break;
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case ID_FILE_EXIT:
                DestroyWindow(hwnd);
                break;
            case ID_FILE_LOAD:
                {
                    OPENFILENAME ofn;
                    wchar_t fileName[MAX_PATH] = L"";

                    ZeroMemory(&ofn, sizeof(ofn));

                    ofn.nMaxFile = MAX_PATH;
                    ofn.lStructSize = sizeof(ofn);
                    ofn.lpstrFile =fileName;
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All files\0*.*\0";
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = L"txt";

                    if(GetOpenFileName(&ofn))
                    {
                        HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                        if(LoadTextfile(hEdit,fileName))
                            SetWindowText(hwnd,fileName);
                            
                    }
                }
                break;
            case ID_FILE_SAVE_AS:
                {
                    saveFile(hwnd);
                           
                }
                break;
            case ID_FILE_SAVE:
                {
                    currenrtSave(hwnd);
                }
                break;
            case ID_FILE_NEW:
                {
                    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                    DWORD textlen = GetWindowTextLength(hEdit);
                    if(textlen!= 0)
                    {
                        const int result = MessageBox(hwnd,L"Do You want to save before creating new File?", L"warning", MB_ICONWARNING | MB_YESNO);
                        switch(result)
                        {
                        case IDYES:
                            saveFile(hwnd);
                            SetDlgItemText(hwnd,ID_MY_TEXT_FIELD,L"");
                            break;
                        case IDNO:
                            SetDlgItemText(hwnd,ID_MY_TEXT_FIELD,L"");
                            break;
                        }
                    }
                    SetWindowText(hwnd, L"S-EDIT");
                }
                break;
           /*- case ID_MY_TEXT_FIELD:
                {
                    switch(HIWORD(wParam))
                    {
                    case EN_CHANGE:
                        EnableMenuItem(hMenu, ID_FILE_SAVE_AS,MF_ENABLED |MF_BYCOMMAND);
                        EnableMenuItem(hMenu, ID_FILE_SAVE,MF_ENABLED |MF_BYCOMMAND);
                        break;
                    }
                }
                break;*/
            case ID_EDIT_UNDO:
                {
                    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                    SendMessage(hEdit,EM_UNDO, 0, 0);
                }
                break;
            case ID_EDIT_CUT:
                {
                    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                    SendMessage(hEdit, WM_CUT, 0, 0);
                }
                break;
            case ID_EDIT_COPY:
                {
                    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                    SendMessage(hEdit, WM_COPY, 0, 0);
                }
                break;
                case ID_EDIT_PASTE:
                {
                    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                }
                break;
            case ID_FONT_DLG:
                {
                    HWND hEdit = GetDlgItem(hwnd, ID_MY_TEXT_FIELD);
                    DoSelectFont(hEdit);
                }
                break;
            case ID_ABOUT:
                MessageBox(NULL,L"created by: Spandan Mondal\r\n Written in: C++ win32 API", L"about", MB_OK);
                break;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_QUIT:
        DestroyWindow(hwnd);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


BOOL LoadTextfile(HWND hwnd, LPCTSTR flName)
{
    HANDLE hFile;
    BOOL success =FALSE;
    hFile = CreateFile(flName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dFileSize;
        dFileSize = GetFileSize(hFile, NULL);
        if(dFileSize != 0xFFFFFFFF)
        {
            LPSTR fileText;
            fileText =(LPSTR)GlobalAlloc(GPTR, dFileSize + 1);
           
            if(fileText !=NULL)
            {
                DWORD dwRead;
                if(ReadFile(hFile, fileText, dFileSize, &dwRead,NULL))
                {
                    
                    
                    if(SetWindowTextA(hwnd, fileText))
                        success = TRUE;
                }
            }

            GlobalFree(fileText);
        }
        CloseHandle(hFile);
        return success;
    }
}

BOOL saveTextFile(HWND textControl, LPCTSTR filename)
{
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    BOOL success = FALSE;

    if(hFile!=INVALID_HANDLE_VALUE)
    {
        DWORD buffLen =GetWindowTextLengthA(textControl);
        LPSTR buffer;
        buffer = (LPSTR)GlobalAlloc(GPTR, buffLen);
        if(GetWindowTextA(textControl,buffer,buffLen))
        {
            DWORD dWritten;
            if(WriteFile(hFile,buffer, buffLen,&dWritten,NULL))
            {
                success = TRUE;
            }

        }

    

    GlobalFree(buffer);
    }
    
    CloseHandle(hFile);
    return success;
}

void saveFile(HWND hwnd)
{
                     OPENFILENAME ofn;
                    wchar_t fileName[MAX_PATH] = L"";

                    ZeroMemory(&ofn, sizeof(ofn));

                    ofn.nMaxFile = MAX_PATH;
                    ofn.lStructSize = sizeof(ofn);
                    ofn.lpstrFile =fileName;
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All files\0*.*\0";
                    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = L"txt";

                    if(GetSaveFileName(&ofn))
                    {
                        HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
                        if(saveTextFile(hEdit,fileName))
                        {
                            
                            SetWindowText(hwnd,fileName);
                            
                        }
                             
                    }
}
void currenrtSave(HWND hwnd)
{
    wchar_t  saveContentName[MAX_PATH] = L"";
    GetWindowText(hwnd,saveContentName,MAX_PATH);

    if(saveContentName != L"S-EDIT")
    {
    HWND hEdit = GetDlgItem(hwnd,ID_MY_TEXT_FIELD);
     if(saveTextFile(hEdit,saveContentName))
         SetWindowText(hwnd,saveContentName);
    }
    else
    {
        saveFile(hwnd);
    }
   
                      
}


void DoSelectFont(HWND hwnd)
{
    HFONT g_hfFont =(HFONT) GetStockObject(DEFAULT_GUI_FONT);
    COLORREF g_rgbText = RGB(0, 0, 0);
    CHOOSEFONT cf = {sizeof(CHOOSEFONT)};
    LOGFONT lf;
    GetObject(g_hfFont, sizeof(LOGFONT), &lf);
    cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = g_rgbText;
    if(ChooseFont(&cf))
    {
        HFONT hf = CreateFontIndirect(&lf);
        if(hf)
        {
            g_hfFont = hf;
            SendMessage(hwnd,WM_SETFONT,WPARAM(g_hfFont), TRUE);
        }
        else
        {
            MessageBox(hwnd, L"Font creation failed!", L"Error", MB_OK |
            MB_ICONEXCLAMATION);
        }
        g_rgbText = cf.rgbColors;
    }
}