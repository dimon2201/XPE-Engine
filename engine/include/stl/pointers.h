#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        class Scope final
        {

        public:
            Scope();
            template<typename ... Args>
            Scope(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::unique_ptr<T, void(*)(void*)> m_Ptr;
        };

        template<typename T>
        Scope<T>::Scope()
        {
            m_Ptr = {
                    alloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        Scope<T>::Scope(Args &&... args)
        {
            m_Ptr = {
                    alloc_construct_args(T, std::forward<Args>(args)...),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void Scope<T>::Reset()
        {
            m_Ptr.reset();
        }

        template<typename T>
        class Ref final
        {

        public:
            Ref();
            template<typename ... Args>
            Ref(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::shared_ptr<T> m_Ptr;
        };

        template<typename T>
        Ref<T>::Ref()
        {
            m_Ptr = {
                    alloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        Ref<T>::Ref(Args &&... args)
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
        class Weak final
        {

        public:
            Weak();
            template<typename ... Args>
            Weak(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::weak_ptr<T> m_Ptr;
        };

        template<typename T>
        Weak<T>::Weak()
        {
            m_Ptr = {
                    alloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        Weak<T>::Weak(Args &&... args)
        {
            m_Ptr = {
                    alloc_construct_args(T, std::forward<Args>(args)...),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void Weak<T>::Reset()
        {
            m_Ptr.reset();
        }

        template<typename T>
        class HotScope final
        {

        public:
            HotScope();
            template<typename ... Args>
            HotScope(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::unique_ptr<T, void(*)(void*)> m_Ptr;
        };

        template<typename T>
        HotScope<T>::HotScope()
        {
            m_Ptr = {
                    halloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        HotScope<T>::HotScope(Args &&... args)
        {
            m_Ptr = {
                    halloc_construct_args(T, std::forward<Args>(args)...),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void HotScope<T>::Reset()
        {
            m_Ptr.reset();
        }

        template<typename T>
        class HotRef final
        {

        public:
            HotRef();
            template<typename ... Args>
            HotRef(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::shared_ptr<T> m_Ptr;
        };

        template<typename T>
        HotRef<T>::HotRef()
        {
            m_Ptr = {
                    halloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        HotRef<T>::HotRef(Args &&... args)
        {
            m_Ptr = {
                    halloc_construct_args(T, std::forward<Args>(args)...),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void HotRef<T>::Reset()
        {
            m_Ptr.reset();
        }

        template<typename T>
        class HotWeak final
        {

        public:
            HotWeak();
            template<typename ... Args>
            HotWeak(Args &&... args);

        public:
            void Reset();

            inline auto& GetPtr() { return m_Ptr; }

            inline T& operator*() const noexcept { return m_Ptr.template operator*(); }
            inline T* operator->() const noexcept { return m_Ptr.template operator->(); }
            T* get() const noexcept { return m_Ptr.get(); }
            explicit operator bool() const noexcept { return m_Ptr; }

        private:
            std::weak_ptr<T> m_Ptr;
        };

        template<typename T>
        HotWeak<T>::HotWeak()
        {
            m_Ptr = {
                    halloc_construct(T),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        template<typename... Args>
        HotWeak<T>::HotWeak(Args &&... args)
        {
            m_Ptr = {
                    halloc_construct_args(T, std::forward<Args>(args)...),
                    xpe::core::MemoryPoolManager::FreeMainMemory
            };
        }

        template<typename T>
        void HotWeak<T>::Reset()
        {
            m_Ptr.reset();
        }

    }

}