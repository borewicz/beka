#include "core.h"

namespace beka
{
	int Core::sendRequest(std::string json, int dest)
	{
		struct request req;
		req.type = dest;	
		strcpy(req.content, json.c_str());
		if (msgsnd(req_id, (void *) &req, sizeof(req.content), 0) == -1)
		{
			perror("sending request failed");
			return -1;
		}
		else
		{
			//std::cout << "sent: " << req.content << std::endl;
			return 0;
		}
	}

	std::string Core::getRequest(int type)
	{
		struct request req;

		if (msgrcv(req_id, (void *) &req, sizeof(req), type, 0) == -1)
		{
			perror("server");
			return NULL;
		}
		else
		{
			//std::cout << "read: " << req.content << std::endl;
			return req.content;
		}
	}

	int Core::exitFromQueue()
	{
		if (msgctl(req_id, IPC_RMID, NULL) == -1) {
			perror("msgctl");
			return -1;
		}
		return 0;
	}
}
