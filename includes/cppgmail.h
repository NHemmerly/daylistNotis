#pragma once

#include <iostream>
#include "cpr/cpr.h"
#include "readTxt.h"
#include "nlohmann/json.hpp"

class Cppgmail {

    public:
        Cppgmail(const std::string& clientId, const std::string& clientSecret)
            : m_clientId(clientId)
            , m_clientSecret(clientSecret)
        {}

        void setAccessToken(const std::string& accessToken) {m_accessToken = accessToken;}

        void getAuthCode() 
        {
            std::cout << "Please enter provided Auth code: ";
            std::cin >> m_authCode;
        }

        void printAuthUrl()
        {
            std::cout << "https://accounts.google.com/o/oauth2/auth?client_id=" + 
                                            m_clientId + 
                                            "&redirect_uri=http://localhost:8080&response_type=code&scope=https://www.googleapis.com/auth/gmail.compose&access_type=offline"
            << std::endl;

            getAuthCode();
        }

        void getRefreshToken(){
            cpr::Response gmail_access = cpr::Post(cpr::Url{m_token},
                        cpr::Parameters{{"client_id", m_clientId},
                                        {"client_secret", m_clientSecret},
                                        {"code", m_authCode},
                                        {"grant_type", "authorization_code"},
                                        {"redirect_uri", m_redirect}});
            std::cout << "flag" << std::endl;
            m_response_J = nlohmann::json::parse(gmail_access.text);
            m_refresh = m_response_J["refresh_token"].get<std::string>();
        }

        void refreshToken(){
            cpr::Response gmail_refresh = cpr::Post(cpr::Url{m_token},
                        cpr::Parameters{{"client_id", m_clientId},
                                        {"client_secret", m_clientSecret},
                                        {"grant_type", "refresh_token"},
                                        {"refresh_token", m_refresh}});
            m_response_J = nlohmann::json::parse(gmail_refresh.text);
        }

        const std::string getAccessToken()
        {
            if (m_refresh.empty())
            {
                printAuthUrl();
                getRefreshToken();
                reader.writeText("gmail_refresh.txt", m_refresh);
            }
            refreshToken();
            return m_response_J["access_token"];
        }

        void sendMessage(const nlohmann::json& message) {
            cpr::Response send_message = cpr::Post(cpr::Url{m_message_send},
                cpr::Body{{message.dump()}},
                cpr::Header{{"authorization", "Bearer "+ m_accessToken},
                            {"Content-Type", "application/json"}});
        }

    private:
        Reader reader;
        //Secrets
        const std::string m_clientId {};
        const std::string m_clientSecret {};
        //Endpoints
        const std::string m_auth {"https://accounts.google.com/o/oauth2/auth"};
        const std::string m_token {"https://oauth2.googleapis.com/token"};
        const std::string m_message_send {"https://gmail.googleapis.com/gmail/v1/users/me/messages/send"};
        const std::string m_redirect {"http://localhost:8080"};
        //Tokens
        std::string m_authCode {};
        std::string m_accessToken {};
        std::string m_refresh {reader.getText("gmail_refresh.txt")};
        //json
        nlohmann::json m_response_J {};
};