//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_SETTINGS_H
#define SAILFISH_CLIENT_PC_SETTINGS_H

namespace rgaa {

    enum class VideoRenderType {
        kSDL,
        kOpenGL,
        kTestQPixmap,
    };

    enum class MultiDisplayMode {
        kSeparated,
        kCombined,
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

        bool IsAudioEnabled();
        bool IsClipboardEnabled();

        VideoRenderType GetVideoRenderType();

    private:

        int ws_width_ = 1280;
        int ws_height_ = 768;

        VideoRenderType render_type_ = VideoRenderType::kOpenGL;

        bool audio_on_ = false;
        bool clipboard_on_ = true;

    };

}

#endif //SAILFISH_CLIENT_PC_SETTINGS_H
