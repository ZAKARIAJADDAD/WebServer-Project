/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_Handler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 17:36:41 by zjaddad           #+#    #+#             */
/*   Updated: 2023/11/22 17:36:44 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"

 void   Webserv::Request_ErrorHandler(clients_base& client)
 {
    std::vector<ServerInfo> &servers = client.p_config->get_m_servers();
    switch (client.exitStatus)
    {
        case 204: {
            std::string file;
            if (!servers[client.server_id].errorpages["204"].empty())
            {
                if (access(servers[client.server_id].errorpages["204"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["204"];
                else
                    file = "html_errors/204.html";
            }
            else
                file = "html_errors/204.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 204 No Content\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 414: {
            std::string file;
            if (!servers[client.server_id].errorpages["414"].empty())
            {
                if (access(servers[client.server_id].errorpages["414"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["414"];
                else
                    file = "html_errors/414.html";
            }
            else
                file = "html_errors/414.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 414 URI Too Long\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 400:{
            std::string file;
            if (!servers[client.server_id].errorpages["400"].empty())
            {
                if (access(servers[client.server_id].errorpages["400"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["400"];
                else
                    file = "html_errors/400.html";
            }
            else
                file = "html_errors/400.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 400 Bad Request\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 413:{
            std::string file;
            if (!servers[client.server_id].errorpages["413"].empty())
            {
                if (access(servers[client.server_id].errorpages["413"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["413"];
                else
                    file = "html_errors/413.html";
            }
            else
                file = "html_errors/413.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 413 Content Too Large\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 500:{
            std::string file;
            if (!servers[client.server_id].errorpages["500"].empty())
            {
                if (access(servers[client.server_id].errorpages["500"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["500"];
                else
                    file = "html_errors/500.html";
            }
            else
                file = "html_errors/500.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 500 Internal Server Error\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 501: {
            std::string file;
            if (!servers[client.server_id].errorpages["501"].empty())
            {
                if (access(servers[client.server_id].errorpages["501"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["501"];
                else
                    file = "html_errors/501.html";
            }
            else
                file = "html_errors/501.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 501 Not Implemented\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 404:{
            std::string file;
            if (!servers[client.server_id].errorpages["404"].empty())
            {
                if (access(servers[client.server_id].errorpages["404"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["404"];
                else
                    file = "html_errors/404.html";
            }
            else
                file = "html_errors/404.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 404 Not Found\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 405:{
            std::string file;
            if (!servers[client.server_id].errorpages["405"].empty())
            {
                if (access(servers[client.server_id].errorpages["405"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["405"];
                else
                    file = "html_errors/405.html";
            }
            else
                file = "html_errors/405.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 405 Method Not Allowed\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 301:{
            
            std::string file;
            if (!servers[client.server_id].errorpages["301"].empty())
            {
                if (access(servers[client.server_id].errorpages["301"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["301"];
                else
                    file = "html_errors/301.html";
            }
            else
                file = "html_errors/301.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 301 Moved Permanently\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 403:{
            
            std::string file;
            if (!servers[client.server_id].errorpages["403"].empty())
            {
                if (access(servers[client.server_id].errorpages["403"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["403"];
                else
                    file = "html_errors/403.html";
            }
            else
                file = "html_errors/403.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 403 Forbidden\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 505:{
            std::string file;
            if (!servers[client.server_id].errorpages["505"].empty())
            {
                if (access(servers[client.server_id].errorpages["505"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["505"];
                else
                    file = "html_errors/505.html";
            }
            else
                file = "html_errors/505.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
        case 508:{
            std::string file;
            if (!servers[client.server_id].errorpages["508"].empty())
            {
                if (access(servers[client.server_id].errorpages["508"].c_str(), R_OK) == 0)
                    file  = servers[client.server_id].errorpages["508"];
                else
                    file = "html_errors/508.html";
            }
            else
                file = "html_errors/508.html";
            std::ifstream fd(file.c_str(), std::ios::in);
            std::string line;
            while (std::getline(fd, line))
                client.final_respons += line;
            std::string header = "HTTP/1.1 508 Loop Detected\r\n";
            header += "Content-Type: html\r\n";
            header += "Content-Length:" + client.ft_to_string(client.final_respons.length()) + "\r\n";
            header += "\r\n";
            client.final_respons = header += client.final_respons;
            break;
        }
    }
 }
