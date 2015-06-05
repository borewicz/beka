#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "core.h"

namespace beka
{
	class Client
	{
		Core core;	
		int requestId = 0;
		int user_id;
		public:
			std::string waitForResponse();
			int connect(std::string);
			int disconnect();
			int getChannels();
			int joinChannel(std::string);
			int getUsersOnChannel(std::string);
			int exitFromChannel(std::string);
			int sendPM(std::string, std::string);
			int sendMessage(std::string, std::string);
			int kickUser(bool, std::string, std::string, std::string);

			Client()
			{
				user_id = getpid();
			}
	};
}

#endif
