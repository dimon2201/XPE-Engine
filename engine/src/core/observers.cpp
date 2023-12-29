namespace xpe {

    namespace core {

        sObserverSampleCount::sObserverSampleCount(u32 sampleCount) : sObserver<u32>(sampleCount)
        {
            m_OnChanged = [](u32 sampleCount) {
                cEventManager::Get().SampleCountChangedEvents.NotifyAll(sampleCount);
            };
        }

        sObserverWindowRatio::sObserverWindowRatio(float ratio) : sObserver<float>(ratio)
        {
            m_OnChanged = [](float ratio) {
                cEventManager::Get().WindowRatioChangedEvents.NotifyAll(ratio);
            };
        }

    }

}