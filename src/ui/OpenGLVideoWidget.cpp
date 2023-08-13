#include <QDebug>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QWheelEvent>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

//#include "Director.h"
//#include "Sprite.h"
//#include "Context.h"
//#include "KeyMapper.h"
//#include "AppInnerMessage.h"
//#include "ProtoMessageMaker.h"
#include "ShaderProgram.h"
#include "OpenGLVideoWidget.h"
#include "OpenGLVideoWidgetShader.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RCloser.h"
#include "rgaa_common/RLog.h"
#include "sdk/RawImage.h"
//#include "statistics/Statistics.h"

namespace rgaa
{

	OpenGLVideoWidget::OpenGLVideoWidget(std::shared_ptr<Context> ctx, RawImageFormat format, QWidget* parent) 
		: QOpenGLWidget(parent), VideoWidgetEvent(ctx) {
		context = ctx;
		raw_image_format = format;
//		statistics = Statistics::Instance();

		auto* timer = new QTimer();
		QObject::connect(timer, &QTimer::timeout, this, [=]() {
			this->update(); 
		});
		timer->start(16);

		setFocusPolicy(Qt::StrongFocus);
		setMouseTracking(true);
	}

	OpenGLVideoWidget::~OpenGLVideoWidget()
	{
		makeCurrent();
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteProgram(shader_program->GetProgramId());

		if (y_texture_id) {
			glDeleteTextures(1, &y_texture_id);
		}
		if (uv_texture_id) {
			glDeleteTextures(1, &uv_texture_id);
		}
		if (u_texture_id) {
			glDeleteTextures(1, &u_texture_id);
		}
		if (v_texture_id) {
			glDeleteTextures(1, &v_texture_id);
		}
		if (rgb_texture_id) {
			glDeleteTextures(1, &rgb_texture_id);
		}
		doneCurrent();
	}

	void OpenGLVideoWidget::initializeGL()
	{
		initializeOpenGLFunctions();

		auto functions = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
		functions->initializeOpenGLFunctions();

		//director = Director::Make(functions);

		auto vertex_shader = kMainVertexShader;
		char* fragment_shader = nullptr;
        LOGI("raw image format : {}", (int)raw_image_format);
		if (raw_image_format == RawImageFormat::kNV12) {
			fragment_shader = const_cast<char*>(kNV12FragmentShader);
		}
		else if (raw_image_format == RawImageFormat::kRGB || raw_image_format == RawImageFormat::kRGBA) {
			fragment_shader = const_cast<char*>(kRGBFragmentShader);
		}
		else if (raw_image_format == RawImageFormat::kI420) {
			fragment_shader = const_cast<char*>(kI420FragmentShader);
		}
		if (!fragment_shader) {
			return;
		}
		
		shader_program = ShaderProgram::Make(functions, vertex_shader, fragment_shader);

		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 1.0f, 0, 0,  0, 0,
			1.0f, -1.0f, 0.0f, 0, 1.0f, 0,  1, 0,
			1.0f,  1.0f, 0.0f, 0, 0, 1.0f,  1, 1,
			-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0, 0, 1
		};

		int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		int posLoc = shader_program->GetAttribLocation("aPos");
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, false, 8 * 4, 0);
		glEnableVertexAttribArray(posLoc);

		int colorLoc = shader_program->GetAttribLocation("aColor");
		glVertexAttribPointer(colorLoc, 3, GL_FLOAT, false, 8 * 4, (void*)(3 * 4));
		glEnableVertexAttribArray(colorLoc);

		int texLoc = shader_program->GetAttribLocation("aTex");
		glVertexAttribPointer(texLoc, 2, GL_FLOAT, false, 8 * 4, (void*)(6 * 4));
		glEnableVertexAttribArray(texLoc);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);

		// cursor
