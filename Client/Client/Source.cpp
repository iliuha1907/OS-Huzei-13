
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

message getMessageWrite(int what)
{
	message msg;
	int num;
	cout << "Enter id of employee:" << endl;
	cin >> num;
	msg.id = num;
	msg.FLAG = what;
	

	return msg;
}

bool readMessage(message msg)
{
	if (msg.id == -1)
	{
		cout << "Can not read right now. Try again" << endl;
		return false;
	}
	cout << "Id = " << msg.empl.num << ", Name = " << msg.empl.name << ", Hours = " << msg.empl.hours << endl;
	
	return true;
}

int getChoice()
{
	int what;
	cout << "Enter what you need to do ( 1 to read, 2 to write, 3 to stop)" << endl;
	cin >> what;
	return what;
}

message formMessage(employee emp)
{
	message msg;
	msg.empl = emp;
	return msg;
}

int work(HANDLE hPipe)
{
	while (true)
	{
		int what = getChoice();
		BOOL canWrite, canRead;
		message msg;
		employee emp;
		int num;
		switch (what)
		{
		case 1:
			msg = getMessageWrite(what);
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
			if (!readMessage(msg))
			{
				continue;
			}
			break;
		case 2:
			msg = getMessageWrite(what);
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
			if (!readMessage(msg))
			{
				continue;
			}
			
			emp = enterEmp();
			emp.num = msg.empl.num;
			msg = formMessage(emp);
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
			DisconnectNamedPipe(hPipe);
			return 0;
		}
	}

}

int main(int argc, char** argv)
{
	int id = GetCurrentProcessId();
	
	HANDLE hPipe = openPipe(pipeName + to_string(id));
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Error at opening pipe" << endl;
		return -1;
	}
	work(hPipe);

	return 0;
}
