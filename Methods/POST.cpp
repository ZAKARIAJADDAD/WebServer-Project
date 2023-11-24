/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 17:19:54 by zjaddad           #+#    #+#             */
/*   Updated: 2023/11/22 17:33:35 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"
#include <unistd.h>

void    clients_base::fill_post_respons(void)
{
    std::string str = "html_errors/201.html";
    std::ifstream fd(str);
    if (fd.is_open())
    {
        this->inf = new std::ifstream(str);
        std::string header = "HTTP/1.1 201 Created\r\n";
        header += "Content-Type: html\r\n";
        header += "Content-Length: ";
        struct stat buf;
        stat(str.c_str(), &buf);
        std::stringstream ss;
        ss << buf.st_size;
        header += ss.str();
        header += "\r\n\r\n";
        this->final_respons = header;
    }
}

std::string    clients_base::generate_filename(void)
{
    std::string filename = p_realpath;
    std::string extension = "";
    std::string content_type = this->map["Content-Type"];
    if (!content_type.empty())
        extension = this->post_extens[content_type];
    struct stat buf;
    stat(filename.c_str(), &buf);
    if (S_ISDIR(buf.st_mode))
    {
        if (filename.back() == '/')
            filename += "uploadfile";
        else
            filename += "/uploadfile";
    }
    else if (S_ISREG(buf.st_mode))
    {
        if (filename.find(".") != std::string::npos)
            filename = filename.substr(0, filename.find_last_of("."));
    }
    else
    {
        if (filename.find(".") != std::string::npos)
            filename = filename.substr(0, filename.find_last_of("."));
    }
    int i = 1;
    while (this->fileExists(filename + extension))
    {
        std::stringstream oss;
        oss << i;
        if (i > 1)
            filename.pop_back();
        if (i > 10)
            filename.pop_back();
        if (i > 100)
            filename.pop_back();
        if (i > 1000)
            filename.pop_back();
        filename += oss.str();
        i++;
    }
    return (filename + extension);
}

void    clients_base::save_body_content(ConfigInfo& config)
{
    std::string tmp;
    tmp = this->p_buffer;
    if (this->first_time == true)
    {
        this->p_savebody = tmp.substr(this->startbody + 1);
        this->byteswritten = this->p_savebody.size();
        this->first_time = false;
    }
    else
    {
        this->p_savebody = tmp;
        this->byteswritten += this->p_savebody.size();
    }
    if (config.uploadFiles.find(this->client_id) != config.uploadFiles.end() && config.uploadFiles[this->client_id] != NULL)
    {
        if (this->fileExists(this->upFpath) && config.uploadFiles[this->client_id]->good())
            config.uploadFiles[this->client_id]->write(this->p_savebody.c_str(), this->p_savebody.size());
        else
        {
            this->exitStatus = 404;
            this->ready = true;
            return;
        }
    }
    std::string content_length = this->map["Content-Length"];
    if ((content_length != "0" && std::atoi(content_length.c_str()) == this->byteswritten))
    {
        this->isuploaded = true;
        this->ready = true;
    }
}

void    clients_base::save_chunked_body(void)
{
    std::string tmp = this->p_buffer;
    std::string chunked_content;
    if (this->byteswritten == this->hexa_bytes)
    {
        std::string hexa = "";
        int counter = 0;
        size_t i;
        if (this->startbody > 2)
            this->startbody -= 2;
        for (i = this->startbody; counter < 2 && i < tmp.size(); i++)
        {
            if ((int)i > this->startbody && tmp[i] == '\n' && tmp[i - 1] == '\r')
                counter++;
            if (counter == 1 && tmp[i] != '\n' && tmp[i] != '\r')
                hexa += tmp[i];
        }
        chunked_content = tmp.substr(i, tmp.size() - i);
        std::stringstream convert(hexa);
        int hexadec;
        convert >> std::hex >> hexadec; 
        this->hexa_bytes += hexadec;
        if (hexadec < (int)chunked_content.size())
        {
            std::string last_thing = chunked_content.substr(hexadec);
            chunked_content = chunked_content.substr(0, hexadec);
            if (last_thing == "\r\n0\r\n\r\n")
            {
                this->isuploaded = true;
                this->ready = true;
            }
        }
        if (hexadec == 0)
        {
            this->isuploaded =true;
            this->ready = true;
        }
        this->byteswritten += chunked_content.size();
        this->startbody = 0;
    }
    else
    {
        this->byteswritten += tmp.size();
        chunked_content = tmp;
    }
    if (this->hexa_bytes < this->byteswritten + this->read_size)
    {
        this->read_size = this->hexa_bytes - this->byteswritten;
        if (this->read_size < 1)
            this->read_size = 1024;
    }
    else if (this->read_size != 1024)
        this->read_size = 1024;
    if (p_config->uploadFiles.find(this->client_id) != p_config->uploadFiles.end() && p_config->uploadFiles[this->client_id] != NULL)
    {
        if (this->fileExists(this->upFpath) && p_config->uploadFiles[this->client_id]->good())
            p_config->uploadFiles[this->client_id]->write(chunked_content.c_str(), chunked_content.size());
        else
        {
            this->exitStatus = 404;
            this->ready = true;
            return;
        }
    }
}

// Post method !! main function
bool    clients_base::post_method(ConfigInfo& config, char* buffer)
{
    std::string buff(buffer, this->bytesRead);
    // struct stat buf;
    this->p_buffer = buff;
    if (this->first_init == true)
    {
        this->p_location = config.get_m_servers()[this->server_id].s_locations[this->id_match_loc];
        this->p_realpath = this->get_realpath(config);
        this->iscontent = false;
        this->ischunked = false;
        if (!this->map["Content-Length"].empty())
            this->iscontent = true;
        if (!this->map["Transfer-Encoding"].empty())
            this->ischunked = true;
        this->isuploaded = false;
        this->first_init = false;
        this->is_cgipost = false;
        this->post_extension();
        this->isup_pass = this->check_uplaod_pass();
        if (this->isup_pass == false)
        {
            std::string tmp = this->p_realpath;
            this->p_realpath = "content_holder";
            this->upFpath = this->generate_filename();
            this->p_realpath = tmp;
        }
        else
            this->upFpath = this->generate_filename();
        if (config.uploadFiles.find(this->client_id) == config.uploadFiles.end())
            config.uploadFiles[this->client_id] = new std::ofstream(this->upFpath, std::ios::app);
        if (this->isup_pass == false)
        {
            if (!this->check_request_content())
            {
                this->exitStatus = 403;
                this->ready = true;
                return false;
            }
            this->is_cgipost = true;
        }
    }
    if ((this->isup_pass || this->is_cgipost) && this->isuploaded == false)
    {
        if (this->iscontent && this->ischunked == false)
            this->save_body_content(config);
        else if (this->ischunked)
            this->save_chunked_body();
        if (this->exitStatus != 0)
            return false;
    }
    else if ((!this->isup_pass && !this->is_cgipost))
    {
        this->ready = true;
        this->exitStatus = 404;
        return false;
    }
    if (this->isuploaded && this->isup_pass)
        this->fill_post_respons();
    else if (this->isuploaded && this->is_cgipost)
    {
        if (config.uploadFiles.find(this->client_id) != config.uploadFiles.end())
        {
            if (config.uploadFiles[this->client_id]->is_open())
                config.uploadFiles[this->client_id]->close();
        }
        this->POST_cgi(config, this->runfile);
    }
    return true;
}


bool    clients_base::check_uplaod_pass(void)
{
    if (this->p_location.upload_pass.empty())
        return false;
    char* holder1;
    char* holder2;
    std::string uploadpass = this->p_location.upload_pass;
    std::string realpathurl = this->p_realpath;
    struct stat buf1, buf2;
    if (stat(uploadpass.c_str(), &buf1) == 0)
    {
        if (S_ISREG(buf1.st_mode))
            uploadpass = uploadpass.substr(0, uploadpass.find_last_of("/"));
        holder1 = realpath(uploadpass.c_str(), NULL);
        uploadpass = holder1;
        if (access(uploadpass.c_str(), W_OK) == -1)
        {
            delete holder1;
            return false;
        }
    }
    if (stat(realpathurl.c_str(), &buf2) == 0)
    {
        if (S_ISREG(buf2.st_mode))
            realpathurl = realpathurl.substr(0, realpathurl.find_last_of("/"));
        holder2 = realpath(realpathurl.c_str(), NULL);
        realpathurl = holder2;
        bool isequel = uploadpass == realpathurl;
        delete holder1;
        delete holder2;
        return (isequel);
    }
    else
    {
        realpathurl = realpathurl = realpathurl.substr(0, realpathurl.find_last_of("/"));
        if (stat(realpathurl.c_str(), &buf2) != 0)
            return (false);
        holder2 = realpath(realpathurl.c_str(), NULL);
        realpathurl = holder2;
        bool isequal = uploadpass == realpathurl;
        delete holder1;
        delete holder2;
        return (isequal);
    }
    return (false);
}

bool    clients_base::post_directory(void)
{
    if (this->request_url.back() != '/')
    {
        std::string header = "HTTP/1.1 301 Moved Permanently\r\n";
        header += "Location: " + this->request_url + "/" + "\r\n";
        header += "Content-Type: text/html\r\n";
        header += "Content-Length: 0\r\n";
        header += "\r\n";
        this->final_respons = header;
        this->ready = true;
        return true;
    }
    else if (!this->p_location.s_index.empty() && !this->p_location.s_cgi_pass.empty())
    {
        for (size_t i = 0; i < this->p_location.s_index.size(); i++)
        {
            if (access(this->p_location.s_index[i].c_str(), R_OK) == -1)
            {
                this->exitStatus = 404;
                this->ready = true;
                return false;
            }
            std::string m_extension = "";
            if (this->p_location.s_index[i].find(".") != std::string::npos)
                m_extension = this->p_location.s_index[i].substr(this->p_location.s_index[i].find_last_of(".") + 1);
            if (m_extension != "php" || m_extension != "py")
                continue;
            for (size_t j = 0; j < this->p_location.s_cgi_pass.size(); j++)
            {
                if (m_extension == this->p_location.s_cgi_pass[i].first)
                {
                    this->runfile = this->p_location.s_index[i];
                    this->extension = "." + m_extension;
                    return true;
                }
            }
        }
    }
    return false;
}

bool    clients_base::check_request_content()
{
    struct stat buffer;
    if(stat(this->p_realpath.c_str(), &buffer) == 0)
    {
        if (access(this->p_realpath.c_str(), R_OK) == -1)
        {
            this->exitStatus = 403;
            this->ready = true;
            return false;
        }
        if (S_ISDIR(buffer.st_mode))
            return this->post_directory();
        if (S_ISREG(buffer.st_mode))
        {
            std::string m_extension = "";
            if (this->p_realpath.find_last_of("."))
                m_extension = this->p_realpath.substr(this->p_realpath.find_last_of(".") + 1);
            if ((!m_extension.empty() && !this->p_location.s_cgi_pass.empty()) && (m_extension == "php" || m_extension == "py"))
            {
                for (size_t a = 0; a < this->p_location.s_cgi_pass.size(); a++)
                {
                    if (m_extension == this->p_location.s_cgi_pass[a].first)
                    {
                        this->runfile = this->p_realpath;
                        this->extension = "." + m_extension;
                        return true;
                    }
                }
            }
            this->exitStatus = 403;
            this->ready = true;
            return false;
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

bool clients_base::fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void clients_base::post_extension(void)
{
    std::string contentTypes = "audio/aac application/x-abiword application/x-freearc image/avif video/x-msvideo \
        application/vnd.amazon.ebook application/octet-stream image/bmp application/x-bzip \
        application/x-bzip2 application/x-cdf application/x-csh text/css text/csv application/msword \
        application/vnd.openxmlformats-officedocument.wordprocessingml.document application/vnd.ms-fontobject \
        application/epub+zip application/gzip image/gif text/html image/vnd.microsoft.icon \
        text/calendar application/java-archive image/jpeg application/javascript application/json \
        application/ld+json audio/midi audio/x-midi audio/mpeg video/mp4 video/mpeg \
        application/vnd.apple.installer+xml application/vnd.oasis.opendocument.presentation application/vnd.oasis.opendocument.spreadsheet \
        application/vnd.oasis.opendocument.text audio/ogg video/ogg application/ogg audio/opus font/otf \
        image/png application/pdf application/x-httpd-php application/vnd.ms-powerpoint application/vnd.openxmlformats-officedocument.presentationml.presentation \
        application/vnd.rar application/rtf application/x-sh image/svg+xml application/x-tar image/tiff image/tiff \
        video/mp2t font/ttf text/plain application/vnd.visio audio/wav video/webm image/webp \
        font/woff font/woff2 application/xhtml+xml application/vnd.ms-excel application/vnd.openxmlformats-officedocument.spreadsheetml.sheet \
        application/xml application/vnd.mozilla.xul+xml application/zip video/3gpp video/3gpp2 application/x-7z-compressed";
    
    std::string extensions = ".aac .abw .arc .avif .avi .azw .bin .bmp .bz .bz2 .cda .csh .css .csv .doc .docx .eot \
        .epub .gz .gif .html .ico .ics .jar .jpeg .js .json .jsonld .mid .midi .mp3 .mp4 .mpeg \
        .mpkg .odp .ods .odt .oga .ogv .ogx .opus .otf .png .pdf .php .ppt .pptx .rar .rtf .sh .svg .tar .tif .tiff \
        .ts .ttf .txt .vsd .wav .webm .webp .woff .woff2 .xhtml .xls .xlsx .xml .xul .zip .3gp .3g2 .7z";
    
    std::stringstream contentTypeStream(contentTypes);
    std::stringstream extensionStream(extensions);
    
    std::string contentType;
    std::string extension;
    
    while (contentTypeStream >> contentType && extensionStream >> extension) {
        this->post_extens[contentType] = extension;
    }
}



clients_base::~clients_base() {};