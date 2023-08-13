#pragma once

#include <QWidget>
#include <memory>
#include "sdk/RawImage.h"
#include "VideoWidget.h"
#include "VideoWidgetEvent.h"

#include <SDL2/SDL.h>

namespace rgaa {
	
	class Thread;
	class Context; 
	class Statistics;

	class SDLVideoWidget : public VideoWidget, public VideoWidgetEvent {
	public:
		SDLVideoWidget(std::shared_ptr<Context> ctx, RawImageFormat format, QWidget* parent = nullptr);
		~SDLVideoWidget();

		void Init(int frame_width, int frame_height);

		void RefreshI420Image(const std::shared_ptr<RawImage>& image) override;
		void RefreshI420Buffer(const char* y_buf, int y_buf_size, const char* u_buf, int u_buf_size, const char* v_buf, int v_buf_size, int width, int height) override;

	protected:
		void mouseMoveEvent(QMouseEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void mouseReleaseEvent(QMouseEvent*) override;
		void resizeEvent(QResizeEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent*) override;
		void wheelEvent(QWheelEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;

		void closeEvent(QCloseEvent* event) override;

	private:
		
		std::shared_ptr<Context> context = nullptr;
		int frame_width_ = 0;
		int frame_height_ = 0;
		RawImageFormat format;

		SDL_Window* screen = nullptr;
		SDL_Renderer* sdlRenderer = nullptr;
		SDL_Texture* sdlTexture = nullptr;
		SDL_Rect sdlRect;
		
		bool init = false;

		int render_fps = 0;
		uint64_t last_update_fps_time = 0;

//		Statistics* statistics = nullptr;

	};

}