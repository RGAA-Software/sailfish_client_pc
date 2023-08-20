#include "SDLVideoWidget.h" 

#include "Context.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RThread.h"
#include "rgaa_common/RLog.h"
//#include "statistics/Statistics.h"
#include "sdk/SailfishSDK.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessage.h"

#include <QMouseEvent>
#include <QString>
#include <QMimeData>
#include <QApplication>

namespace rgaa
{

	SDLVideoWidget::SDLVideoWidget(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, int dup_idx, RawImageFormat format, QWidget* parent)
		: VideoWidget(sdk, dup_idx, parent), VideoWidgetEvent(ctx, sdk, dup_idx) {
		this->context = ctx;
		this->format = format;

		//this->statistics = Statistics::Instance();
        setUpdatesEnabled(false);
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
        sdl_renderer_ = SDL_CreateRenderer(screen, -1, 0);
		if (!sdl_renderer_) {
			printf("create renderer failed !");
			return;
		}
		
		grabKeyboard();
        setMouseTracking(true);

		printf("sdl widget init success. \n");

        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, [this]() {
            Update();
        });
        timer_->start(17);

	}

	SDLVideoWidget::~SDLVideoWidget() {
		LOGI("sdl widget exit ..... {} ", VideoWidget::dup_idx_);
	}
	
	void SDLVideoWidget::Init(int frame_width, int frame_height) {
		if (init) {
			return;
		}
		init = true;

		this->frame_width_ = frame_width;
		this->frame_height_ = frame_height;

        sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, frame_width, frame_height);
        SDL_SetTextureScaleMode(sdl_texture_, SDL_ScaleMode::SDL_ScaleModeBest);
        sdl_rect_.x = 0;
        sdl_rect_.y = 0;
        sdl_rect_.w = frame_width;
        sdl_rect_.h = frame_height;
	}

	void SDLVideoWidget::RefreshI420Image(const std::shared_ptr<RawImage>& image) {
        if (exit_) {
            return;
        }
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
		int ret = SDL_UpdateYUVTexture(sdl_texture_, NULL,
                                       (uint8_t*)y_buf, width,
                                       (uint8_t*)u_buf, width/2,
                                       (uint8_t*)v_buf, width/2);

		render_fps += 1;
		auto current_time = GetCurrentTimestamp();
		if (current_time - last_update_fps_time >= 1000) {
			//statistics->render_fps = render_fps;
			render_fps = 0;
			last_update_fps_time = current_time;

			//statistics->streaming_time += 1;
		}
	}

    void SDLVideoWidget::Update() {
        if (exit_) {
            return;
        }
        SDL_RenderClear(sdl_renderer_);
        SDL_RenderCopy(sdl_renderer_, sdl_texture_, nullptr, nullptr);
        {
            std::lock_guard<std::mutex> guard(cursor_mtx_);
            if (target_cursor_texture_ && target_cursor_size_ > 0) {
				SDL_Rect src_rect = {0, 0, target_cursor_size_, target_cursor_size_};
				SDL_Rect dst_rect = {
                    (int)(target_x_ * target_x_scale_),
                    (int)(target_y_ * target_y_scale_),
                    target_cursor_size_,
                    target_cursor_size_
                };
                SDL_RenderCopy(sdl_renderer_, target_cursor_texture_, &src_rect, &dst_rect);
            }
        }
        SDL_RenderPresent(sdl_renderer_);
    }

    void SDLVideoWidget::RefreshCursor(int x, int y, const std::shared_ptr<RawImage>& cursor) {
        if (sdl_cursor_textures_.find(cursor->img_width) == sdl_cursor_textures_.end()) {
            auto texture = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, cursor->img_width, cursor->img_height);
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_ScaleModeBest);
            sdl_cursor_textures_[cursor->img_width] = texture;
        }

        auto texture = sdl_cursor_textures_[cursor->img_width];
        //SDL_Rect rect;
        //SDL_UpdateTexture(texture, &rect, cursor->Data(), cursor->img_width * cursor->img_ch);

		void* pixels = nullptr;
		int pitch = 0;
		SDL_LockTexture(texture, nullptr, &pixels, &pitch);

		if (pixels) {
			memcpy(pixels, cursor->Data(), cursor->Size());
		}

		SDL_UnlockTexture(texture);

        std::lock_guard<std::mutex> guard(cursor_mtx_);
        target_cursor_texture_ = texture;
		target_cursor_size_ = cursor->img_width;
		target_x_ = x;
		target_y_ = y;

        QScreen* screen = QGuiApplication::primaryScreen();
        qreal scaleFactor = screen->devicePixelRatio();
        int after_adapter_width = VideoWidget::width() * scaleFactor;
        int after_adapter_height = VideoWidget::height() * scaleFactor;

        target_x_scale_ = after_adapter_width * 1.0f / this->frame_width_;
        target_y_scale_ = after_adapter_height * 1.0f / this->frame_height_;

        LOGI("target x: {} y: {} size: {}, window: {}x{}, frame : {}x{}, after_adapter: {}, after_adapter height ; {}, scale factor: {}", target_x_, target_y_, target_cursor_size_,
             VideoWidget::width(), VideoWidget::height(), this->frame_width_, this->frame_height_, after_adapter_width, after_adapter_height, scaleFactor);
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

    void SDLVideoWidget::enterEvent(QEnterEvent *event) {
        QCursor cursor(Qt::BlankCursor);
        //QApplication::setOverrideCursor(cursor);
    }

    void SDLVideoWidget::leaveEvent(QEvent *event) {
        QCursor cursor(Qt::ArrowCursor);
        //QApplication::setOverrideCursor(cursor);
    }

    void SDLVideoWidget::Exit() {
        VideoWidget::Exit();
        if (timer_) {
            timer_->stop();
        }
        if (sdl_texture_) {
            SDL_DestroyTexture(sdl_texture_);
        }
        if (sdl_renderer_) {
            SDL_DestroyRenderer(sdl_renderer_);
        }
        if (screen) {
            SDL_DestroyWindow(screen);
        }
        SDL_Quit();
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

    void SDLWidgetWrapper::closeEvent(QCloseEvent *event) {
        LOGI("SDLWidgetWrapper closeEvent...");
        event->ignore();

        emit OnCloseEvent();
    }

    void SDLWidgetWrapper::dragEnterEvent(QDragEnterEvent *event) {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }

    void SDLWidgetWrapper::dragLeaveEvent(QDragLeaveEvent *event) {

    }

    void SDLWidgetWrapper::dropEvent(QDropEvent *event) {
        if (!event->mimeData()->hasUrls()) {
            return;
        }

        auto urls = event->mimeData()->urls();
        for (auto& url : urls) {
            LOGI("URL : {}", url.toLocalFile().toStdString());
        }
    }


}