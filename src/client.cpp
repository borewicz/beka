#include "client.h"

namespace beka
{
	std::string Client::waitForResponse()
	{
		std::string result;
		if (!(result = core.getRequest(user_id)).empty())
		{
			return result;
		}
		else
		{
			return NULL;
		}
	}

	int Client::connect(std::string nick)
	{
		std::string result;

		//=====
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("connect");
		writer.String("username");
		writer.String(nick.c_str());
		writer.String("temp_id");
		writer.Int(getpid());
		writer.String("request_id");
		writer.Int(requestId);
		writer.EndObject();
		//====

		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;
			if (!(result = core.getRequest(getpid())).empty())
			{
				rapidjson::Document d;
				d.Parse(result.c_str());
				requestId++;


				if (strcmp(d["status"].GetString(), "OK") == 0)
				{
					user_id = d["user_id"].GetInt();
					std::cout << "git!\n";
					return 0;
				}
				else
					return -1;
			}
			return 0;
		}
		else
			return -1;
	}

	int Client::disconnect()
	{
		std::string result;

		//=====
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("quit_server");
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId);
		writer.EndObject();
		//====

		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			//core.exitFromQueue();
			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   {
			   return 0;
			   }	
			   else
			   {
			   std::cout << "error!" << std::endl;
			   return -1;
			   }
			   }
			   */
			return 0;
		}
		else
			return -1;
	}

	int Client::getChannels()
	{
		std::string result;
		std::vector<std::string> channelsList;

		//=====
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("list_channels");
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId);
		writer.EndObject();
		//====

		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;			
			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   {
			   std::cout << d["response"].Size() << std::endl;
			   for (rapidjson::SizeType i = 0; i < d["response"].Size(); i++) 
			   {
			   printf("a[%d] = %s\n", i, d["response"][i].GetString());
			   channelsList.push_back(d["response"][i].GetString());
			   }
			   }
			   }
			   */
			return 0;
		}
		return -1;
	}

	int Client::joinChannel(std::string channelName)
	{
		std::string result;

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("join_channel");
		writer.String("channel_name");
		writer.String(channelName.c_str());
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId); 
		writer.EndObject();
		//====
		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;

			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   {
			   if (d["channel_created"].GetBool() == true)
			   return 1; //1 - ale nowy kanał najlepszy
			   else
			   return 0; //0 - dodan
			   }
			   }
			   */
		}
		return -1;
	}

	int Client::getUsersOnChannel(std::string channelName)
	{
		std::string result;
		std::vector<std::string> channelsList;

		//=====
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("list_users");
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId);
		writer.String("channel_name");
		writer.String(channelName.c_str());
		writer.EndObject();
		//====

		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;

			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   {
			   std::cout << d["response"].Size() << std::endl;
			   for (rapidjson::SizeType i = 0; i < d["response"].Size(); i++) 
			   {
			   printf("a[%d] = %s\n", i, d["response"][i].GetString());
			   channelsList.push_back(d["response"][i].GetString());
			   }
			   }
			   }
			   */
		}
	}

	int Client::exitFromChannel(std::string channelName)
	{
		std::string result;

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("quit_channel");
		writer.String("channel_name");
		writer.String(channelName.c_str());
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId); 
		writer.EndObject();
		//====
		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;

			/*
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   return 0;
			   }
			   */
		}
		return -1;
	}

	int Client::sendMessage(std::string channelName, std::string message)
	{
		std::string result;
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("send_channel_message");
		writer.String("channel_name");
		writer.String(channelName.c_str());
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId); 
		writer.String("message");
		writer.StartObject();
		writer.String("text");
		writer.String(message.c_str());
		writer.EndObject();
		writer.EndObject();
		//====
		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;

			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   return 0; 
			   }
			   */
			return 0;
		}
		return -1;
	}

	int Client::sendPM(std::string user, std::string message)
	{
		std::string result; 
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("send_message");
		writer.String("user");
		writer.String(user.c_str());
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId); 
		writer.String("message");
		writer.StartObject();
		writer.String("text");
		writer.String(message.c_str());
		writer.EndObject();
		writer.EndObject();
		//====
		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;

			/* 
			   if (!(result = core.getRequest(user_id)).empty())
			   {
			   rapidjson::Document d;
			   d.Parse(result.c_str());
			   requestId++;

			   if (strcmp(d["status"].GetString(), "OK") == 0)
			   return 0; 
			   }
			   */
			return 0;
		}
		return -1;
	}

	int Client::kickUser(bool ban, std::string user, std::string channel, std::string reason)
	{
		/*
		 * {
"user_id" : "0"
"request" : "kick",
"ban" : true
"kicked_user" : 23,
"reason" : "bo tak"
"channel" : "nazwa"
"request_id" : 0
}
		 */
		std::string result; 
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("request");
		writer.String("kick");
		writer.String("ban");
		writer.Bool(ban);
		writer.String("kicked_user");
		writer.String(user.c_str());
		writer.String("user_id");
		writer.Int(user_id);
		writer.String("request_id");
		writer.Int(requestId); 
		writer.String("reason");
		writer.String(reason.c_str());
		writer.String("channel_name");
		writer.String(channel.c_str());
		writer.EndObject();
		//====
		if (core.sendRequest(s.GetString(), 1) != -1)
		{
			requestId++;
			return 0;
		}
		return -1;
	}
}

