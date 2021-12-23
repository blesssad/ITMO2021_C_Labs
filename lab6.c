/*Лабораторная работ. Архиватор файлов.

Целью лабораторной работы является разработка программы по архивированию
и распаковке нескольких файлов в один архив. Архиватор должен
1. Уметь архивировать несколько (один и более) указанных файлов в архив с
расширением *.arc

2. Уметь распаковывать файловых архив, извлекая изначально запаковонные
файлы

3. Предоставлять список файлов упакованных в архиве

4. Сжимать и разжимать данные при архивировании с помощью алгоритма
Хаффмана (опциональное задание, оценивается доп баллами)
Архиватор должен быть выполнен в виде консольного приложения,
принимающего в качестве аргументов следующий параметры
● --file FILE
Имя файлового архива с которым будет работать архиватор

● --create
Команда для создания файлового архива

● --extract;
Команда для извлечения из файлового архива файлов

● --list
Команда для предоставления списка файлов, хранящихся в архиве

● FILE1 FILE2 .... FILEN
Свободные аргументы для передачи списка файлов для запаковки*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxfiles 10

void create(char nameofarc[], char* namesoffile[], int filecount)
{
    long long buf[maxfiles];
    for (int i = 4; i < filecount; i++)
    {
        FILE* file = fopen(namesoffile[i], "rb");
        fseek(file, 0, SEEK_END);
        buf[i - 4] = ftell(file);
        fclose(file);
    }
    FILE* arc = fopen(nameofarc, "wb");
    for (int i = 0; i < filecount-4; i++)
    {
        fprintf(arc, "file name:%s size:%lld ", namesoffile[i + 4], buf[i]);
    }
    fprintf(arc, "\n");
    int c;
    for (int i = 4; i < filecount; i++)
    {
        FILE* file = fopen(namesoffile[i], "rb");
        while ((c = getc(file)) != EOF)
        {
            putc(c, arc);
        }
        fclose(file);
    }
    fclose(arc);
}

void extract(char nameofarc[])
{
    FILE* arc = fopen(nameofarc, "rb");
    long long start = 0;
    long long j = 0;
    int c = 0;
    while ((c = getc(arc)) != EOF)
    {
        j++;
        if (c == '\n')
        {
            start = j;
            break;
        }  
    }
    fseek(arc, 0, SEEK_SET);
    int i = 0;
    long long end;
    char nameoffile[50];
    long long filescan;
    long long sizeoffile[maxfiles];
    while (fscanf(arc, "file name:%s size:%lld ", nameoffile, &sizeoffile[i]) != 0)
    {   
        filescan = ftell(arc);
        FILE* file = fopen(nameoffile, "wb");
        if (file == NULL)
            break;
        end = start + sizeoffile[i];
        i++;
        fseek(arc, start, SEEK_SET);
        while (start != end)
        {
            c = fgetc(arc);
            fputc(c, file);
            start++;
        }
        fclose(file);
        fseek(arc, filescan, SEEK_SET);
    } 
    fclose(arc);  
}

void list(char nameofarc[])
{
    FILE* arc = fopen(nameofarc, "rb");
    char buf[100];
    printf("Archive list:\n");
    while (fgets(buf, 100, arc))
    {
        if ((buf[0] == 'f') && (buf[1] == 'i') && (buf[2] == 'l') && (buf[3] == 'e'))
            printf("%s", buf);        
    }
    fclose(arc);
}

int main(int argc, char* argv[])
{
    if (strcmp(argv[1], "--file") == 0)
        if (strcmp(argv[3], "--create") == 0)
        {
            create(argv[2], argv, argc);
            return 0;
        }
        if (strcmp(argv[3], "--extract") == 0)
        {
            extract(argv[2]);
            return 0;
        }
        if (strcmp(argv[3], "--list") == 0)
        {
            list(argv[2]);
            return 0;
        }		
}