//		cursor = std::make_shared<Sprite>(director);
//		cursor->ForceImageSize(32, 32);

	}

	void OpenGLVideoWidget::RefreshNV12Image(const RawImagePtr& image) {
		if (!image || !image->img_buf || image->img_format != RawImageFormat::kNV12
			|| image->img_buf_size <= 0) {
			return;
		}

		int y_size = image->img_width * image->img_height;
		RefreshNV12Buffer(image->Data(), y_size, image->Data() + y_size, y_size / 2, image->img_width, image->img_height);
	}

	void OpenGLVideoWidget::RefreshNV12Buffer(const char* y_buf, int y_buf_size, const char* uv_buf, int uv_buf_size, int width, int height) {
		std::lock_guard<std::mutex> guard(buf_mtx);
		if (!y_buffer || width != tex_width || height != tex_height) {
			free(y_buffer);
			y_buffer = (char*)malloc(y_buf_size);
		}
		if (!uv_buffer || width != tex_width || height != tex_height) {
			free(uv_buffer);
			uv_buffer = (char*)malloc(uv_buf_size);
		}

		if (tex_width != width || tex_height != height) {
			need_create_texture = true;
		}
		memcpy(y_buffer, y_buf, y_buf_size);
		memcpy(uv_buffer, uv_buf, uv_buf_size);
		tex_width = width;
		tex_height = height;
	}

	void OpenGLVideoWidget::RefreshRGBImage(const std::shared_ptr<RawImage>& image) {
		assert(image->Size() == image->img_width * image->img_height * image->img_ch);
		RefreshRGBBuffer(image->Data(), image->img_width, image->img_height, image->img_ch);
	}

	void OpenGLVideoWidget::RefreshRGBBuffer(const char* buf, int width, int height, int channel) {
		//std::lock_guard<std::mutex> guard(buf_mtx);
		int size = width * height * channel;
		if (!rgb_buffer) {
			rgb_buffer = (char*)malloc(size);
		}
		if (tex_width != width || tex_height != height) {
			need_create_texture = true;
		}
		memcpy(rgb_buffer, buf, size);
		tex_width = width;
		tex_height = height;
		tex_channel = channel;
	}

	void OpenGLVideoWidget::RefreshI420Image(const std::shared_ptr<RawImage>& image) {
		int y_buf_size = image->img_width * image->img_height;
		int uv_buf_size = y_buf_size / 4;
		char* buf = image->Data();
		RefreshI420Buffer(buf, y_buf_size,
			buf + y_buf_size, uv_buf_size,
			buf + y_buf_size + uv_buf_size, uv_buf_size,
			image->img_width, image->img_height
		);
	}

	void OpenGLVideoWidget::RefreshI420Buffer(const char* y_buf, int y_buf_size,
		const char* u_buf, int u_buf_size,
		const char* v_buf, int v_buf_size,
		int width, int height) {
		if (!y_buffer) {
			y_buffer = (char*)malloc(y_buf_size);
		}
		if (!u_buffer) {
			u_buffer = (char*)malloc(u_buf_size);
		}
		if (!v_buffer) {
			v_buffer = (char*)malloc(v_buf_size);
		}
	
		if (tex_width != width || tex_height != height) {
			need_create_texture = true;
		}
		memcpy(y_buffer, y_buf, y_buf_size);
		memcpy(u_buffer, u_buf, u_buf_size);
		memcpy(v_buffer, v_buf, v_buf_size);
		
		tex_width = width;
		tex_height = height;
	}

	void OpenGLVideoWidget::resizeEvent(QResizeEvent* event) {
		QOpenGLWidget::resizeEvent(event);

//		VideoWidgetResized msg;
//		msg.width = event->size().width();
//		msg.height = event->size().height();
//		context->SendAppMessage(msg);

	}

	void OpenGLVideoWidget::paintGL() {
		if (!shader_program) {
			return;
		}

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		shader_program->Active();

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//		glPixelStorei(GL_PACK_ALIGNMENT, 1);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (raw_image_format == RawImageFormat::kNV12) {

			std::lock_guard<std::mutex> guard(buf_mtx);

			// 如果还没创建texture，在这里创建或者texture大小变了，重新创建
			if (y_buffer && uv_buffer && need_create_texture) {
				need_create_texture = false;
				InitNV12Texture();
			}
			if (y_buffer) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, y_texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y_buffer);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			if (uv_buffer) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, uv_texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, uv_buffer);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		// todo : SUPPORT RGBA/RGB separate
		else if (raw_image_format == RawImageFormat::kRGBA || raw_image_format == RawImageFormat::kRGB) {

			//std::lock_guard<std::mutex> guard(buf_mtx);

			if (rgb_buffer && need_create_texture) {
				need_create_texture = false;
				InitRGBATexture();
			}
			if (rgb_buffer) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, rgb_texture_id);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb_buffer);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_RGB, GL_UNSIGNED_BYTE, rgb_buffer);
				//glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else if (raw_image_format == RawImageFormat::kI420) {

			//std::lock_guard<std::mutex> guard(buf_mtx);

			if (y_buffer && u_buffer && v_buffer && need_create_texture) {
				need_create_texture = false;
				InitI420Texture();
			}

			if (y_buffer) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, y_texture_id);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y_buffer);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, y_buffer);
				//glGenerateMipmap(GL_TEXTURE_2D);
			}
			if (u_buffer) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, u_texture_id);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u_buffer);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width/2, tex_height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, u_buffer);
				//glGenerateMipmap(GL_TEXTURE_2D);
			}
			if (v_buffer) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, v_texture_id);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v_buffer);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width / 2, tex_height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, v_buffer);
				//glGenerateMipmap(GL_TEXTURE_2D);
			}
		}

		if (raw_image_format == RawImageFormat::kRGB || raw_image_format == RawImageFormat::kRGBA) {
			//glUniform1i(glGetUniformLocation(shader, "image1"), 0);
			shader_program->SetUniform1i("image1", 0);
		}
		else if (raw_image_format == RawImageFormat::kNV12) {
			//glUniform1i(glGetUniformLocation(shader, "image1"), 0);
			//glUniform1i(glGetUniformLocation(shader, "image2"), 1);
			shader_program->SetUniform1i("image1", 0);
			shader_program->SetUniform1i("image2", 1);
		}
		else if (raw_image_format == RawImageFormat::kI420) {
			//glUniform1i(glGetUniformLocation(shader, "imageY"), 0);
			//glUniform1i(glGetUniformLocation(shader, "imageU"), 1);
			//glUniform1i(glGetUniformLocation(shader, "imageV"), 2);
			shader_program->SetUniform1i("imageY", 0);
			shader_program->SetUniform1i("imageU", 1);
			shader_program->SetUniform1i("imageV", 2);
		}

		model = glm::mat4(1.0f);
		shader_program->SetUniformMatrix("model", model);
