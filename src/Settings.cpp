//
// Created by RGAA on 2023-08-10.
//

#include "Settings.h"

namespace rgaa {

    void Settings::LoadSettings() {

    }

    int Settings::GetWSWidth() {
        return ws_width_;
    }

    int Settings::GetWSHeight() {
        return ws_height_;
    }

    VideoRenderType Settings::GetVideoRenderType() {
        return render_type_;
    }

    bool Settings::IsAudioEnabled() {
        return audio_on_;
    }

    bool Settings::IsClipboardEnabled() {
        return clipboard_on_;
    }

}