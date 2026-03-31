#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Utils/ExtString.h>
#include <Tilc/Graphics/Mesh.h>
#include "Tilc/Utils/Matrix.h"
#include <vector>

namespace Tilc::Graphics
{
	class DECLSPEC TModel
	{
	public:
		TModel(const Tilc::TExtString& Path, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis = false)
		{
			LoadModel(Path, Pipeline, ReverseTexturesOnYAxis);
		}
        virtual ~TModel();

		void LoadModel(const Tilc::TExtString& Path, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis = false);
		void Render(Tilc::TMatrix4<float>& ProjectionMatrix, Tilc::TMatrix4<float>& ViewMatrix, Tilc::TMatrix4<float>& ModelMatrix);

		std::vector<TMesh*> m_Meshes;
		Tilc::TExtString m_Directory;
	};
}
