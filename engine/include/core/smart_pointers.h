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

            void Destroy();

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
            if (m_Ptr != nullptr)
                main_destruct(T, m_Ptr);
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void Scope<T>::Create(Args &&... args)
        {
            m_Ptr = main_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        void Scope<T>::Destroy()
        {
            Scope<T>::~Scope();
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

            void Destroy();

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
            if (m_Ptr != nullptr)
                hot_destruct(T, m_Ptr);
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void HotScope<T>::Create(Args &&... args)
        {
            m_Ptr = hot_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        void HotScope<T>::Destroy()
        {
            HotScope<T>::~HotScope();
        }

    }

}