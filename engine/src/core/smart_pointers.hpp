#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        class cScope : public cObject
        {

        public:

            cScope() = default;
            ~cScope();

            cScope(const cScope<T>& other) = delete;
            cScope<T>& operator =(const cScope<T>& other) = delete;
            cScope<T>& operator =(cScope<T>& other) = delete;

            cScope(cScope<T>&& other) noexcept = delete;
            cScope<T>& operator =(cScope<T>&& other) noexcept = delete;

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
        cScope<T>::~cScope()
        {
            if (m_Ptr != nullptr)
                main_destruct(T, m_Ptr);
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void cScope<T>::Create(Args &&... args)
        {
            m_Ptr = main_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        void cScope<T>::Destroy()
        {
            cScope<T>::~cScope();
        }

        template<typename T>
        class cHotScope : public cHotObject
        {

        public:

            cHotScope() = default;
            ~cHotScope();

            cHotScope(const cHotScope<T>& other) = delete;
            cHotScope(cHotScope<T>& other) = delete;
            cHotScope<T>& operator =(const cHotScope<T>& other) = delete;
            cHotScope<T>& operator =(cHotScope<T>& other) = delete;
            cHotScope<T>& operator =(cHotScope<T> other) = delete;

            cHotScope(cHotScope<T>&& other) noexcept = delete;
            cHotScope<T>& operator =(cHotScope<T>&& other) noexcept = delete;

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
        cHotScope<T>::~cHotScope()
        {
            if (m_Ptr != nullptr)
                hot_destruct(T, m_Ptr);
            m_Ptr = nullptr;
        }

        template<typename T>
        template<typename... Args>
        void cHotScope<T>::Create(Args &&... args)
        {
            m_Ptr = hot_construct_args(T, std::forward<Args>(args)...);
        }

        template<typename T>
        void cHotScope<T>::Destroy()
        {
            cHotScope<T>::~cHotScope();
        }

    }

}