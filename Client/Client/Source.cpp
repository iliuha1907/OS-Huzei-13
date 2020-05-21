
#include<iostream>
#include<Windows.h>
#include<string>

#include"C:\\Users\\HP\\source\\repos\\Server\\Server\\Parameter.h"
#include"C:\\Users\\HP\\source\\repos\\Server\\Server\\Employee.h"
#include"C:\\Users\\HP\\source\\repos\\Server\\Server\\Message.h"



using namespace std;
string pipeName = "\\\\.\\pipe\\AlphaBravo";

employee enterEmp()
{
	employee emp;
	cout << "Enter data to insert" << endl;
	cout << "Enter id" << endl;
	cin >> emp.num;
	cout << "Enter name" << endl;
	cin >> emp.name;
	cout << "Enter hours" << endl;
	cin >> emp.hours;
	return emp;
}

HANDLE openPipe(string name)
{
	HANDLE hPipe = CreateFile((name).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	return hPipe;
}

bool writeEmpl(int what, HANDLE hPipe)
{
	BOOL canWrite;
	message msg;
	int num;
	cout << "Enter id of employee:" << endl;
	cin >> num;
	msg.id = num;
	msg.FLAG = what;
	canWrite = WriteFile(hPipe, &msg, sizeof(message), NULL, NULL);
	if (!canWrite)
	{
		cout << "Error at writing to pipe";
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	string id = argv[0];
	cout << pipeName + id << endl;
	HANDLE hPipe = openPipe(pipeName + id);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Error at opening pipe" << endl;
		return -1;
	}
	while (true)
	{
		int what;
		cout << "Enter what you need to do ( 1 to read, 2 to write, 3 to stop)" << endl;
		cin >> what;
		BOOL canWrite, canRead;
		message msg;
		employee emp;
		int num;
		switch (what)
		{
		case 1:
			
			cout << "Enter id of employee:" << endl;
			cin >> num;
			msg.id = num;
			msg.FLAG = what;
			canWrite = WriteFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canWrite)
			{
				cout << "Error at writing to pipe";
				return -1;
			}
		/*	if (!writeEmpl(what, hPipe))
			{
				cout << "Error at writing to pipe";
				return -1;
			}*/
			canRead = ReadFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canRead)
			{
				cout << "Error at reading from pipe";
				return -1;
			}
			if (msg.data[1].compare("") == 0)
			{
				cout << "Can not read right now. Try again" << endl;
				continue;
			}
			cout << "ID = " << msg.data[0] << ", Name = " << msg.data[1] << ", Hours = " << msg.data[2] << endl;
			break;
		case 2:
			cout << "Enter id of employee:" << endl;
			cin >> num;
			msg.id = num;
			msg.FLAG = what;
			canWrite = WriteFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canWrite)
			{
				cout << "Error at writing to pipe";
				return -1;
			}
			canRead = ReadFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canRead)
			{
				cout << "Error at reading from pipe";
				return -1;
			}
			if (msg.data[1].compare("") == 0)
			{
				cout << "Can not read right now. Try again" << endl;
				continue;
			}
			cout << "ID = " << msg.data[0] << ", Name = " << msg.data[1] << ", Hours = " << msg.data[2] << endl;
			emp = enterEmp();
			msg.data[0] = to_string(emp.num);
			msg.data[1] = emp.name;
			msg.data[2] = to_string(emp.hours);
			canWrite = WriteFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canWrite)
			{
				cout << "Error at writing to pipe";
				return -1;
			}
			break;
		default:
			msg.FLAG = 3;
			canWrite = WriteFile(hPipe, &msg, sizeof(message), NULL, NULL);
			if (!canWrite)
			{
				cout << "Error at writing to pipe";
				return -1;
			}
			return 0;
		}
	}
	
	system("pause");
	return 0;
}
