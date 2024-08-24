#include <iostream>
#include <vector>
#include "../includes/cpr/cpr.h"
#include "../includes/readTxt.h"
#include "../includes/nlohmann/json.hpp"
#include "../includes/base64/base64.hpp"

#define CLIENT_SECRET   "client_secret.txt"
#define CLIENT_ID       "client_id.txt"
#define REFRESH_TOKEN   "refresh.txt"
#define AUTH64          "auth64.txt"
#define GMAIL_CLIENT    "gmail_client.txt"
#define GMAIL_SECRET    "gmail_secret.txt"
#define GMAIL_REFRESH   "gmail_refresh.txt"

using json = nlohmann::json;

const std::string& getAuthCode(std::string& authCode)
{
    std::cout << "Please enter provided authCode: ";
    std::cin >> authCode;
    return authCode;
}

const std::string& storeDaylist(const json& jsonItem, std::string& listItems)
{
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
    const std::string playlist      {"https://api.spotify.com/v1/me/playlists"};
    const std::string token         {"https://accounts.spotify.com/api/token"};
    const std::string authorize     {"https://accounts.spotify.com/authorize"};
    const std::string gmailAuth     {"https://accounts.google.com/o/oauth2/auth"};
    const std::string gmailToken    {"https://oauth2.googleapis.com/token"};
    const std::string gmailSend     {"https://gmail.googleapis.com/gmail/v1/users/me/messages/send"};
    const std::string redirect      {"http://localhost:8080"};

    Reader reader; // Reader class object to hide file i/o code
    const std::string clientSecret  {reader.getText(CLIENT_SECRET)};
    const std::string clientId      {reader.getText(CLIENT_ID)};
    const std::string auth64        {reader.getText(AUTH64)};

    std::string refresh             {reader.getText(REFRESH_TOKEN)};
    std::string accessToken{};

    const std::string gmailClient   {reader.getText(GMAIL_CLIENT)};
    const std::string gmailSecret   {reader.getText(GMAIL_SECRET)};

    std::string gmailRefresh        {reader.getText(GMAIL_REFRESH)};
    std::string gmailAccess         {};

    json gmail_access_J {};
    json spotify_token_J {};
    json daylist_J {};
    json spotify_refresh_J {};

    // Basically all of the above could exist as #defines or as local variables farther down
    // no need to globalize throughout main 
    // URLs are a prime candidate for #defines client secrets and IDs only need to stay around for their respective 
    // API calls and then they can dip

    if (refresh.empty())
    {
        cpr::Response spotifyAuth = cpr::Get(cpr::Url{authorize},
                        cpr::Parameters{{"response_type","code"},
                                        {"client_id", clientId},
                                        {"redirect_uri", redirect},
                                        {"scope", "playlist-read-private"},
                                        {"show_dialog", "true"}},
                        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}});

        std::cout << spotifyAuth.url << std::endl;                                                           //1. Use this URL to authenticate for the first time

        std::string authCode{};
        authCode = getAuthCode(authCode);                                                          //2. After being redirected, copy and paste code from URL into the terminal

        cpr::Response spotifyToken = cpr::Post(cpr::Url{token},                                      //3. New Request created using the code pasted in 2
                        cpr::Parameters{{"grant_type", "authorization_code"},
                                        {"code", authCode},
                                        {"redirect_uri",redirect}},
                        cpr::Header{{"Authorization", "Basic " + auth64},
                                    {"Content-Type", "application/x-www-form-urlencoded"}});

        spotify_token_J = json::parse(spotifyToken.text);                                          //4. Access Token and refresh token saved to dedicated variables
        accessToken = spotify_token_J[0]["access_token"];
        refresh = spotify_token_J[0]["refresh_token"].get<std::string>();

        reader.writeText(REFRESH_TOKEN, refresh);

    } else 
    {
        cpr::Response refreshedToken = cpr::Post(cpr::Url{token},
                        cpr::Parameters{{"grant_type", "refresh_token"},
                                        {"refresh_token", refresh}},
                        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"},
                                    {"Authorization", "Basic " + auth64}});
        spotify_refresh_J = json::parse(refreshedToken.text);

        accessToken = spotify_refresh_J["access_token"];

    }

    std::cout << refresh << std::endl;

    cpr::Response daylist_R = cpr::Get(cpr::Url{"https://api.spotify.com/v1/playlists/37i9dQZF1EP6YuccBxUcC1"},
                    cpr::Header{{"Authorization", "Bearer " + accessToken}});

    daylist_J = json::parse(daylist_R.text);


    const std::string daylistName {daylist_J["name"]}; 
    std::cout << daylistName << std::endl;

    std::string daylistItems {};
    daylistItems = storeDaylist(daylist_J, daylistItems);

    std::cout << daylistItems << std::endl;

    //First time auth with gmail api

    if (gmailRefresh.empty())
    {

        const std::string gmailAuthUrl = "https://accounts.google.com/o/oauth2/auth?client_id=" + 
                                            gmailClient + 
                                            "&redirect_uri=http://localhost:8080&response_type=code&scope=https://www.googleapis.com/auth/gmail.compose&access_type=offline";

        std::cout << gmailAuthUrl << std::endl;

        std::string gmailAuthCode {};
        gmailAuthCode = getAuthCode(gmailAuthCode);

        //Retrieve access code and refresh token

        cpr::Response gmail_access = cpr::Post(cpr::Url{gmailToken},
                        cpr::Parameters{{"client_id", gmailClient},
                                        {"client_secret", gmailSecret},
                                        {"code", gmailAuthCode},
                                        {"grant_type", "authorization_code"},
                                        {"redirect_uri", redirect}});

        std::cout << gmail_access.text << std::endl;

        gmail_access_J = json::parse(gmail_access.text);

        gmailAccess = gmail_access_J["access_token"];
        std::cout << "json usage error" << std::endl;
        gmailRefresh = gmail_access_J["refresh_token"];

        reader.writeText(GMAIL_REFRESH, gmailRefresh);
    } else {
        // Refresh gmail access token
        cpr::Response gmail_refresh = cpr::Post(cpr::Url{gmailToken},
                        cpr::Parameters{{"client_id", gmailClient},
                                        {"client_secret", gmailSecret},
                                        {"grant_type", "refresh_token"},
                                        {"refresh_token", gmailRefresh}});

        gmail_access_J = json::parse(gmail_refresh.text);
        gmailAccess = gmail_access_J["access_token"];
    }

    std::string testString = {reader.getText("test.txt")};
    
    testString += "Subject: " + daylistName + "\n\n";

    testString += daylistItems;
    std::string rawTest = base64::to_base64(testString);

    json raw;
    raw["raw"] = rawTest;

    cpr::Response send_message = cpr::Post(cpr::Url{gmailSend},
                    cpr::Body{{raw.dump()}},
                    cpr::Header{{"authorization", "Bearer "+ gmailAccess},
                                {"Content-Type", "application/json"}});

    std::cout << send_message.text << std::endl;
    
}