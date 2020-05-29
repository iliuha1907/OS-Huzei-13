#include <windows.h>
#include <iostream>
#include<vector>

CRITICAL_SECTION cs;

/*
* These are arrays of events. The first is for main from each thread that sets an event by its number.
* Second - for messages to each thread from main. An index is also a thread number.
*/
HANDLE *hOutEvent, *hMainEvent;

volatile int* arr;//this is base array
int sizeArr;
volatile bool* isOver;//through this array, each thread detects if a completion request has been sent

/*The function that threads perform*/
void thread(int num)
{
	std::vector<int> marked;//indexes of marked items are stored here
	srand(num);
	while (true)
	{
		int index = rand() % sizeArr;
		EnterCriticalSection(&cs);
		std::cout << "Hello from " << num + 1 << ", Number = " << index << std::endl;
		LeaveCriticalSection(&cs);

		if (arr[index] == 0)// if not 0, then exactly stop
		{
			EnterCriticalSection(&cs);//here several threads could theoretically pass the test, so we need one more
			if (arr[index] == 0)
			{
				Sleep(5);
				arr[index] = num + 1;
				marked.push_back(index);//adds index
				Sleep(5);
			}
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			std::cout << "Thread #" << num + 1 << ", marked " << marked.size()
				<< " elements, unable to mark at index " << index << std::endl;
			LeaveCriticalSection(&cs);

			SetEvent(hOutEvent[num]);//the thread tells main that it has stopped. It sets an event by its number
			WaitForSingleObject(hMainEvent[num], INFINITE);/// now expects a response from main by its number

			if (isOver[num])//if it was selected to stop
			{
				for (int i = 0; i < marked.size(); i++)//for all its marked indexes it puts 0
				{
					EnterCriticalSection(&cs);
					arr[marked[i]] = 0;
					LeaveCriticalSection(&cs);
				}
				return;
			}
		}
	}
}

/*The function creates events as well as threads.*/
void initThrEv(HANDLE* hThread, DWORD &IDThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		//creates a thread that will not be launched immediately
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, (LPVOID)i, CREATE_SUSPENDED, &IDThread);
		if (hThread[i] == NULL)
		{
			exit(GetLastError());
		}
		hOutEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);//with manual reset
		if (hOutEvent[i] == NULL)
		{
			exit(GetLastError());
		}
		hMainEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);//with auto reset
		if (hMainEvent[i] == NULL)
		{
			exit(GetLastError());
		}
	}
}

/*Function to launch each created thread*/
void startTHR(HANDLE* hThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		ResumeThread(hThread[i]);
	}
}

/*function to reset events*/
void resEvents(int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!isOver[i])//you need to reset the event if the thread will be able to set it (still exists)
		{
			ResetEvent(hOutEvent[i]);
			SetEvent(hMainEvent[i]);
		}
	}
}

void printArray(volatile int* arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << arr[i] << " ";
	}
	std::cout << std::endl;
}


int main()
{
	int n, countLeft = 0;
	HANDLE*	hThread;
	DWORD	IDThread;

	std::cout << "Enter number of threads" <<std:: endl;
	std::cin >> n;
	std::cout << "Enter size of array" << std::endl;
	std::cin >> sizeArr;

	InitializeCriticalSection(&cs);
	hThread = new HANDLE[n];
	hOutEvent = new HANDLE[n];
	hMainEvent = new HANDLE[n];
	isOver = new bool[n] {false};
	arr = new int[sizeArr] {0};

	initThrEv(hThread, IDThread, n);//creates events, as well as threads that will not be launched immediately

	startTHR(hThread, n);//now launch

	while (countLeft != n)//until everyone is stopped
	{
		int number;
		WaitForMultipleObjects(n, hOutEvent, TRUE, INFINITE);//waits for all events from all threads

		printArray(arr, sizeArr);
		std::cout << "Enter number of THR to stop" << std::endl;
		std::cin >> number;
		isOver[number - 1] = true;

		SetEvent(hMainEvent[number - 1]);//tells a specific thread to continue the work
		WaitForSingleObject(hThread[number - 1], INFINITE);//waiting for the completion of this thread

		printArray(arr, sizeArr);
		resEvents(n);//resets events, but only for existing threads

		countLeft++;//stopped threads counter
	}

	DeleteCriticalSection(&cs);
	delete[] hOutEvent;
	delete[] hMainEvent;
	delete[] isOver;
	delete[] hThread;

	std::cout << "Stop Machine!" << std::endl;
	system("pause");
	return 0;
}
