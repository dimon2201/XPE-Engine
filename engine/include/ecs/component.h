#pragma once

namespace xpe {

    namespace ecs {

        using namespace core;

        class Entity;

        struct ENGINE_API Component : public Object, res::JsonObject
        {
            ecs::Entity* Entity = nullptr;

            Component() = default;

            Component(const string& tag) { m_Tag = tag; }
        };

        struct ENGINE_API ComponentStorage : public Object
        {
            template<typename T>
            [[nodiscard]] inline size_t GetSize() const
            {
                return m_Components.size() / sizeof(T);
            }

            template<typename T>
            [[nodiscard]] inline size_t GetCapacity() const
            {
                return m_Components.capacity() / sizeof(T);
            }

            template<typename T>
            inline T* Get(int i)
            {
                return (T*) (m_Components[i * sizeof(T)]);
            }

            [[nodiscard]] inline bool HasCapacity() const
            {
                return m_Components.size() < m_Components.capacity();
            }

            template<typename T>
            T* Get(const string& tag);

            Component* GetAddress(usize componentSize, const string& tag);

            template<typename T>
            void Reserve(usize newCapacity);

            template<typename T>
            void Resize(usize newSize);

            template<typename T, typename... Args>
            T* Add(Args &&... args);

            template<typename T, typename... Args>
            void Update(T* component, Args &&... args);

            template<typename T>
            void Rename(const string& oldTag, const string& newTag);

            template<typename T>
            void EraseAt(int index, usize typeSize);

            template<typename T>
            void Erase(const string& tag);

            template<typename T>
            void ForEach(const std::function<void(T*)> &iterateFunction);

            void Clear();

            inline vector<u8>& Data()
            {
                return m_Components;
            }

            inline bool NotEmpty()
            {
                return m_Components.size() > 0;
            }

            inline bool Empty()
            {
                return m_Components.size() <= 0;
            }

        private:
            vector<u8> m_Components;
        };

        template<typename T>
        void ComponentStorage::Reserve(size_t newCapacity)
        {
            m_Components.reserve(sizeof(T) * newCapacity);
        }

        template<typename T>
        void ComponentStorage::Resize(size_t newSize)
        {
            m_Components.resize(sizeof(T) * newSize);
        }

        template<typename T, typename... Args>
        T* ComponentStorage::Add(Args &&... args)
        {
            usize componentsSize = m_Components.size();
            m_Components.resize(componentsSize + sizeof(T));
            T* component = reinterpret_cast<T*>(&m_Components[componentsSize]);
            ::new (component) T(std::forward<Args>(args)...);
            return component;
        }

        template<typename T, typename... Args>
        void ComponentStorage::Update(T* component, Args &&... args)
        {
            component->~T();
            ::new (component) T(std::forward<Args>(args)...);
        }

        template<typename T>
        void ComponentStorage::Rename(const string &oldTag, const string &newTag)
        {
            u64 newId = Hash(newTag);
            usize size = m_Components.size();
            usize typeSize = sizeof(T);

            for (usize i = 0; i < size; i += typeSize)
            {
                T* component = (T*) &m_Components[i];
                u64 oldId = Hash(component->GetTag());
                if (oldId == newId)
                {
                    component->SetTag(newTag);
                    break;
                }
            }
        }

        template<typename T>
        void ComponentStorage::EraseAt(int index, usize typeSize)
        {
            auto begin = m_Components.begin() + (index * typeSize);
            auto end = begin + typeSize;
            m_Components.erase(begin, end);
        }

        template<typename T>
        void ComponentStorage::Erase(const string& tag)
        {
            u64 id = Hash(tag);
            usize size = m_Components.size();
            usize typeSize = sizeof(T);

            for (usize i = 0; i < size; i += typeSize)
            {
                T* component = (T*) &m_Components[i];
                u64 cid = Hash(component->GetTag());
                if (cid == id)
                {
                    component->~T();
                    EraseAt<T>(i / typeSize, typeSize);
                    break;
                }
            }
        }

        template<typename T>
        void ComponentStorage::ForEach(const std::function<void(T*)> &iterateFunction)
        {
            usize size = m_Components.size();
            usize step = sizeof(T);

            for (usize i = 0; i < size; i += step)
            {
                T* component = (T*) &m_Components[i];
                iterateFunction(component);
            }
        }

        template<typename T>
        T* ComponentStorage::Get(const string& tag)
        {
            u64 id = Hash(tag);
            usize size = m_Components.size();
            usize step = sizeof(T);

            for (usize i = 0; i < size; i += step)
            {
                T* component = (T*) &m_Components[i];
                u64 cid = Hash(component->GetTag());
                if (cid == id) {
                    return component;
                }
            }

            return nullptr;
        }

    }

}