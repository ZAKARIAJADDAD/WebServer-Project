/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 17:02:58 by enja              #+#    #+#             */
/*   Updated: 2023/11/23 11:35:25 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"

Webserv::Webserv()
{
	
}

void    Webserv::DropClient(int i)
{
	close(clients[i].client_id);
	FD_CLR(clients[i].client_id, &rfds);
	FD_CLR(clients[i].client_id, &wfds);
	if (clients[i].inf != NULL)
	{
		clients[i].inf->close();
		delete clients[i].inf;
	}
	if (clients[i].outf != NULL)
	{
		clients[i].outf->close();
		delete clients[i].outf;
		std::remove(("dir" + clients[i].m_id + ".html").c_str());
	}
	if (clients[i].inf_cgi != NULL)
	{
		clients[i].inf_cgi->close();
		delete clients[i].inf_cgi;
		std::remove(("cgi" + clients[i].m_id).c_str());
	}
	//------ For POST method -------
	if (clients[i].p_config->uploadFiles.find(clients[i].client_id) != clients[i].p_config->uploadFiles.end())
	{
		if (clients[i].p_config->uploadFiles[clients[i].client_id]->is_open())
			clients[i].p_config->uploadFiles[clients[i].client_id]->close();
		delete clients[i].p_config->uploadFiles[clients[i].client_id];
		if (clients[i].isup_pass == false)
		{
			std::remove(clients[i].upFpath.c_str());
		}
		clients[i].p_config->uploadFiles.erase(clients[i].client_id);
	}
	if (clients[i].dr != NULL)
		delete clients[i].dr;
	if (clients[i].arrDe.size() != 0)
	{
		for (size_t j = 0; j < clients[i].arrDe.size(); j++)
		{
			if (clients[i].arrDe[j] != NULL)
				delete clients[i].arrDe[j];
		}
		clients[i].arrDe.clear();
	}
	//----- END ---------
	clients[i].req_lines.clear();
	clients[i].status = 0;
	clients[i].pid = 0;
	clients[i].request.clear();
	clients[i].request_url.clear();
	clients[i].request_host.clear();
	clients[i].final_respons.clear();
	clients[i].matchlocation.clear();
	clients[i].extension.clear();
	clients[i].map.clear();
	clients[i].map_extens.clear();
	clients.erase(clients.begin()+i);
}

 void    Webserv::ReInitialize_fds()
 {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&erfds);

		file = -1;
		for (size_t i = 0; i < sockets.size(); i++)
		{
			int socket = sockets[i];
			FD_SET(socket, &rfds);
			if (socket > file)
				file = socket;
		}
		for (size_t i = 0; i < clients.size(); i++)
		{
			int cl = clients[i].client_id;
			FD_SET(cl, &rfds);
			FD_SET(cl, &wfds);
			FD_SET(cl, &erfds);
			if (cl > file)
				file = cl;
		}
 }

int Webserv::Accept_clietns(ConfigInfo& config)
{
		for (size_t i = 0; i < sockets.size(); i++)
		{
			if (FD_ISSET(sockets[i], &rfds))
			{
				clients_base client(config);
				client.client_id = accept(sockets[i], nullptr, nullptr);
				if (client.client_id == -1)
				{
					std::cerr << "ERROR ACCEPT" << std::endl;
					return -1;
				}
				client.ready = false;
				client.parsed = false;
				client.body = false;
				client.post_sig = false;
				client.valid = false;
				client.is_sending = true;
				client.first = false;
				client.executed = false;
				client.exitStatus = 0;
				client.server_id = i;
				client.inf = NULL;
				client.outf = NULL;
				client.inf_cgi = NULL;
				client.check = false;
				// client.query = NULL;
				//------ For POST method ------

				client.startbody = 0;
				client.read_size = 1024;
				client.hexa_bytes = 0;
				client.byteswritten = 0;
				client.first_time = true;
				client.first_init = true;
				client.findbody = false;
				client.whilecgi = false;
				client.is_deleting = false;
				client.delerror = false; 
				client.dr = NULL;
				std::stringstream id;
				id << client.client_id;
				client.m_id = id.str();
				client.deep = 0;
				clients.push_back(client);
			}
		}
		return 1;
}

void Webserv::MultiPlexer(ConfigInfo &configinfo)
{
	sockets = configinfo.get_m_sockets();
	while (1)
	{
		ReInitialize_fds();
		int activ = select(file + 1 , &rfds , &wfds , &erfds , 0);
		if (activ == -1)
		{
			std::cerr << "ERROR SELECT" << std::endl;
			continue;
		}

		if (Accept_clietns(configinfo) == -1)
			continue;
		
		for (size_t i = 0; i < clients.size(); i++)
		{
			if (FD_ISSET(clients[i].client_id, &rfds) && clients[i].ready == false)
			{
				char *buffer = new char[READ_SIZE];
				if (clients[i].executed || clients[i].is_deleting)
					goto cgi;
				clients[i].bytesRead = recv(clients[i].client_id, buffer, clients[i].read_size, 0);
				if (clients[i].bytesRead == 0 || clients[i].bytesRead == -1)
				{
					delete[] buffer;
					DropClient(i);
					continue;
				}				
				if (clients[i].parsed == false)
				{
						std::string buff(buffer, clients[i].bytesRead);
						clients[i].request +=  buff;
						pars_request(clients[i], configinfo);
						Request_ErrorHandler(clients[i]);
				}
				if (clients[i].parsed == true && clients[i].exitStatus == 0)
				{
					cgi:
					if (clients[i].matchingLocation(configinfo, buffer) == false)
					{
						clients[i].ready = true;
						clients[i].parsed = true;
						Request_ErrorHandler(clients[i]);
					}
				}
				delete[] buffer;
			}
			else if (FD_ISSET(clients[i].client_id, &wfds) && clients[i].ready == true)
			{
				if(clients[i].exitStatus == 0)
				{
					if (clients[i].executed)
					{
						if (clients[i].request_method == "GET")
							clients[i].call_Cgi(*clients[i].p_config, clients[i].runfile);
						else if (clients[i].request_method == "POST")
							clients[i].POST_cgi(*clients[i].p_config, clients[i].runfile);
						if (clients[i].exitStatus != 0)
						{
							Request_ErrorHandler(clients[i]);
							goto errorhanlder;
						}
					}
					if (clients[i].inf != NULL)
					{
						char *buffer = new char[READ_SIZE];
						clients[i].inf->read(buffer, READ_SIZE);
						clients[i].bytesRead = clients[i].inf->gcount();
						std::string nbuff(buffer, clients[i].bytesRead);
						delete[] buffer;
						clients[i].final_respons += nbuff;
						nbuff.clear();
					}
					else if (clients[i].whilecgi == false)
						continue;
					else if (clients[i].inf_cgi != NULL)
					{
						char *buffer = new char[READ_SIZE];
						clients[i].inf_cgi->read(buffer, READ_SIZE);
						clients[i].bytesRead = clients[i].inf_cgi->gcount();
						std::string nbuff(buffer, clients[i].bytesRead);
						delete[] buffer;
						if (clients[i].check == false)
						{
							nbuff = clients[i].check_headers(nbuff);
						}
						clients[i].final_respons = nbuff;
						nbuff.clear();
					}
					if (clients[i].bytesRead == 0 || clients[i].bytesRead == -1)
					{
						clients[i].is_sending = false;
					}
					ssize_t bites = send(clients[i].client_id, clients[i].final_respons.c_str(), clients[i].final_respons.size(), 0);

					if (bites == -1 || bites == 0)
					{
						DropClient(i);
						continue;
					}
					clients[i].final_respons.clear();
				}
				else
				{
					errorhanlder:
					ssize_t bites = send(clients[i].client_id, clients[i].final_respons.c_str(), clients[i].final_respons.size(), 0);
					if (bites == -1 || bites == 0)
					{
						DropClient(i);
						continue;
					}
					DropClient(i);
					continue;
				}
				if (clients[i].is_sending == false)
				{
					DropClient(i);
					continue;
				}
			}
			else if(FD_ISSET(clients[i].client_id, &erfds))
			{
				std::cerr << "ERROR" << std::endl;
				DropClient(i);
				break;
			}
		}
	}
}

Webserv::~Webserv() {}