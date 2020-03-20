#include <windows.h>
#include <iostream>
#include<vector>

using namespace std;

CRITICAL_SECTION cs;

/*
*Это массивы событий. Первый - для main от каждого треда, который по своему номеру ставит событие.
*Второй - для сообщений каждому треду от main. Индекс - это также номер треда.
*/
HANDLE *hOutEvent, *hMainEvent;

volatile int* arr;//это основной массив
int sizeArr;
volatile bool* isOver;//через этот массив каждый тред смотрит, не отправлен ли запрос на завершение

/*Функция, которую выполняют треды*/
void thread(int num)
{
	vector<int> marked;//здесь хранятся индексы помеченных элементов
	srand(num);
	while(true)
	{
		int index = rand() % sizeArr;
		EnterCriticalSection(&cs);
		cout << "Hello from " << num + 1 << ", Number = " << index << endl;
		LeaveCriticalSection(&cs);
		
		if (arr[index] == 0)//если не 0, то точно на завершение
		{
			EnterCriticalSection(&cs);//здесь несколько тредов теоритически могли пройти проверку, поэтому нужна еще одна
			if (arr[index] == 0)
			{
				Sleep(5);
				arr[index] = num + 1;
				marked.push_back(index);//добавляем индекс
				Sleep(5);
			}
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			cout << "Thread #" << num + 1 << ", marked " << marked.size() << " elements, unable to mark at index " << index << endl;
			LeaveCriticalSection(&cs);

			SetEvent(hOutEvent[num]);//тред говорит main, что он остановился. По своему номеру выставляет событие
			WaitForSingleObject(hMainEvent[num], INFINITE);//теперь по своему номеру ожидает ответ от main

			if (isOver[num])//если он был выбран для завершения
			{
				for (int i = 0; i < marked.size(); i++)//по всем своим помеченным индексам ставит 0
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

/*Функция создает события, а также треды*/
void initThrEv(HANDLE* hThread, DWORD &IDThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		//создаем тред, который сразу запущен не будет
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, (LPVOID)i, CREATE_SUSPENDED, &IDThread);
		if (hThread[i] == NULL)
		{
			exit(GetLastError());
		}
		hOutEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);//с ручным сбросом
		if (hOutEvent[i] == NULL)
		{
			exit(GetLastError());
		}
		hMainEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);//с авто сбросом
		if (hMainEvent[i] == NULL)
		{
			exit(GetLastError());
		}
	}
}

/*Функция для запуска каждого созданного треда*/
void startTHR(HANDLE* hThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		ResumeThread(hThread[i]);
	}
}

/*функция для сброса событий*/
void resEvents(int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!isOver[i])//нужно сбросить событие, если тред будет в состоянии его выставить (еще существует)
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
		cout << arr[i] << " ";
	}
	cout << endl;
}


int main()
{
	int n, countLeft = 0;
	HANDLE*	hThread;
	DWORD	IDThread;

	cout << "Enter number of threads" << endl;
	cin >> n;
	cout << "Enter size of array" << endl;
	cin >> sizeArr;

	InitializeCriticalSection(&cs);
	hThread = new HANDLE[n];
	hOutEvent = new HANDLE[n];
	hMainEvent = new HANDLE[n];
	isOver = new bool[n] {false};
    arr = new int[sizeArr] {0};

	initThrEv(hThread,IDThread,n);//создает события, а также треды, которые не будут запущены сразу
	
	startTHR(hThread, n);//вот теперь запуск

	while (countLeft !=n)//пока не остановлены все
	{
		int number;
		WaitForMultipleObjects(n, hOutEvent, TRUE, INFINITE);//ждет все события от всех тредов

		printArray(arr, sizeArr);
		cout << "Enter number of THR to stop" << endl;
		cin >> number;
		isOver[number-1] = true;

		SetEvent(hMainEvent[number-1]);//говорит конкретному треду продолжить работу
		WaitForSingleObject(hThread[number-1], INFINITE);//ждет завершение этого треда

		printArray(arr, sizeArr);
		resEvents(n);//сбрасывает события, но только для еще существующих тредов

		countLeft++;//счетчик остановленных тредов
	}
	
	DeleteCriticalSection(&cs);
	delete[] hOutEvent;
	delete[] hMainEvent;
	delete[] isOver;
	delete[] hThread;

	cout << "Stop Machine!" << endl;
	system("pause");
	return 0;
}