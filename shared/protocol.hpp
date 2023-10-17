#pragma once

#include <cstddef>

namespace g {

    struct MessageStructure {
        enum Type {
            AskConnect,
            AcceptConnect,
            DeclineConnect,

            Message,
            MessageRecived,
        } type;

        size_t packageSize = 0;
    };
}
