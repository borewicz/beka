#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#include <readline/readline.h>
#include <readline/history.h>

#include "client.h"

using namespace beka;

void parseJSON(std::string json)
{
	rapidjson::Document d;
	d.Parse(json.c_str());

	if (d.HasMember("notification"))
	{
		if (strcmp(d["notification"].GetString(), "new_message") == 0)
		{
			std::chrono::time_point<std::chrono::system_clock> oldTime = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(d["message"]["date"].GetInt()));
			time_t ttp = std::chrono::system_clock::to_time_t(oldTime);
			char mbstr[100];
			std::strftime(mbstr, sizeof(mbstr), "%H:%M", std::localtime(&ttp));
			printf("%s to %s: %s (%s)\n", d["message"]["source"].GetString(), d["message"]["target"].GetString(), d["message"]["text"].GetString(), mbstr);
		}
		else if (strcmp(d["notification"].GetString(), "kick") == 0)
		{
			printf("%s kicked yuo from %s, reason: %s\n", d["user"].GetString(), d["channel_name"].GetString(), d["reason"].GetString());
		}
	}
	/*
	 * 
	 */
	else if (strcmp(d["status"].GetString(), "OK") == 0)
	{
		if (d.HasMember("response"))
		{
			for (rapidjson::SizeType i = 0; i < d["response"].Size(); i++) 
				printf("%s  ", d["response"][i].GetString());
			printf("\n");
		}
	}
	else if (strcmp(d["status"].GetString(), "NOT_JOINED") == 0)
	{
		printf("not joined!\n");
	}
	else if (strcmp(d["status"].GetString(), "BANNED") == 0)
	{
		printf("yuo are banned!\n");
	}
	else if (strcmp(d["status"].GetString(), "ALREADY_JOINED") == 0)
	{
		printf("you already joined this channel\n");
	}
	else if (strcmp(d["status"].GetString(), "NOT_ENOUGH_PRIVILEGES") == 0)
	{
		printf("you don't have enough privileges!\n");
	}
	else if (strcmp(d["status"].GetString(), "NO_USER") == 0)
	{
		printf("user cannot into existing xD");
	}
	else if (strcmp(d["status"].GetString(), "UNKNOWN_REQUEST") == 0)
	{
		printf("not supported, available requests are: ");
		for (rapidjson::SizeType i = 0; i < d["available_requests"].Size(); i++) 
			printf("%s  ", d["available_requests"][i].GetString());
		printf("\n");
	}

}

int main(int argc, char *argv[])
{
	Client client;

	std::string choice, result;
	bool disconnected = false;

	if (argc < 2)
	{
		printf("usage: beka [nickname]\n");
		exit(0);
	}

	if (client.connect(argv[1]) != 0)
	{
		printf("cannot connect, maybe name conflict?\n");
		exit(-1);
	}

	if (fork() == 0)
	{
		while (!disconnected)
		{
			if (!(result = client.waitForResponse()).empty())
			{
				parseJSON(result);
			}
		}
	}
	else
	{
		char *buf;
		rl_bind_key('\t',rl_insert);
		while (!disconnected && (buf = readline("")) != NULL)
		{
			if (buf[0] != 0)
				add_history(buf);
			char * pch;
			pch = strtok(buf, " ,.-");
			while (pch != NULL)
			{
				if (strcmp(pch, "quit") == 0)
				{
					client.disconnect();
					disconnected = true;
					pch = NULL;
				}
				else if (strcmp(pch, "list") == 0)
				{
					client.getChannels();
					pch = NULL;					
				}
				else if (strcmp(pch, "join") == 0)
				{
					pch = strtok (NULL, " ,.-");
					if (pch != NULL)
						client.joinChannel(pch);
					else
						printf("usage: join [channel_name]\n");
					pch = NULL;
				}
				else if (strcmp(pch, "users") == 0)
				{
					pch = strtok (NULL, " ,.-");
					if (pch != NULL)
						client.getUsersOnChannel(pch);
					else
						printf("usage: users [channel_name]\n");
					pch = NULL;
				}
				else if (strcmp(pch, "leave") == 0)
				{
					pch = strtok (NULL, " ,.-");
					if (pch != NULL)
						client.exitFromChannel(pch);
					else
						printf("usage: leave [channel_name]\n");
					pch = NULL;
				}
				else if (strcmp(pch, "pm") == 0)
				{
					try
					{
						pch = strtok (NULL, " ,.-");
						std::string channel = pch;
						pch = strtok (NULL, "");
						client.sendPM(channel, pch);
					}
					catch (...)
					{
						printf("usage: pm [nick] [message]\n");
					}
					pch = NULL;
				}
				else if (strcmp(pch, "message") == 0)
				{
					try
					{
						pch = strtok (NULL, " ,.-");
						std::string channel = pch;
						pch = strtok (NULL, "");
						client.sendMessage(channel, pch);
					}
					catch (...)
					{
						printf("usage: message [channel] [message]\n");
					}
					pch = NULL;
				}
				else if (strcmp(pch, "ban") == 0)
				{
					try
					{
						pch = strtok (NULL, " ,.-");
						std::string user = pch;
						pch = strtok (NULL, " ,.-");
						std::string channel = pch;
						pch = strtok (NULL, "");					
						client.kickUser(true, user, channel, pch);
					}
					catch (...)
					{
						printf("usage: ban [user] [channel] [reason]\n");
					}
					pch = NULL;
				}
				else if (strcmp(pch, "kick") == 0)
				{
					try
					{
						pch = strtok (NULL, " ,.-");
						std::string user = pch;
						pch = strtok (NULL, " ,.-");
						std::string channel = pch;
						pch = strtok (NULL, "");					
						client.kickUser(false, user, channel, pch);
					}
					catch (...)
					{
						printf("usage: kick [user] [channel] [reason]\n");
					}
					pch = NULL;
				}
				else 
				{
					printf("bad!\n");
					pch = NULL;
				}
			}
		}
		free(buf);
		return 0;
	}
}
