/*
$remote_addr - - [$local_time] “$request” $status $bytes_send

$remote_addr - источник запроса
$local_time - время запроса
$request - запрос
$status - статус ответ
$bytes_send - количество переданных в ответе байт

Например:
198.112.92.15 - - [03/Jul/1995:10:50:02 -0400] "GET /shuttle/countdown/
HTTP/1.0" 200 3985

Требуется
1. Подготовить список запросов, которые закончились 5xx ошибкой, с
количеством неудачных запросов
2. Найти временное окно (длительностью параметризуются), когда
количество запросов на сервер было максимально

Примечание:
1. Для парсинга строк проще всего воспользоваться библиотеками
stdio.h и string.h стандартной библиотеки
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


typedef struct log{
    char remote_addr[32];
    char local_time[32];
    char request[340];
    int status;
    char bytes_send[16];
    time_t date;
} log;

typedef struct Node{
    time_t value;
    struct Node *next;
} Node;

typedef struct{
    Node *head;
    Node *tail;
} queue;

log log_parse(char *s)
{
    log l;
    sscanf(s, "%s - - [%[^]]] \"%[^\"]\" %d %s", l.remote_addr, l.local_time,
           l.request, &l.status, l.bytes_send);
    int i = 0;
    char month[4];
    char months[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug", "Sep", "Oct", "Nov", "Dec"};
    struct tm time;
    time.tm_isdst = -1;
    sscanf(l.local_time, "%d/%3s/%d:%d:%d:%d", &time.tm_mday, month, &time.tm_year, &time.tm_hour, &time.tm_min, &time.tm_sec);
    time.tm_year -= 1900;
    for(i = 0; i<=11; i++)
    {
        if(strcmp(months[i], month) == 0)
        {
            time.tm_mon = i;
        } 
    }
    l.date = mktime(&time);
    return l;
}

int error(char file_name[])
{
    FILE* file = fopen(file_name, "r"); 
    log str_log;
    char str[2048];
    long long count = 0;
    while (fgets(str, 2048, file)) 
    {
        str_log = log_parse(str);
        if (str_log.status / 100 == 5)
        {
            printf("%s",str);
            count++;
        }
    }
    fclose(file);
    return count;
}

void convert_time(time_t x, char* time_str)
{
    struct tm *tmp = localtime(&x);
    strftime(time_str, 21, "%d/%b/%Y:%H:%M:%S", tmp);
}

void push(queue **q, time_t x)
{
    Node *temp;
    temp = (Node*)malloc(sizeof(Node));
    temp->value = x;
    temp->next = NULL;
    if (((*q)->head) == NULL) {
        *q = (queue*)malloc(sizeof(queue));
        (*q)->head = (*q)->tail = temp;
    }
    else {
        (*q)->tail->next = temp;
        (*q)->tail = (*q)->tail->next;
    }
}

void pop(queue **q)
{
    if ((*q)->head == (*q)->tail)
        *q = NULL;
    else
        (*q)->head = (*q)->head->next;
}

void window_search(char file_name[], char window_time[])
{
    FILE* file = fopen(file_name, "r"); 
    log str_log;
    queue *q = malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    char str[2048];
    char temp[22];
    long long count = 0;
    long long window_time_ll = strtoll(window_time, NULL, 10);
    time_t start_win;
    time_t end_win;
    int window_count = 0;
    while (fgets(str, 2048, file)) 
    {
        str_log = log_parse(str);
        if (q->head != NULL && q->tail != NULL) 
        {
            while (((long long) (difftime(q->tail->value, q->head->value)) > window_time_ll) && (count > 0))
            {
                pop(&q);
                count--;
            }
        }
        if (count > window_count) 
        {
            window_count = count;
            start_win = q->head->value;
            end_win = q->tail->value;
        }
        push(&q, str_log.date);
        count++;
    }
    printf("Count of requests:%d\n", window_count);
    convert_time(start_win,temp);
    printf("From:%s\n", temp);
    convert_time(end_win, temp);
    printf("To:%s\n", temp);
    fclose(file);
    free(q);
}

int main(int argc, char* argv[])
{
    long long errors_count;
    if (strcmp(argv[2],"-err") == 0)
    {
        errors_count = error(argv[1]);
        printf("Count of errors: %lld \n",errors_count);
    }
    if (strcmp(argv[2],"-win") == 0)
    {
        window_search(argv[1], argv[3]); 
    }
    return 0;
}