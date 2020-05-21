#ifndef PARAMETER_H
#define PARAMETER_H

#include<string>
#include<Windows.h>
#include<map>

typedef struct parameter {
	HANDLE pipe;
	int number;
	std::string fileName;
	std::map<int, int>* index;
}parameter;

#endif
