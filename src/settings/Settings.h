//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_SETTINGS_H
#define SAILFISH_CLIENT_PC_SETTINGS_H

namespace rgaa {

    enum class VideoRenderType {
        kOpenGL,
        kSDL,
        kTestQPixmap,
    };

    class Settings {
    public:

        static Settings* Instance() {
            static Settings sts;
            return &sts;
        }

        void LoadSettings();

        int GetWSWidth();
        int GetWSHeight();

        VideoRenderType GetVideoRenderType();

    private:

        int ws_width_ = 1280;
        int ws_height_ = 768;

        VideoRenderType render_type_ = VideoRenderType::kSDL;

    };

}

#endif //SAILFISH_CLIENT_PC_SETTINGS_H
