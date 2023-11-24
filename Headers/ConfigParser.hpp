#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP
#include "includes.hpp"


const std::string DEFAULT_CONFIG_FILE = "Configfile.conf";

class  Location
{
    public:
        std::string location_path;
        bool    isGET;
        bool    isPOST;
        bool    isDELETE;
        std::string root;
        std::vector<std::string> s_index;
        std::string autoindex;
        std::string upload_pass;
        std::string redirection;
        std::vector<std::pair<std::string, std::string> > s_cgi_pass;

        bool M_checked;
};

class   ServerInfo
{
    public:
        std::string s_port;
        std::string host;
        std::string max_clients;
        std::string serverName;
        std::vector<Location> s_locations;
        std::vector<std::string>    locs_paths;
        std::map<std::string, std::string> errorpages;

        void    reset_info(void)
        {
            s_port.clear();
            host.clear();
            max_clients.clear();
            serverName.clear();
            s_locations.clear();
            locs_paths.clear();
            errorpages.clear();
        };
};

class ConfigInfo
{
    private:
        ServerInfo                      s_info_holder;
        std::string                     ConfigFileName;
        std::vector<std::string>        m_lines;
        std::vector<ServerInfo>         m_servers;
        std::vector<int>                m_sockets;
    public:
    // -------- public attributs ----------- //
        std::map<int, std::ofstream*>  uploadFiles;
    // -------- Construct and some member functions -----------   //
        ConfigInfo();
        ConfigInfo(std::string inputfile);
 
        // --- Methodes --- //
        void    ParsConfigFile();


        int     Init_DataServ(int index);
        int     Init_Locations(int index);
        void	Sockets_Maker(void);
        std::string& removeSpace(std::string& str);
        void    isValidToken(int start, int end, std::string validTokens, std::string input);
        bool    checkForHost(std::string host);
        bool    checkForPort(std::string Port);
        bool    is_strdigit(std::string str);
        void    CheckDupPort(void);

        ~ConfigInfo();

    // getters : get ports and servers info from config file.
        std::vector<ServerInfo>& get_m_servers(void) { return(this->m_servers); };
        std::vector<int>   get_m_sockets(void) const { return(this->m_sockets); };
    
    // Printing errors
        void print_n_exit(std::string error_str, int n);
        void print_n_exit(std::string error_str);

};

#endif