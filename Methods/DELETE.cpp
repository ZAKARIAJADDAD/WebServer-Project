/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 17:19:11 by zjaddad           #+#    #+#             */
/*   Updated: 2023/11/23 12:10:32 by zjaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/mlp_request.hpp"

bool clients_base::delete_file(std::string& root)
{
    if (access(root.c_str(), W_OK) == -1)
    {
        this->exitStatus = 403;
        this->ready = true;
        this->delerror = true;
        return false;
    }
    if (std::remove(root.c_str()) == 0)
    {
        this->ready = true;
        this->exitStatus = 204;
        return false;
    }
    this->delerror = true;
    this->exitStatus = 500;
    return false;
}

bool clients_base::delete_dir(std::string& root, ConfigInfo& config)
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
        this->exitStatus = 1;
        return true;
    }
    if (access(root.c_str(), W_OK) == -1)
    {
        this->exitStatus = 403;
        this->ready = true;
        this->delerror = true;
        return false;
    }
    (void)config;
    struct dirent *de;
    std::string fld_name;
        fld_name = root;
    DIR *dr = opendir(fld_name.c_str());
    if (dr == NULL)
    {
        this->exitStatus = 404;
        this->delerror = true;
        this->ready = true;
        return false;
    }
    this->deep += 1;
    while ((de = readdir(dr)) != NULL)
    {
        if (this->strCompare(de->d_name,  ".") == true || this->strCompare(de->d_name, "..") == true)
        {
            if (this->strCompare(de->d_name,  ".") == true)
            {
                // delete &de->d_ino;
                this->arrDe.push_back(&de->d_ino);
            }
            continue;
        }
        std::string new_root;
        std::string d_name = de->d_name;
        if (de->d_type == DT_DIR)
        {
            new_root = "";
            new_root.clear();
            if (new_root.back() != '/')
                new_root = root + d_name + "/";
            else
                new_root = root + d_name;
            d_name = "";
            d_name.clear();
            this->delete_dir(new_root, config);
            if (this->delerror)
            {   
                delete dr;
                return false;
            }
        }
        else
        {
            new_root = "";
            new_root.clear();
            new_root = root + d_name;
            d_name = "";
            d_name.clear();
            this->delete_file(new_root);
            if (this->delerror)
            {
                delete dr;
                return false;
            }
        }
    }
    this->delete_file(root);
    if (this->delerror)
    {   
        delete dr;
        return false;
    }
    this->exitStatus = 204;
    this->ready = true;
    delete dr;
    this->deep -= 1;
    return false;
}

bool clients_base::delete_method(ConfigInfo& config)
{
    std::string root = get_realpath(config);
    struct stat sb;
    if (stat(root.c_str(), &sb) == 0)
    {
        if (access(root.c_str(), W_OK) == -1)
        {
            this->exitStatus = 403;
            this->ready = true;
            return false;
        }
        if (S_ISREG(sb.st_mode))
        {
            return this->delete_file(root);
        }
        else if (S_ISDIR(sb.st_mode))
        {
            return this->delete_dir(root, config);
        }
        
    }
    else
    {
        this->exitStatus = 404;
        this->ready = true;
        return false;
    }
    this->ready = true;
    return true;
}
