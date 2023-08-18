//
// Created by RGAA on 2023-08-17.
//

#ifndef SAILFISH_CLIENT_PC_STREAMDBMANAGER_H
#define SAILFISH_CLIENT_PC_STREAMDBMANAGER_H

#include <vector>
#include <memory>

namespace rgaa {

    class StreamItem;

    class StreamDBManager {
    public:

        StreamDBManager();
        ~StreamDBManager();

        std::vector<std::shared_ptr<StreamItem>> GetStreamItems();


    };

}

#endif //SAILFISH_CLIENT_PC_STREAMDBMANAGER_H
