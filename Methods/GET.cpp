/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 15:13:46 by zjaddad           #+#    #+#             */
/*   Updated: 2023/11/22 17:33:20 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"

void clients_base::extension_map()
{
    std::string s = ".aac .abw .arc .avif .avi .azw .bin .bmp .bz .bz2 .cda .csh .css .csv .doc .docx .eot \
     .epub .gz .gif .htm .html .ico .ics .jar .jpeg .jpg .js .json .jsonld .mid .midi .mjs .mp3 .mp4 .mpeg \
     .mpkg .odp .ods .odt .oga .ogv .ogx .opus .otf .png .pdf .php .ppt .pptx .rar .rtf .sh .svg .tar .tif .tiff \
     .ts .ttf .txt .vsd .wav .weba .webm .webp .woff .woff2 .xhtml .xls .xlsx .xml .xul .zip .3gp .3g2 .7z";
     
    std::string mimtype = "audio/aac application/x-abiword application/x-freearc image/avif \
    video/x-msvideo application/vnd.amazon.ebook application/octet-stream image/bmp application/x-bzip \
    application/x-bzip2 application/x-cdf application/x-csh text/css text/csv application/msword \
    application/vnd.openxmlformats-officedocument.wordprocessingml.document application/vnd.ms-fontobject \
    application/epub+zip application/gzip image/gif text/html text/html image/vnd.microsoft.icon \
    text/calendar application/java-archive image/jpeg image/jpeg text/javascript application/json \
    application/ld+json audio/midi audio/x-midi text/javascript audio/mpeg video/mp4 video/mpeg \
    application/vnd.apple.installer+xml application/vnd.oasis.opendocument.presentation application/vnd.oasis.opendocument.spreadsheet \
    application/vnd.oasis.opendocument.text audio/ogg video/ogg application/ogg audio/opus font/otf \
    image/png application/pdf application/x-httpd-php application/vnd.ms-powerpoint application/vnd.openxmlformats-officedocument.presentationml.presentation \
    application/vnd.rar application/rtf application/x-sh image/svg+xml application/x-tar image/tiff image/tiff \
    video/mp2t font/ttf text/plain application/vnd.visio audio/wav audio/webm video/webm image/webp \
    font/woff font/woff2 application/xhtml+xml application/vnd.ms-excel application/vnd.openxmlformats-officedocument.spreadsheetml.sheet \
    application/xml application/vnd.mozilla.xul+xml application/zip video/3gpp video/3gpp2 application/x-7z-compressed";
    
    std::stringstream ss(s);
    std::stringstream sss(mimtype);
    std::string word;
    std::string value;
    while (ss >> word && sss >> value)
            this->map_extens[word] = value;
    ss.clear();
    sss.clear();
	ss.str("");
	sss.str("");
}

bool clients_base::open_file(std::string &str)
{
    std::ifstream tmp(str);
    if (tmp.is_open())
    {
         struct stat sb;
          std::streampos length;
         if (stat(str.c_str(), &sb) == 0)
            length = sb.st_size;
        this->inf = new std::ifstream(str);

        std::string header = "HTTP/1.1 200 OK\r\n";
        header += "Content-Type: " + this->map_extens[this->extension] + "\r\n";
        header += "Content-Length: " + this->ft_to_string(length) + "\r\n";
        header += "\r\n";
        this->final_respons = header;
        this->ready = true;
        this->is_sending = true;
        return true;
    }
    else
    {
        this->exitStatus = 404;
        this->ready = true;
        return false;
    }
}

bool clients_base::is_file(std::string& str, ConfigInfo& config)
{
    std::string file_name = str.substr(str.find_last_of("/") + 1);
    size_t found = file_name.find_last_of(".");
    if (found != std::string::npos)
        this->extension = file_name.substr(found);
    if (!config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass.empty() && found != std::string::npos)
    {        
        if (this->extension == ".php" || this->extension == ".py")
        {
            for (size_t i = 0; i < config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass.size(); i++)
            {
                std::string m_extension = this->extension.substr(1);
                if (config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_cgi_pass[i].first == m_extension)
                    return this->call_Cgi(config, str);
            }
        }
        return this->open_file(str);
    }
    return this->open_file(str);
}

bool clients_base::is_autoindex_on(std::string& str)
{
    struct dirent *de;
    std::string fld_name;
        fld_name = str;
    this->dr = opendir(fld_name.c_str());
    if (this->dr == NULL)
    {
        this->exitStatus = 404;
        return false;
    }
    while ((de = readdir(this->dr)) != NULL)
    {
        if (this->strCompare(de->d_name,  ".") == true || this->strCompare(de->d_name, "..") == true)
        {
            if (this->strCompare(de->d_name,  ".") == true)
            {
                delete &de->d_ino;
            }
            continue;
        }
        bool isDir = de->d_type == DT_DIR;
        this->dir_content.push_back(std::make_pair (de->d_name, isDir));
    }
    std::sort(this->dir_content.begin(), this->dir_content.end());
    std::ofstream tmp(("dir" + this->m_id +".html").c_str());
    if (tmp.is_open())
    {
        this->outf = new std::ofstream(("dir" + this->m_id +".html").c_str());
        std::string html_content;
        tmp << "<html><head>";
        tmp << "<style>";
        tmp << "body { font-family: Arial, sans-serif; }";
        tmp << ".container { max-width: 800px; margin: 0 auto; padding: 20px;";
        tmp << "background-color: #f0f0f0;"; // Add your background color here
        tmp << "border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
        tmp << "ul { list-style: none; padding: 0; }";
        tmp << "li { margin: 5px 0; }";
        tmp << "a { text-decoration: none; color: #333; }";
        tmp << "a:hover { color: #0066cc; }";
        tmp << "h1 { text-align: center; }";
        tmp << "</style>";
        tmp << "</head>";
        tmp << "<body>";
        tmp << "<div class='container'>";
        tmp << "<h1>Directory Listing</h1>";
        tmp << "<ul>";
        std::string link;
        for(std::vector<std::pair <std::string, bool > >::iterator it = this->dir_content.begin(); it != this->dir_content.end(); it++)
        {
            std::string url =  this->request_url  + it->first ;
            for (size_t i = 0; i < url.size(); i++)
            {
                while (url[i + 1] && url[i] == '/' && url[i + 1] == '/')
                    url.erase(i, 1);
            }
            if (it ->second == true)
                url += "/";
            link = "<a href='" + url + "'>" + it ->first + "</a>";
            html_content += "<li>" + link + "</li>";
        }
        html_content += "</ul></body></html>";
        tmp << html_content;
        tmp.close();
        std::ifstream tmp(("dir" + this->m_id +".html").c_str());
        tmp.seekg(0, std::ios::end);
        std::streampos length = tmp.tellg();
        tmp.close();
        this->inf = new std::ifstream(("dir" + this->m_id +".html").c_str());
        std::string header = "HTTP/1.1 200 OK\r\n";
        header += "Content-Type: text/html\r\n";
        header += "Content-Length:" + this->ft_to_string(length) + "\r\n";
        header += "\r\n";
        this->final_respons = header;
        this->ready = true;
        this->is_sending = true;
        if (this->dr != NULL)
        {
            
            delete this->dr;
            this->dr = NULL;
        }

        this->dir_content.clear();
        return true;
    }
    else
    {
        this->exitStatus = 404;
        this->ready = true;
        return false;
    }
    return true;
}

bool clients_base::is_directory(ConfigInfo& config, std::string& str)
{
    std::string path;
    if (this->request_url.back() != '/')
    {
        std::string header = "HTTP/1.1 301 Moved Permanently\r\n";
        header += "Location: " + this->request_url + "/" + "\r\n";
        header += "Content-Type: text/html\r\n";
        header += "Content-Length: 0\r\n";
        header += "\r\n";
        this->final_respons = header;
        this->ready = true;
        this->exitStatus = 1;
        return true;
    }
    else if (!config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_index.empty())
    {
        path = str + config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_index[0];
        if (access(path.c_str(), F_OK) == -1)
        {
            this->exitStatus = 404;
            this->ready = true;
            return false;
        }
        if (access(path.c_str(), R_OK) == -1)
        {
            this->exitStatus = 403;
            this->ready = true;
            return false;
        }
        return this->is_file(path, config);
    }
    else if (config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].autoindex == "on")
    {
        return this->is_autoindex_on(str);
    }
    else if (config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].s_index.empty() && \
    (config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].autoindex.empty() || config.get_m_servers()[this->server_id].s_locations[this->id_match_loc].autoindex == "off"))
    {
        this->exitStatus = 403;
        this->ready = true;
        return false;
    }
    return true;
}

bool clients_base::get_method(ConfigInfo& config)
{
    if (this->executed)
        return this->call_Cgi(config, this->runfile);
    std::string root;
    root = get_realpath(config);
    struct stat sb;
    if (stat(root.c_str(), &sb) == 0)
    {
        if (access(root.c_str(), R_OK) == -1)
        {
            this->exitStatus = 403;
            this->ready = true;
            return false;
        }
        this->extension_map();
        if (S_ISREG(sb.st_mode))
        {
            return this->is_file(root, config);
        }
        else if (S_ISDIR(sb.st_mode))
        {
            return this->is_directory(config, root);
        }
    }
    else
    {
        this->exitStatus = 404;
        this->ready = true;
        return false;
    }
    return true;
}
