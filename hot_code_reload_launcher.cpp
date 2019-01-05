#include <stdio.h>
#include <windows.h>

void CopyString(char *source, char *destination, int startIndex, int length)
{
    int destinationIndex = 0;
    for(int sourceIndex = startIndex; sourceIndex < (startIndex + length); sourceIndex++, destinationIndex++)
    {
        destination[destinationIndex] = source[sourceIndex];
    }
}

int StringLength(char *str)
{
    int length = 0;
    while(str[length] != 0) length++;
    return length;
}

void Concatenate(char *first, char *second)
{
    int lengthOfFirst = StringLength(first);
    char *firstEnd = first + lengthOfFirst;
    CopyString(second, firstEnd, 0, StringLength(second) + 1);
}

int GetDirectoryNameEndIndex(char *path)
{
    int result = 0;
    for(int i = 0; path[i] != 0; i++)
    {
        if(path[i] == '\\' || path[i] == '/')
        {
            result = i - 1;
        }
    }
    return result;
}


FILETIME GetLastWriteTime(char *filename)
{
    WIN32_FILE_ATTRIBUTE_DATA fileData = {};
    GetFileAttributesEx(filename,GetFileExInfoStandard,&fileData);
    return fileData.ftLastWriteTime;
}

int GetFilenameStartIndex(char *path)
{
    int result = 0;
    int length = StringLength(path);
    
    for(int i = length - 1; i >= 0; i--)
    {
        if(path[i] == '\\' || path[i] == '/') 
        {
            result = i + 1; 
            break;
        }
    }
    return result;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "ERROR: No executable path specified\n");
        return -1;
    }
    
    if(argc < 3)
    {
        fprintf(stderr, "No base directory specified, assuming the base directory as the one in which the .exe resides.\n");
    }
    
    char *exePath = argv[1];
    FILETIME lastWriteTime = GetLastWriteTime(exePath);
    
    char tempExePath[512] = {};
    CopyString(exePath, tempExePath, 0, StringLength(exePath) - 4);
    
    char *baseDirectory;
    char temp[512] = {};
    if(argc < 3)
    {
        CopyString(tempExePath, temp, 0, GetDirectoryNameEndIndex(tempExePath) + 1);
        baseDirectory = temp;
    }
    else
    {
        baseDirectory = argv[2];
    }
    
    char filename[512] = {}; 
    CopyString(exePath, filename, GetFilenameStartIndex(exePath), StringLength(exePath) - GetFilenameStartIndex(exePath));
    
    char cmdArgsBase[512] = {};
    char cmdArgs[512] = {};
    Concatenate(cmdArgsBase, filename);
    Concatenate(cmdArgsBase, " ");
    
    CopyString(cmdArgsBase, cmdArgs, 0,  StringLength(cmdArgsBase));
    Concatenate(cmdArgs, "hcr_initialized");
    
    Concatenate(tempExePath, "_temp.exe");
    CopyFile(exePath, tempExePath, FALSE);
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi = {0};
    CreateProcessA(tempExePath, cmdArgs, NULL, NULL, FALSE, 0, NULL, baseDirectory, &si, &pi);
    
    CopyString(cmdArgsBase, cmdArgs, 0,  StringLength(cmdArgsBase));
    cmdArgs[StringLength(cmdArgsBase)] = '\0';
    Concatenate(cmdArgs, "hcr_reloaded\0");
    
    bool running = true;
    while(running)
    {
        FILETIME newLastWriteTime = GetLastWriteTime(exePath);
        if(CompareFileTime(&lastWriteTime, &newLastWriteTime))
        {
            Sleep(1000);
            lastWriteTime = GetLastWriteTime(exePath);
            
            fprintf(stdout, "Exe has changed...\n");
            TerminateProcess(pi.hProcess, 0);
            // 500 ms timeout; use INFINITE for no timeout
            fprintf(stdout, "Terminating current instance...\n");
            const DWORD result = WaitForSingleObject(pi.hProcess, INFINITE);
            if (result == WAIT_OBJECT_0) 
            {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                
                CopyFile(exePath, tempExePath, FALSE);
                fprintf(stdout, "Launching new instance...\n");
                CreateProcessA(tempExePath, cmdArgs, NULL, NULL, FALSE, 0, NULL, baseDirectory, &si, &pi);
            }
            else 
            {
                fprintf(stderr, "Termination error\n");
            }
        }
        else
        {
            if(WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
            {
                running = false;
            }
        }
    }
    
    return 0;
}