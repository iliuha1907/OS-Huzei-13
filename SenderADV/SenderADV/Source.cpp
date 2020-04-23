#include <windows.h>
#include <iostream>
#include<fstream>
#include<string>

using namespace std;

int getNumber(string name)
{
	ifstream fin(name);
	int number = 0;
	while (fin.peek() != EOF)
	{
		string str;
		fin >> str;
		if (str != "")
		{
			number++;
		}
	}
	fin.close();
	return number;
}

void writeFile(string file, string data)
{
	ofstream fout(file, ios::app);
	fout << data << endl;
	fout.close();
}

int main(int argc, char* argv[])
{
	string name = argv[0];
	int num = atoi(argv[1]);
	int id = atoi(argv[2]);
	string eventStart = "P" + to_string(id);
	HANDLE	hMutex, hEventStart, hEventRec, hEventWait;


	// открываем мьютекс
	hMutex = OpenMutex(SYNCHRONIZE, FALSE, "DemoMutex");
	if (hMutex == NULL)
	{
		cout << "Open mutex failed." << endl;
		cout << "Press any key to exit." << endl;
		cin.get();

		return GetLastError();
	}

	hEventStart = OpenEvent(EVENT_MODIFY_STATE, FALSE, eventStart.c_str());
	if (hEventStart == NULL)
	{
		cout << "Open event failed." << endl;
		cout << "Press any key to exit." << endl;
		cin.get();

		return GetLastError();
	}
	SetEvent(hEventStart);


	hEventRec = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Read");
	if (hEventRec == NULL)
	{
		cout << "Open event failed." << endl;
		cout << "Press any key to exit." << endl;
		cin.get();

		return GetLastError();
	}

	hEventWait = CreateEvent(NULL, FALSE, FALSE, ("Wait"+to_string(id)).c_str());
	if (hEventWait == NULL)
	{
		return GetLastError();
	}


	string text;
	bool can = true;
	for (;;)
	{
		if (can)
		{
			WaitForSingleObject(hMutex, INFINITE);

			cout << "Enter text or 0 to stop" << endl;
			getline(cin, text);
			if (text[0] == '0')
			{
				break;
			}
			
			int number = getNumber(name);
	
			if (number < num)
			{
				writeFile(name, text);
				SetEvent(hEventRec);
			}
			else
			{
				can = false;
			}
			ResetEvent(hEventWait);
			ReleaseMutex(hMutex);
		}
		else
		{
			cout << "Can not write anymore. Waiting for the permition" << endl;
			WaitForSingleObject(hEventWait, INFINITE);
			cout << "Got it" << endl;
			can = true;
		}
	}
	SetEvent(hEventRec);

	CloseHandle(hMutex);
	CloseHandle(hEventStart);
	CloseHandle(hEventWait);
	return 0;
}