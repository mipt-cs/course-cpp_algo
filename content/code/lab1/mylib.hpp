#ifndef MY_LIBRARY_H_INCLUDED
#define MY_LIBRARY_H_INCLUDED

#include <cstdlib>

//считываем число
int read_number();

//получаем простые делители числа
// сохраняем их в массив, чей адрес нам передан
void factorize(int number, int *Divisor, int *Divisor_top);

//выводим число
void print_number(int number);

//распечатывает массив размера A_size в одной строке через TAB
void print_array(int A[], size_t A_size);

#endif // MY_LIBRARY_H_INCLUDED