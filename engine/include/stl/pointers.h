#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        using Weak = std::weak_ptr<T>;

        template<typename T>
        class Scope final
        {

        public:

            Scope() = default;
            ~Scope();

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

            template<typename... Args>
            void Create(Args &&... args);

            void Reset();

            inline auto& GetPtr() { return m_Ptr; }
            inline T* Get() const noexcept { return m_Ptr.get(); }
            inline T& operator*() const noexcept { return *m_Ptr.get(); }
            inline T* operator->() const noexcept { return m_Ptr.get(); }
            inline explicit operator bool() const noexcept { return m_Ptr.get(); }

        private:
            std::shared_ptr<T> m_Ptr;
        };

        template<typename T>
        template<typename... Args>
        void Ref<T>::Create(Args &&... args)
        {
            m_Ptr = {
                alloc_construct_args(T, std::forward<Args>(args)...),
                xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void Ref<T>::Reset()
        {
            m_Ptr.reset();
        }

        template<typename T>
        class HotScope final
        {

        public:

            HotScope() = default;
            ~HotScope();

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

            template<typename... Args>
            void Create(Args &&... args);

            void Reset();

            inline auto& GetPtr() { return m_Ptr; }
            inline T* Get() const noexcept { return m_Ptr.get(); }
            inline T& operator*() const noexcept { return *m_Ptr.get(); }
            inline T* operator->() const noexcept { return m_Ptr.get(); }
            inline explicit operator bool() const noexcept { return m_Ptr.get(); }

        private:
            std::shared_ptr<T> m_Ptr;
        };

        template<typename T>
        template<typename... Args>
        void HotRef<T>::Create(Args &&... args)
        {
            m_Ptr = {
                halloc_construct_args(T, std::forward<Args>(args)...),
                xpe::core::MemoryPoolManager::FreeHotMemory
            };
        }

        template<typename T>
        void HotRef<T>::Reset()
        {
            m_Ptr.reset();
        }

    }

}