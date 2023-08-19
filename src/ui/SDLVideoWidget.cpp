#include "SDLVideoWidget.h" 

#include "Context.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RThread.h"
#include "rgaa_common/RLog.h"
//#include "statistics/Statistics.h"
#include "sdk/SailfishSDK.h"

#include <QMouseEvent>

namespace rgaa
{

	SDLVideoWidget::SDLVideoWidget(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, int dup_idx, RawImageFormat format, QWidget* parent)
		: VideoWidget(sdk, dup_idx, parent), VideoWidgetEvent(ctx, sdk, dup_idx) {
		this->context = ctx;
		this->format = format;

		//this->statistics = Statistics::Instance();

		if (SDL_Init(SDL_INIT_EVERYTHING)) {
			printf("Could not initialize SDL - %s\n", SDL_GetError());
			return;
		}
		screen = SDL_CreateWindowFrom((void*)this->winId());
		if (screen == NULL)
		{
			printf("Could not create window - %s\n", SDL_GetError());
			return;
		}
		sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
		if (!sdlRenderer) {
			printf("create renderer failed !");
			return;
		}
		
		grabKeyboard();
        setMouseTracking(true);

		printf("sdl widget init success. \n");

	}

	SDLVideoWidget::~SDLVideoWidget() {
		LOGI("sdl widget exit ..... ");
	}
	
	void SDLVideoWidget::Init(int frame_width, int frame_height) {
		if (init) {
			return;
		}
		init = true;

		this->frame_width_ = frame_width;
		this->frame_height_ = frame_height;

		sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, frame_width, frame_height);
        SDL_SetTextureScaleMode(sdlTexture, SDL_ScaleMode::SDL_ScaleModeBest);
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = frame_width;
		sdlRect.h = frame_height;
	}

	void SDLVideoWidget::RefreshI420Image(const std::shared_ptr<RawImage>& image) {
		int y_buf_size = image->img_width * image->img_height;
		int uv_buf_size = y_buf_size / 4;
		char* buf = image->Data();
		RefreshI420Buffer(buf, y_buf_size, // y
			buf + y_buf_size, uv_buf_size, // u
			buf + y_buf_size + uv_buf_size, uv_buf_size, // v
			image->img_width, image->img_height
		);

        last_refresh_image_time_ = GetCurrentTimestamp();
        last_refresh_image_ = image;
	}

	void SDLVideoWidget::RefreshI420Buffer(const char* y_buf, int y_buf_size, const char* u_buf, int u_buf_size, const char* v_buf, int v_buf_size, int width, int height) {
		int ret = SDL_UpdateYUVTexture(sdlTexture, NULL,
			(uint8_t*)y_buf, width,
			(uint8_t*)u_buf, width/2,
			(uint8_t*)v_buf, width/2);
		ret = SDL_RenderClear(sdlRenderer);
		ret = SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);

		render_fps += 1;
		auto current_time = GetCurrentTimestamp();
		if (current_time - last_update_fps_time >= 1000) {
			//statistics->render_fps = render_fps;
			render_fps = 0;
			last_update_fps_time = current_time;

			//statistics->streaming_time += 1;
		}
	}

	void SDLVideoWidget::resizeEvent(QResizeEvent* event) {
		VideoWidget::resizeEvent(event);
		VideoWidgetEvent::OnWidgetResize(event->size().width(), event->size().height());

//        auto current_time = GetCurrentTimestamp();
//        auto time_diff = current_time - last_refresh_image_time_;
//        LOGI("resize : {} {} {}ms", event->size().width(), event->size().height(), time_diff);
//
//        if ( time_diff > 20
//            && last_refresh_image_time_ > 0
//            && last_refresh_image_) {
//            RefreshI420Image(last_refresh_image_);
//            LOGI("Update in resize event...");
//        }
	}

	void SDLVideoWidget::mouseMoveEvent(QMouseEvent* e) {
		VideoWidget::mouseMoveEvent(e);
		VideoWidgetEvent::OnMouseMoveEvent(e);
        //LOGI("move : {} {} ", e->x(), e->y());
	}

	void SDLVideoWidget::mousePressEvent(QMouseEvent* e) {
		VideoWidget::mousePressEvent(e);
		VideoWidgetEvent::OnMousePressEvent(e);
	}

	void SDLVideoWidget::mouseReleaseEvent(QMouseEvent* e) {
		VideoWidget::mouseReleaseEvent(e);
		VideoWidgetEvent::OnMouseReleaseEvent(e);
	}

	void SDLVideoWidget::mouseDoubleClickEvent(QMouseEvent* e) {
		VideoWidget::mouseDoubleClickEvent(e);
	}

	void SDLVideoWidget::wheelEvent(QWheelEvent* e) {
		VideoWidget::wheelEvent(e);
		VideoWidgetEvent::OnWheelEvent(e);
	}

	void SDLVideoWidget::keyPressEvent(QKeyEvent* e) {
		VideoWidget::keyPressEvent(e);
		VideoWidgetEvent::OnKeyPressEvent(e);
	}

	void SDLVideoWidget::keyReleaseEvent(QKeyEvent* e) {
		VideoWidget::keyReleaseEvent(e);
		VideoWidgetEvent::OnKeyReleaseEvent(e);
	}

	void SDLVideoWidget::closeEvent(QCloseEvent* event) {
		VideoWidget::closeEvent(event);
	}

    void SDLVideoWidget::Exit() {
        VideoWidget::Exit();
        if (sdlRenderer) {
            SDL_DestroyRenderer(sdlRenderer);
        }
        if (screen) {
            SDL_DestroyWindow(screen);
        }
        SDL_Quit();
    }
}