//		shader_program->SetUniformMatrix("view", director->GetView());
//		shader_program->SetUniformMatrix("projection", director->GetProjection());

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		shader_program->Release();

//		if (cursor) {
//			cursor->Render(0);
//		}

		render_fps += 1;
		auto current_time = GetCurrentTimestamp();
		if (current_time - last_update_fps_time > 1000) {
			//statistics->render_fps = render_fps;
			render_fps = 0;
			last_update_fps_time = current_time;
		}
	}

	void OpenGLVideoWidget::InitNV12Texture() {
		glGenTextures(1, &y_texture_id);
		glBindTexture(GL_TEXTURE_2D, y_texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);

		glGenTextures(1, &uv_texture_id);
		glBindTexture(GL_TEXTURE_2D, uv_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLVideoWidget::InitRGBATexture() {
		glGenTextures(1, &rgb_texture_id);
		glBindTexture(GL_TEXTURE_2D, rgb_texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLVideoWidget::InitI420Texture() {
		auto create_luminance_texture = [this](GLuint& tex_id, int width, int height, bool is_uv) {
			glGenTextures(1, &tex_id);
			glBindTexture(GL_TEXTURE_2D, tex_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, is_uv ? width / 2 : width, is_uv ? height / 2 : height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		};
		create_luminance_texture(y_texture_id, tex_width, tex_height, false);
		create_luminance_texture(u_texture_id, tex_width, tex_height, true);
		create_luminance_texture(v_texture_id, tex_width, tex_height, true);

        LOGI("Init I420 texture : {} x {} ", tex_width, tex_height);
	}

	void OpenGLVideoWidget::resizeGL(int width, int height) {
		VideoWidgetEvent::OnWidgetResize(width, height);
//		director->Init(width, height);
		glViewport(0, 0, width, height);
//		if (cursor) {
//			cursor->OnWindowResized(width, height);
//		}
		std::cout << "resizeGL width : " << width << " height : " << height << std::endl;
	}

	MouseKey OpenGLVideoWidget::GetMouseKey(QMouseEvent* e) {
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

	void OpenGLVideoWidget::mouseMoveEvent(QMouseEvent* e) {
		QOpenGLWidget::mouseMoveEvent(e);
		VideoWidgetEvent::OnMouseMoveEvent(e);
	}

	void OpenGLVideoWidget::mousePressEvent(QMouseEvent* e) {
		QOpenGLWidget::mousePressEvent(e);
		VideoWidgetEvent::OnMousePressEvent(e);
	}

	void OpenGLVideoWidget::mouseReleaseEvent(QMouseEvent* e) {
		QOpenGLWidget::mouseReleaseEvent(e);
		VideoWidgetEvent::OnMouseReleaseEvent(e);
	}

	void OpenGLVideoWidget::mouseDoubleClickEvent(QMouseEvent* e) {
		QOpenGLWidget::mouseDoubleClickEvent(e);
	}

	void OpenGLVideoWidget::wheelEvent(QWheelEvent* e) {
		QOpenGLWidget::wheelEvent(e);
		VideoWidgetEvent::OnWheelEvent(e);
	}

	void OpenGLVideoWidget::keyPressEvent(QKeyEvent* e) {
		QOpenGLWidget::keyPressEvent(e);
		VideoWidgetEvent::OnKeyPressEvent(e);
	}

	void OpenGLVideoWidget::keyReleaseEvent(QKeyEvent* e) {
		QOpenGLWidget::keyReleaseEvent(e);
		VideoWidgetEvent::OnKeyReleaseEvent(e);
	}

	std::shared_ptr<Sprite> OpenGLVideoWidget::GetCursorSprite() {
		return cursor;
	}
}
