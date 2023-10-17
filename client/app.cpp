#include "client.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace g {
    class AppClient {
    public:
        AppClient()
        {
            client.acceptConnectHook = [this]() {
                std::cout << "Connection has been accepted by server.\n";
            };
            client.declineConnectHook = [this]() {
                std::cout << "Connection has been declined by server.\n";
                this->toExit = true;
            };
            client.msgReceiveHook = [this](const char *package, size_t size) {
                std::cout << "Server: \"" << std::string(package, size) << "\"\n";
            };
        }

        void run()
        {
            while (not toExit) {
                try {
                    std::string command;
                    std::cin >> command;
                    if (command == "connect") {
                        std::string ip;
                        // TODO: add port
                        std::cin >> ip;
                        client.connect(ip, 8888);
                    } else if (command == "send") {
                        std::string text;
                        std::getline(std::cin, text);
                        client.send(text);
                    } else {
                        std::cout << "Unknown command!\n";
                    }
                } catch (std::exception &ex) {
                    std::cout << "Error: \"" << ex.what() << "\"\n";
                }
            }
        }

        ~AppClient()
        {

        }

    private:
        Client client;
        bool toExit = false;
    };
}

int main()
{
    g::AppClient app;

    app.run();
}
