struct Filetime
{
    FILETIME filetime;
};


struct ProcessInformation
{
    PROCESS_INFORMATION pi;
};

Filetime GetLastWriteTime(char *filename)
{
    Filetime result = {};
    WIN32_FILE_ATTRIBUTE_DATA fileData = {};
    GetFileAttributesEx(filename,GetFileExInfoStandard,&fileData);
    result.filetime = fileData.ftLastWriteTime;
    return result;
}


int CompareFiletime(Filetime *first, Filetime *second)
{
    /*
-1 First early then second
   0  First same as  second
1 First late then second
*/
    return CompareFileTime(&(first->filetime), &(second->filetime));
}

void CopyFile(char *sourceFilepath, char *destinationFilepath)
{
    CopyFile(sourceFilepath, destinationFilepath, FALSE);
}


ProcessInformation LaunchProcess(char *path, char **cmdArgs, char *baseDirectory)
{
    ProcessInformation result = {};
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi = {0};
    
    char cmdArgsSpaceSeparated[32768] = {}; 
    int stringIndex = 0;
    Concatenate(cmdArgsSpaceSeparated, cmdArgs[stringIndex]);
    stringIndex++;
    while(true)
    {
        if(cmdArgs[stringIndex] == (char *)0) break;
        Concatenate(cmdArgsSpaceSeparated, " ");
        Concatenate(cmdArgsSpaceSeparated, cmdArgs[stringIndex]);
        stringIndex++;
    }
    
    CreateProcessA(path, cmdArgsSpaceSeparated, NULL, NULL, FALSE, 0, NULL, baseDirectory, &si, &pi);
    result.pi = pi;
    return result;
}

void SleepSeconds(int seconds)
{
    Sleep(seconds * 1000);
}

void KillProcess(ProcessInformation pInfo)
{
    TerminateProcess(pInfo.pi.hProcess, 0);
    const DWORD result = WaitForSingleObject(pInfo.pi.hProcess, INFINITE);
    if (result == WAIT_OBJECT_0) 
    {
        CloseHandle(pInfo.pi.hProcess);
        CloseHandle(pInfo.pi.hThread);
    }
    else 
    {
        fprintf(stderr, "Termination error\n");
    }
}

bool HasProcessExited(ProcessInformation pInfo)
{
    bool exited = false;
    if(WaitForSingleObject(pInfo.pi.hProcess, 0) == WAIT_OBJECT_0)
    {
        exited = true;
    }
    return exited;
}

