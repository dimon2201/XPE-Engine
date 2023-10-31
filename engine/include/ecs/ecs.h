#pragma once

namespace physx
{
    struct PxScene;
}

namespace xpe
{

    namespace ecs
    {
        using namespace core;
        using namespace res;
        using namespace math;

        class cEntity;

        struct ENGINE_API sComponent : public cObject, public cJson
        {
            cEntity* Entity = nullptr;
            bool FollowEntity = true;
        };

        struct ENGINE_API sComponentStorage : public cObject
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

            sComponent* GetAddress(usize componentSize, cEntity* entity);

            template<typename T>
            void Reserve(usize newCapacity);

            template<typename T>
            void Resize(usize newSize);

            template<typename T, typename... Args>
            T* Add(Args &&... args);

            template<typename T, typename... Args>
            void Update(T* component, Args &&... args);

            template<typename T>
            void EraseAt(int index, usize typeSize);

            template<typename T>
            void Erase(cEntity* entity);

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
        void sComponentStorage::Reserve(size_t newCapacity)
        {
            m_Components.reserve(sizeof(T) * newCapacity);
        }

        template<typename T>
        void sComponentStorage::Resize(size_t newSize)
        {
            m_Components.resize(sizeof(T) * newSize);
        }

        template<typename T, typename... Args>
        T* sComponentStorage::Add(Args &&... args)
        {
            usize componentsSize = m_Components.size();
            m_Components.resize(componentsSize + sizeof(T));
            T* component = reinterpret_cast<T*>(&m_Components[componentsSize]);
            ::new (component) T(std::forward<Args>(args)...);
            return component;
        }

        template<typename T, typename... Args>
        void sComponentStorage::Update(T* component, Args &&... args)
        {
            component->~T();
            ::new (component) T(std::forward<Args>(args)...);
        }

        template<typename T>
        void sComponentStorage::EraseAt(int index, usize typeSize)
        {
            auto begin = m_Components.begin() + (index * typeSize);
            auto end = begin + typeSize;
            m_Components.erase(begin, end);
        }

        template<typename T>
        void sComponentStorage::Erase(cEntity* entity)
        {
            usize size = m_Components.size();
            usize typeSize = sizeof(T);

            for (usize i = 0; i < size; i += typeSize)
            {
                T* component = (T*) &m_Components[i];
                if (component->Entity == entity)
                {
                    component->~T();
                    EraseAt<T>(i / typeSize, typeSize);
                    break;
                }
            }
        }

        template<typename T>
        void sComponentStorage::ForEach(const std::function<void(T*)> &iterateFunction)
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
        T* sComponentStorage::Get(const string& tag)
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

        class cEntity;
        class cGlobal;

        typedef usize ComponentType;
        typedef usize GlobalType;

        class ENGINE_API cScene : public cObject, public cJson
        {

        public:

            virtual ~cScene();

            void AddEntity(const string& tag, cEntity* entity);

            void RemoveEntity(const string& tag);

            void RenameEntity(const string& oldTag, const string& newTag);

            cEntity* GetEntity(const string& tag);

            inline unordered_map<string, cEntity*>& GetEntities() { return m_Entities; }

            template<typename T, typename... Args>
            T* AddGlobal(Args&&... args);

            template<typename T>
            void RemoveGlobal();

            template<typename T>
            T* GetGlobal();

            inline unordered_map<uword, cGlobal*>& GetGlobals() { return m_Globals; }

            template<typename T>
            void ReserveComponents(usize capacity);

            template<typename T, typename... Args>
            T* AddComponent(cEntity* entity, Args&&... args);

            template<typename T>
            void RemoveComponent(cEntity* entity);

            void RemoveComponents(cEntity* entity);

            template<typename T>
            T* GetComponent(cEntity* entity);

            template<typename T>
            sComponentStorage& GetComponents();

            template<typename T>
            void EachComponent(const std::function<void(T*)>& iterateFunction);

            template<typename T>
            usize GetComponentsCount();

            void ToJson(json &root) override;

            void FromJson(json &root) override;

            physx::PxScene* PhysicsScene;

        protected:
            template<typename T>
            inline ComponentType GetComponentType() { return typeid(T).hash_code(); }

            template<typename T>
            inline GlobalType GetGlobalType() { return typeid(T).hash_code(); }

            template<typename T>
            void InvalidateComponentAddresses();

            void InvalidateComponentAddresses(ComponentType componentType, usize componentSize);

        private:
            unordered_map<string, cEntity*> m_Entities;
            unordered_map<ComponentType, sComponentStorage> m_ComponentStorages;
            unordered_map<cEntity*, unordered_map<ComponentType, sComponent*>> m_ComponentAddresses;
            unordered_map<GlobalType, cGlobal*> m_Globals;
        };

