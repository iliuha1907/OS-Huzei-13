
#include<Windows.h>
#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include"Message.h"
#include"Employee.h"
#include"Parameter.h"

using namespace std;
CRITICAL_SECTION csOutput, csReading;
bool *isWriting, *isReading;
string fileName;

void createFile(string name, int numberOfRec, map<int,int> &index)
{
	ofstream file(name, ios::binary);
	for (int i = 0; i < numberOfRec; i++)
	{
		isReading[i] = false;
		isWriting[i] = false;
		cout << "Enter data for the " << i + 1 << " one " << endl;
		employee emp;
		
		cout << "Enter id" << endl;
		cin >> emp.num;
		cout << "Enter name" << endl;
		cin >> emp.name;
		cout << "Enter hours" << endl;
		cin >> emp.hours;
		file.write((char*)&emp, sizeof(employee));
		
		index[emp.num] = i;
	}
	file.close();
}

void startTHR(HANDLE* hThread, int n)
{
	for (int i = 0; i < n; i++)
	{
		ResumeThread(hThread[i]);
	}
}

void printFile(string name)
{
	ifstream fin(name, ios::binary);
	employee emp;
	while (fin.read((char*)&emp, sizeof(employee))) {
		cout << "Id: " << emp.num << ", Name: " << emp.name << ", hours: " << emp.hours << endl;
	}
	fin.close();
}

employee readEmpl(string name, int pos)
{
	employee emp;
	ifstream fin(name, ios::binary);
	fin.seekg(pos * sizeof(employee));
	fin.read((char*)&emp, sizeof(employee));
	fin.close();
	/*printFile(name);
	ifstream fin(name, ios::binary);
	employee emp;
	while (fin.read((char*)&emp, sizeof(employee))) {
		cout << "Id: " << emp.num << ", Name: " << emp.name << ", hours: " << emp.hours << endl;
	}
	fin.close();*/
	return emp;
}

void writeEmpl(employee emp, string name, int pos)
{
	ofstream fout(name, ios::binary || ios::app);
	fout.seekp(pos * sizeof(employee));
	fout.write((char*)&emp, sizeof(employee));
	fout.close();
}

