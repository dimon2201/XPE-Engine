#pragma once

namespace xpe
{
	namespace render
	{
		struct InputLayout;
		struct Pipeline;
		struct Texture;
		class Context;
		class Canvas;
		class BatchManager;

		class Skybox : public core::Object
		{
			public:
				Skybox(render::BatchManager* batchManager, Canvas* canvas, Texture* cubemap);
				~Skybox();

				void Draw();

			private:
				BatchManager* m_BatchManager;
				InputLayout* m_InputLayout;
				Pipeline* m_Pipeline;
				Texture* m_Cubemap;
		};
	}
}