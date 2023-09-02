//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_SETTINGSCONTENT_H
#define SAILFISH_SERVER_SETTINGSCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class Settings;
    class MultiDisplayModeWidget;

    class SettingsContent : public AppContent {
    public:

        SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~SettingsContent();

        void OnContentShow() override;
        void OnContentHide() override;

    private:

        Settings* settings_ = nullptr;
        MultiDisplayModeWidget* combined_;
        MultiDisplayModeWidget* separated_;
    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
