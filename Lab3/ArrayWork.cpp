#include<iostream>

#include "ArrayWork.h"

/*
Functions to work with array
*/

//gets variable to which it writes size and returns created array
int* createArray(int &size)
{
	std::cout << "Enter size" << std::endl;
	std::cin >> size;
	int* arr = new int[size];
	return arr;
}

//gets array and size of it to fill with numbers
void fillArray(int* arr, int size)
{
	std::cout << "Enter elements" << std::endl;
	for (int i = 0; i < size; i++)
	{
		std::cin >> arr[i];
	}
}

//gets array and size of it to print ot out
void printArr(int* arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << arr[i] << " ";
	}
	std::cout << std::endl;
}

//gets array and frees up memory for it
void freeArray(int* arr)
{
	delete[]arr;
}
