//
// Created by RGAA on 2023/8/14.
//

#include "AppStreamList.h"

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QtWidgets/QMenu>

#include "sdk/StreamItem.h"
#include "sdk/StreamDBManager.h"
#include "Context.h"
#include "AppMessage.h"
#include "rgaa_common/RMessage.h"
#include "rgaa_common/RMessageQueue.h"
#include "rgaa_common/RLog.h"
#include "WidgetHelper.h"
#include "MessageDialog.h"
#include "StreamItemWidget.h"

namespace rgaa {

    class MainItemDelegate : public QStyledItemDelegate
    {

    public:
        MainItemDelegate(QObject* pParent) {}
        virtual ~MainItemDelegate() {}

        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
            editor->setGeometry(option.rect);
        }
    };

    // - - -- - - -- - - - -- -

    AppStreamList::AppStreamList(const std::shared_ptr<Context>& ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        db_mgr_ = context_->GetDBManager();

        CreateLayout();
        Init();

        LoadStreamItems();
    }

    AppStreamList::~AppStreamList() {
        context_->RemoveMessageTask(stream_added_task_id_);
    }

    void AppStreamList::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);

//        QPainter painter(this);
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QBrush(QColor(0x07a7374)));
//        painter.drawRect(0, 0, width(), height());

    }

    void AppStreamList::CreateLayout() {
        auto root_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(root_layout);

        auto delegate = new MainItemDelegate(this);
        stream_list_ = new QListWidget(this);
        stream_list_->setItemDelegate(delegate);

        stream_list_->setMovement(QListView::Static);
        stream_list_->setViewMode(QListView::IconMode);
        stream_list_->setFlow(QListView::LeftToRight);
        stream_list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        stream_list_->setResizeMode(QListWidget::Adjust);
        stream_list_->setContextMenuPolicy(Qt::CustomContextMenu);
        stream_list_->setSpacing(10);

        QObject::connect(stream_list_, &QListWidget::customContextMenuRequested, this, [=](const QPoint& pos) {
            QListWidgetItem* cur_item = stream_list_->itemAt(pos);
            if (cur_item == nullptr) { return; }
            int index = stream_list_->row(cur_item);
            RegisterActions(index);
        });

        QObject::connect(stream_list_, &QListWidget::itemDoubleClicked, this, [=, this](QListWidgetItem *item) {
            int index = stream_list_->row(item);
            StreamItem stream_item = streams_.at(index);
            NotifyStartStream(stream_item);
        });

        root_layout->addSpacing(10);
        root_layout->addWidget(stream_list_);
        root_layout->addSpacing(10);

        setLayout(root_layout);
    }

    void AppStreamList::Init() {
        stream_added_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeStreamAdded, [=, this](const std::shared_ptr<Message>& msg) {
            auto target_msg = std::dynamic_pointer_cast<StreamItemAdded>(msg);
            auto item = target_msg->item_;
            auto db_mgr = context_->GetDBManager();
            db_mgr->AddStream(item);

            LoadStreamItems();

        }));
    }

    void AppStreamList::RegisterActions(int index) {
        std::vector<QString> actions = {
            tr("Start"),
            tr("Stop"),
            "",
            tr("Edit"),
            tr("Delete"),
        };
        QMenu* menu = new QMenu();
        for (int i = 0; i < actions.size(); i++) {
            QString action_name = actions.at(i);
            if (action_name.isEmpty()) {
                menu->addSeparator();
                continue;
            }

            QAction* action = new QAction(action_name, menu);
            menu->addAction(action);
            QObject::connect(action, &QAction::triggered, this, [=]() {
                ProcessAction(i, streams_.at(index));
            });
        }
        menu->exec(QCursor::pos());
        delete menu;
    }

    void AppStreamList::ProcessAction(int index, const StreamItem& item) {
        if (index == 0) {
            // start
            NotifyStartStream(item);
        }
        else if (index == 1) {
            // stop
        }
        else if (index == 3) {
            // edit
        }
        else if (index == 4) {
            // delete
            DeleteStream(item);
        }
    }

    void AppStreamList::NotifyStartStream(const StreamItem& item) {
        if (dbk_callback_) {
            dbk_callback_(item);
        }
    }

    QListWidgetItem* AppStreamList::AddItem(const StreamItem& stream) {
        auto item = new QListWidgetItem(stream_list_);
        item->setSizeHint(QSize(220, 145));
        auto widget = new StreamItemWidget(stream.bg_color, stream_list_);

        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        root_layout->setContentsMargins(20, 0, 20, 0);

        auto layout = new QVBoxLayout();
        layout->addStretch();
        WidgetHelper::ClearMargin(layout);
        root_layout->addLayout(layout);

        auto gap = 5;

        // name
        auto name = new QLabel(stream_list_);
        name->setObjectName("st_name");
        name->setText(stream.stream_name.c_str());
        name->setStyleSheet(R"(color:#386487; font-size:14px; font-weight:bold;)");
        layout->addWidget(name);

        // host
        auto host = new QLabel(stream_list_);
        host->setObjectName("st_host");
        host->setText(stream.stream_host.c_str());
        host->setStyleSheet(R"(color:#386487; font-size:14px; )");
        layout->addSpacing(gap);
        layout->addWidget(host);

        //
        auto port = new QLabel(stream_list_);
        port->setObjectName("st_port");
        port->setText(std::to_string(stream.stream_port).c_str());
        port->setStyleSheet(R"(color:#386487; font-size:14px; )");
        layout->addSpacing(gap);
        layout->addWidget(port);

        //
        auto bitrate = new QLabel(stream_list_);
        bitrate->setObjectName("st_bitrate");
        std::string bt_str = std::to_string(stream.encode_bps) + " Mbps";
        bitrate->setText(bt_str.c_str());
        bitrate->setStyleSheet(R"(color:#386487; font-size:14px; )");
        layout->addSpacing(gap);
        layout->addWidget(bitrate);

        auto fps = new QLabel(stream_list_);
        fps->setObjectName("st_fps");
        std::string fps_str = std::to_string(stream.encode_fps) + " FPS";
        fps->setText(fps_str.c_str());
        fps->setStyleSheet(R"(color:#386487; font-size:14px; )");
        layout->addSpacing(gap);
        layout->addWidget(fps);

        layout->addStretch();

        root_layout->addLayout(layout);
        layout->addSpacing(6);
        widget->setLayout(root_layout);
        stream_list_->setItemWidget(item, widget);
        return item;
    }

    void AppStreamList::LoadStreamItems() {
        auto db_mgr = context_->GetDBManager();
        streams_ = db_mgr->GetAllStreams();

        context_->PostUITask([=, this]() {
            int count = stream_list_->count();
            for (int i = 0; i < count; i++) {
                auto item = stream_list_->takeItem(0);
                delete item;
            }
            for (const auto& stream : streams_) {
                AddItem(stream);
            }
        });
    }

    void AppStreamList::DeleteStream(const StreamItem& item) {
        auto alert = MessageDialog::Make(context_, tr("Do you want to *DELETE* the stream ?"));
        if (alert->exec() == DialogButton::kCancel) {
            return;
        }

        auto mgr = context_->GetDBManager();
        mgr->DeleteStream(item._id);

        LoadStreamItems();
    }

    void AppStreamList::SetOnItemDoubleClickedCallback(OnItemDoubleClickedCallback&& cbk) {
        dbk_callback_ = std::move(cbk);
    }

}