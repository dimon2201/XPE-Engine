namespace xpe {

    namespace core {

        ObserverSampleCount::ObserverSampleCount(u32 sampleCount) : Observer<u32>(sampleCount)
        {
            m_OnChanged = [](u32 sampleCount) {
                EventManager::Get().SampleCountChangedEvents.NotifyAll(sampleCount);
            };
        }

    }

}