/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_parsing.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 17:36:12 by zjaddad           #+#    #+#             */
/*   Updated: 2023/11/22 17:36:15 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"
#include "../Headers/ConfigParser.hpp"

std::string clients_base::get_realpath(ConfigInfo& config)
{
	std::string	tmp;
	std::string	cpy;
	std::string	realpath;
	Location	m_loc = config.get_m_servers()[this->server_id].s_locations[this->id_match_loc]; 
	tmp = (m_loc.root.back() == '/'? m_loc.root: (m_loc.root + "/"));
    realpath = tmp;
    cpy = this->request_url.substr(this->matchlocation.length());
    if (!cpy.empty() && cpy.find("/") != std::string::npos)
    {
        cpy = cpy.substr(cpy.find("/"));
        while (!cpy.empty() && cpy[0] == '/')
            cpy.erase(0, 1);
        realpath += cpy;
    }
	return (realpath);
}

std::string	clients_base::ft_to_string(int str)
{
	std::stringstream ss;
	ss << str;
	return ss.str();
}

std::string clients_base::check_headers(std::string &buffer)
{
	std::string header;
	this->file_len -= buffer.find("\r\n\r\n");
	this->file_len -= 4;
	this->check = true;
	if (buffer.find("Content-Length") == std::string::npos)
	{
		header = "HTTP/1.1 200 OK\r\n";
		header += "Content-Length: " + this->ft_to_string(this->file_len) + "\r\n" + buffer;
	}
	else
	{
		header = "HTTP/1.1 200 OK\r\n" + buffer;
	}
	return header;
}

bool clients_base::strCompare(std::string s1, std::string s2)
{
	return s1 == s2;
}

bool clients_base::allowdedChar(std::string &uri)
{
	size_t found;
	found = uri.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%");
	if (found == std::string::npos)
		return true;
	return false;
}

bool clients_base::checkSignalErrors(clients_base &client, std::string &s)
{
	std::string arrMeth[] = {"POST", "DELETE", "GET"};
	std::stringstream ss(s);

	std::string str;
	client.counter = 0;

	while (ss >> str)
		client.counter++;
	ss.clear();
	ss.str("");
	ss << s;
	ss >> client.request_method >> client.request_url >> client.request_protocol;
	if (client.counter != 3 || *client.request_url.begin() != '/' || allowdedChar(client.request_url) == false)
	{
		client.exitStatus = 400;
		return false;
	}
	size_t found = client.request_url.find('?');
	if (found != std::string::npos)
	{
		this->query = client.request_url.substr(client.request_url.find("?") + 1);
		client.request_url = client.request_url.substr(0, client.request_url.find("?"));
	}
	int i = 0;
	while (i < 3)
	{
		if (strCompare(client.request_method, arrMeth[i]) == true)
			break;
		i++;
	}
	if (i == 3)
	{
		client.exitStatus = 501;
		return false;
	}
	if (strCompare(client.request_protocol, "HTTP/1.1") == false)
	{
		client.exitStatus = 505;
		return false;
	}
	if (client.request_url.length() > 2048)
	{
		client.exitStatus = 414;
		return false;
	}
	ss.clear();
	ss.str("");
	return true;
}

bool clients_base::the_welcome_index(clients_base &client)
{
	std::ifstream fd("welcome_page/index.html");
	std::string line;
	while (std::getline(fd, line))
		client.final_respons += line;
	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: html\r\n";
	header += "Content-Length:" + this->ft_to_string(client.final_respons.length()) + "\r\n";
	header += "\r\n";
	client.final_respons = header += client.final_respons;
	this->ready = true;
	this->is_sending = true;
	this->exitStatus = 1;
	return true;
}

bool clients_base::matchingLocation(ConfigInfo &config, char *buffer)
{
	if (this->request_url.size() == 1)
		return (the_welcome_index(*this));
	if (this->valid == false)
	{
		std::vector<ServerInfo> tmpSevers = config.get_m_servers();
		size_t j;
		std::string substring = this->request_url;
		while (this->valid == false && !substring.empty())
		{
			for (j = 0; j < tmpSevers[this->server_id].locs_paths.size(); j++)
			{
				if (substring == tmpSevers[this->server_id].locs_paths[j])
				{
					if (!tmpSevers[this->server_id].s_locations[j].redirection.empty())
					{
						std::cerr << "|" << tmpSevers[this->server_id].s_locations[j].redirection << "|" << std::endl;
						std::string header = "HTTP/1.1 301 Moved Permanently\r\n";
						header += "Location: " + tmpSevers[this->server_id].s_locations[j].redirection + "\r\n";
						header += "Content-Type: text/html\r\n";
						header += "Content-Length: 0\r\n";
						header += "\r\n";
						this->final_respons = header;
						this->exitStatus = 1;
						this->ready = true;
						return true;
					}
					this->matchlocation = substring;
					this->id_match_loc = j;
					if (this->request_method == "GET" && tmpSevers[this->server_id].s_locations[j].isGET == true)
					{
						this->valid = true;
						goto get;
					}
					else if (this->request_method == "POST" && tmpSevers[this->server_id].s_locations[j].isPOST == true)
					{
						this->valid = true;
						goto post;
					}
					else if (this->request_method == "DELETE" && tmpSevers[this->server_id].s_locations[j].isDELETE == true)
					{
						this->valid = true;
						goto delet;
					}
					else
					{
						this->exitStatus = 405;
						this->ready = true;
						return false;
					}
				}
			}

			if (valid == false)
			{
				if (substring.back() == '/')
					substring.pop_back();
				else if (substring.find_last_of("/", substring.length()) != std::string::npos)
					substring = substring.substr(0, substring.find_last_of("/", substring.length()) + 1);
			}
		}
		if (substring.empty())
		{
			this->exitStatus = 404;
			this->ready = true;
			return false;
		}
	}
	get:
	if (this->request_method == "GET")
		return this->get_method(config);
	post:
	if (this->request_method == "POST")
		return this->post_method(config, buffer);
	delet:
	if (this->request_method == "DELETE")
		return delete_method(config);
	return true;
}

void Webserv::pars_request(clients_base &client, ConfigInfo &configinfos)
{
	size_t pos, idx;
	bool sig;
	client.exitStatus = 0;
	idx = 0;
	pos = client.request.find('\n');
	for (std::string::iterator it = client.request.begin(); it != client.request.end(); it++)
	{
		client.req_lines.push_back(client.request.substr(idx, pos - idx - 1));
		idx = pos + 1;
		pos = client.request.find('\n', pos + 1);
	}
	if (client.first == false)
	{

		sig = client.checkSignalErrors(client, client.req_lines[0]);
		if (sig == false)
		{
			client.parsed = true;
			client.ready = true;
			return;
		}
		client.first = true;
	}
	for (size_t i = 0; i < client.request.size(); i++)
	{
		if (client.body == true)
		{
			client.exitStatus = 400;
			client.parsed = true;
			client.ready = true;
			return;
		}
		if (client.request[i] == '\n')
		{
			if (client.request[i - 1] == '\r' && client.request[i - 2] == '\n' && client.request[i - 3] == '\r' && client.request_method != "POST")
			{
				client.body = true;
			}
			if (client.request[i - 1] == '\r' && client.request[i - 2] == '\n' && client.request[i - 3] == '\r' && client.findbody == false)
			{
				client.parsed = true;
				client.startbody = i;
				client.findbody = true;
			}
		}
	}
	size_t i;
	client.first == false ? i = 1 : i = 0;
	while (i < client.req_lines[i].size())
	{
		std::string key, value;
		pos = client.req_lines[i].find(":");
		key = client.req_lines[i].substr(0, pos);
		value = client.req_lines[i].substr(pos + 2, client.req_lines[i].find('\n', pos + 2));
		client.map[key] = value;
		i++;
	}
	if (client.map["Transfer-Encoding"] != "chunked" && !client.map["Transfer-Encoding"].empty())
	{
		client.exitStatus = 400;
		client.parsed = true;
		client.ready = true;
		return;
	}
	if ((client.map["Transfer-Encoding"].empty() && client.map["Content-Length"].empty() && client.request_method == "POST") || client.map["Host"].empty())
	{
		client.exitStatus = 400;
		client.parsed = true;
		client.ready = true;
		return;
	}
	else if (!client.map["Content-Length"].empty() && !configinfos.get_m_servers()[client.server_id].max_clients.empty())
	{
		double contentLenght, maxBodySize;

		contentLenght = std::stod(client.map["Content-Length"]);
		maxBodySize = std::stod(configinfos.get_m_servers()[client.server_id].max_clients);
		if (contentLenght > maxBodySize)
		{
			client.exitStatus = 413;
			client.parsed = true;
			client.ready = true;
			return;
		}
	}
	client.parsed = true;
}