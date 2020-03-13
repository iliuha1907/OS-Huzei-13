//============================================================================
// Name        : OsiFirst.cpp
// Author      : iliuha1907
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include<cstring>
#include<fstream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};
bool waitCheck(pid_t pid)
{
	int status;
	if(waitpid(pid,&status,0)==-1)
	{
		return false;
	}
	return true;
}

void writeBinFile(string binName, int numRec)
{
	ofstream fileStrmOut(binName, ios::binary);
	for (int i = 0; i < numRec; i++)
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
		strcpy(empl.name, name);
		empl.hours = hours;
		fileStrmOut.write((char*)&empl, sizeof(empl));
	}
	fileStrmOut.close();
}

void readBinFile(string binName)
{
	employee bufEmpl;
	ifstream f2(binName, ios::binary | ios::in);
	while (f2.read((char*)&bufEmpl, sizeof(employee)))
	{
	     cout << bufEmpl.num << " " << bufEmpl.name << " " << bufEmpl.hours << endl;
	}
	f2.close();
}

void writeReport(string binName, string repName, int pay)
{
	ofstream fout(repName);
	fout << "Report based on " << binName << endl;
	fout << "******************************" << endl;
	employee buf;
	ifstream f2(binName, ios::binary | ios::in);
	while (f2.read((char*)&buf, sizeof(employee)))
	{
		fout << buf.num << " " << buf.name << " Hours: " << buf.hours << " Payment: " << buf.hours*pay << endl;
	}
	f2.close();
	fout.close();
}

void readReport(string repName)
{
	ifstream fin(repName);
	string tempEmpl = "";
	while (fin.peek() != EOF)
	{
		getline(fin, tempEmpl);
		cout << tempEmpl << endl;
	}
	fin.close();
}

int main()
{
	string binName,repName;
	int numRec,pay;
	cout << "Enter name of the file" << endl;
    getline(cin, binName);
	cout << "Enter number of records in file" << endl;
	cin >> numRec;
	pid_t pid = fork();
	if(pid<0)
	{
		cout<<"Error"<<endl;
		return -1;
	}
	else if(pid>0)//MAIN
	{
		if(waitCheck(pid))
		{
			readBinFile(binName);
		}
		else
		{
			cout<<"Error";
			return -1;
		}
	}
	else//Creator -fsanitize=address
	{
		writeBinFile(binName,numRec);
		exit(0);
	}

	cout << "Enter name of the report" << endl;
	cin.ignore();
	getline(cin, repName);
	cout << "Enter pay value" << endl;
	cin >> pay;
	pid=fork();
	if(pid<0)
	{
		cout<<"Error"<<endl;
		return -1;
	}
	else
	{
		if(pid>0)//MAIN
		{
			if(waitCheck(pid))
			{
				readReport(repName);
			}
			else
			{
				cout<<"Error"<<endl;
				return -1;
			}
		}
		else//Reporter
		{
			writeReport(binName,repName,pay);
	    }
	}

	return 0;
}
