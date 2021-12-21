/*Задача.
Реализовать редактор текстовой метаинформации mp3 файла.
В качестве стандарта метаинформации принимаем ID3v2.
Редактор представлять из себя консольную программу
принимающую в качестве аргументов имя файла через параметра
--filepath , а также одну из выбранных команд
1. --show - отображение всей метаинформации в виде таблицы
2. --set=prop_name --value=prop_value - выставляет значение
определенного поля метаинформации с именем prop_name в
значение prop_value
3. --get=prop_name - вывести определенное поле
метаинформации с именем prop_name

Например:
app.exe --filepath=Song.mp3 --show
app.exe --filepath=Song.mp3 --get=TIT2
app.exe --filepath=Song.mp3 --set=COMM --value=Test

Примечание.

При выполнения данной работы разрешается
использовать только стандартную библиотеку языка С.
Исключением может являться процесс разбора аргументов
командной строки.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>

typedef struct{
    char action[20];
    char name[40];
}info;

typedef struct{
    uint8_t ID3[3];
    uint8_t Version;
    uint8_t Subversion;
    uint8_t Flag;
    uint8_t Size[4];
} ID3v2_header;

typedef struct{
    char Frame_type[4];
    uint8_t Size[4];
    uint8_t Flag[2];
} ID3v2_frame;

info parse(char str[])
{
    info data;
    char sign[2] = "=";
    char temp1[20], temp2[40];
    char* index = strtok(str,sign);
    strcpy(data.action, index);
    while (index != NULL)
    {
        strcpy(data.name, index);
        index = strtok(NULL,sign);
    }
    //printf("%s\n", data.action);
    //printf("%s\n", data.name);
    return data;
}

int btoi(uint8_t ch[4], int is_header, int version) 
{
    if (version == 4 || is_header)
        return (int)ch[0] << 21 | (int)ch[1] << 14 | (int)ch[2] << 7 | (int)ch[3] << 0;
    return (int)ch[0] << 24 | (int)ch[1] << 16 | (int)ch[2] << 8 | (int)ch[3] << 0;
}

void itob(int x, uint8_t* ch, int version)
{
    int bits = 127;
    if (version == 4) {
        ch[3] = x & bits;
        ch[2] = (x >> 7) & bits;
        ch[1] = (x >> 14) & bits;
        ch[0] = (x >> 21) & bits;
    }
    else {
        ch[3] = x & bits;
        ch[2] = (x >> 8) & bits;
        ch[1] = (x >> 16) & bits;
        ch[0] = (x >> 24) & bits;
    }
}

void show(char* f)
{
    FILE* file = fopen(f,"rb");
    if (file != NULL)
    {
        int unicode;
        ID3v2_header header;
        ID3v2_frame frame;
        fread(&header, 1, 10, file);
        int fullsize = btoi(header.Size, 1, header.Version);
        printf("size: %d\n", fullsize);
        while (fread(&frame, 1, 10, file)) 
        {
            if (frame.Frame_type[0] == 0 || ftell(file) >= fullsize) 
                break; 
            unicode = fgetc(file);
            int framesize = btoi(frame.Size, 0, header.Version); 
            uint8_t* str = (uint8_t*)calloc(framesize,1);
            fread(str, 1, framesize-1, file);
            printf("tag: %-7s size: %-7d value: ", frame.Frame_type, framesize); 
            if (unicode) 
                wprintf(L"%ls\n", str+2); 
            else 
                printf("%s\n", str);
            free(str); 
        }
        fclose(file);
    }
}

void show_frame(char* f, char* tag)
{
    FILE* file = fopen(f,"rb");
    if (file != NULL)
    {
        int unicode, i = 0;
        ID3v2_header header;
        ID3v2_frame frame;
        fread(&header, 1, 10, file);
        int fullsize = btoi(header.Size, 1, header.Version);
        while (fread(&frame, 1, 10, file)) 
        {
            if (ftell(file) >= fullsize) 
            {
                printf("This tag doesn't exist!\n");
                break;
            }   
            unicode = fgetc(file);
            int framesize = btoi(frame.Size, 0, header.Version); 
            uint8_t* str = (uint8_t*)calloc(framesize,1);
            fread(str, 1, framesize-1, file);
            if (strcmp(frame.Frame_type, tag) == 0)
            {
                printf("tag: %-7s size: %-7d value: ", frame.Frame_type, framesize); 
                if (unicode) 
                    wprintf(L"%ls\n", str+2); 
                else 
                    printf("%s\n", str);
                break;
            }
            free(str); 
        }          
    }
    fclose(file);
}

void change_tag(char* f, char* tag, char* value)
{
    FILE* file = fopen(f,"rb");
    FILE* file2 = fopen("test7.mp3","wb");
    if (file != NULL)
    {
        int unicode, i = 0, framesize, framesize2, len = strlen(value), filesize , c;
        ID3v2_header header, header2;
        ID3v2_frame frame, frame2;
        fread(&header, 1, 10, file);
        fwrite(&header, 1, 10, file2);
        int fullsize = btoi(header.Size, 1, header.Version);
        while(fread(&frame, 1, 10, file)) 
        {
            if (ftell(file) >= fullsize) 
                break;
            unicode = fgetc(file);
            if(strcmp(frame.Frame_type, tag) == 0)
            {
                framesize = btoi(frame.Size, 0, header.Version);
                fwrite(&frame, 1, 10, file2);
                fputc(unicode,file2);
                fwrite(value,1,len,file2);
                fseek(file,len, SEEK_CUR);
                i = 0;
            }
            else
            {
                fwrite(&frame, 1, 10, file2);
                fputc(unicode,file2);
                framesize = btoi(frame.Size, 0, header.Version);
                while (i < framesize-1)
                {
                    c = fgetc(file);
                    fputc(c,file2);
                    i++;
                }
                i = 0;
            }  
        }
        while(!feof(file))
        {
            c = fgetc(file);
            fputc(c,file2);
        } 
            
    }
    fclose(file);
    fclose(file2);
}
int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    info data = parse(argv[1]);
    if(strcmp(data.action,"--filepath") == 0)
    {
        if(strcmp(argv[2],"--show") == 0)
        {
            show(data.name);
        }
        else
        {
            info data2 = parse(argv[2]);
            if(strcmp(data2.action,"--get") == 0)
            {
                show_frame(data.name, data2.name);
            } 
            if(strcmp(data2.action,"--set") == 0)
            {
                info data3 = parse(argv[3]);
                if(strcmp(data3.action,"--value") == 0)
                {
                    change_tag(data.name, data2.name, data3.name);
                } 
            } 
        }
    }
    return 0;
}