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

        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0x07a7374)));
        painter.drawRect(0, 0, width(), height());

    }

    void AppStreamList::CreateLayout() {
        auto root_layout = new QHBoxLayout(this);

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

        QObject::connect(stream_list_, &QListWidget::itemDoubleClicked, this, [](QListWidgetItem *item) {

        });

        root_layout->addWidget(stream_list_);

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

    }

    QListWidgetItem* AppStreamList::AddItem(const StreamItem& stream) {
        auto item = new QListWidgetItem(stream_list_);
        item->setSizeHint(QSize(220, 180));
        auto widget = new QWidget(stream_list_);
        widget->setStyleSheet("background:#dddddd;");

        auto layout = new QVBoxLayout();

        // id
        auto id = new QLabel(stream_list_);
        id->setObjectName("st_id");
        id->setText(stream.stream_id.c_str());
        layout->addWidget(id);

        // name
        auto label = new QLabel(stream_list_);
        label->setObjectName("st_name");
        label->setText(stream.stream_name.c_str());
        layout->addWidget(label);

        // alive
        auto alive = new QLabel(stream_list_);
        alive->setObjectName("st_alive");
        alive->setText("not alive");
        layout->addWidget(alive);

        // pid
        auto pid = new QLabel(stream_list_);
        pid->setObjectName("st_pid");
        pid->setText("Non");
        layout->addWidget(pid);

        // index
        auto heart_index = new QLabel(stream_list_);
        heart_index->setObjectName("st_index");
        heart_index->setText("0");
        layout->addWidget(heart_index);

        layout->addStretch();
        widget->setLayout(layout);
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

}