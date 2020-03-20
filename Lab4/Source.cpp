#include <windows.h>
#include <iostream>
#include<vector>

using namespace std;

CRITICAL_SECTION cs;

/*
*��� ������� �������. ������ - ��� main �� ������� �����, ������� �� ������ ������ ������ �������.
*������ - ��� ��������� ������� ����� �� main. ������ - ��� ����� ����� �����.
*/
HANDLE *hOutEvent, *hMainEvent;

volatile int* arr;//��� �������� ������
int sizeArr;
volatile bool* isOver;//����� ���� ������ ������ ���� �������, �� ��������� �� ������ �� ����������

/*�������, ������� ��������� �����*/
void thread(int num)
{
	vector<int> marked;//����� �������� ������� ���������� ���������
	srand(num);
	while(true)
	{
		int index = rand() % sizeArr;
		EnterCriticalSection(&cs);
		cout << "Hello from " << num + 1 << ", Number = " << index << endl;
		LeaveCriticalSection(&cs);
		
		if (arr[index] == 0)//���� �� 0, �� ����� �� ����������
		{
			EnterCriticalSection(&cs);//����� ��������� ������ ������������ ����� ������ ��������, ������� ����� ��� ����
			if (arr[index] == 0)
			{
				Sleep(5);
				arr[index] = num + 1;
				marked.push_back(index);//��������� ������
				Sleep(5);
			}
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			cout << "Thread #" << num + 1 << ", marked " << marked.size() << " elements, unable to mark at index " << index << endl;
			LeaveCriticalSection(&cs);

			SetEvent(hOutEvent[num]);//���� ������� main, ��� �� �����������. �� ������ ������ ���������� �������
			WaitForSingleObject(hMainEvent[num], INFINITE);//������ �� ������ ������ ������� ����� �� main

			if (isOver[num])//���� �� ��� ������ ��� ����������
			{
				for (int i = 0; i < marked.size(); i++)//�� ���� ����� ���������� �������� ������ 0
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

/*������� ������� �������, � ����� �����*/
void initThrEv(HANDLE* hThread, DWORD &IDThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		//������� ����, ������� ����� ������� �� �����
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, (LPVOID)i, CREATE_SUSPENDED, &IDThread);
		if (hThread[i] == NULL)
		{
			exit(GetLastError());
		}
		hOutEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);//� ������ �������
		if (hOutEvent[i] == NULL)
		{
			exit(GetLastError());
		}
		hMainEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);//� ���� �������
		if (hMainEvent[i] == NULL)
		{
			exit(GetLastError());
		}
	}
}

/*������� ��� ������� ������� ���������� �����*/
void startTHR(HANDLE* hThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		ResumeThread(hThread[i]);
	}
}

/*������� ��� ������ �������*/
void resEvents(int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!isOver[i])//����� �������� �������, ���� ���� ����� � ��������� ��� ��������� (��� ����������)
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

	initThrEv(hThread,IDThread,n);//������� �������, � ����� �����, ������� �� ����� �������� �����
	
	startTHR(hThread, n);//��� ������ ������

	while (countLeft !=n)//���� �� ����������� ���
	{
		int number;
		WaitForMultipleObjects(n, hOutEvent, TRUE, INFINITE);//���� ��� ������� �� ���� ������

		printArray(arr, sizeArr);
		cout << "Enter number of THR to stop" << endl;
		cin >> number;
		isOver[number-1] = true;

		SetEvent(hMainEvent[number-1]);//������� ����������� ����� ���������� ������
		WaitForSingleObject(hThread[number-1], INFINITE);//���� ���������� ����� �����

		printArray(arr, sizeArr);
		resEvents(n);//���������� �������, �� ������ ��� ��� ������������ ������

		countLeft++;//������� ������������� ������
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