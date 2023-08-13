#pragma once

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <memory>

#include "messages.pb.h"

namespace rgaa {

	class Context;

	class VideoWidgetEvent {
	public:
		
		VideoWidgetEvent(std::shared_ptr<Context> ctx);
		virtual ~VideoWidgetEvent();

		MouseKey GetMouseKey(QMouseEvent* e);
		void OnWidgetResize(int w, int h);
		
		void OnMouseMoveEvent(QMouseEvent*);
		void OnMousePressEvent(QMouseEvent*);
		void OnMouseReleaseEvent(QMouseEvent*);
		void OnMouseDoubleClickEvent(QMouseEvent*);
		void OnWheelEvent(QWheelEvent* event);
		void OnKeyPressEvent(QKeyEvent* event);
		void OnKeyReleaseEvent(QKeyEvent* event);

	public:
		
		std::shared_ptr<Context> context = nullptr;

		int width = 0;
		int height = 0;

		// mouse
		int invalid_position = -10002200;
		int last_cursor_x = invalid_position;
		int last_cursor_y = invalid_position;
	};

}