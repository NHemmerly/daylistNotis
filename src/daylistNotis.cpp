#include "../includes/httplib.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>

#define CA_CERT_FILE "../.secrets/ca-certificates.crt"

int main()
{
    const std::string base = "https://api.spotify.com/v1/";
    const std::string token = "https://accounts.spotify.com/api/token";
    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        httplib::SSLClient cli("accounts.spotify.com");
        cli.set_ca_cert_path(CA_CERT_FILE);
        cli.enable_server_certificates_verification(true);
    #endif


    if (auto res = cli.Post("/api/token", "grant_type=client_credentials&client_id=###&client_secret=###", "application/x-www-form-urlencoded")) {
        if (res -> status == httplib::StatusCode::OK_200) {
            std::cout << res->body << std::endl;
        }
    } else {
        auto err = res.error();
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }

}