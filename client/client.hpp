#include "../shared/protocol.hpp"
#include "tcpsocket.hpp"

#include <iostream>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <cstring>

namespace g {
    class Client {
    public:
        Client() :
            socket([](int errorCode, std::string errorMessage){
                throw std::runtime_error(errorMessage);
            })
        {
            socket.onRawMessageReceived = [this](const char* message, int length) {
                assert(length >= sizeof(struct MessageStructure));
                const struct MessageStructure *msg = (const struct MessageStructure*)(message);

                switch (msg->type) {
                case MessageStructure::Type::AcceptConnect:
                    if (not this->acceptConnectHook)
                        break;
                    this->acceptConnectHook();
                    break;

                case MessageStructure::Type::DeclineConnect:
                    if (not this->declineConnectHook)
                        break;
                    this->declineConnectHook();
                    break;

                case MessageStructure::Type::Message:
                    if (not this->msgReceiveHook)
                        break;

                    const char *package = message + msg->packageSize;
                    msgReceiveHook(package, msg->packageSize);

                    struct MessageStructure ans;
                    ans.type = MessageStructure::Type::MessageRecived;
                    socket.Send((char*)&ans, sizeof(ans));
                    break;
                }
            };
        }

        void connect(const std::string &address, int port)
        {
            socket.Connect(address, port, [this]() {
                struct MessageStructure ans;
                ans.type = MessageStructure::Type::AskConnect;
                this->socket.Send((char*)&ans, sizeof(ans));
            },
            [](int errorCode, std::string errorMessage){
                throw std::runtime_error(errorMessage);
            });
        }

        void send(const std::string &text)
        {
            struct MessageStructure msg;
            msg.type = MessageStructure::Type::Message;
            msg.packageSize = text.size();
            char *package = new char[sizeof(msg) + text.size()];
            std::memcpy(package, &msg, sizeof(msg));
            std::memcpy(package + sizeof(msg), text.c_str(), text.size());

            socket.Send(package, sizeof(msg) + text.size());
        }

        ~Client()
        {
            socket.Close();
        }

    public:
        std::function<void(const char *package, size_t size)> msgReceiveHook;
        std::function<void()> acceptConnectHook;
        std::function<void()> declineConnectHook;

    private:
        TCPSocket<> socket;
    };
}
