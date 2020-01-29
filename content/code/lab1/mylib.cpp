#include <iostream>

#include "mylib.hpp"

//считываем число
int read_number()
{
	int number;
	std::cin >> number;
	return number;
}

//получаем простые делители числа
// сохраняем их в массив, чей адрес нам передан
void factorize(int x, int *Divisor, int *Divisor_top)
{
	for (int d = 2; d <= x; d++) {
		while (x%d == 0) {
			Divisor[(*Divisor_top)++] = d;
			x /= d;
		}
	}
}

//выводим число
void print_number(int number)
{
	std::cout << number << std::endl;
}

//распечатывает массив размера A_size в одной строке через TAB
void print_array(int A[], size_t A_size)
{
	for(int i = A_size-1; i >= 0; i--)
	{
		std::cout << A[i] << '\t';
	}
	std::cout << std::endl;
}