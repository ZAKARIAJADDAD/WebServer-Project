/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 16:15:38 by enja              #+#    #+#             */
/*   Updated: 2023/11/23 12:21:18 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/includes.hpp"
#include "../Headers/ConfigParser.hpp"
#include "../Headers/mlp_request.hpp"


void    display_config(ConfigInfo configinfo)
{
    std::cout << "Valid" << std::endl;
    std::vector<ServerInfo> servers_data = configinfo.get_m_servers();
    // for (size_t i = 0; i < servers_data.size(); i++)
    // {

    //     std::cout << "Server N:" << i << std::endl;
    //     std::cout << "{\n\tPort :" << servers_data[i].s_port << std::endl;
    //     std::cout << "\tHost :" << servers_data[i].host << std::endl;
    //     std::cout << "\tMaxC :" << servers_data[i].max_clients << std::endl;
    //     std::cout << "\tName :" << servers_data[i].serverName << std::endl;

    //     for (size_t j = 0; j < servers_data[i].s_locations.size(); j++)
    //     {
    //         std::cout << "\tLocation N:" << j << std::endl;
    //         std::cout << "\t{" << std::endl << "\t\t";
    //         std::cout << "Path  :" << servers_data[i].s_locations[j].location_path << std::endl << "\t\t";
    //         std::cout << "GET   :" << servers_data[i].s_locations[j].isGET << std::endl << "\t\t";
    //         std::cout << "POST  :" << servers_data[i].s_locations[j].isPOST << std::endl << "\t\t";
    //         std::cout << "DELETE:" << servers_data[i].s_locations[j].isDELETE << std::endl << "\t\t";
    //         std::cout << "Root  :" << servers_data[i].s_locations[j].root << std::endl << "\t\t";
    //         for (size_t a = 0; a < servers_data[i].s_locations[j].s_index.size(); a++)
    //             std::cout << "Index["<< a <<"] :|" << servers_data[i].s_locations[j].s_index[a] << "| ";
    //         std::cout << std::endl;
    //         // for (size_t b = 0; b < servers_data[i].s_locations[j].s_cgi_pass.size(); b++)
    //             // std::cout << "cgi_pass ["<< b <<"] : type =|" << servers_data[i].s_locations[j].s_cgi_pass.first() << ;
    //         std::cout << "\t\tAutoin:" << servers_data[i].s_locations[j].autoindex << std::endl << "\t";
    //         std::cout << "{" << std::endl;
    //     }
    //     std::cout << "}\n";
    // }
}

 #include <signal.h>
int main(int ac, char **av)
{
    try
    {
        signal(SIGPIPE, SIG_IGN);
        std::string inputfile = DEFAULT_CONFIG_FILE;
        if (ac >= 2)
            inputfile = av[1];
        ConfigInfo  configinfo(inputfile);
        configinfo.ParsConfigFile();
        // display_config(configinfo);
        Webserv obj;
        obj.MultiPlexer(configinfo);
    }
    catch (...)
    {
        std::cout << "ERROR" << std::endl;
    }
    return 0;
}