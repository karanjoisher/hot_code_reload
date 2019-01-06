struct Filetime
{
    time_t filetime;
};


struct ProcessInformation
{
    pid_t processID;
};

Filetime GetLastWriteTime(char *filename)
{
    Filetime result = {};
    struct stat fileData = {};
    stat(filename, &fileData);
    result.filetime = fileData.st_mtime;
    return result;
}

int CompareFiletime(Filetime *first, Filetime *second)
{
    /*
-1 First early then second
   0  First same as  second
1 First late then second
*/
    
    double seconds = difftime(first->filetime, second->filetime);
    
    if(seconds < 0.0)
    {
        return -1;
    }
    else if(seconds == 0.0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void CopyFile(char *sourceFilepath, char *destinationFilepath)
{
    int sourceFD = open(sourceFilepath, O_RDONLY);
    
    struct stat sourceFileData = {};
    stat(sourceFilepath, &sourceFileData);
    char *buffer = new char[sourceFileData.st_size];
    if(sourceFD == -1)
    {
        fprintf(stderr, "ERROR: Could not open source file:%s for copying\n", sourceFilepath);
        delete[] buffer;
        close(sourceFD);
        return;
    }
    ssize_t bytesRead = read(sourceFD, (void *)buffer, sourceFileData.st_size);
    if(bytesRead != sourceFileData.st_size)
    {
        fprintf(stderr, "ERROR: Could not read source file:%s for copying\n", sourceFilepath);
        delete[] buffer;
        close(sourceFD);
        return;
    }
    close(sourceFD);
    
    int destinationFD = open(destinationFilepath, O_WRONLY | O_CREAT);
    if(destinationFD == -1)
    {
        fprintf(stderr, "ERROR: Could not open destination file:%s for copying\n", destinationFilepath);
        delete[] buffer;
        close(destinationFD);
        return;
    }
    
    ssize_t bytesWritten = write(destinationFD, (void *)buffer, sourceFileData.st_size);
    if(bytesWritten != sourceFileData.st_size)
    {
        fprintf(stderr, "ERROR: Could not write to destination file:%s while copying\n", destinationFilepath);
        delete[] buffer;
        close(destinationFD);
        return;
    }
    
    delete[] buffer;
    close(destinationFD);
    return;
}

ProcessInformation LaunchProcess(char *path, char **cmdArgs, char *baseDirectory)
{
    ProcessInformation result = {};
    chdir(baseDirectory);
    pid_t processID;
    posix_spawn(&processID, path, NULL, NULL, cmdArgs, environ);
    result.processID = processID;
    return result;
}

void SleepSeconds(int seconds)
{
    sleep(seconds);
}

void KillProcess(ProcessInformation pInfo)
{
    kill(pInfo.processID, SIGTERM);
    sleep(1);
    kill(pInfo.processID, SIGKILL); 
}

bool HasProcessExited(ProcessInformation pInfo)
{
    bool exited = false;
    int status;
    int returnProcessID = waitpid(pInfo.processID, &status, WNOHANG);
    if(returnProcessID == pInfo.processID)
    {
        exited = true;
    }
    return exited;
}
