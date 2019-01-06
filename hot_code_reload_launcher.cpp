#include <stdio.h>
#include "common.cpp"
#if PLATFORM_WINDOWS
#include <windows.h>
#include "windows.cpp"
#elif PLATFORM_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <spawn.h>
#include <signal.h>
extern char **environ;
#include "linux.cpp"
#endif

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
    Filetime lastWriteTime = GetLastWriteTime(exePath);
    
    char tempExePath[512] = {};
    
#if PLATFORM_WINDOWS
    int extensionLength = 4;
    char *tempExeSuffix = "_temp.exe";
#elif PLATFORM_LINUX
    int extensionLength = 0;
    char *tempExeSuffix = "_temp";
#endif
    
    CopyString(exePath, tempExePath, 0, StringLength(exePath) - extensionLength);
    
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
    
    char *cmdArgs[] = {filename, "hcr_initialized", (char *)0}; 
    
    Concatenate(tempExePath, tempExeSuffix);
    CopyFile(exePath, tempExePath);
    
    ProcessInformation pInfo = LaunchProcess(tempExePath, cmdArgs, baseDirectory);
    cmdArgs[1] = "hcr_reloaded";
    
    bool running = true;
    while(running)
    {
        Filetime newLastWriteTime = GetLastWriteTime(exePath);
        if(CompareFiletime(&lastWriteTime, &newLastWriteTime))
        {
            SleepSeconds(1);
            
            lastWriteTime = GetLastWriteTime(exePath);
            fprintf(stdout, "Exe has changed...\n");
            
            fprintf(stdout, "Terminating current instance...\n");
            KillProcess(pInfo);
            CopyFile(exePath, tempExePath);
            fprintf(stdout, "Launching new instance...\n");
            pInfo = LaunchProcess(tempExePath, cmdArgs, baseDirectory);
        }
        else
        {
            running = !(HasProcessExited(pInfo));
        }
    }
    return 0;
}