#include <iostream>
#include <vector>
#include "../includes/ccpotify.h"
#include "../includes/cppgmail.h"

#define CLIENT_SECRET   "client_secret.txt"
#define CLIENT_ID       "client_id.txt"
#define AUTH64          "auth64.txt"
#define GMAIL_CLIENT    "gmail_client.txt"
#define GMAIL_SECRET    "gmail_secret.txt"
#define GMAIL_REFRESH   "gmail_refresh.txt"

using json = nlohmann::json;

const std::string storeDaylist(const json& jsonItem)
{
    std::string listItems {};
    for (int i = 0; i < 10; ++i)
    {
        listItems += std::to_string(i + 1) + ". "
                  + jsonItem["tracks"]["items"][i]["track"]["name"].get<std::string>() 
                  + " - " 
                  + jsonItem["tracks"]["items"][i]["track"]["artists"][0]["name"].get<std::string>() 
                  + "\n";
    }
    return listItems;
}

int main()
{

    Reader reader; // Reader class object to hide file i/o code
    const std::string clientSecret  {reader.getText(CLIENT_SECRET)};
    const std::string clientId      {reader.getText(CLIENT_ID)};
    const std::string auth64        {reader.getText(AUTH64)};
    const std::string gmailClient   {reader.getText(GMAIL_CLIENT)};
    const std::string gmailSecret   {reader.getText(GMAIL_SECRET)};
    json gmail_access_J {};
    json daylist_J {};

    //Call spotify api to get daylist
    CCpotify ccpotify {clientId, clientSecret, auth64};
    ccpotify.setAccessToken(ccpotify.getAccessToken());

    daylist_J = ccpotify.getPlaylist("https://api.spotify.com/v1/playlists/37i9dQZF1EP6YuccBxUcC1");
    daylist_J = ccpotify.getPlaylist("https://api.spotify.com/v1/playlists/37i9dQZF1EP6YuccBxUcC1"); //Maybe calling twice will fix the weird error where 
                                                                                                    //daylist has no name

    const std::string daylistName {daylist_J["name"]};

    //Call gmail to send email as text message
    Cppgmail cppgmail {gmailClient, gmailSecret};
    cppgmail.setAccessToken(cppgmail.getAccessToken());

    std::string mimeFormat = {reader.getText("toFrom.txt")};

    mimeFormat += "Subject: " + daylistName + "\n\n";

    mimeFormat += storeDaylist(daylist_J);

    cppgmail.sendMessage(mimeFormat);

}