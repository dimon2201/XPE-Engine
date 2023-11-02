#pragma once

#include <core/observer.h>

namespace xpe {

    namespace core {

        struct sObserverSampleCount : public sObserver<u32> {
            sObserverSampleCount(u32 sampleCount);
        };

        struct sObserverWindowRatio : public sObserver<float> {
            sObserverWindowRatio(float ratio);
        };

    }

}