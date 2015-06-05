#ifndef __CORE_H__
#define __CORE_H__

#include <sys/msg.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace beka
{
	class Core
	{
		struct request
		{
			long type;
			char content[2047 - sizeof(long)];
		};

		int req_id;

		public:
			int sendRequest(std::string, int);
			int exitFromQueue();
			std::string getRequest(int);

		Core()
		{
			if ((req_id = msgget(12345, 0644 | IPC_CREAT)) == -1)
			{
				perror("mssget");
				exit(-1);
			}
		}
	};

}
#endif 

