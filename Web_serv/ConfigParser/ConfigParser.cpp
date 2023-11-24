#include "../../Headers/ConfigParser.hpp"

ConfigInfo::ConfigInfo()
{
    // Never use it ???
}

ConfigInfo::ConfigInfo(std::string inputfile)
    : ConfigFileName(inputfile)
{}

ConfigInfo::~ConfigInfo()
{
    // Delete Data Config ???
}

void    ConfigInfo::print_n_exit(std::string error_str, int n)
{
    std::cerr << error_str << ": Syntax error in line:" << n << "\n";
    throw std::runtime_error("");
}

void    ConfigInfo::print_n_exit(std::string error_str)
{
    std::cerr <<  error_str << std::endl;
    throw std::runtime_error("");
}

 void    ConfigInfo::ParsConfigFile(void)
{
    std::ifstream   m_configfile(this->ConfigFileName);
    if (!m_configfile.is_open())
        this->print_n_exit("Can not open the input file!");
    std::string line;
    while (std::getline(m_configfile, line))
        this->m_lines.push_back(line);
    m_configfile.close();
    int i = -1;
    while(++i < (int)this->m_lines.size())
    {
        line = this->m_lines[i];
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (line.compare(0 , line.length(), "server{") == 0 || line.compare(0 , line.length(), "server") == 0)
        {
            i = this->Init_DataServ(i);
        }
        else if (line[0] != '#' && line[0]!= ';' && !line.empty())
        {
            this->print_n_exit("Config ERROR", i + 1);
        }
    }
    if (this->m_servers.size() < 1)
        this->print_n_exit("ERROR: Empty config file");
    this->CheckDupPort();
    this->Sockets_Maker();
}

void    ConfigInfo::CheckDupPort(void)
{
    std::vector<std::string> hosts;
    std::vector<std::string> ports;
    for (size_t i = 0; i < m_servers.size(); i++)
    {
        hosts.push_back(m_servers[i].host);
        ports.push_back(m_servers[i].s_port);
    }
    for (size_t a = 0; a < hosts.size(); a++)
    {
        for (size_t b = a + 1; b < hosts.size(); b++)
        {
            if (hosts[a] == hosts[b] && ports[a] == ports[b])
            {
                m_servers.erase(m_servers.begin() + b);
                hosts.erase(hosts.begin() + b);
                ports.erase(ports.begin() + b);
                if (b <= hosts.size())
                    b--;
            }
        }
    }
}

