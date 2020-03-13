#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include<thread>
#include <string.h>
#include <fstream>
using namespace std;
struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[])
{
	int countRec = 0;
	string name = "";
	string directory = "";
	if (argc == 2)
	{
		countRec = atoi(argv[0]);
		name = argv[1];
	}
	else
	{
		cout << "Error in parameters" << endl;
		return -1;
	}
	ofstream fileStrmOut(name, ios::binary);
	for (int i = 0; i < countRec; i++)
	{
		int num;
		double hours;
		char name[10];
		cout << "Enter number for an employee" << endl;
		cin >> num;
		cout << "Enter name of the employee" << endl;
		cin >> name;
		cout << "Enter hours for the employee" << endl;
		cin >> hours;
		employee empl;
		empl.num = num;
		strcpy_s(empl.name, name);
		empl.hours = hours;
		fileStrmOut.write((char*)&empl, sizeof(empl));
	}
	fileStrmOut.close();
	return 0;
}