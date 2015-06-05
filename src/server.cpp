#include "server.h"

namespace beka
{
	/*
	 * Funkcja czekająca na jakiekolwiek nowe dane na kolejce.
	 */
	std::string Server::waitForResponse()
	{
		std::string result;
		if (!(result = core.getRequest(1)).empty())
		{
			return result;
		}
		else
		{
			return NULL;
		}
	}

	/*
	 * Dodawanie nowego użytkownika.
	 */
	int Server::addUser(std::string nick, int pid, int request_id)
	{
		int random = 0;
		if (isUsernameOccupied(nick))
		{
			//core.sendRequest("{\"status\":\"NAME_CONFLICT\"}", pid);
			sendStatus("NAME_CONFLICT", request_id, pid);
		}
		else
		{
			do
			{
				random = 2 + rand() % 32766;
			}
			while (users.count(random));
			users.insert(std::make_pair(random, nick));

			std::cout << "new client, id = " << random << std::endl;

			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);

			writer.StartObject();
			writer.String("status");
			writer.String("OK");
			writer.String("user_id");
			writer.Int(random);
			writer.String("request_id");
			writer.Int(request_id);
			writer.EndObject();
			return core.sendRequest(s.GetString(), pid);
		}
	}

	/*
	 * Usuwanie użytkownika.
	 */
	int Server::deleteUser(int user_id, int request_id)
	{
		int random = 0;
		if (!users.count(user_id))
		{
			//core.sendRequest("{\"status\":\"NOT_CONNECTED\"}", user_id);
			sendStatus("NOT_CONNECTED", request_id, user_id);
		}
		else
		{
			std::cout << users[user_id] << " exits" << std::endl;
			users.erase(user_id);

			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);

			writer.StartObject();
			writer.String("status");
			writer.String("OK");
			writer.String("request_id");
			writer.Int(request_id); //co to kurwa jest		
			writer.EndObject();
			return core.sendRequest(s.GetString(), user_id);
		}
	}


	/* 
	 * Funkcja sprawdza, czy nie ma już kogoś o tym samym nicku na serwerze. 
	 */
	bool Server::isUsernameOccupied(std::string username)
	{
		bool found = false;
		std::map<int, std::string>::iterator it = users.begin(); 
		while(it != users.end())
		{
			found = (it->second == username);
			if(found)
				break;
			++it;
		}
		return found;
	}

	/* 
	 * Wysyła do klienta listę dostępnych kanałów.
	 */
	int Server::sendChannelsList(int user_id, int request_id)
	{
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		writer.StartObject();
		writer.String("status");
		writer.String("OK");
		writer.String("request_id");
		writer.Int(request_id);
		writer.String("response");
		writer.StartArray();
		for (int i = 0; i < channels.size(); i++)
			writer.String(channels[i].name.c_str());
		writer.EndArray();
		writer.EndObject();
		return core.sendRequest(s.GetString(), user_id);
	}

	/*
	 * Dołączanie użytkownika do kanału. Zanim dokonamy dodania sprawdzamy, czy kanał istnieje.
	 */
	int Server::joinUserToChannel(int user_id, std::string channelName, int request_id)
	{
		bool channel_created = true;

		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].name == channelName)
			{
				if(std::find(channels[i].bannedUsers.begin(), channels[i].bannedUsers.end(), user_id) != channels[i].bannedUsers.end()) {
					sendStatus("BANNED", request_id, user_id);
					return -1;
				}
				if(std::find(channels[i].users.begin(), channels[i].users.end(), user_id) != channels[i].users.end()) {
					sendStatus("ALREADY_JOINED",request_id, user_id);
					return -1;
				}
				channel_created = false;
				channels[i].users.push_back(user_id);
				break;
			}
		}
		if (channel_created == true)
		{
			Channel channel;
			channel.name = channelName;
			channel.users.push_back(user_id);
			channels.push_back(channel);
		}
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		writer.StartObject();
		writer.String("status");
		writer.String("OK");
		writer.String("request_id");
		writer.Int(request_id); 
		writer.String("channel_created");
		writer.Bool(channel_created);
		writer.EndObject();
		return core.sendRequest(s.GetString(), user_id);
	}

	/*
	 * Wysyłanie listy użytkowników w danym kanale do klienta.
	 */
	int Server::sendUsersOnChannel(int user_id, std::string channelName, int request_id)
	{
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		writer.StartObject();
		writer.String("status");
		writer.String("OK");
		writer.String("request_id");
		writer.Int(request_id);
		writer.String("response");
		writer.StartArray();
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].name == channelName)
			{
				for (int j = 0; j < channels[i].users.size(); j++)
					writer.String(users[channels[i].users[j]].c_str());
				break;
			}
		}
		writer.EndArray();
		writer.EndObject();
		return core.sendRequest(s.GetString(), user_id);
	}

	/*
	 * Usuwanie użytkownika z kanału.
	 */
	int Server::removeUserFromChannel(int user_id, std::string channelName, int request_id)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (strcmp(channels[i].name.c_str(), channelName.c_str()) == 0)
			{
				channels[i].users.erase(std::remove(channels[i].users.begin(), channels[i].users.end(), user_id), channels[i].users.end());
				/*
				 * Zakładamy, że jeżeli ostatni użytkownik opuszcza kanał, to zostaje on automatycznie usunięty (tak, jak na prawdziwym IRCu).
				 */
				if (channels[i].users.size() == 0)
					channels.erase(channels.begin() + i);

				rapidjson::StringBuffer s;
				rapidjson::Writer<rapidjson::StringBuffer> writer(s);

				writer.StartObject();
				writer.String("status");
				writer.String("OK");
				writer.String("request_id");
				writer.Int(request_id); 
				writer.EndObject();
				return core.sendRequest(s.GetString(), user_id);

			}
		}
		return sendStatus("NOT_JOINED", request_id, user_id);
	}

	/*
	 * Wysyłanie wiadomości prywatnej.
	 */
	int Server::sendPM(std::string nick, std::string message, int user_id, int request_id)
	{
		for (std::map<int, std::string>::iterator it = users.begin(); it != users.end(); ++it)
		{
			if ((strcmp(it->second.c_str(), nick.c_str()) == 0))
			{
				int result = transferMessage(users[user_id], message, nick, it->first);
				if (result != -1)
					sendStatus("OK", request_id, user_id);
				return 0;
			}
		}
		sendStatus("NO_USER", request_id, user_id);		
		return -1;
	}

	/*
	 * Wysyłanie wiadomości na kanał.
	 */
	int Server::sendToChannel(std::string channel, std::string message, int user_id, int request_id)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if ((strcmp(channels[i].name.c_str(), channel.c_str()) == 0))
			{
				for (int j = 0; j < channels[i].users.size(); j++)
				{
					int result = transferMessage(users[user_id], message, channel, channels[i].users[j]);
					if (result != -1)
						sendStatus("OK", request_id, user_id);
				}
				return 0;
			}
		}
		sendStatus("NOT_JOINED", request_id, user_id);
		return -1;
	}

	/*
	 * Wysyłanie wiadomości na kolejkę.
	 */
	int Server::transferMessage(std::string source, std::string message, std::string target, int user_id)
	{
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		auto timePoint = std::chrono::system_clock::now().time_since_epoch();
		int seconds = std::chrono::duration_cast<std::chrono::seconds>(timePoint).count();
		writer.StartObject();
		writer.String("notification");
		writer.String("new_message");
		writer.String("message");
		writer.StartObject();
		writer.String("source");
		writer.String(source.c_str());
		writer.String("target");
		writer.String(target.c_str());
		writer.String("text");
		writer.String(message.c_str());
		writer.String("date");
		writer.Int(seconds); //TODO: chrono
		writer.EndObject();
		writer.String("notification_id");
		writer.Int(notification_id);
		writer.EndObject();

		notification_id++;	
		return core.sendRequest(s.GetString(), user_id);		
	}

	/*
	 * Wysyłanie wiadomości informującej o statusie operacji.
	 */
	int Server::sendStatus(std::string status, int request_id, int user_id)
	{
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		writer.StartObject();
		writer.String("status");
		writer.String(status.c_str());
		writer.String("request_id");
		writer.Int(request_id); 
		writer.EndObject();
		return core.sendRequest(s.GetString(), user_id);
	}

	/*
	 * W przypadku nieznanego zapytania serwer zwraca listę obsługiwanych zapytań.
	 */
	int Server::sendUnknownRequest(int user_id, int request_id)
	{
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);

		writer.StartObject();
		writer.String("status");
		writer.String("UNKNOWN_REQUEST");
		writer.String("request_id");
		writer.Int(request_id); 
		writer.String("available_requests");
		writer.StartArray();
		writer.String("connect");
		writer.String("quit_server");
		writer.String("list_channels");
		writer.String("quit_channel");
		writer.String("join_channel");
		writer.String("list_users");
		writer.String("send_message");
		writer.String("send_channel_message");
		writer.String("kick_user");
		writer.EndArray();
		writer.EndObject();
		return core.sendRequest(s.GetString(), user_id);		
	}

	void Server::terminateServer()
	{
		core.exitFromQueue();
	}

	int Server::getUserId(std::string nick)
	{
		for (std::map<int, std::string>::iterator it = users.begin(); it != users.end(); ++it)
		{
			std::cout << it->first;

			if ((strcmp(it->second.c_str(), nick.c_str()) == 0))
			{
				return it->first;
			}
		}
		return -1;
	}

	int Server::kickUser(bool ban, int user_id, std::string bannedUser, std::string channel, std::string reason, int request_id)
	{
		int banned_user_id = getUserId(bannedUser);
		if (banned_user_id != -1)
		{
			for (int i = 0; i < channels.size(); i++)
			{
				if ((strcmp(channels[i].name.c_str(), channel.c_str()) == 0))
				{
					if (channels[i].users[0] == user_id)
					{
						if (ban == true)
						{
							channels[i].bannedUsers.push_back(banned_user_id);
						}
						channels[i].users.erase(std::remove(channels[i].users.begin(), channels[i].users.end(), banned_user_id), channels[i].users.end());
						rapidjson::StringBuffer s;
						rapidjson::Writer<rapidjson::StringBuffer> writer(s);

						writer.StartObject();
						writer.String("notification");
						writer.String("kick");
						writer.String("user");
						writer.String(users[user_id].c_str());
						writer.String("ban");
						writer.Bool(ban);
						writer.String("channel_name");
						writer.String(channel.c_str());
						writer.String("reason");
						writer.String(reason.c_str());
						writer.String("notification_id");
						writer.Int(notification_id);
						writer.EndObject();

						notification_id++;	
						//std::cout << banned_user_id << std::endl;
						return core.sendRequest(s.GetString(), banned_user_id);	
					}
					else 
					{
						sendStatus("NOT_ENOUGH_PRIVILEGES", request_id, user_id);
						return -1;
					}
				}
			}
			sendStatus("NO_USER", request_id, user_id);
			return -1;
		}
		else
			return -1;
	}


}
