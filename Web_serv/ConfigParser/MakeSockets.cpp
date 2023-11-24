#include "../../Headers/ConfigParser.hpp"

void	ConfigInfo::Sockets_Maker(void)
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		int servsocket;
		int serv_port;
		std::istringstream(m_servers[i].s_port) >> serv_port;
		if ((servsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			this->print_n_exit("ERROR :The Socket can not be opened!");
		struct sockaddr_in serverAddr;
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(serv_port);
		std::string arr[4];
		int j = 0;
		std::string tmp;
		for (size_t k = 0; k < m_servers[i].host.size(); k++)
		{
			if (j < 4 && m_servers[i].host[k] != '.')
				tmp += m_servers[i].host[k];
			else
			{
				arr[j++] = tmp;
				tmp = "";
			}
		}
		arr[j] = tmp;
		serverAddr.sin_addr.s_addr = htonl(std::atoi(arr[0].c_str()) << 24 | std::atoi(arr[1].c_str()) << 16 | std::atoi(arr[2].c_str()) << 8 | std::atoi(arr[3].c_str()));
		int opt=1;
		if (setsockopt(servsocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
			perror("setsockopt");

		bind(servsocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
		fcntl(servsocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		if (listen(servsocket, SOMAXCONN) == -1)
			this->print_n_exit("Can not listen to the Socket!");
		this->m_sockets.push_back(servsocket);
	}
}
