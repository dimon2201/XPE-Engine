namespace xpe {

    namespace core {

        void DebugManager::SetDebugger(Debugger* const debugger) {
            m_Debugger = debugger;
            m_Debugger->SetCallback(this);
        }

        void DebugManager::LogMessages() {
            auto messages = m_Debugger->GetMessageQueue();
            for (const auto& message : messages) {
                ReceiveMessage(message);
            }
        }

        void DebugManager::LogLastMessage() {
            auto message = m_Debugger->GetLastMessage();
            ReceiveMessage(message);
        }

        void DebugManager::ReceiveMessage(const DebugMessage& message) {

        }

    }

}