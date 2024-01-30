#pragma once

namespace xpe
{
    namespace core
    {
        template<typename... FunctionArgs>
        struct sEvent
        {
            u64 Id = 0;
            std::function<void(FunctionArgs...)> Function = {};
            int Priority = 0;

            sEvent() = default;

            sEvent(const string& name, int priority, const std::function<void(FunctionArgs...)>& function)
            : Id(Hash(name)), Priority(priority), Function(function) {}

            sEvent(u64 id, int priority, const std::function<void(FunctionArgs...)>& function)
            : Id(id), Priority(priority), Function(function) {}

            inline friend bool operator<(const sEvent<FunctionArgs...>& e1, const sEvent<FunctionArgs...>& e2)
            {
                return e1.Priority < e2.Priority;
            }

            inline friend bool operator>(const sEvent<FunctionArgs...>& e1, const sEvent<FunctionArgs...>& e2)
            {
                return e1.Priority > e2.Priority;
            }
        };

        template<typename... FunctionArgs>
        class cEventBuffer : public cObject
        {

        public:
            ~cEventBuffer();

            template<typename... Args>
            void Add(Args &&... eventArgs);

            void Remove(const string& eventName);

            void Remove(u64 eventId);

            void Clear();

            void Reserve(const usize count);

            template<typename... Args>
            void Notify(Args &&... args);

            inline const std::vector<sEvent<FunctionArgs...>>& GetEvents() const
            {
                return m_Events;
            }

        private:
            std::vector<sEvent<FunctionArgs...>> m_Events;
            std::mutex m_Mutex;
        };

        template<typename... FunctionArgs>
        cEventBuffer<FunctionArgs...>::~cEventBuffer()
        {
            m_Events.clear();
        }

        template<typename... FunctionArgs>
        template<typename... Args>
        void cEventBuffer<FunctionArgs...>::Add(Args &&... eventArgs)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Events.emplace_back(std::forward<Args>(eventArgs)...);
            // store event functions by priority in ascending order
            std::sort(m_Events.begin(), m_Events.end(), [](sEvent<FunctionArgs...>& e1, sEvent<FunctionArgs...>& e2) { return e1 < e2; });
        }

        template<typename... FunctionArgs>
        void cEventBuffer<FunctionArgs...>::Remove(const string& eventName)
        {
            Remove(Hash(eventName));
        }

        template<typename... FunctionArgs>
        void cEventBuffer<FunctionArgs...>::Remove(u64 eventId)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            usize size = m_Events.size();
            for (int i = 0 ; i < size ; i++)
            {
                auto& event = m_Events[i];
                if (eventId == event.Id)
                {
                    m_Events.erase(m_Events.begin() + i);
                    break;
                }
            }
        }

        template<typename... FunctionArgs>
        void cEventBuffer<FunctionArgs...>::Clear()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Events.clear();
        }

        template<typename... FunctionArgs>
        void cEventBuffer<FunctionArgs...>::Reserve(const usize count)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Events.reserve(count);
        }

        template<typename... FunctionArgs>
        template<typename... Args>
        void cEventBuffer<FunctionArgs...>::Notify(Args &&... args)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            for (auto& event : m_Events)
            {
                event.Function(args...);
            }
        }

        enum eWindowFrameResizedPriority
        {
            RENDER_SYSTEM = 1,
            TEXTURE = 2,
            RENDER_TARGET = 3,
            CAMERA = 4
        };

        struct ENGINE_API MEvent final
        {
            static cEventBuffer<void> WindowClosed;
            static cEventBuffer<int, int> WindowResized;
            static cEventBuffer<int, int> WindowFrameResized;
            static cEventBuffer<int, int> WindowMoved;
            static cEventBuffer<void> WindowFocused;
            static cEventBuffer<void> WindowFocusLost;
            static cEventBuffer<float> WindowRatioChanged;

            static cEventBuffer<const eKey> KeyPressed;
            static cEventBuffer<const eKey> KeyReleased;
            static cEventBuffer<const eKey> KeyHold;

            static cEventBuffer<const eMouse> MousePressed;
            static cEventBuffer<const eMouse> MouseReleased;
            static cEventBuffer<const eMouse> MouseHold;

            static cEventBuffer<const double, const double> CursorMoved;
            static cEventBuffer<void> CursorEntered;
            static cEventBuffer<void> CursorLeft;

            static cEventBuffer<const double, const double> ScrollChanged;

            static cEventBuffer<const u32> CharTyped;
            static cEventBuffer<const u32, const int> CharModsTyped;

            static cEventBuffer<u32> SampleCountChanged;
        };
    }
}