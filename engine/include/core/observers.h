#pragma once

#include <core/observer.h>

namespace xpe {

    namespace core {

        struct ObserverSampleCount : public Observer<u32> {
            ObserverSampleCount(u32 sampleCount);
        };

    }

}