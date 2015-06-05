#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>

#include "core.h"

namespace beka
{
	class Server 
	{
		struct Channel
		{
			std::string name;
			std::vector<int> users; //adminem będzie pierwsza osoba na kanale xD
			std::vector<int> bannedUsers;
		};

		std::map<int, std::string> users;
		//std::map<std::string, std::vector<int>> channels;
		std::vector<Channel> channels;
		Core core;
		int notification_id;

		public:
			int addUser(std::string, int, int);
			int deleteUser(int, int);
			int sendChannelsList(int, int);
			int joinUserToChannel(int, std::string, int);
			std::string waitForResponse();
			int sendUsersOnChannel(int, std::string, int); 
			int removeUserFromChannel(int, std::string, int);
			int sendPM(std::string, std::string, int, int);
			int sendToChannel(std::string, std::string, int, int);
			int sendUnknownRequest(int, int);
			void terminateServer();
			int kickUser(bool, int, std::string, std::string, std::string, int);

			Server()
			{
				srand(time(0));
			}
		private:
			bool isUsernameOccupied(std::string);
			int transferMessage(std::string, std::string, std::string, int);
			int sendStatus(std::string, int, int);
			int getUserId(std::string nick);
			
	};
}

#endif
