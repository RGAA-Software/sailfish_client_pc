#include "MessageMaker.h"

namespace rgaa
{

	std::shared_ptr<NetMessage> MessageMaker::MakeMouseInfo(MouseKey& key, bool pressed, bool released, float x, float y, float dx, float dy) {
		auto message = std::make_shared<NetMessage>();
		MouseInfoReport* info = new MouseInfoReport();
		info->set_key(key);
		info->set_pressed(pressed);
		info->set_released(released);
		info->set_mouse_x(x);
		info->set_mouse_y(y);
		info->set_mouse_dx(dx);
		info->set_mouse_dy(dy);
		info->set_middle_scroll(0);
		message->set_allocated_mouse_info(info);
		return message;
	}
	
	std::shared_ptr<NetMessage> MessageMaker::MakeWheelScrollMouseInfo(float x, float y, int scroll) {
		auto message = std::make_shared<NetMessage>();
		MouseInfoReport* info = new MouseInfoReport();
		info->set_key(MouseKey::kMiddle);
		info->set_pressed(false);
		info->set_released(false);
		info->set_mouse_x(x);
		info->set_mouse_y(y);
		info->set_mouse_dx(0);
		info->set_mouse_dy(0);
		info->set_middle_scroll(scroll);
		message->set_allocated_mouse_info(info);
		return message;
	}

	std::shared_ptr<NetMessage> MessageMaker::MakeKeyboardInfo(int vk, bool pressed, int scancode, bool caps_lock, bool num_lock) {
		auto message = std::make_shared<NetMessage>();
		KeyboardInfoReport* info = new KeyboardInfoReport();
		info->set_vk(vk);
		info->set_pressed(pressed);
		info->set_scancode(scancode);
		info->set_caps_lock(caps_lock);
		info->set_num_lock(num_lock);
		message->set_allocated_keyboard_info(info);
		return message;
	}

	std::shared_ptr<NetMessage> MessageMaker::MakeACK(MessageType type, uint64_t send_time, uint64_t frame_index) {
		auto message = std::make_shared<NetMessage>();
		MessageACK* ack = new MessageACK();
		ack->set_type(type);
		ack->set_send_time(send_time);
		ack->set_frame_index(frame_index);
		message->set_allocated_message_ack(ack);
		return message;
	}

    std::shared_ptr<NetMessage> MessageMaker::MakeStartRecording(bool audio) {
        auto message = std::make_shared<NetMessage>();
        auto info = new StartRecordingReport();
        info->set_audio(audio);
        message->set_allocated_start_recording(info);
        return message;
    }

    std::shared_ptr<NetMessage> MessageMaker::MakeStopRecording() {
        auto message = std::make_shared<NetMessage>();
        auto info = new StopRecordingReport();
        message->set_allocated_stop_recording(info);
        return message;
    }

}