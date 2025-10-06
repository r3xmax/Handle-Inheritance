#include <Windows.h>
#include <iostream>
#include <string>
#include <wchar.h>

std::wstring askFileReName() {

    std::wstring fileReName;

    std::wcout << L"[+] Introduce the filename used to rename file: ";
    std::getline(std::wcin, fileReName);

    if (fileReName.length() == 0) {
        std::wcout << L"[!] Error. It's necessary to introduce at least 1 character in order to modify the filename. Exiting...\n";
        exit(-1);
    }

    return fileReName;
}

void renameFile(std::wstring fileReName, HANDLE hFile) {

    size_t bsizeFileReName = fileReName.size() * sizeof(wchar_t) + sizeof(wchar_t);
    size_t wsizeFileReName = fileReName.size() + 1;


    FILE_RENAME_INFO* fri = (FILE_RENAME_INFO*)malloc(sizeof(FILE_RENAME_INFO) + bsizeFileReName);

    fri->ReplaceIfExists = TRUE;
    fri->RootDirectory = NULL;
    fri->FileNameLength = fileReName.size() * sizeof(wchar_t);
    wmemcpy_s(fri->FileName, wsizeFileReName, fileReName.c_str(), wsizeFileReName);

    size_t sizeTotalFri = sizeof(FILE_RENAME_INFO) + bsizeFileReName;

    std::wcout << L"[+] Trying to rename the file...\n";

    BOOL renameOk = SetFileInformationByHandle(hFile, FileRenameInfo, fri, (DWORD)sizeTotalFri);

    if (!renameOk) {
        std::wcout << L"[!] Got a error when trying to rename the file. Code error: " << GetLastError() << "\n";
        exit(-1);
    }

    std::wcout << L"[+] File renamed sucessfully!";

    free(fri);

}

int wmain(int argc, wchar_t** argv) {

    std::wcout << L"[+] Child process running..." << "\n";

    std::wstring fileReName = askFileReName();

    wchar_t* hFileStringValue = argv[1];
    uintptr_t hFileNumberValue = _wtoi64(hFileStringValue);
    HANDLE hFile = reinterpret_cast<HANDLE>(hFileNumberValue);

    std::wcout << L"[+] Checking file object handle validity...\n";

    if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
        std::wcout << L"[!] Invalid handle. Exiting...\n";
        exit(-1);
    }

    std::wcout << L"[+] The file handle is valid! Continuing...\n";

    renameFile(fileReName, hFile);

    return 0;
    
}