#pragma once

#include <memory>

#include "messages.pb.h"

namespace rgaa
{
	
	class NetMessage;

	class MessageMaker {
	public:

		static std::shared_ptr<NetMessage> MakeMouseInfo(MouseKey& key, bool pressed, bool released, float x, float y, float dx, float dy);
		static std::shared_ptr<NetMessage> MakeWheelScrollMouseInfo(float x, float y, int scroll);
		static std::shared_ptr<NetMessage> MakeKeyboardInfo(int vk, bool pressed, int scancode, bool caps_lock, bool num_lock);
		static std::shared_ptr<NetMessage> MakeACK(MessageType type, uint64_t send_time, uint64_t frame_index);
        static std::shared_ptr<NetMessage> MakeStartRecording(bool audio, int bitrate, int approximate_fps);
        static std::shared_ptr<NetMessage> MakeStopRecording();
        static std::shared_ptr<NetMessage> MakeHeartBeat(uint64_t idx);
		static std::shared_ptr<NetMessage> MakeClipboard(const std::string& text);
        static std::shared_ptr<NetMessage> MakeClipboardStatus(bool on);
        static std::shared_ptr<NetMessage> MakeDebugStatus(bool on);
        static std::shared_ptr<NetMessage> MakeAudioStatus(bool on);

	};

}