/*Задача
Реализовать пользовательский тип для целого беззнакового числа
фиксированной длины uint1024_t
Для вышеуказанного типа реализовать функции с следующими
сигнатурами:
1. uint1024_t from_uint(unsigned int x) - генерация из числа
2. uint1024_t add_op(uint1024_t x, uint1024_t y) - сложение
3. uint1024_t subtr_op(uint1024_t x, uint1024_t y) - вычетание
4. uint1024_t mult_op(uint1024_t x, uint1024_t y) - умножение
5. void printf_value(uint1024_t x) - вывод в стандартный поток
вывода
6. void scanf_value(uint1024_t* x) - чтение из стандартного потока
ввода
Примечание:
1. Переполнение - Undefined Behavior
2. При реализации думать об оптимальном использовании памяти
3. Реализовать программу демонстрирующую работоспособность
вышеуказанных функций*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int base = 1000000000;

typedef struct {
    int *numbers;
    int length;
} uint1024_t;

uint1024_t init() 
{
    uint1024_t number;
    number.length = 0;
    number.numbers = NULL;
    return number;
}

uint1024_t init_with_length(int length) 
{
    uint1024_t number;
    number.length = length;
    number.numbers = calloc(length, sizeof(int));
    return number;
}

uint1024_t from_uint(unsigned int x) 
{
    int length = x >= base ? 2 : 1;
    uint1024_t number = init_with_length(length);
    number.numbers[0] = (int) (x % base);
    if (length > 1)
        number.numbers[1] = (int) (x / base);
    return number;
}

void push_number(uint1024_t *number, int x) 
{
    number->length++;
    if (realloc(number->numbers, number->length * sizeof(int)) == NULL) 
    {
        printf("Error: Memory out");
    }
    number->numbers[number->length - 1] = x;
}

void delete_zeros(uint1024_t *x) 
{
    if (x->length == 0)
        return;
    int prevSize = x->length;
    while (x->length-1 > 0 && x->numbers[x->length-1] == 0)
    {
        x->length--;
    }
    if (prevSize == x->length)
        return;
    if (realloc(x->numbers, x->length * sizeof(int)) == NULL) 
    {
        printf("Error: Memory out");
    }
}

void scanf_value(uint1024_t *number) 
{
    char newNumber[310];
    scanf("%309s", newNumber);
    int x = 0, size = strlen(newNumber);
    int numberCount = size % 9 == 0 ? size/9 : (size/9)+1;
    *number = init_with_length(numberCount);
    int numberLength = 0;
    for (int i = size-9; i >= 0; i-=9) 
    {
        int j = i;
        while (j-i < 9) 
        {
            x = x*10 + newNumber[j]-'0';
            j++;
        }
        number->numbers[numberLength++] = x;
        x = 0;
    }
    for (int i = 0; i < size-numberLength*9; ++i)
        x = x*10 + newNumber[i]-'0';
    if (numberLength < numberCount)
        number->numbers[numberCount-1] = x;
}

void printf_uint1024t(uint1024_t number) 
{
    if (number.numbers == NULL || number.length == 0) 
    {
        printf("Error. Can't print your number");
        return;
    }
    printf("%u", number.numbers[number.length-1]);
    for (int i = number.length - 2; i >= 0; --i)
        printf("%09u", number.numbers[i]);
    printf("\n");
}

uint1024_t add_op(uint1024_t x, uint1024_t y) 
{
    if (x.length < y.length) {
        uint1024_t temp = x;
        x = y;
        y = temp;
    }
    int remains = 0;
    uint1024_t result = init_with_length(x.length);
    for (int i = 0; i < x.length; ++i) 
    {
        result.numbers[i] = remains + x.numbers[i] + (i < y.length ? y.numbers[i] : 0);
        remains = 0;
        if (result.numbers[i] >= base) 
        {
            remains++;
            result.numbers[i] -= base;
        }
    }
    if (remains > 0)
        push_number(&result, remains);
    return result;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) 
{
    int max = x.length > y.length ? x.length : y.length;
    uint1024_t result = init_with_length(max);
    int remains = 0;
    for (int i = 0; i < max; ++i) 
    {
        result.numbers[i] = (i < x.length ? x.numbers[i] : 0) - remains - (i < y.length ? y.numbers[i] : 0);
        remains = 0;
        if (result.numbers[i] < 0) 
        {
            remains++;
            result.numbers[i] += base;
        }
    }
    delete_zeros(&result);
    return result;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y) 
{
    int length = x.length + y.length;
    uint1024_t result = init_with_length(length);
    for (int i = 0; i < x.length; i++) 
    {
        int remains = 0, lastJ = 0;
        for (int j = 0; j < y.length; j++) 
        {
            long long multi = (long long) x.numbers[i] * (long long) (j < y.length ? y.numbers[j] : 0);
            long long num = result.numbers[i + j] + multi + remains;
            result.numbers[i + j] = (int) (num % base);
            remains = (int) (num / base);
            lastJ = j+1;
        }
        if (remains > 0)
            result.numbers[i + lastJ] += remains;
    }
    delete_zeros(&result);
    return result;
}

int main() 
{
    uint1024_t u1 = init();
    uint1024_t u2 = init();
    printf("First number: ");
    scanf_value(&u1);
    printf("Second number: ");
    scanf_value(&u2);
    uint1024_t add = add_op(u1, u2);
    printf("Sum:");
    printf_uint1024t(add);
    uint1024_t sub = subtr_op(u1, u2);
    printf("Subtr:");
    printf_uint1024t(sub);
    uint1024_t mult = mult_op(u1, u2);
    printf("Mult:");
    printf_uint1024t(mult);
    return 0;
}