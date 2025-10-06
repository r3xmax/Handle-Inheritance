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

## Key Takeaways
- How to configure inheritable handles in Windows (bInheritHandle, bInheritHandles).
- How to pass handles between processes.
- Practical use of APIs like CreateFileW, CreateProcessW, and SetFileInformationByHandle.
