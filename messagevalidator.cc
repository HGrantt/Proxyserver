#include "messagevalidator.h"
#include <iostream>

//----------Public:----------


MessageValidator::MessageValidator() : wordlist() {}

void MessageValidator::AddInvalidWord(std::string const& word)
{
    wordlist.push_back(ToLowerCase(word));
}

std::string MessageValidator::FixRequestHeader(std::string message)
{
    std::string newmessage = ToLowerCase(message);
    size_t beginpos = newmessage.find("http://");
    if (beginpos != std::string::npos)
    {
      size_t endpos = newmessage.find("/", beginpos+8);
      message.erase(beginpos, endpos-beginpos);
    }
    
    return message;
}

std::string MessageValidator::GetField(std::string const& message, std::string what)
{
    size_t first;
    size_t last;
    std::string lower = ToLowerCase(message);
    first = lower.find(ToLowerCase(what));
    
    if (first != std::string::npos)
    {
	std::string crlf = {0x0d, 0x0a};
	last = lower.find(crlf, first) + 2;
	return message.substr(first, last - first);
    }
    return "";
}

std::string MessageValidator::GetNewRequest(std::string const& message)
{
    std::string crlf = {0x0d, 0x0a};
    return GetField(message, "GET") + GetField(message, "Host:") + "Connection: close" + crlf + crlf;
    //GetField(message, "Connection:") + crlf;
}

bool MessageValidator::ValidRequest(std::string const& message)
{
    std::string storedmessage = ToLowerCase(message);
    std::string urlfield = GetURLFieldFromRequest(storedmessage);
    if (InappropriateContent(urlfield))
    {
        return false;
    }
    return true;
}

bool MessageValidator::ValidResponse(std::string const& message)
{
    std::string storedmessage = ToLowerCase(message);
    if (ResponseIsHTML(storedmessage))
    {
        std::string htmlcontent = GetHTMLFileContentsFromResponse(storedmessage);
        if (InappropriateContent(htmlcontent))
        {
            return false;
        }
    }
    return true;
}


//----------Private:----------


std::string MessageValidator::GetHTMLFileContentsFromResponse(std::string const& message)
{
    size_t beginpos = message.find({0x0d, 0x0a, 0x0d, 0x0a});
    size_t endpos = message.find("</html>", beginpos);
    return message.substr(beginpos, endpos - beginpos + 7);
}

std::string MessageValidator::GetURLFieldFromRequest(std::string const& message)
{
    size_t beginpos = message.find("/");
    size_t endpos = message.find(" ", beginpos);
    return message.substr(beginpos, endpos - beginpos);
}

bool MessageValidator::InappropriateContent(std::string const& content)
{
    std::vector<std::string>::const_iterator it;
    for (it = wordlist.begin(); it != wordlist.end(); ++it)
    {
        if (content.find(*it) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

bool MessageValidator::ResponseIsHTML(std::string const& message)
{
    size_t beginpos = message.find("content-type:");
    std::string contenttype = message.substr(beginpos, 19);
    if (contenttype.find("text") == std::string::npos)
    {
        return false;
    }
    return true;
}

std::string MessageValidator::ToLowerCase(std::string str)
{
    for (unsigned int charindex = 0; charindex < str.length(); ++charindex)
    {
        str[charindex] = tolower(str[charindex]);
    }
    return str;
}
