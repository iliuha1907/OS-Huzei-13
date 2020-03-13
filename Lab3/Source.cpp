#include <windows.h>
#include <iostream>
#include <string.h>
#include <process.h>
#include"ArrayWork.h"

int sizeArr;
volatile int indMin, indMax;
volatile int aver;
CRITICAL_SECTION crSec;

//searches min and max
void minMax(int* arr)
{
	int min, max;
	if (arr[0] < arr[1]) 
	{
		min = arr[0];
		max = arr[1];
		indMin = 0;
		indMax = 1;
	}
	else
	{
		min = arr[1];
		max = arr[0];
		indMin = 1;
		indMax = 0;
	}

	for (int i = 2; i < sizeArr; i++)
	{
		if (arr[i] < min) 
		{
			min = arr[i];
			indMin = i;
		}
		else if (arr[i] > max)
		{
			max = arr[i];
			indMax = i;
		}
		Sleep(7);
	}
	EnterCriticalSection(&crSec);
	std::cout << "Min = " << min << " Max = " << max << std::endl;
	LeaveCriticalSection(&crSec);
}

//searches average value 
void average(int* arr)
{
	aver = 0;
	for (int i = 0; i < sizeArr; i++)
	{
		aver += arr[i];
		Sleep(12);
	}
	aver /= sizeArr;
	EnterCriticalSection(&crSec);
	std::cout << "Average value = " << aver << std::endl;
	LeaveCriticalSection(&crSec);
}

//changes min and max to average value
void changeArr(int* arr)
{
	arr[indMin] = aver;
	arr[indMax] = aver;
}

/*gets handle to which writes appropriate handle; func, which is function for thread; lParam, which is parameter for func; 
*id to which writes appropriate id
*/
void createTHR(HANDLE &hTHR, LPTHREAD_START_ROUTINE func, LPVOID lParam, DWORD &id)
{
	hTHR = CreateThread(NULL, 0, func, lParam, 0, &id);
	if (hTHR == NULL)
	{
		std::cout << "Error" << std::endl;
		exit(GetLastError());
	}
}


int main()
{
	HANDLE hThreadMinMax, hThreadAver;
	DWORD IDThreadMinMax, IDThreadAver;
	
	int* arr = createArray(sizeArr);
	fillArray(arr, sizeArr);

	InitializeCriticalSection(&crSec);
	createTHR(hThreadMinMax, (LPTHREAD_START_ROUTINE)minMax, arr, IDThreadMinMax);
	createTHR(hThreadAver, (LPTHREAD_START_ROUTINE)average, arr, IDThreadAver);

	WaitForSingleObject(hThreadMinMax, INFINITE);
	WaitForSingleObject(hThreadAver, INFINITE);

	changeArr(arr);
	printArr(arr, sizeArr);

	DeleteCriticalSection(&crSec);
	CloseHandle(hThreadMinMax);
	CloseHandle(hThreadAver);
	freeArray(arr);

	system("pause");
	return 0;
}
