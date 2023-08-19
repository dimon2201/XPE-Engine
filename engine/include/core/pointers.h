#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        class Scope final
        {

        public:

            Scope() = default;
            ~Scope();

            Scope(const Scope<T>& other) = delete;
            Scope<T>& operator =(const Scope<T>& other) = delete;
            Scope<T>& operator =(Scope<T>& other) = delete;

            Scope(Scope<T>&& other) noexcept = delete;
            Scope<T>& operator =(Scope<T>&& other) noexcept = delete;

            template<typename... Args>
            void Create(Args &&... args);

            inline T* Get() const noexcept { return m_Ptr; }
            inline T& operator*() const noexcept { return *m_Ptr; }
            inline T* operator->() const noexcept { return m_Ptr; }
            inline explicit operator bool() const noexcept { return m_Ptr; }

        private:
            T* m_Ptr = nullptr;
        };

        template<typename T>
        Scope<T>::~Scope()
        {
            if (m_Ptr != nullptr) {
                dealloc(m_Ptr);
            }
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void Scope<T>::Create(Args &&... args)
        {
            m_Ptr = alloc_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        class Ref final
        {

        public:

            Ref() = default;

            ~Ref();

            Ref(T* ptr);

            Ref(const Ref<T>& other);
            Ref<T>& operator =(const Ref<T>& other);

            Ref(Ref<T>& other);

            Ref(Ref<T>&& other) noexcept;
            Ref<T>& operator =(Ref<T>&& other) noexcept;

            template<typename... Args>
            void Create(Args &&... args);

            inline T* Get() const noexcept { return m_Ptr; }

            inline T& operator*() const noexcept { return *m_Ptr; }
            inline T* operator->() const noexcept { return m_Ptr; }

            inline explicit operator bool() const noexcept { return m_Ptr; }

            inline bool operator==(const Ref<T>& other) { return m_Ptr == other.m_Ptr; }
            inline bool operator==(const T* other) { return m_Ptr == other; }

            inline bool operator!=(const Ref<T>& other) { return m_Ptr != other.m_Ptr; }
            inline bool operator!=(const T* other) { return m_Ptr != other; }

        private:
            T* m_Ptr = nullptr;
            static std::unordered_map<void*, s64> s_CountTable;
        };

        template<typename T>
        std::unordered_map<void*, s64> Ref<T>::s_CountTable = {};

        template<typename T>
        template<typename... Args>
        void Ref<T>::Create(Args &&... args)
        {
            m_Ptr = alloc_construct_args(T, std::forward<Args>(args)...);
            s_CountTable.insert({ m_Ptr, 1 });
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
        }

        template<typename T>
        Ref<T>::~Ref()
        {
            s_CountTable[m_Ptr] -= 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
            if (s_CountTable[m_Ptr] < 0) {
                if (m_Ptr != nullptr) {
                    dealloc(m_Ptr);
                }
                m_Ptr = nullptr;
            }
        }

        template<typename T>
        Ref<T>::Ref(T* ptr)
        {
            m_Ptr = ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
        }

        template<typename T>
        Ref<T>::Ref(const Ref<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
        }

        template<typename T>
        Ref<T>& Ref<T>::operator=(const Ref<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
            return *this;
        }

        template<typename T>
        Ref<T>::Ref(Ref<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
        }

        template<typename T>
        Ref<T>::Ref(Ref<T> &&other) noexcept
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
        }

        template<typename T>
        Ref<T>& Ref<T>::operator=(Ref<T> &&other) noexcept
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            std::cout << "Ref = " << m_Ptr << ", count = " << s_CountTable[m_Ptr] << std::endl;
            return *this;
        }

        template<typename T>
        class HotScope final
        {

        public:

            HotScope() = default;
            ~HotScope();

            HotScope(const HotScope<T>& other) = delete;
            HotScope(HotScope<T>& other) = delete;
            HotScope<T>& operator =(const HotScope<T>& other) = delete;
            HotScope<T>& operator =(HotScope<T>& other) = delete;
            HotScope<T>& operator =(HotScope<T> other) = delete;

            HotScope(HotScope<T>&& other) noexcept = delete;
            HotScope<T>& operator =(HotScope<T>&& other) noexcept = delete;

            template<typename... Args>
            void Create(Args &&... args);

            inline T* Get() const noexcept { return m_Ptr; }
            inline T& operator*() const noexcept { return *m_Ptr; }
            inline T* operator->() const noexcept { return m_Ptr; }
            inline explicit operator bool() const noexcept { return m_Ptr; }

        private:
            T* m_Ptr = nullptr;
        };

        template<typename T>
        HotScope<T>::~HotScope()
        {
            if (m_Ptr != nullptr) {
                dehalloc(m_Ptr);
            }
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void HotScope<T>::Create(Args &&... args)
        {
            m_Ptr = halloc_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        class HotRef final
        {

        public:

            HotRef() = default;

            ~HotRef();

            HotRef(const HotRef<T>& other);
            HotRef<T>& operator =(const HotRef<T>& other);

            HotRef(HotRef<T>& other);

            HotRef(HotRef<T>&& other) noexcept;
            HotRef<T>& operator =(HotRef<T>&& other) noexcept;

            template<typename... Args>
            void Create(Args &&... args);

            inline T* Get() const noexcept { return m_Ptr; }
            inline T& operator*() const noexcept { return *m_Ptr; }
            inline T* operator->() const noexcept { return m_Ptr; }
            inline explicit operator bool() const noexcept { return m_Ptr; }

            inline bool operator==(const HotRef<T>& other) { return m_Ptr == other.m_Ptr; }
            inline bool operator==(const T* other) { return m_Ptr == other; }

            inline bool operator!=(const HotRef<T>& other) { return m_Ptr != other.m_Ptr; }
            inline bool operator!=(const T* other) { return m_Ptr != other; }

        private:
            T* m_Ptr = nullptr;
            static std::unordered_map<void*, s64> s_CountTable;
        };

        template<typename T>
        std::unordered_map<void*, s64> HotRef<T>::s_CountTable = {};

        template<typename T>
        template<typename... Args>
        void HotRef<T>::Create(Args &&... args)
        {
            m_Ptr = halloc_construct_args(T, std::forward<Args>(args)...);
            s_CountTable.insert({ m_Ptr, 1 });
        }

        template<typename T>
        HotRef<T>::~HotRef()
        {
            s_CountTable[m_Ptr] -= 1;
            if (s_CountTable[m_Ptr] < 0) {
                if (m_Ptr != nullptr) {
                    dehalloc(m_Ptr);
                }
                m_Ptr = nullptr;
            }
        }

        template<typename T>
        HotRef<T>::HotRef(const HotRef<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
        }

        template<typename T>
        HotRef<T>& HotRef<T>::operator=(const HotRef<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            return *this;
        }

        template<typename T>
        HotRef<T>::HotRef(HotRef<T> &&other) noexcept
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
        }

        template<typename T>
        HotRef<T>& HotRef<T>::operator=(HotRef<T> &&other) noexcept
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
            return *this;
        }

        template<typename T>
        HotRef<T>::HotRef(HotRef<T> &other)
        {
            m_Ptr = other.m_Ptr;
            s_CountTable[m_Ptr] += 1;
        }

    }

}