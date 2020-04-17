#include <windows.h>
#include <iostream>
#include<fstream>
#include<string>
using namespace std;

void setEvents(HANDLE* events, int numbProc)
{
	for (int i = 0; i < numbProc; i++)
	{
		SetEvent(events[i]);
	}
}

void cleanFile(string fileName)
{
	ofstream cleaner(fileName, ios::out | ios::trunc);
	cleaner.close();
}

bool read(string fileName)
{
	bool hasRead = false;
	string text;
	ifstream fin(fileName);
	while (fin >> text)
	{
		if (text != "")
		{
			hasRead = true;
			cout << "MAIN Process: " << text << endl;
		}
	}
	fin.close();
	return hasRead;
}

void createFile(string fileName)
{
	ifstream fin(fileName);
	fin.close();
}

void initHandles(HANDLE* eventsStart, PROCESS_INFORMATION* piInfos, STARTUPINFO	&si , int numbProc, string buf)
{
	for (int i = 0; i < numbProc; i++)
	{
		if (!CreateProcess("C:\\Users\\HP\\source\\repos\\SenderADV\\Debug\\SenderADV.exe", (LPSTR)((buf + " " + to_string(i)).c_str()),
			NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &si, &piInfos[i]))
		{
			cout << "The new process is not created." << endl;
			cout << "Press any key to exit." << endl;
			cin.get();

			exit(GetLastError());
		}
		eventsStart[i] = CreateEvent(NULL, FALSE, FALSE, ("P" + to_string(i)).c_str());
		if (eventsStart[i] == NULL)
		{
			cout << "Failed to create event" << endl;
			cin.get();
			exit(GetLastError());
		}
	}
}

void openEvents(int numbProc, HANDLE* eventsSender)
{
	for (int i = 0; i < numbProc; i++)
	{
		eventsSender[i] = OpenEvent(EVENT_MODIFY_STATE, FALSE, ("Wait" + to_string(i)).c_str());
		if (eventsSender[i] == NULL)
		{
			cout << "Open event failed." << endl;
			cout << "Press any key to exit." << endl;
			cin.get();

			exit(GetLastError());
		}
	}
}

HANDLE deployMutex(string name)
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, "DemoMutex");
	if (hMutex == NULL)
	{
		cout << "Create mutex failed." << endl;
		cout << "Press any key to exit." << endl;
		cin.get();

		exit(GetLastError());
	}
	return hMutex;
}

int main()
{
	HANDLE	hMutex, hEventRead;
	HANDLE *eventsStart, *eventsSender;
	STARTUPINFO	si;
	PROCESS_INFORMATION* piInfos;
	string buf = "C:\\Users\\HP\\";
	string path = "C:\\Users\\HP\\";
	string name;
	int numberRec, numbProc;

	cout << "Enter name of the file" << endl;
	getline(cin, name);
	cout << "Enter number of records" << endl;
	cin >> numberRec;
	buf.append(name);
	buf.append(" ");
	buf.append(to_string(numberRec));
	cout << "Enter number of processes" << endl;
	cin >> numbProc;

	piInfos = new PROCESS_INFORMATION[numbProc];
	eventsStart = new HANDLE[numbProc];
	eventsSender = new HANDLE[numbProc];
	
	hMutex = deployMutex("DemoMutex");

	hEventRead = CreateEvent(NULL, FALSE, FALSE, "Read");
	if (hEventRead == NULL)
	{
		cin.get();
		return GetLastError();
	}

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	createFile(path + name);

	initHandles(eventsStart, piInfos, si, numbProc, buf);

	WaitForMultipleObjects(numbProc, eventsStart, TRUE, INFINITE);

	openEvents(numbProc, eventsSender);

	bool canRead = true;
	
	for (;;)
	{
		if (canRead)
		{
			WaitForSingleObject(hMutex, INFINITE);

			char symb;
			cout << "Enter 0 to stop or anything to continue" << endl;
			cin >> symb;
			if (symb == '0')
			{
				break;
			}
			
			bool hasRead = read(path + name);
			cleanFile(path + name);

			if (!hasRead)
			{
				canRead = false;
			}

			setEvents(eventsSender, numbProc);
			ReleaseMutex(hMutex);
		}
		else
		{
			cout << "Waiting for data" << endl;
			WaitForSingleObject(hEventRead, INFINITE);
			canRead = true;
		}

	}

	for (int i = 0; i < numbProc; i++)
	{
		TerminateProcess(piInfos[i].hProcess, 0);
		CloseHandle(piInfos[i].hProcess);
		CloseHandle(piInfos[i].hThread);
		CloseHandle(eventsStart[i]);
		CloseHandle(eventsSender[i]);
	}
	CloseHandle(hMutex);
	CloseHandle(hEventRead);
	delete[] eventsStart;
	delete[] eventsSender;
	delete[] piInfos;

	return 0;
}