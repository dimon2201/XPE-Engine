#pragma once

#include <core/observer.hpp>

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