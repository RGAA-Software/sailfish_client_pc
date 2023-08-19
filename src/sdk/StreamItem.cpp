//
// Created by RGAA on 2023/8/14.
//

#include "StreamItem.h"

namespace rgaa {

    bool StreamItem::IsValid() const {
        return !stream_id.empty();
    }

}
