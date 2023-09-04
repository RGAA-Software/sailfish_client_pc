//
// Created by RGAA on 2023-08-11.
//

#include "WSClient.h"

#include <chrono>
#include <iostream>

#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RThread.h"

#include "MessageMaker.h"
#include "Settings.h"

#define DEBUG_VIDEO_FILE 0

namespace rgaa {

    WSClient::WSClient(const StreamItem& item) {
        this->stream_item_ = item;
        this->ip_ = stream_item_.stream_host;
        this->port_ = stream_item_.stream_port;
        this->target_url = "ws://" + ip_ + ":" + std::to_string(port_);

#if DEBUG_VIDEO_FILE
        debug_video_file_ = std::ofstream("recv_file.h265", std::ios::binary);
#endif
    }

    WSClient::~WSClient() {
#if DEBUG_VIDEO_FILE
        if (debug_video_file_.is_open()) {
            debug_video_file_.close();
        }
#endif
    }

    void WSClient::Connect() {
        auto exec_func = [=, this]() {
            while (!stop_connecting) {
                try {
                    ws_client = std::make_shared<client>();
                    ws_client->set_access_channels(websocketpp::log::alevel::none);
                    ws_client->clear_access_channels(websocketpp::log::alevel::frame_payload);
                    ws_client->init_asio();

                    ws_client->set_open_handler(std::bind(&WSClient::OnOpen, this, ws_client.get(), ::_1));
                    ws_client->set_close_handler(std::bind(&WSClient::OnClose, this, ws_client.get(), ::_1));
                    ws_client->set_message_handler(std::bind(&WSClient::OnMessage, this, ws_client.get(), ::_1, ::_2));

                    std::cout << "target URL : " << target_url << std::endl;

                    websocketpp::lib::error_code ec;
                    auto conn = ws_client->get_connection(target_url, ec);
                    if (ec) {
                        std::cout << "could not create connection because: " << ec.message() << std::endl;
                        return;
                    }

                    ws_client->connect(conn);
                    ws_client->run();

                    if (!stop_connecting) {
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }
                }
                catch (websocketpp::exception const& e) {
                    std::cout << "error : " << e.what() << " will retry." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }

            LOGI("Exit the loop...");
            already_exit_loop_ = true;
        };
        ws_thread = std::make_shared<Thread>(exec_func, "ws_thread", false);
    }

    void WSClient::Exit() {
        stop_connecting = true;
        if (ws_client) {
            try {
                ws_client->stop();
            }
            catch (std::exception& e) {
                LOGE("Error : {}", e.what());
            }
        }

        while (!already_exit_loop_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (ws_thread->IsJoinable()) {
            ws_thread->Join();
            LOGI("Exit the ws_thread");
        }
    }

    void WSClient::OnOpen(client* c, websocketpp::connection_hdl hdl) {
        target_server = hdl;

        bool audio_on = Settings::Instance()->IsAudioEnabled();

        auto msg = MessageMaker::MakeStartRecording(audio_on, stream_item_.encode_bps, stream_item_.encode_fps);
        auto msg_str = msg->SerializeAsString();
        c->send(hdl, msg_str, binary);
    }

    void WSClient::OnClose(client* c, websocketpp::connection_hdl hdl) {
        target_server.reset();
        LOGI("close...");
    }

    void WSClient::OnMessage(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        if (!msg || msg->get_payload().empty()) {
            return;
        }

        ProcessMessage(hdl, msg);

//        const auto& msg_str = msg->get_payload();
//        this->msg_processor->ProcessMessage(msg_str, [=](std::shared_ptr<Message> ack) {
//            if (c && ack) {
//                auto smsg = ack->SerializeAsString();
//                c->send(hdl, smsg, binary);
//            }
//        });
    }

    void WSClient::PostBinaryMessage(const std::shared_ptr<Data>& msg) {
        if (!msg) {
            return;
        }
        if (ws_client && target_server.lock()) {
            ws_client->send(target_server, msg->CStr(), msg->Size(), binary);
        }
    }

    void WSClient::PostBinaryMessage(const std::string& msg) {
        if (ws_client && target_server.lock()) {
            ws_client->send(target_server, msg, binary);
        }
    }

    void WSClient::PostTextMessage(const std::string& msg) {
        if (msg.empty()) {
            return;
        }
        if (ws_client && target_server.lock()) {
            ws_client->send(target_server, msg, text);
        }
    }

    void WSClient::ProcessMessage(websocketpp::connection_hdl hdl, message_ptr msg) {
        const auto& msg_str = msg->get_payload();

        if (msg->get_opcode() == text) {
            LOGI("Text : {}", msg_str);
        }
        else if (msg->get_opcode() == binary) {
            if (msg_cbk_) {
                msg_cbk_(msg_str);
            }
#if DEBUG_VIDEO_FILE
            // very previous debug version, the data is bare, not wrapped by protobuf
            if (debug_video_file_.is_open()) {
                debug_video_file_.write(msg_str.data(), msg_str.size());
            }
            LOGI("Data arrived : {}", msg_str.size());
#endif
        }

    }

    void WSClient::SetOnMessageCallback(OnMessageCallback cbk) {
        msg_cbk_ = std::move(cbk);
    }

}