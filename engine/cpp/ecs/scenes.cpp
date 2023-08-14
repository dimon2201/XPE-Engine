#include <ecs/scenes.hpp>

namespace xpe {

    namespace ecs {

        MainScene::MainScene()
        {
            Init();
        }

        MainScene::MainScene(const string &tag)
        {
            m_Tag = tag;
            Init();
        }

        void MainScene::Init()
        {
            Skybox = AddGlobal<ecs::Skybox>("Skybox");
            PerspectiveCamera = AddGlobal<control::PerspectiveCamera>("PerspectiveCamera");
            OrthoCamera = AddGlobal<control::OrthoCamera>("OrthoCamera");
        }

        void MainScene::ToJson(json &root)
        {

        }

        void MainScene::FromJson(json &root)
        {

        }

    }

}