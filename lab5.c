/*Лабораторная работ. Игра жизнь
Целью лабораторной работы является реализация игры “Жизнь” , позволяющая
выводить поколение игры в монохромную картинку в формате BMP. Плоскость
“вселенной” игры ограничена положительными координатами.
Лабораторная работы должна быть выполнена в виде консольного приложения
принимающего в качестве аргументов следующие параметры:

1. --input input_file.bmp
Где input_file.bmp - монохромная картинка в формате bmp,
хранящая начальную ситуация (первое поколение) игры

2. --output dir_name
Название директории для хранения поколений игры в виде
монохромной картинки

3. --max_iter N
Максимальное число поколений которое может эмулировать
программа. Необязательный параметр, по-умолчанию бесконечность

4. --dump_freq N
Частота с которой программа должно сохранять поколения виде
картинки. Необязательный параметр, по-умолчанию равен 1

Программа должна предусматривать исключительные ситуации, которые могут
возникать во время ее работы и корректно их обрабатывать.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define HeaderSize 62 //54+8

typedef struct bmp_struct{
    uint8_t *header;
    uint8_t *image;
    uint8_t *data;
    int file_size;
} bmp_struct;


bmp_struct create_struct(char *file_name, int* width, int* height)
{
    FILE* file = fopen(file_name,"rb");
    bmp_struct bmp;
    uint8_t *head = malloc(HeaderSize * sizeof(uint8_t));
    fread(head, 1, HeaderSize, file);
    int w = head[18] + (((uint8_t) head[19]) << 8) + (((uint8_t) head[20]) << 16) + (((uint8_t) head[21]) << 24);
    int h = head[22] + (((uint8_t) head[23]) << 8) + (((uint8_t) head[24]) << 16) + (((uint8_t) head[25]) << 24);
    int lineSize = (w / 32) * 4;
    if (w % 32 != 0) 
    {
        lineSize += 4;
    }
    int fileSize = lineSize * h;
    uint8_t *img = malloc(w * h);
    uint8_t *data = malloc(fileSize); 
    fread(data, 1, fileSize, file);
    int cur_byte;
    for(int i = 0, g = h - 1; i < h ; i++, g--) //преобразование байтов в структуру   
    {
        for(int j = 0; j < w; j++) 
        {
            cur_byte = j / 8;
            uint8_t data_byte = data[lineSize * i + cur_byte];
            uint8_t color = 0x80 >> j % 8; 
            int pos = g * w + j;
            if ((data_byte & color) != 0)
                img[pos] = 1 ;
            else    
                img[pos] = 0;
        }
    }
    *width = w; 
    *height = h;
    bmp.header = head;
    bmp.image = img;
    bmp.data = data;
    bmp.file_size = fileSize;
    fclose(file);
    return bmp;
}


void create_data(int height, int width, uint8_t **matrix, uint8_t *data)
{
    int cur_pos = 0;
    uint8_t cur_byte = 0;
    for (int i = height; i >= 1; i--) 
    {
        for (int j = 1; j <= width; j++) 
        {
            uint8_t color = 0x80 >> (j - 1) % 8;
            if (matrix[i][j] == 0) 
            {
                cur_byte = cur_byte | color;
            }
            if ((j % 8) == 0 || j == width) 
            { 
                data[cur_pos] = cur_byte;
                cur_pos++;
                cur_byte = 0;
                color = 0x80;
            }
        }
        while ((cur_pos) % 4 != 0) 
        {
            cur_pos++;
        }
    }
}


uint8_t **game_rules(int height, int width, uint8_t **cur_gen, uint8_t **new_gen)
{
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {
            if (cur_gen[i][j] == 0)
            {
                int cnt = 0;
                for (int k = -1; k <= 1; k++)
                {
                    for (int p = -1; p <= 1; p++)
                    {
                        if (cur_gen[i + k][j + p] == 1)
                            cnt++;
                    }
                }
                if (cnt == 3)
                    new_gen[i][j] = 1;
                else
                    new_gen[i][j] = 0;
            }
            else 
            {
                int cnt = -1;
                for (int k = -1; k <= 1; k++)
                {
                    for (int p = -1; p <= 1; p++)
                    {
                        if (cur_gen[i + k][j + p] == 1)
                            cnt++;
                    }
                }
                if (cnt != 2 && cnt != 3)
                    new_gen[i][j] = 0;
                else
                    new_gen[i][j] = 1;
            }
        }
    }
    printf("\n");
    return new_gen;
}


void make_all_bmp(int iter, int freq, int height, int width, uint8_t** for_cur_gen, uint8_t** for_new_gen, char* dir_name, bmp_struct bmp)
{
    uint8_t *bmp_header = bmp.header;
    uint8_t *data = bmp.data;
    int cur_iter = 0;
    int freq_cnt = 0;
    int file_cnt = 0;
    uint8_t **cur_matrix;
    while (cur_iter < iter)
    {
        if (cur_iter % 2 == 0)
        {
            game_rules(height, width, for_new_gen, for_cur_gen);
            cur_matrix = for_cur_gen;
        }
        else 
        {
            game_rules(height, width, for_cur_gen, for_new_gen);
            cur_matrix = for_new_gen;
        }
        freq_cnt++;
        if (freq_cnt == freq)
        {
            freq_cnt = 0;
            file_cnt++;
            char file_path[100] = "";
            char file_name[100] = "gen";
            int length = snprintf(NULL, 0, "%d", file_cnt);
            char str[length + 1];
            snprintf(str, length + 1, "%d", file_cnt);
            strcat(file_name, str);
            strcat(file_path, dir_name);
            strcat(file_path, "\\");
            strcat(file_path, file_name);
            strcat(file_path, ".bmp\0");
            FILE *output_file = fopen(file_path, "wb");
            printf("%s generated!\n", file_name);
            create_data(height, width, cur_matrix, data);
            fwrite(bmp_header, sizeof(uint8_t), HeaderSize, output_file);
            fwrite(data, sizeof(uint8_t), bmp.file_size, output_file);
        }
        cur_iter++;
    }
}


int main(int argc, char* argv[])
{
    char* file_name;
    char* dir_name;
    int iter, freq = 1, width, height;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i],"--input") == 0)
        {
            i++;
            file_name = argv[i];
        }
        else if(strcmp(argv[i],"--output") == 0)
        {
            i++;
            dir_name = argv[i];
        }
        else if(strcmp(argv[i],"--max_iter") == 0)
        {
            i++;
            iter = atoi(argv[i]);
        }
        else if(strcmp(argv[i],"--dump_freq") == 0)
        {
            i++;
            freq = atoi(argv[i]);
        }
        else
        {
            printf("Parameter %s don't exists!", argv[i]);
            return 1;
        }
    }
    bmp_struct bmp = create_struct(file_name, &width, &height);
    uint8_t *img = bmp.image;
    uint8_t **for_cur_gen = (uint8_t**) malloc((height + 2) * sizeof (uint8_t*));
    for (int i = 0; i < height + 2; i++)
    {
        for_cur_gen[i] = (uint8_t*) malloc((width + 2) * sizeof (uint8_t));
    }
    uint8_t **for_new_gen = (uint8_t**) malloc((height + 2) * sizeof (uint8_t*));
    for (int i = 0; i < height + 2; i++)
    {
        for_new_gen[i] = (uint8_t*) malloc((width + 2) * sizeof (uint8_t));
    }
    for(int i = 0; i < height-1; i++) 
    {
        for(int j = 0; j < width-1; j++)
        {
            if (img[i * width + j] != 0)
            {
                for_cur_gen[i][j] = 0;
                for_new_gen[i][j] = 0;
            }
            else
            {
                for_cur_gen[i][j] = 1;
                for_new_gen[i][j] = 1;
            }
        }
    }
    make_all_bmp(iter, freq, height, width, for_cur_gen, for_new_gen, dir_name, bmp);
    free(img);
    free(for_cur_gen);
    free(for_new_gen);
    return 0;
}