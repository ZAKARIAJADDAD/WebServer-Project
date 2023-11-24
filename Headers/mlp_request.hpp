/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlp_request.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 16:15:43 by enja              #+#    #+#             */
/*   Updated: 2023/11/23 11:28:26 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

#include "includes.hpp"
#include "ConfigParser.hpp"

class clients_base {

        public:
                clock_t start;
                //---- Post attributes -----
                ConfigInfo*     p_config;
		Location        p_location;
                std::string     upFpath;
                std::string     p_buffer;
                std::string	p_realpath;
                std::string     p_savebody;
                std::string     hexaholder;
                std::string     runfile;
                int     byteswritten;
                int     hexa_bytes;
                int     read_size;
                bool    first_time;
                bool    iscontent;
                bool    ischunked;
		bool	isuploaded;
                bool    first_init;
                bool    isup_pass;
                bool    is_cgipost;
                bool    findbody;
                bool    whilecgi;
                bool    is_deleting;
                bool    delerror;
                std::string m_id;

                //---- Get attributes -----
                pid_t pid;
		int client_id;
		int	status;
                std::string query;
                std::ifstream* inf;
                std::ifstream* inf_cgi;
                std::ofstream* outf;
		std::filebuf*  pbuf;
                int file_len;
                int server_id;
                int id_match_loc;
                int startbody;
                int exitStatus, counter;
                bool ready;
                bool is_sending;
                bool valid;
                bool first;
                bool parsed;
                bool post_sig;
                ssize_t bytesRead;
                bool body;
                bool executed;
                std::string request;
                bool check;
                bool header;
                DIR* dr;

                int deep;

        public:
                std::string request_method;
                std::string request_url;
                std::string request_protocol;
                std::string request_host;
                std::string final_respons;
                std::string matchlocation;
                std::string extension;
        public:
                std::vector<std::string> req_lines;
                std::map<std::string, std::string> map;
                std::map<std::string, std::string> map_extens;
                std::vector<std::pair <std::string, bool> > dir_content;
                std::map<std::string, std::string> cgiEnv;
                std::vector<ino_t *> arrDe;

                std::map<std::string, std::string> post_extens;
        public:
                
                std::string check_headers(std::string& buffer);
                bool checkSignalErrors(clients_base& client, std::string &clnt);
                bool strCompare(std::string, std::string);
                bool allowdedChar(std::string &url);
                bool matchingLocation(ConfigInfo &config, char *buffer);
                bool is_file(std::string& str, ConfigInfo& config);
                bool is_directory(ConfigInfo& config, std::string& str);
                bool is_autoindex_on(std::string& str);
                bool open_file(std::string &str);
                

                bool get_method(ConfigInfo& config);
                
                bool delete_method(ConfigInfo& config);
                bool delete_file(std::string& root);
                bool delete_dir(std::string& root, ConfigInfo& config);
                
                std::string get_realpath(ConfigInfo& config);
                void        extension_map();
                void        get_CgiEnv(ConfigInfo& config, std::string& file);
                bool	    call_Cgi(ConfigInfo& config, std::string& file);

                bool	the_welcome_index(clients_base &client);

                //----- POST methods functions --------
                bool    post_method(ConfigInfo& config, char* buffer);
                std::string	generate_filename(void);
                void    post_extension(void);
                void    fill_post_respons(void);

                bool    check_uplaod_pass(void);
                bool    fileExists(const std::string& filename);
		bool    check_request_content(void);
                bool    post_directory(void);

                void    save_body_content(ConfigInfo& config);
                void    save_chunked_body(void);
                bool    POST_cgi(ConfigInfo& config, std::string& file);
                clients_base(ConfigInfo& config) : p_config(&config) {};
                ~clients_base();
                

                static void error_generator();
                std::string	ft_to_string(int str);
};

class Webserv{

            private:

                    std::vector<clients_base> clients;
                    std::vector<int>           sockets;
                    fd_set rfds, wfds, erfds;
                    int file;
                    std::map<int, std::string> Errors;

            public:
                    
                    Webserv();
                    ~Webserv();
                    void    MultiPlexer();
                    void    MultiPlexer(ConfigInfo &configinfo);
                    void    pars_request(clients_base& client, ConfigInfo &configinfos);
                    void    Request_ErrorHandler(clients_base& client);
                    void    DropClient(int i);
                    void    ReInitialize_fds();
                    int    Accept_clietns(ConfigInfo& config);
};