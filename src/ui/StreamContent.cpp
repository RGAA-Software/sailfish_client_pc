//
// Created by RGAA on 2023/8/16.
//

#include "StreamContent.h"

#include "settings/Settings.h"
#include "rgaa_common/RLog.h"
#include "Context.h"
#include "AppStreamList.h"
#include "WidgetHelper.h"

namespace rgaa {

    StreamContent::StreamContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);

        auto stream_list = new AppStreamList(context_, this);
        stream_list_ = stream_list;
        root_layout->addWidget(stream_list);

        setLayout(root_layout);

        stream_list_->SetOnItemDoubleClickedCallback([=, this](const StreamItem& item) {
            if (starting_stream_cbk_) {
                starting_stream_cbk_(item);
            }
        });
    }

    StreamContent::~StreamContent() {

    }

    void StreamContent::SetOnStartingStreamCallback(OnStartingStreamCallback&& cbk) {
        starting_stream_cbk_ = std::move(cbk);
    }

    void StreamContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void StreamContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}