DWORD  WINAPI work(void* param)
{
	parameter p = *(((parameter*)param));
	cout << "Started working thr " << (p).number << endl;
	cout << "Handle: " << (p).pipe << endl;
	EnterCriticalSection(&csOutput);
	ConnectNamedPipe((p).pipe, NULL);
	LeaveCriticalSection(&csOutput);
	
	cout << "Continuied working thr " << (p).number << endl;
	cout << "Handle: " << (p).pipe << endl;
	message msg;
	BOOL canRead;
	map<int, int>* data= (p).index;
	
	while (true)
	{
		
		canRead = ReadFile((p).pipe, &msg, sizeof(message), NULL, NULL);
		if (!canRead)
		{
			EnterCriticalSection(&csOutput);
			cout << "Error in reading" << endl;
			LeaveCriticalSection(&csOutput);
			exit(0);
		}
		cout << "Got message " << (p).number << endl;
		int position = (*data)[msg.id];

		BOOL canWrite;
		switch (msg.FLAG)
		{
		case 1:
		
			if (isWriting[position])
			{
				msg.data[0] = "Occupated!";
				canWrite = WriteFile((p).pipe, &msg, sizeof(message), NULL, NULL);
				if (!canWrite)
				{
					EnterCriticalSection(&csOutput);
					cout << "Error in writing" << endl;
					LeaveCriticalSection(&csOutput);
					exit(0);
				}
			}
			else
			{
				EnterCriticalSection(&csReading);
				if (isWriting[position])
				{
					continue;
				}
				if (!isReading[position])
				{
					isReading[position] = true;
				}
				else
				{
					continue;
				}
				LeaveCriticalSection(&csReading);
				
				employee emp = readEmpl(fileName,position);
				string num = to_string(emp.num);
				string hours = to_string(emp.hours);
				msg.data[0] = num;
				msg.data[1] = emp.name;
				msg.data[2] = hours;
				cout << "Writing back " << (p).number << endl;
				canWrite = WriteFile((p).pipe, &msg, sizeof(message), NULL, NULL);
				if (!canWrite)
				{
					EnterCriticalSection(&csOutput);
					cout << "Error in writing" << endl;
					LeaveCriticalSection(&csOutput);
					exit(0);
				}
				isReading[position] = false;
				//LeaveCriticalSection(&csOutput);
			}
			break;
		case 2:
			if (isReading[position] || isWriting[position])
			{
				msg.data[0] = "Occupated!";
				canWrite = WriteFile((p).pipe, &msg, sizeof(message), NULL, NULL);
				if (!canWrite)
				{
					EnterCriticalSection(&csOutput);
					cout << "Error in writing" << endl;
					LeaveCriticalSection(&csOutput);
					exit(0);
				}
			}
			else
			{

				EnterCriticalSection(&csOutput);
				if (isReading[position])
				{
					continue;
				}
				if (!isWriting[position])
				{
					isWriting[position] = true;
				}
				else
				{
					continue;
				}
				LeaveCriticalSection(&csOutput);
				employee emp = readEmpl(fileName, position);
				string num = to_string(emp.num);
				string hours = to_string(emp.hours);
				msg.data[0] = num;
				msg.data[1] = emp.name;
				msg.data[2] = hours;
				canWrite = WriteFile((p).pipe, &msg, sizeof(message), NULL, NULL);
				if (!canWrite)
				{
					EnterCriticalSection(&csOutput);
					cout << "Error in writing" << endl;
					LeaveCriticalSection(&csOutput);
					exit(0);
				}

				canRead = ReadFile((p).pipe, &msg, sizeof(message), NULL, NULL);
				if (!canRead)
				{
					EnterCriticalSection(&csOutput);
					cout << "Error in reading" << endl;
					LeaveCriticalSection(&csOutput);
					exit(0);
				}
				emp.num = atoi(msg.data[0].c_str());
				strcpy_s(emp.name, msg.data[1].c_str());
				emp.hours = atof(msg.data[2].c_str());
				writeEmpl(emp, fileName, position);
				isWriting[position] = false;
				
			}

			break;
		default:
			exit(0);
		}
	}
}

bool createProc(int id, string path)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL canCreate;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	canCreate = CreateProcess(path.c_str(), (char*)(to_string(id).c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL,
		NULL, &si, &pi);
	if (!canCreate)
	{
		return false;
	}
	return true;
}




int main()
{
	int numberOfRec, numberOfProc;
	
	string  pipeName = "\\\\.\\pipe\\AlphaBravo";
	string pathProc = "C:\\Users\\HP\\source\\repos\\Client\\Debug\\Client.exe";
	map<int, int> index;
	cout << "Enter name of the file" << endl;
	getline(cin, fileName);
	cout << "Enter number of records" << endl;
	cin >> numberOfRec;
	cout << "Enter number of processes" << endl;
	cin >> numberOfProc;
	isReading = new bool[numberOfProc];
	isWriting = new bool[numberOfProc];
	createFile(fileName, numberOfRec, index);
	printFile(fileName);

	
	HANDLE* threads = new HANDLE[numberOfProc];
	HANDLE* pipes = new HANDLE[numberOfProc];
	
	InitializeCriticalSection(&csOutput);
	InitializeCriticalSection(&csReading);
	for (int i = 0; i < numberOfProc; i++)
	{
		parameter p;
		p.index = &index;
		p.number = i;
		p.fileName = fileName;
		pipes[i] = CreateNamedPipe((pipeName + to_string(i)).c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			1,
			sizeof(message),
			sizeof(message),
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
		p.pipe = pipes[i];
		threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)work, (void*)&p, NULL, NULL);
		createProc(i, pathProc);	
	}

	//startTHR(threads, numberOfProc);

	WaitForMultipleObjects(numberOfProc, threads, TRUE, INFINITE);
	printFile(fileName);

	system("pause");
	return 0;
}