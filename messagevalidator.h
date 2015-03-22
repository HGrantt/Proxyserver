#ifndef MESSAGEVALIDATOR_H
#define MESSAGEVALIDATOR_H

#include <vector>
#include <string>
#include <cctype>

class MessageValidator
{
public:
    MessageValidator();

    //Add word to the filter
    void AddInvalidWord(std::string const& word);

    //Cleaning upp the header for simpler message
    std::string FixRequestHeader(std::string message);

    //Get field from request
    std::string GetField(std::string const& message, std::string what);

    //Reassembles the request
    std::string GetNewRequest(std::string const& message);

    //Checks if request is valid
    bool ValidRequest(std::string const& message);

    //Checks if response is valid
    bool ValidResponse(std::string const& message);

private:
    std::string GetHTMLFileContentsFromResponse(std::string const& message);
    std::string GetURLFieldFromRequest(std::string const& message);
    bool InappropriateContent(std::string const& content);
    bool ResponseIsHTML(std::string const& message);
    std::string ToLowerCase(std::string str);

    std::vector<std::string> wordlist;
};

#endif
