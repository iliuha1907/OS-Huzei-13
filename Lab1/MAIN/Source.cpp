#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include<thread>
#include<string>
#include<fstream>
#include"C:\Users\HP\source\repos\Reporter\Reporter\Header.h"
using namespace std;



const string directory = "C:\\Users\\HP\\source\\repos\\Creator\\";

int main()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO sti;
	BOOL canCreate;
	string cmdArg="";
	string binName;
	string reportName;
	int numRec = 0;
	int pay = 0;
	ZeroMemory(&sti, sizeof(STARTUPINFO));
	sti.cb = sizeof(STARTUPINFO);

	cout << "Enter name of the file" << endl;
	getline(cin, binName);
	cout << binName << endl;
	cout << "Enter number of records in file" << endl;
	cin >> numRec;

	cmdArg += to_string(numRec);
	cmdArg += " ";
	cmdArg += directory;
	cmdArg += binName;

	canCreate = CreateProcess("C:\\Users\\HP\\source\\repos\\Creator\\Debug\\Creator.exe",
		(LPSTR)cmdArg.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &sti, &pi);
	if (canCreate==FALSE) {
		cout << "Error"<< endl;
		system("pause");
		return -1;
	}
	
	WaitForSingleObject(pi.hProcess,INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	employee bufEmpl;
	ifstream f2(directory+binName, ios::binary | ios::in);
	while (f2.read((char*)&bufEmpl, sizeof(employee)))
	{
		cout << bufEmpl.num << " " << bufEmpl.name << " " << bufEmpl.hours << endl;
	}
	f2.close();

	cout << "Enter name of the report" << endl;
	getline(cin, reportName);
	cout << "Enter pay value" << endl;
	cin >> pay;

	cmdArg.clear();
	cmdArg += to_string(pay);
	cmdArg += " ";
	cmdArg += directory;
	cmdArg += reportName;
	cmdArg += " ";
	cmdArg += directory;
	cmdArg += binName;

	canCreate = CreateProcess("C:\\Users\\HP\\source\\repos\\Reporter\\Debug\\Reporter.exe", (LPSTR)cmdArg.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &sti, &pi);
	if (canCreate == FALSE) {
		cout << "Error" << endl;
		system("pause");
		return -1;
		
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	unsigned int Error;
	GetExitCodeProcess(pi.hProcess, (LPDWORD)&Error);
	cout << "Error " << Error<<endl;

	
	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	ifstream fin(directory+reportName);
	string tempEmpl = "";
	while (fin.peek() != EOF)
	{
		getline(fin, tempEmpl);
		cout << tempEmpl << endl;
	}
	fin.close();
	system("pause");
	return 0;
}