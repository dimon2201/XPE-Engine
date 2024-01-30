#pragma once

#include <canvas.hpp>
#include <shaders.hpp>
#include <render_debugger.hpp>
#include <render_manager.hpp>
#include <camera_manager.hpp>
#include <anim_manager.hpp>
#include <audio_manager.hpp>
#include <physics_manager.hpp>

#ifdef K_EDITOR
#include <editor_manager.hpp>
#endif

namespace xpe
{
    namespace core
    {
        class ENGINE_API cApp : public cObject {

        public:
            cApp(const string& title) { m_Config.Title = title; }
            ~cApp() {}

            void Run();

            void LoadGameLib(const string& dllpath);

        protected:
            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}
            virtual void Render();

            virtual void InitShaders();

            void LockFPSFromConfig();

            void StartWatchFiles();
            void StopWatchFiles();

            void GameLibModified(const string& watchpath, const string& filepath);
            void ShaderModified(const string& watchpath, const string& filepath);
            void ModelModified(const string& watchpath, const string& filepath);
            void FontModified(const string& watchpath, const string& filepath);
            void MaterialModified(const string& watchpath, const string& filepath);
            void SkyboxModified(const string& watchpath, const string& filepath);

            float m_DeltaTime = 6; // milliseconds

            sAppConfig m_Config;

            std::atomic_bool m_Opened = true;

            cMultiFileWatcher* m_FileWatcher = nullptr;
            cDirectoryWatcher* m_DirWatcher = nullptr;

            cScene* m_Scene = nullptr;

            cCanvas* m_Canvas = nullptr;
        };
    }
}

xpe::core::cApp* CreateApplication();