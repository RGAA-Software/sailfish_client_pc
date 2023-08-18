//
// Created by RGAA on 2023-08-17.
//

#include "StreamDBManager.h"

#include <vector>

#include "StreamItem.h"

namespace rgaa {

    StreamDBManager::StreamDBManager() {

    }

    StreamDBManager::~StreamDBManager() {

    }

    std::vector<std::shared_ptr<StreamItem>> StreamDBManager::GetStreamItems() {
        std::vector<std::shared_ptr<StreamItem>> items;
        for (int i = 0; i < 5; i++) {
            auto item = std::make_shared<StreamItem>();
            item->stream_id_ = "id : " + std::to_string(i);
            item->stream_name_ = "name : " + std::to_string(i * 2);
            items.push_back(item);
        }
        return items;
    }

}
