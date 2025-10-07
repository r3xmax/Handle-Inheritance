# Handle-Inheritance
This project is a minimal Windows API (WinAPI) demo written in C++ that explores handle inheritance between parent and child processes. The goal is to show how a parent process can create a file, pass its handle to a child process, and then let the child perform operations on the same file object without reopening it.

# Concept
The demo is composed of two small programs, each with its own role:

## 1. Parent process (parent_creator.cpp)
  - Prompts the user for a filename.
  - Creates the file with inheritable attributes (SECURITY_ATTRIBUTES).
  - Launches a child process while passing the inherited file handle as a command-line argument.

## 2. Child process (child_renamer.cpp)
  - Prompts the user for a new filename.
  - Validates the received file handle.
  - Uses SetFileInformationByHandle with FileRenameInfo to rename the file through the inherited handle.

<img width="614" height="381" alt="image" src="https://github.com/user-attachments/assets/c46673b3-2387-4005-b0dc-5ae35f3054ba" />

## 3. How I made it

Let's take a look at the **main concepts** that made **Handle Inheritance** possible.

### bInheritHandle member of SECURITY_ATTRIBUTES

To allow a handle to be inherited by a child process, the `bInheritHandle` member of the `SECURITY_ATTRIBUTES` structure must be set to `TRUE` when calling the `CreateFileW` function.

```cpp
SECURITY_ATTRIBUTES fileSA;
    fileSA.nLength = sizeof(SECURITY_ATTRIBUTES);
    fileSA.lpSecurityDescriptor = NULL;
    fileSA.bInheritHandle = TRUE; // <-- this
```

### bInheritHandles property in CreateProcessW function

The syntax of `CreateProcessW` function is the following:

```cpp
BOOL CreateProcessW(
  [in, optional]      LPCWSTR               lpApplicationName,
  [in, out, optional] LPWSTR                lpCommandLine,
  [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
  [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
  [in]                BOOL                  bInheritHandles,
  [in]                DWORD                 dwCreationFlags,
  [in, optional]      LPVOID                lpEnvironment,
  [in, optional]      LPCWSTR               lpCurrentDirectory,
  [in]                LPSTARTUPINFOW        lpStartupInfo,
  [out]               LPPROCESS_INFORMATION lpProcessInformation
);
```

The `bInheritHandles` parameter expects a boolean value (`BOOL`). If set to `TRUE`, the child process can inherit handles from the parent process. The handles must have been created with `bInheritHandle` set to `TRUE` in their `SECURITY_ATTRIBUTES`, and the child process must have the necessary access rights to use them.

In the project, we applied this concept:

```cpp
BOOL childProcessOk = CreateProcessW(appPath, &cmdLine[0], NULL, NULL, TRUE, 0, NULL, NULL, &siw, &pi);
```

### Transferring the Handle Value (Command Line)

Once all requirements are met, it's time to transfer the handle value from the parent process to the child process. This is necessary because, although the child process has access to the file object associated with the handle, it doesn't know the handle's value.

A common practice for transferring the handle value is to pass it through the command line when starting the child process (using the `lpCommandLine` parameter of the `CreateProcessW` function).

```cpp
// PARENT PROCESS
PROCESS_INFORMATION launchChild(HANDLE hFile) {

<SNIP>

    uintptr_t hFileNumberValue = reinterpret_cast<uintptr_t>(hFile);
    std::wstring hFileStringValue = std::to_wstring(hFileNumberValue);

    std::wstring cmdLine = L"child_renamer.exe " + hFileStringValue;

<SNIP>
    
    BOOL childProcessOk = CreateProcessW(appPath, &cmdLine[0], NULL, NULL, TRUE, 0, NULL, NULL, &siw, &pi);
```

First, we convert the handle value into a `uintptr_t` type, which allows converting a pointer into a numeric value without losing information. Then, we convert this numeric handle value into a wide-character string and store it in the dynamic buffer `cmdLine`. Finally, we create the child process by specifying the `cmdLine` variable, which contains the handle value as a string.

Let's take a look at the child process program. First, by using `int argc, wchar_t** argv`, we obtain the arguments passed when the child process was started. Then, we take the first argument and store it. After that, we convert the `wchar_t` value into a `uintptr_t` type. Finally, we convert the `uintptr_t` value into a `HANDLE` type.

```cpp
int wmain(int argc, wchar_t** argv) {

    std::wcout << L"[+] Child process running..." << "\n";

    std::wstring fileReName = askFileReName();

    wchar_t* hFileStringValue = argv[1];
    uintptr_t hFileNumberValue = _wtoi64(hFileStringValue);
    HANDLE hFile = reinterpret_cast<HANDLE>(hFileNumberValue);
```

Now, we can use the handle to perform the required operations.
