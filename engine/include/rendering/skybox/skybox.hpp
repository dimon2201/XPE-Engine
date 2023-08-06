#pragma once

#include <core/types.hpp>
#include <stl/string.h>

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
	}

	namespace skybox
	{
		class Skybox : public core::Object
		{
			public:
				Skybox(render::Context* context, render::BatchManager* batchManager, render::Canvas* canvas, render::Texture* cubemap);
				~Skybox();

				void Draw();

			private:
				render::Context* m_Context;
				render::BatchManager* m_BatchManager;
				render::InputLayout* m_InputLayout;
				render::Pipeline* m_Pipeline;
				render::Texture* m_Cubemap;
		};
	}
}