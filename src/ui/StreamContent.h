//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_INFORMATIONCONTENT_H
#define SAILFISH_SERVER_INFORMATIONCONTENT_H

#include "AppContent.h"

#include "sdk/StreamItem.h"

namespace rgaa {

    class Context;
    class AppStreamList;

    using OnStartingStreamCallback = std::function<void(const StreamItem&)>;

    class StreamContent : public AppContent {
    public:

        explicit StreamContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~StreamContent();

        void SetOnStartingStreamCallback(OnStartingStreamCallback&& cbk);

        void OnContentShow() override;
        void OnContentHide() override;

    private:

        AppStreamList* stream_list_ = nullptr;
        OnStartingStreamCallback starting_stream_cbk_;

    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