        template<typename T>
        void cScene::ReserveComponents(usize capacity)
        {
            ComponentType type = GetComponentType<T>();
            m_ComponentStorages[type] = {};
            m_ComponentStorages[type].Reserve<T>(capacity);
        }

        template<typename T, typename... Args>
        T* cScene::AddComponent(cEntity* entity, Args&&... args)
        {
            ComponentType type = GetComponentType<T>();
            T* component;

            // reallocate more memory if capacity exceeds
            auto& componentStorage = m_ComponentStorages[type];
            if (!componentStorage.HasCapacity())
            {
                componentStorage.Reserve<T>(componentStorage.GetSize<T>() * 2);
                // because component storage memory changed, we need to invalidate all addresses that use it.
                // it may be expensive operation when scene has a lot of entities,
                // but it depends only on component storage capacity and reserve weight variables.
                InvalidateComponentAddresses<T>();
            }
            component = componentStorage.Add<T>(std::forward<Args>(args)...);
            component->Entity = entity;
            m_ComponentAddresses[entity][type] = component;

            return component;
        }

        template<typename T>
        void cScene::RemoveComponent(cEntity* entity)
        {
            ComponentType type = GetComponentType<T>();
            sComponent*& component = m_ComponentAddresses[entity][type];

            if (component == nullptr)
            {
                LogWarning("Component for entity {0} does not exist", entity->GetTag());
                return;
            }

            // remove component from storage
            m_ComponentStorages[type].Erase<T>(entity);
            component = nullptr;
        }

        template<typename T>
        T* cScene::GetComponent(cEntity* entity)
        {
            return (T*) m_ComponentAddresses[entity][GetComponentType<T>()];
        }

        template<typename T>
        sComponentStorage& cScene::GetComponents()
        {
            return m_ComponentStorages[GetComponentType<T>()];
        }

        template<typename T>
        void cScene::EachComponent(const std::function<void(T*)>& iterateFunction)
        {
            m_ComponentStorages[GetComponentType<T>()].ForEach<T>(iterateFunction);
        }

        template<typename T>
        void cScene::InvalidateComponentAddresses()
        {
            InvalidateComponentAddresses(GetComponentType<T>(), sizeof(T));
        }

        template<typename T>
        usize cScene::GetComponentsCount()
        {
            return m_ComponentStorages[GetComponentType<T>()].GetSize();
        }

        template<typename T>
        void cScene::RemoveGlobal()
        {
            auto it = m_Globals.find(GetGlobalType<T>());
            if (it != m_Globals.end()) {
                m_Globals.erase(it);
                delete it.operator->();
            }
        }

        template<typename T>
        T* cScene::GetGlobal()
        {
            return static_cast<T*>(m_Globals[GetGlobalType<T>()]);
        }

        class ENGINE_API cEntity : public cObject, public cJson
        {

        public:
            sTransform Transform;
            cEntity* Parent = nullptr;
            vector<cEntity*> Children;

            JsonClass(
                cEntity,
                m_Tag,
                Transform
            )

            cEntity(const string& tag, cScene* scene);

            ~cEntity();

            inline void SetScene(cScene* scene) { m_Scene = scene; }

            [[nodiscard]] inline const cScene* GetScene() const { return m_Scene; }

            inline void Rename(const string& tag)
            {
                m_Scene->RenameEntity(m_Tag, tag);
            }

            template<typename T, typename... Args>
            T* Add(Args &&... args);

            template<typename T>
            T* Get();

            template<typename T>
            void Remove();

            void RemoveAll();

            template<typename T>
            bool Has();

            inline bool operator ==(const cEntity& other) const
            {
                return m_Tag == other.GetTag();
            }

        protected:
            cScene* m_Scene = nullptr;
        };

        template<typename T>
        T* cEntity::Get()
        {
            return m_Scene->GetComponent<T>(this);
        }

        template<typename T, typename... Args>
        T* cEntity::Add(Args &&... args)
        {
            return m_Scene->AddComponent<T>(this, std::forward<Args>(args)...);
        }

        template<typename T>
        void cEntity::Remove()
        {
            m_Scene->RemoveComponent<T>(this);
        }

        template<typename T>
        bool cEntity::Has()
        {
            return Get<T>() != nullptr;
        }

        struct ENGINE_API cGlobal : public cObject, public cJson
        {};

        template<typename T, typename... Args>
        T* cScene::AddGlobal(Args&&... args)
        {
            T* global = new T(std::forward<Args>(args)...);
            m_Globals.insert({ GetGlobalType<T>(), static_cast<cGlobal*>(global) });
            return global;
        }

        class ENGINE_API cSystem : public cObject {

        public:
            virtual void Update(cScene* scene, const cTime& dt) = 0;

        };

    }

}