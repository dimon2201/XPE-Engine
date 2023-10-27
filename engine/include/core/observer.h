#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        struct Observer {
            T Value;

            Observer(const T& value, const std::function<void(T)> onChanged = {})
            : Value(value), m_OnChanged(onChanged) {}

            inline Observer& operator=(const T& other) {
                if (other != Value) {
                    Value = other;
                    m_OnChanged(Value);
                }
            }

            inline void operator()() { m_OnChanged(); }
            inline operator T() const { return Value; }

            inline bool operator==(const T& other) { return Value == other; }
            inline bool operator!=(const T& other) { return Value != other; }
            inline bool operator>(const T& other) { return Value > other; }
            inline bool operator<(const T& other) { return Value < other; }
            inline bool operator<=(const T& other) { return Value <= other; }
            inline bool operator>=(const T& other) { return Value >= other; }

            inline bool operator==(u32 other) { return Value == other; }
            inline bool operator!=(u32 other) { return Value != other; }
            inline bool operator>(u32 other) { return Value > other; }
            inline bool operator<(u32 other) { return Value < other; }
            inline bool operator<=(u32 other) { return Value <= other; }
            inline bool operator>=(u32 other) { return Value >= other; }

            inline bool operator==(int other) { return Value == other; }
            inline bool operator!=(int other) { return Value != other; }
            inline bool operator>(int other) { return Value > other; }
            inline bool operator<(int other) { return Value < other; }
            inline bool operator<=(int other) { return Value <= other; }
            inline bool operator>=(int other) { return Value >= other; }

        protected:
            std::function<void(T)> m_OnChanged;
        };

    }

}