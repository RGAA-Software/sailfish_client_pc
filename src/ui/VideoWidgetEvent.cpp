#include "VideoWidgetEvent.h"

#include "messages.pb.h"
//#include "Context.h"
//#include "ProtoMessageMaker.h"
//#include "AppInnerMessage.h"
//#include "KeyMapper.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace rgaa {

	VideoWidgetEvent::VideoWidgetEvent(std::shared_ptr<Context> ctx) {
		this->context = ctx;
	}

	VideoWidgetEvent::~VideoWidgetEvent() {

	}

	void VideoWidgetEvent::OnWidgetResize(int w, int h) {
		this->width = w;
		this->height = h;
	}

	MouseKey VideoWidgetEvent::GetMouseKey(QMouseEvent* e) {
		MouseKey key = MouseKey::kLeft;
		if (e->button() == Qt::LeftButton) {
			key = MouseKey::kLeft;
		}
		else if (e->button() == Qt::RightButton) {
			key = MouseKey::kRight;
		}
		else if (e->button() == Qt::MiddleButton) {
			key = MouseKey::kMiddle;
		}
		return key;
	}

	void VideoWidgetEvent::OnMouseMoveEvent(QMouseEvent* e) {
		if (width == 0 || height == 0) {
			return;
		}
		float x = e->x() * 1.0 / width;
		float y = e->y() * 1.0 / height;

		float dx = 0;
		float dy = 0;

		if (last_cursor_x != invalid_position && last_cursor_y != invalid_position) {
			dx = (e->x() - last_cursor_x) * 1.0 / width;
			dy = (e->y() - last_cursor_y) * 1.0 / height;
		}
		last_cursor_x = e->x();
		last_cursor_y = e->y();

		MouseKey key = GetMouseKey(e);

//		auto mouse_msg = ProtoMessageMaker::MakeMouseInfo(key, false, false, x, y, dx, dy);
//		MouseKeyboardProtoMessage msg;
//		msg.msg = mouse_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}

	void VideoWidgetEvent::OnMousePressEvent(QMouseEvent* e) {
		float x = e->x() * 1.0 / width;
		float y = e->y() * 1.0 / height;
		MouseKey key = GetMouseKey(e);
//		auto mouse_msg = ProtoMessageMaker::MakeMouseInfo(key, true, false, x, y, 0, 0);
//		MouseKeyboardProtoMessage msg;
//		msg.msg = mouse_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}

	void VideoWidgetEvent::OnMouseReleaseEvent(QMouseEvent* e) {
		float x = e->x() * 1.0 / width;
		float y = e->y() * 1.0 / height;
		MouseKey key = GetMouseKey(e);

//		auto mouse_msg = ProtoMessageMaker::MakeMouseInfo(key, false, true, x, y, 0, 0);
//		MouseKeyboardProtoMessage msg;
//		msg.msg = mouse_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}

	void VideoWidgetEvent::OnMouseDoubleClickEvent(QMouseEvent*) {

	}

	void VideoWidgetEvent::OnWheelEvent(QWheelEvent* e) {
		float x = e->position().x() * 1.0 / width;
		float y = e->position().y() * 1.0 / height;

//		int scroll = e->angleDelta().y();
//		std::cout << "scroll : " << scroll << " x : " << x << " y : " << y << std::endl;
//		auto mouse_msg = ProtoMessageMaker::MakeWheelScrollMouseInfo(x, y, scroll);
//		MouseKeyboardProtoMessage msg;
//		msg.msg = mouse_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}

	void VideoWidgetEvent::OnKeyPressEvent(QKeyEvent* e) {
//        int vk = QtKeyToVK(e->key());
//        int num_lock_state;
//        int caps_lock_state;
//#ifdef WIN32
//        num_lock_state = GetKeyState(VK_NUMLOCK);
//        caps_lock_state = GetKeyState(VK_CAPITAL);
//        std::cout << "num lock : " << num_lock_state << " caps : " << caps_lock_state << std::endl;
//#endif
//		auto keyboard_msg = ProtoMessageMaker::MakeKeyboardInfo(vk, true, 0, caps_lock_state, num_lock_state);
//		MouseKeyboardProtoMessage msg;
//		msg.type = 1;
//		msg.vk = vk;
//		msg.pressed = true;
//		msg.msg = keyboard_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}

	void VideoWidgetEvent::OnKeyReleaseEvent(QKeyEvent* e) {
//		int vk = QtKeyToVK(e->key());
//		std::cout << "key : " << vk << " released ----><----" << std::endl;
//        int num_lock_state;
//        int caps_lock_state;
//#ifdef WIN32
//        num_lock_state = GetKeyState(VK_NUMLOCK);
//        caps_lock_state = GetKeyState(VK_CAPITAL);
//		std::cout << "num lock : " << num_lock_state << " caps : " << caps_lock_state << std::endl;
//#endif
//		auto keyboard_msg = ProtoMessageMaker::MakeKeyboardInfo(vk, false, 0, caps_lock_state, num_lock_state);
//		MouseKeyboardProtoMessage msg;
//		msg.type = 1;
//		msg.vk = vk;
//		msg.pressed = false;
//		msg.msg = keyboard_msg->SerializeAsString();
//		context->SendAppMessage(msg);
	}


}
