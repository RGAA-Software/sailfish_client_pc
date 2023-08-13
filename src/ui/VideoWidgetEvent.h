#pragma once

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <memory>

namespace rgaa {

	class Context;
    class NetMessage;
    class QtKeyConverter;

    using OnMouseKeyboardEventCallback = std::function<void(const std::shared_ptr<NetMessage>& msg)>;

	class VideoWidgetEvent {
	public:
		
		VideoWidgetEvent(const std::shared_ptr<Context>& ctx);
		virtual ~VideoWidgetEvent();

		int GetMouseKey(QMouseEvent* e);
		void OnWidgetResize(int w, int h);
		
		void OnMouseMoveEvent(QMouseEvent*);
		void OnMousePressEvent(QMouseEvent*);
		void OnMouseReleaseEvent(QMouseEvent*);
		void OnMouseDoubleClickEvent(QMouseEvent*);
		void OnWheelEvent(QWheelEvent* event);
		void OnKeyPressEvent(QKeyEvent* event);
		void OnKeyReleaseEvent(QKeyEvent* event);

        void RegisterMouseKeyboardEventCallback(OnMouseKeyboardEventCallback cbk);

    private:

        void SendCallback(const std::shared_ptr<NetMessage>& msg);

    protected:
		
		std::shared_ptr<Context> context = nullptr;
        std::shared_ptr<QtKeyConverter> key_converter_ = nullptr;

		int width = 0;
		int height = 0;

		// mouse
		int invalid_position = -10002200;
		int last_cursor_x = invalid_position;
		int last_cursor_y = invalid_position;

        OnMouseKeyboardEventCallback event_cbk_;

	};

}