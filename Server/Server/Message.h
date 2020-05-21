#ifndef MESSAGE_H
#define MESSAGE_H

#include<string>

typedef struct message {
	int id;
	int FLAG;
    std::string fileName;
	std::string data[3];
}message;

#endif
