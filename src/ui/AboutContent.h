//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_ABOUTCONTENT_H
#define SAILFISH_SERVER_ABOUTCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class AboutContent : public AppContent {
    public:

        explicit AboutContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~AboutContent() override;

        void OnContentShow() override;
        void OnContentHide() override;
    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
