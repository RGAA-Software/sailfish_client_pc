//
// Created by RGAA on 2023-08-11.
//

#ifndef SAILFISH_CLIENT_PC_WSCLIENT_H
#define SAILFISH_CLIENT_PC_WSCLIENT_H

#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"

using client = websocketpp::client<websocketpp::config::asio_client>;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;
using websocketpp::frame::opcode::value::binary;
using websocketpp::frame::opcode::value::text;

#include <string>
#include <memory>
#include <fstream>

#include "StreamItem.h"

namespace rgaa {

    class Data;
    class Thread;

    using OnMessageCallback = std::function<void(const std::string& data)>;

    class WSClient {
    public:

        explicit WSClient(const StreamItem& item);
        ~WSClient();

        void Connect();
        void Exit();

        void OnOpen(client* c, websocketpp::connection_hdl hdl);
        void OnClose(client* c, websocketpp::connection_hdl hdl);
        void OnMessage(client* c, websocketpp::connection_hdl hdl, message_ptr msg);

        void PostBinaryMessage(const std::string& msg);
        void PostBinaryMessage(const std::shared_ptr<Data>& msg);
        void PostTextMessage(const std::string& msg);

        void ProcessMessage(websocketpp::connection_hdl hdl, message_ptr msg);

        void SetOnMessageCallback(OnMessageCallback cbk);

    private:

        std::string ip_{};
        int port_{0};

        websocketpp::connection_hdl target_server;
        std::string target_url;
        std::shared_ptr<Thread> ws_thread = nullptr;
        std::shared_ptr<Thread> msg_thread = nullptr;
        std::shared_ptr<client> ws_client = nullptr;
        bool stop_connecting = false;
        bool already_exit_loop_ = false;

        OnMessageCallback msg_cbk_;

        std::ofstream debug_video_file_;

        StreamItem stream_item_;

    };

}

#endif //SAILFISH_CLIENT_PC_WSCLIENT_H
