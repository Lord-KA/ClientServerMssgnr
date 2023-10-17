#include "../shared/protocol.hpp"

#include "tcpserver.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <iterator>

namespace g {
    class Server {
    private:
        struct Client {
            enum State {
                connecting,
                connected,
                allowedMsg,
                disconnected,
            } state;
            TCPSocket<> *socket;
            // TODO: std::string name;
        };

    public:
        Server()
        {
            tcpServer.onNewConnection = [this](TCPSocket<> *newClient) {
                this->clients.insert({newClient, {Client::State::connecting, newClient}});

                newClient->onSocketClosed = [newClient, this](int errorCode) {
                    // TODO: upgrade logging.
                    this->clients.erase(newClient);
                };

                newClient->onRawMessageReceived = [newClient, this](const char* message, int length) {
                    assert(length >= sizeof(struct MessageStructure));
                    const struct MessageStructure *msg = (const struct MessageStructure*)(message);

                    switch (msg->type) {
                    case MessageStructure::Type::AskConnect: {
                            // TODO add pub key
                            if (not this->askConnectHook)
                                break;

                            struct MessageStructure ans;
                            ans.packageSize = 0;
                            if (this->askConnectHook(newClient)) {
                                ans.type = MessageStructure::Type::AcceptConnect;
                                this->clients[newClient].state = Client::State::connected;
                            } else {
                                ans.type = MessageStructure::Type::DeclineConnect;
                                this->clients[newClient].state = Client::State::disconnected;
                                // TODO drop the client
                            }
                            newClient->Send((char*)&ans, sizeof(ans));
                            break;
                        }

                    case MessageStructure::Type::Message: {
                            std::cerr << "HERE " << (this->clients[newClient].state != Client::State::allowedMsg) << "\n";
                            if (not this->msgReceiveHook or
                                    this->clients[newClient].state != Client::State::allowedMsg)
                                break;
                            const char *package = message + sizeof(struct MessageStructure);
                            msgReceiveHook(newClient, package, msg->packageSize);
                            break;
                        }

                    case MessageStructure::Type::MessageRecived: {
                            if (not this->msgConfirmHook)
                                break;
                            this->msgConfirmHook(newClient);
                            break;
                        }
                    }
                };
            };
        }

        void start(int port)
        {
            tcpServer.Bind(port, [](int errorCode, std::string errorMessage) {
                // TODO: refactor logs
                throw std::runtime_error(errorMessage);
            });
            tcpServer.Listen([](int errorCode, std::string errorMessage) {
                // TODO: refactor logs
                throw std::runtime_error(errorMessage);
            });
        }

        void send(TCPSocket<> *rcvr, const std::string &text)
        {
            struct MessageStructure msg;
            msg.type = MessageStructure::Type::Message;
            msg.packageSize = text.size();
            char *package = new char[sizeof(msg) + text.size()];
            std::memcpy(package, &msg, sizeof(msg));
            std::memcpy(package + sizeof(msg), text.c_str(), text.size());

            rcvr->Send(package, sizeof(msg) + text.size());
            delete[] package;

            // Allow incomming messages.
            this->clients[rcvr].state = Client::State::allowedMsg;
        }

        auto listClients()
        {
            return clients;
        }

        TCPSocket<> *getClient(size_t idx)
        {
            auto it = clients.begin();
            std::advance(it, idx);
            return (TCPSocket<>*)it->first;
        }

        ~Server()
        {
            tcpServer.Close();
        }

    private:
        TCPServer<> tcpServer;
        std::map<TCPSocket<>*, struct Client> clients;

    public:
        std::function<bool(TCPSocket<> *client)> askConnectHook;
        std::function<void(TCPSocket<> *client, const char *data, size_t size)> msgReceiveHook;
        std::function<void(TCPSocket<> *client)> msgConfirmHook;
    };
}
