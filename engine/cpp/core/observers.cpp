namespace xpe {

    namespace core {

        sObserverSampleCount::sObserverSampleCount(u32 sampleCount) : sObserver<u32>(sampleCount)
        {
            m_OnChanged = [](u32 sampleCount) {
                cEventManager::Get().SampleCountChangedEvents.NotifyAll(sampleCount);
            };
        }

    }

}