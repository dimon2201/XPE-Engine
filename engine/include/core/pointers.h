#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        class Scope : public Object
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

        static std::unordered_map<void*, s64> RefCountTable = {};

        template<typename T>
        class Ref : public Object
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
            void AddCount(T* ptr);

            T* m_Ptr = nullptr;
        };

        template<typename T>
        template<typename... Args>
        void Ref<T>::Create(Args &&... args)
        {
            m_Ptr = alloc_construct_args(T, std::forward<Args>(args)...);
            RefCountTable.insert({ m_Ptr, 1 });
        }

        template<typename T>
        Ref<T>::~Ref()
        {
            RefCountTable[m_Ptr] -= 1;
            if (RefCountTable[m_Ptr] <= 0) {
                if (m_Ptr != nullptr) {
                    dealloc(m_Ptr);
                }
                m_Ptr = nullptr;
            }
        }

        template<typename T>
        void Ref<T>::AddCount(T* ptr)
        {
            m_Ptr = ptr;
            auto& count = RefCountTable[m_Ptr];
            if (count == 0) {
                count = 1;
            }
            ++count;
        }

        template<typename T>
        Ref<T>::Ref(T* ptr)
        {
            AddCount(ptr);
        }

        template<typename T>
        Ref<T>::Ref(const Ref<T> &other)
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        Ref<T>& Ref<T>::operator=(const Ref<T> &other)
        {
            AddCount(other.m_Ptr);
            return *this;
        }

        template<typename T>
        Ref<T>::Ref(Ref<T> &other)
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        Ref<T>::Ref(Ref<T> &&other) noexcept
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        Ref<T>& Ref<T>::operator=(Ref<T> &&other) noexcept
        {
            AddCount(other.m_Ptr);
            return *this;
        }

        template<typename T>
        class HotScope : public HotObject
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
        class HotRef : public HotObject
        {

        public:

            HotRef() = default;

            ~HotRef();

            HotRef(T* ptr);

            HotRef(const HotRef<T>& other);
            HotRef<T>& operator =(const HotRef<T>& other);

            HotRef(HotRef<T>& other);

            HotRef(Ref<T>&& other) noexcept;
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
            void AddCount(T* ptr);

            T* m_Ptr = nullptr;
        };

        template<typename T>
        template<typename... Args>
        void HotRef<T>::Create(Args &&... args)
        {
            m_Ptr = halloc_construct_args(T, std::forward<Args>(args)...);
            RefCountTable.insert({ m_Ptr, 1 });
        }

        template<typename T>
        HotRef<T>::~HotRef()
        {
            RefCountTable[m_Ptr] -= 1;
            if (RefCountTable[m_Ptr] <= 0) {
                if (m_Ptr != nullptr) {
                    dehalloc(m_Ptr);
                }
                m_Ptr = nullptr;
            }
        }

        template<typename T>
        void HotRef<T>::AddCount(T* ptr)
        {
            m_Ptr = ptr;
            auto& count = RefCountTable[m_Ptr];
            if (count == 0) {
                count = 1;
            }
            ++count;
        }

        template<typename T>
        HotRef<T>::HotRef(T* ptr)
        {
            AddCount(ptr);
        }

        template<typename T>
        HotRef<T>::HotRef(const HotRef<T> &other)
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        HotRef<T>& HotRef<T>::operator=(const HotRef<T> &other)
        {
            AddCount(other.m_Ptr);
            return *this;
        }

        template<typename T>
        HotRef<T>::HotRef(HotRef<T> &other)
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        HotRef<T>::HotRef(Ref<T> &&other) noexcept
        {
            AddCount(other.m_Ptr);
        }

        template<typename T>
        HotRef<T>& HotRef<T>::operator=(HotRef<T> &&other) noexcept
        {
            AddCount(other.m_Ptr);
            return *this;
        }

    }

}