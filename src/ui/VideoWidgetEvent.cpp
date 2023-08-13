#include "VideoWidgetEvent.h"

#include "messages.pb.h"
//#include "Context.h"
#include "sdk/MessageMaker.h"
#include "QtKeyConverter.h"
#include "messages.pb.h"
#include "rgaa_common/RLog.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace rgaa {

	VideoWidgetEvent::VideoWidgetEvent(const std::shared_ptr<Context>& ctx) {
		this->context = ctx;
        this->key_converter_ = std::make_shared<QtKeyConverter>();
	}

	VideoWidgetEvent::~VideoWidgetEvent() {

	}

	void VideoWidgetEvent::OnWidgetResize(int w, int h) {
		this->width = w;
		this->height = h;
	}

	int VideoWidgetEvent::GetMouseKey(QMouseEvent* e) {
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

		auto key = (MouseKey)GetMouseKey(e);
		auto mouse_msg = MessageMaker::MakeMouseInfo(key, false, false, x, y, dx, dy);
        SendCallback(mouse_msg);
        //LOGI("move: {} {} ", last_cursor_x, last_cursor_y);
	}

	void VideoWidgetEvent::OnMousePressEvent(QMouseEvent* e) {
		float x = e->x() * 1.0 / width;
		float y = e->y() * 1.0 / height;
		auto key = (MouseKey)GetMouseKey(e);
		auto mouse_msg = MessageMaker::MakeMouseInfo(key, true, false, x, y, 0, 0);
        SendCallback(mouse_msg);
	}

	void VideoWidgetEvent::OnMouseReleaseEvent(QMouseEvent* e) {
		float x = e->x() * 1.0 / width;
		float y = e->y() * 1.0 / height;
		auto key = (MouseKey)GetMouseKey(e);

		auto mouse_msg = MessageMaker::MakeMouseInfo(key, false, true, x, y, 0, 0);
        SendCallback(mouse_msg);
	}

	void VideoWidgetEvent::OnMouseDoubleClickEvent(QMouseEvent*) {

	}

	void VideoWidgetEvent::OnWheelEvent(QWheelEvent* e) {
		float x = e->position().x() * 1.0 / width;
		float y = e->position().y() * 1.0 / height;

		int scroll = e->angleDelta().y();
//		std::cout << "scroll : " << scroll << " x : " << x << " y : " << y << std::endl;
		auto mouse_msg = MessageMaker::MakeWheelScrollMouseInfo(x, y, scroll);
        SendCallback(mouse_msg);
	}

	void VideoWidgetEvent::OnKeyPressEvent(QKeyEvent* e) {
        int vk = key_converter_->ToVK(e->key());
        LOGI("Key pressed , vk : {}", vk);
        int num_lock_state;
        int caps_lock_state;
#ifdef WIN32
        num_lock_state = GetKeyState(VK_NUMLOCK);
        caps_lock_state = GetKeyState(VK_CAPITAL);

        std::map<int, bool> sys_key_status = key_converter_->GetSysKeyStatus();

#endif
		auto keyboard_msg = MessageMaker::MakeKeyboardInfo(vk, true, 0, caps_lock_state, num_lock_state);
        SendCallback(keyboard_msg);
	}

	void VideoWidgetEvent::OnKeyReleaseEvent(QKeyEvent* e) {
		int vk = key_converter_->ToVK(e->key());
		LOGI("Key release , vk : {}", vk);
        int num_lock_state;
        int caps_lock_state;
#ifdef WIN32
        num_lock_state = GetKeyState(VK_NUMLOCK);
        caps_lock_state = GetKeyState(VK_CAPITAL);
#endif
		auto keyboard_msg = MessageMaker::MakeKeyboardInfo(vk, false, 0, caps_lock_state, num_lock_state);
        SendCallback(keyboard_msg);
	}

    void VideoWidgetEvent::RegisterMouseKeyboardEventCallback(OnMouseKeyboardEventCallback cbk) {
        event_cbk_ = std::move(cbk);
    }

    void VideoWidgetEvent::SendCallback(const std::shared_ptr<NetMessage>& msg) {
        if (event_cbk_) {
            event_cbk_(msg);
        }
    }

}
