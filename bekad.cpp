#include "server.h"

#include <cstdlib>
#include <signal.h>

using namespace beka;

Server server;

void sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		server.terminateServer();
		printf("\nexiting...\n");
		exit(0);
	}
}

int main()
{
	std::string result;
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");
	while (1)
	{
		if (!(result = server.waitForResponse()).empty())
		{
			rapidjson::Document d;
			d.Parse(result.c_str());

			if (strcmp(d["request"].GetString(), "connect") == 0)
				server.addUser(d["username"].GetString(), d["temp_id"].GetInt(), d["request_id"].GetInt());
			else if (strcmp(d["request"].GetString(), "quit_server") == 0)
				server.deleteUser(d["user_id"].GetInt(), d["request_id"].GetInt());	
			else if (strcmp(d["request"].GetString(), "list_channels") == 0)
				server.sendChannelsList(d["user_id"].GetInt(), d["request_id"].GetInt());
			else if (strcmp(d["request"].GetString(), "join_channel") == 0)
				server.joinUserToChannel(d["user_id"].GetInt(), d["channel_name"].GetString(), d["request_id"].GetInt());
			else if (strcmp(d["request"].GetString(), "list_users") == 0)
				server.sendUsersOnChannel(d["user_id"].GetInt(), d["channel_name"].GetString(), d["request_id"].GetInt());
			else if (strcmp(d["request"].GetString(), "quit_channel") == 0)
				server.removeUserFromChannel(d["user_id"].GetInt(), d["channel_name"].GetString(), d["request_id"].GetInt());
			else if (strcmp(d["request"].GetString(), "send_message") == 0)
			{
				server.sendPM(d["user"].GetString(), d["message"]["text"].GetString(), d["user_id"].GetInt(), d["request_id"].GetInt());
			}
			else if (strcmp(d["request"].GetString(), "send_channel_message") == 0)
			{
				server.sendToChannel(d["channel_name"].GetString(), d["message"]["text"].GetString(), d["user_id"].GetInt(), d["request_id"].GetInt());
			}
			else if (strcmp(d["request"].GetString(), "kick") == 0)
			{
				/*
{"request":"kick","ban":true,"kicked_user":"niema","user_id":10370,"request_id":4,"reason":"bo tak","channel":"guwno"}
				 * 
				 */
				server.kickUser(d["ban"].GetBool(), d["user_id"].GetInt(), d["kicked_user"].GetString(), d["channel_name"].GetString(), d["reason"].GetString(), d["request_id"].GetInt());
			}
			else
			{
				server.sendUnknownRequest(d["user_id"].GetInt(), d["request_id"].GetInt());
			}
		}
	}
	return 0;
}
