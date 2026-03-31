#include "Tilc/Graphics/Model.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Game.h"

Tilc::Graphics::TModel::~TModel()
{
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        if (m_Meshes[i])
        {
            delete m_Meshes[i];
        }
    }
    m_Meshes.clear();
}

void Tilc::Graphics::TModel::LoadModel(const Tilc::TExtString& Path, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis)
{
	m_Directory = Path.substr(0, Path.find_last_of('/'));
	LoadMeshesFromFile(Path, m_Meshes, Pipeline, ReverseTexturesOnYAxis);
}

void Tilc::Graphics::TModel::Render(Tilc::TMatrix4<float>& ProjectionMatrix, Tilc::TMatrix4<float>& ViewMatrix, Tilc::TMatrix4<float>& ModelMatrix)
{
    Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
        m_Meshes[i]->m_Pipeline->Bind();

        m_Meshes[i]->m_Pipeline->SetVec3(m_Meshes[i]->m_Pipeline->m_FragmentShader, "CameraPosition", Tilc::GameObject->GetCurrentCamera()->m_Position);
        Tilc::GameObject->DoLighting(m_Meshes[i]);

		// material properties
		unsigned int DiffuseNumber = 1;
		unsigned int SpecularNumber = 1;
		std::string Name, Number;
		int TextureTypeOffset;
		for (unsigned int i = 0; i < m_Meshes[i]->m_Textures.size(); ++i)
		{
			Name = m_Meshes[i]->m_Textures[i].m_Type;
			if (Name == "TextureDiffuse")
			{
				Number = std::to_string(DiffuseNumber++);
				TextureTypeOffset = 0;
			}
			else if (Name == "TextureSpecular")
			{
				Number = std::to_string(SpecularNumber++);
				TextureTypeOffset = 4;
			}
			m_Meshes[i]->m_Pipeline->SetInt(m_Meshes[i]->m_Pipeline->m_FragmentShader, "Material." + Name + Number, i + TextureTypeOffset);
		}
		m_Meshes[i]->m_Pipeline->SetFloat(m_Meshes[i]->m_Pipeline->m_FragmentShader, "Material.Shininess", 32.0f);

        m_Meshes[i]->m_Pipeline->SetMat4(m_Meshes[i]->m_Pipeline->m_VertexShader, "Model", ModelMatrix.Elems);
        m_Meshes[i]->m_Pipeline->SetMat4(m_Meshes[i]->m_Pipeline->m_VertexShader, "MVP", MVPMatrix.Elems);

		m_Meshes[i]->Render(GL_TRIANGLES, true);
	}
}
