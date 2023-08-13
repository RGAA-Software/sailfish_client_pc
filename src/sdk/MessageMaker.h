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

	};

}