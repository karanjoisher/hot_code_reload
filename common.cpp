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


