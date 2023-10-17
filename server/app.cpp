#include "server.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace g {
    class AppServer {
    public:
        AppServer()
        {
            server.askConnectHook = [this](TCPSocket<> *client) {
                std::cout << "Do you want client \"" << client << "\" to connect (Y)\n";
                // TODO: fight with async io here
                return true;
                std::string ans;
                std::cin >> ans;
                if (ans == "Y") {
                    return true;
                } else {
                    return false;
                }
            };
            server.msgReceiveHook = [this](TCPSocket<> *client, const char *text, size_t size) {
                std::cout << "Client \"" << client << "\" sent: \"" << std::string(text, size) << "\"\n";
            };
            server.msgConfirmHook = [this](TCPSocket<> *client) {
                std::cout << "Client \"" << client << "\" got your message\n";
            };
        }

        void run()
        {
            server.start(8888);
            while (not toExit) {
                try {
                    std::string command;
                    std::cin >> command;
                    // XXX TODO broadcast.
                    if (command == "list") {
                        int count = 0;
                        for (auto [socket, status]: server.listClients()) {
                            std::printf("%d: %p\n", count++, (void*)socket);
                        }
                    } else if (command == "send") {
                        size_t to;
                        std::cin >> to;
                        std::string text;
                        std::getline(std::cin, text);
                        std::cerr << text << std::endl;
                        server.send(server.getClient(to), text);
                    } else {
                        std::cout << "Unknown command!\n";
                    }
                } catch (std::exception &ex) {
                    std::cout << "Error: \"" << ex.what() << "\"\n";
                }
            }
        }

        ~AppServer()
        {

        }

    private:
        Server server;
        bool toExit = false;
    };
}

int main()
{
    g::AppServer app;

    app.run();
}
