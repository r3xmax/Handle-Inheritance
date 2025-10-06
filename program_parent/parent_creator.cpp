#include <Windows.h>
#include <iostream>
#include <string>

std::wstring askFileCreateName() {

    std::wstring fileCreateName;

    std::wcout << L"[+] Introduce the filename used when creating the file: ";
    std::getline(std::wcin, fileCreateName);

    if (fileCreateName.length() == 0) {
        std::wcout << L"[!] Error. It's necessary to introduce at least 1 character in order to asign the filename. Exiting...\n";
        exit(-1);
    }

    return fileCreateName;
}

HANDLE createCustomFile(std::wstring fileCreateName) {

    SECURITY_ATTRIBUTES fileSA;
    fileSA.nLength = sizeof(SECURITY_ATTRIBUTES);
    fileSA.lpSecurityDescriptor = NULL;
    fileSA.bInheritHandle = TRUE;

    std::wcout << L"[+] Attempting to create the file with name " << fileCreateName.c_str() << "\n";

    HANDLE hFile = CreateFileW(fileCreateName.c_str(), GENERIC_ALL, 0, &fileSA, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcout << L"[!] Error while creating the file. Code: " << GetLastError() << "\n";
        exit(-1);
    } else {
        std::wcout << L"[+] File created successfully! Continuing with the program workflow...\n";
    }

    return hFile;
}

PROCESS_INFORMATION launchChild(HANDLE hFile) {

    const wchar_t* appPath = L"..\\program_child\\child_renamer.exe";

    uintptr_t hFileNumberValue = reinterpret_cast<uintptr_t>(hFile);
    std::wstring hFileStringValue = std::to_wstring(hFileNumberValue);

    std::wstring cmdLine = L"child_renamer.exe " + hFileStringValue;

    STARTUPINFOW siw = {};
    siw.cb = sizeof(siw);

    PROCESS_INFORMATION pi = {};
    
    BOOL childProcessOk = CreateProcessW(appPath, &cmdLine[0], NULL, NULL, TRUE, 0, NULL, NULL, &siw, &pi);

    if (childProcessOk == FALSE) {
        std::wcout << L"[!] Getting an error while creating the child process... Error: " << GetLastError() << "\n";
        exit(-1);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    return pi;

}

int wmain() {

    std::wcout << L"[+] Parent process running...\n";

    std::wstring fileCreateName = askFileCreateName();

    HANDLE hFile = createCustomFile(fileCreateName);

    PROCESS_INFORMATION pi = launchChild(hFile);
}