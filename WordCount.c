#include <stdio.h>
#include <string.h>

int words(FILE* file)
{
	int i = 0;
	char leftC, rightC, p;
	leftC = fgetc(file);
    while ((rightC = fgetc(file)) != EOF)
    {
    	if ((leftC != ' ') && (rightC == ' ' || rightC == '\n' || rightC == '\0'))
    		i = i + 1;
		if 	((leftC == ',') && (rightC == ' ' || rightC == '\n') && (p == ' '))
			i = i - 1;
		p = leftC;
    	leftC = rightC;
	}
    fclose(file);
	if (leftC == ' ')
    	return i;
	else 
		return i + 1;
}

int lines(FILE* file)
{
	int i = 0;
	char c;
    while ((c = fgetc(file)) != EOF)
    {
    	if (c == '\n')
    	{
    		i = i + 1;
		}
	}
	i = i + 1;
    fclose(file);
    return i;
}

int bytes(FILE* file)
{
	int size;
    fseek(file,0,SEEK_END);
    size = ftell(file);
    return size;
}

void help()
{
	printf("Options:\n");
    printf("\t-l <File name>, --lines <File name>\t Output row count\n");        
   	printf("\t-c <File name>, --bytes <File name>\t Output file size in bytes\n");       
    printf("\t-w <File name>, --words <File name>\t Output word count\n");
}

int main(int argc, char* argv[])
{
	FILE* file = fopen(argv[2], "r"); 
	if (argc < 3) 
	{
        help();
        return 1;
    }
	if (strcmp(argv[1], "-l") == 0 || (strcmp(argv[1], "--lines") == 0))
	{
		printf("Strings: %d\n",lines(file));
		return 0;
	}
	if (strcmp(argv[1], "-c") == 0 || (strcmp(argv[1], "--bytes") == 0))
	{
		printf("Bytes: %d\n",bytes(file));
		return 0;
	}
	if (strcmp(argv[1], "-w") == 0 || (strcmp(argv[1], "--words") == 0))
	{
		printf("Words: %d\n",words(file));
		return 0;
	}			
}