#pragma once

#include <iostream>
#include "cpr/cpr.h"
#include "readTxt.h"
#include "nlohmann/json.hpp"

class CCpotify
{
    public:
        CCpotify(const std::string& clientID, const std::string& clientSecret, const std::string& auth64)
            : m_clientID (clientID)
            , m_clientSecret (clientSecret)
            , m_auth64 (auth64)
        {}

        Reader reader;

        void setAccessToken(const std::string& accessToken) {m_accessToken = accessToken;}

        void authorizeSpotify()
        {
            cpr::Response spotifyAuth = cpr::Get(cpr::Url{m_authorize},
                        cpr::Parameters{{"response_type","code"},
                                        {"client_id", m_clientID},
                                        {"redirect_uri", m_redirect},
                                        {"scope", "playlist-read-private"},
                                        {"show_dialog", "true"}},
                        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}});

            std::cout << spotifyAuth.url << std::endl;
        }

        void retrieveFirstToken()
        {
            cpr::Response spotifyToken = cpr::Post(cpr::Url{m_token},                                      
                        cpr::Parameters{{"grant_type", "authorization_code"},
                                        {"code", m_authCode},
                                        {"redirect_uri",m_redirect}},
                        cpr::Header{{"Authorization", "Basic " + m_auth64},
                                    {"Content-Type", "application/x-www-form-urlencoded"}});

            m_response_J = nlohmann::json::parse(spotifyToken.text);
            m_refresh = m_response_J["refresh_token"].get<std::string>(); 
        }

        void refreshToken()
        {
            cpr::Response accessResponse = cpr::Post(cpr::Url{m_token},
                        cpr::Parameters{{"grant_type", "refresh_token"},
                                        {"refresh_token", m_refresh}},
                        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"},
                                    {"Authorization", "Basic " + m_auth64}});
            m_response_J = nlohmann::json::parse(accessResponse.text);
        }

        void getAuthCode()
        {
            std::cout << "Please enter provided authCode: ";
            std::cin >> m_authCode;
        }

        const std::string getAccessToken()
        {
            if (m_refresh.empty())
            {
                authorizeSpotify();
                getAuthCode();
                retrieveFirstToken();
                reader.writeText("spotify_refresh.txt", m_refresh);
            } 
            refreshToken();
        
            return m_response_J["access_token"];;
        }

        const nlohmann::json getPlaylist(const std::string& playlistURL)
        {
            cpr::Response daylist_R = cpr::Get(cpr::Url{playlistURL},
                    cpr::Header{{"Authorization", "Bearer " + m_accessToken}});

            nlohmann::json response_J = nlohmann::json::parse(daylist_R.text);
            return response_J;
        }

    private:
        //Secrets
        const std::string m_clientID{};
        const std::string m_clientSecret{};
        const std::string m_auth64{};
        //Endpoints
        const std::string m_playlists{"https://api.spotify.com/v1/me/playlists"};
        const std::string m_token{"https://accounts.spotify.com/api/token"};
        const std::string m_authorize{"https://accounts.spotify.com/authorize"};
        std::string m_redirect{"http://localhost:8080"};
        //tokens
        std::string m_authCode{};
        std::string m_accessToken {};
        std::string m_refresh {reader.getText("spotify_refresh.txt")}; //reader.getText("spotify_refresh.txt")
        //jsonResponses
        nlohmann::json m_response_J {};

};