int    ConfigInfo::Init_DataServ(int index)
{
    std::string line;
    line = this->m_lines[index];
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    if (line.compare(0 , line.length(), "server") == 0)
    {
        line = this->m_lines[++index];
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (line.compare(0 , line.length(), "{"))
            this->print_n_exit("ERROR : Invalid declaration of server!");
    }
    int bracketsOpened = 1;
    while (++index < (int)this->m_lines.size() && bracketsOpened != 0)
    {
        line = this->m_lines[index];
        line = this->removeSpace(line);
        if (line.compare(0, 10, "location /") == 0)
        {
            index = this->Init_Locations(index);
        }
        else if (line.compare(0, 7, "listen ") == 0)
        {
            if (!this->s_info_holder.host.empty() && !this->s_info_holder.s_port.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            if (line.find(":") != std::string::npos) 
            {
                this->s_info_holder.host = line.substr(7, line.find(":") - 7);
                if (!this->checkForHost(this->s_info_holder.host))
                    this->print_n_exit("ERROR: Invalid IP", index + 1);
                this->s_info_holder.s_port = line.substr(line.find(":") + 1, line.length());
            }
            else
                this->print_n_exit("ERROR: Invalid IP (Forma = host:port)", index + 1);
            if (!this->checkForPort(this->s_info_holder.s_port))
                this->print_n_exit("ERROR: Invalid port", index + 1);
        }
        else if (line.compare(0, 12, "server_name ") == 0)
        {
            if (!this->s_info_holder.serverName.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            this->s_info_holder.serverName = line.substr(12, line.length());
        }
        else if (line.compare(0, 11, "error_page ") == 0)
        {
            if (line.find_first_of(" ") != line.find_last_of(" "))
            {
                if (!this->s_info_holder.errorpages[line.substr(11, line.find_last_of(" ") - 11)].empty())
                    this->print_n_exit("ERROR : Duplicate line", index + 1);
                this->s_info_holder.errorpages[line.substr(11, line.find_last_of(" ") - 11)] = line.substr(line.find_last_of(" ") + 1);
            }
            else
                this->print_n_exit("Config ERROR", index + 1);
        }
        else if (line.compare(0, 21, "max_client_body_size ") == 0)
        {
            if (!this->s_info_holder.max_clients.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            this->s_info_holder.max_clients = line.substr(21, line.length());
            if (!this->is_strdigit(this->s_info_holder.max_clients))
                this->print_n_exit("ERROR: Invalid number (Not digit)", index + 1);
        }
        else if (line.compare(0, line.length(), "}") == 0)
            bracketsOpened--;
        else if (!line.empty() && line.compare(0, 1, "#"))
            this->print_n_exit("Config ERROR", index + 1);
    }
    index--;
    this->m_servers.push_back(this->s_info_holder);
    this->s_info_holder.reset_info();
    return (index);
}

int ConfigInfo::Init_Locations(int index)
{
    Location    location;
    location.M_checked = false;
    std::string line;
    line = this->m_lines[index];
    line = this->removeSpace(line);
    if (line.compare(0, 10, "location /") == 0)
    {
        int a = 9;
        while (line[a] && line[a] != ' ' && line[a] != '{')
            a++;
        location.location_path = line.substr(9, a - 9);

        if (line[line.length() - 1] != '{')
        {
            line = this->m_lines[index + 1];
            line = this->removeSpace(line);
            if (line.compare(0, line.length(), "{"))
                this->print_n_exit("ERROR : Invalid declaration of location.");
        }
    }
    int openedBrakets = 1;
    while (++index < (int)this->m_lines.size() && openedBrakets != 0)
    {
        line = this->m_lines[index];
        line = this->removeSpace(line);
        if (line.compare(0, 14, "allow_methods ") == 0)
        {
            if (location.M_checked == true)
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            location.M_checked = true;
            line.find("GET") != std::string::npos ? location.isGET = true : location.isGET = false;
            line.find("POST") != std::string::npos ? location.isPOST = true : location.isPOST = false;
            line.find("DELETE") != std::string::npos ? location.isDELETE = true : location.isDELETE = false;
            this->isValidToken(14, line.length(), "GET POST DELETE", line);
        }
        else if (line.compare(0, 5, "root ") == 0)
        {
            if (!location.root.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            location.root = line.substr(5, line.length());
        }
        else if (line.compare(0, 10, "autoindex ") == 0)
        {
            if (!location.autoindex.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            location.autoindex = line.substr(10, line.length());
            if ((location.autoindex != "on" && location.autoindex != "off"))
                this->print_n_exit("ERROR : ", index + 1);
        }
        else if (line.compare(0, 6, "index ") == 0)
        {
            if (location.s_index.size() != 0)
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            for (int k = 6; line[k]; k++)
            {
                int start = k;
                while (line[k] && line[k] != ' ' && line[k] != '\t')
                    k++;
                location.s_index.push_back(line.substr(start, k - start));
            }
        }
        else if (line.compare(0, 12, "upload_pass ") == 0)
        {
            if (!location.upload_pass.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            location.upload_pass = line.substr(12, line.length());
        }
        else if (line.compare(0, 9, "cgi_pass ") == 0)
        {
            int k = 9;
            while (line[k] && line[k] != ' ' && line[k] != '\t')
                k++;
            if (location.s_cgi_pass.size() != 0)
            {
                for (size_t i = 0; i < location.s_cgi_pass.size(); i++)
                {
                    if (line.substr(9, k - 9) == location.s_cgi_pass[i].first)
                        this->print_n_exit("ERROR : Duplicate line", index + 1);
                }
            }
            location.s_cgi_pass.push_back(std::make_pair(line.substr(9, k - 9), line.substr(k + 1, line.length())));
        }
        else if (line.compare(0, 7, "return ") == 0)
        {
            if (!location.redirection.empty())
                this->print_n_exit("ERROR : Duplicate line", index + 1);
            location.redirection = line.substr(7, line.length());
        }
        else if (line == "}")
            openedBrakets--;
        else if (line[0] != '#')
            this->print_n_exit("Config ERROR", index + 1);
    }
    if (location.s_index.size() == 0 && location.autoindex.empty())
            this->print_n_exit("ERROR: No autoindex in location :" + location.location_path);
    index--;
    s_info_holder.s_locations.push_back(location);
    s_info_holder.locs_paths.push_back(location.location_path);
    return (index);
}

std::string&    ConfigInfo::removeSpace(std::string& str)
{
    int i = 0;
    while (str[0] == ' ' || str[0] == '\t')
        str.erase(0, 1);
    while (str[i])
    {
        if ((str[i] == ' ' || str[i] == '\t') && (str[i + 1] == ' ' || str[i + 1] == '\t'))
        {
            str.erase(i, 1);
            continue;
        }
        i++;
    }
    if (i > 0 && (str[i - 1] == ' ' || str[i - 1] == '\t'))
        str.erase(i - 1, 1);
    return (str);
}

 void    ConfigInfo::isValidToken(int start, int end, std::string validTokens, std::string input)
{
    bool dupget = false;
    bool duppost = false;
    bool dupdelete = false;
    std::vector<std::string> tokens;
    input = input.substr(start, end - start);
    start = 0;
    std::string::size_type end2 = input.find(' ');
    std::string::size_type end3 = input.find('\t');
    std::string::size_type end1 = end2 < end3 ? end2: end3;
    while (end1 != std::string::npos)
    {
        tokens.push_back(input.substr(start, end1 - start));
        start = end1 + 1;
        end2 = input.find(' ', start);
        end3 = input.find('\t', start);
        end1 = end2 < end3 ? end2: end3;
    }
    if (start != (int)input.length())
        tokens.push_back(input.substr(start));
    validTokens.insert(0, " ");
    validTokens.insert(validTokens.length(), " ");
    std::string token;
    for (size_t a = 0; a < tokens.size(); a++)
    {
        token = " " + tokens[a] + " ";
        if (token == " POST ")
        {
            if (duppost == false)
                duppost = true;
            else if (duppost == true)
                this->print_n_exit("ERROR: 'POST' is duplicated in the config file.");
        }
        else if (token == " GET ")
        {
            if (dupget == false)
                dupget = true;
            else if (dupget == true)
                this->print_n_exit("ERROR: 'GET' is duplicated in the config file.");
        }
        else if (token == " DELETE ")
        {
            if (dupdelete == false)
                dupdelete = true;
            else if (dupdelete == true)
                this->print_n_exit("ERROR: 'DELETE' is duplicated in the config file.");
        }
        if (validTokens.find(token) == std::string::npos)
            this->print_n_exit("Invalid token '" + tokens[a] + "' on config file! ");
    }
}

//<--------------------------- Some utils -------------------------------------->

bool ConfigInfo::checkForHost(std::string host)
{
    size_t num = 0;
    size_t dot = 0;
    size_t pos = 0;
    size_t i = 0;
    for (i = 0; i < host.size(); i++)
    {
        if (host[i] == '.')
        {
            if (num > 3 || (host[pos] == '0' && (i - pos) > 1) || host.substr(pos, i - pos).empty() || std::stoul(host.substr(pos, i - pos)) > 255)
                return false;
            pos = i + 1;
            dot++;
            num = 0;
        }
        else if (isdigit(host[i]))
            num++;
        else
            return false;
        if (dot > 3)
            return false;
    }
    if (num > 3 || (host[pos] == '0' && (i - pos) > 1) || host.substr(pos, i - pos).empty() || std::stoul(host.substr(pos, i - pos)) > 255)
        return false;
    return dot == 3;
}

 bool    ConfigInfo::checkForPort(std::string Port)
{
    if (Port.length() < 4 || Port.length() > 5)
        return false;
    if (std::atoi(Port.c_str()) > 65535 || std::atoi(Port.c_str()) < 1024)
        return false;
    for (size_t i = 0; i < Port.size(); i++)
    {
        if (!isdigit(Port[i]))
            return false;
    }
    return true;
}

bool    ConfigInfo::is_strdigit(std::string str)
{
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}