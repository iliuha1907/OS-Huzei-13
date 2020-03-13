#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include<thread>
#include<string>
#include<fstream>
#include"Header.h"
using namespace std;
int main(int argc, char* argv[])
{
	int pay = 0;
	string name = "";
	string report = "";
	if (argc == 3)
	{
		pay = atoi(argv[0]);
		name = argv[2];
		report = argv[1];
	}
	else
	{
		cout << "Error in parameters" << endl;
		return -1;
	}
	ofstream fout(report);
	fout << "Report based on " << name << endl;
	fout << "******************************" << endl;
	employee buf;
	ifstream f2(name, ios::binary | ios::in);
	while (f2.read((char*)&buf, sizeof(employee)))
	{
		fout << buf.num << " " << buf.name << " Hours: " << buf.hours << " Payment: " << buf.hours*pay << endl;
	}
	f2.close();
	fout.close();
	return 0;
}