#pragma once

#define GLAD_GL_IMPLEMENTATION
#ifdef APIENTRY
# undef APIENTRY
#endif
#include <glad/glad.h>
#ifdef APIENTRY
# undef APIENTRY
#endif
#include "Tilc/DllGlobals.h"
#include <Tilc/Graphics/OpenGL/Shader.h>
#include <Tilc/Utils/Types.h>
#include <Tilc/Utils/ExtString.h>
#include <vector>
#include <initializer_list>
#include <SDL3/SDL.h>

namespace Tilc::Graphics
{
	enum class EVertexComponent
	{
		EVC_None,
		EVC_Position,
		EVC_Color,
		EVC_TexCoord,
		EVC_Normal
	};

	struct DECLSPEC FVertexComponent
	{
		EVertexComponent m_Type = EVertexComponent::EVC_None;
		int m_ElemsCount = 0;
		int m_Location = -1;
		int m_StartIndexInStride = 0;
	};

	struct DECLSPEC FTextureBinding
	{
		int m_BindingUnit;
		GLuint m_Texture;
		Tilc::TExtString m_Type;
	};


	class TMesh;

	// Load mesh from model file using assimp library
	int DECLSPEC LoadMeshesFromFile(const std::string& ModelFileName, std::vector<TMesh*>& Meshes, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis = false);



	class DECLSPEC TMesh
	{
	public:
		bool m_DoRender = true;
		bool m_Wireframe = false;

		// Pointer set from outside and managed (deleted) by external code
		Tilc::Graphics::OpenGL::TPipeline* m_Pipeline = nullptr;

		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		GLuint m_EBO = 0;
		int m_VertexCount = 0;
		int m_StrideElemsCount = 0;
		std::vector<GLfloat> m_VertexData;
		std::vector<Tilc::float16> m_VertexData16;
		std::vector<GLuint> m_IndexData;
		std::vector<GLubyte> m_IndexData8;
		std::vector<FVertexComponent> m_VertexComponents;
		std::vector<FTextureBinding> m_Textures;
		std::vector<GLuint> m_ExtraBuffers;
        // if m_AlphaValue foar Alpha uniform should be set within fragment shader
        bool m_UseAlpha = false;
        float m_Alpha = 1.0f;
        bool m_NeedRegenerateBuffers = false;
        int DiffuseTexturesCount = 0;
        int SpecularTexturesCount = 0;

		TMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline);
		virtual ~TMesh();
		// InstancesCount = 0 means that we use normal render call
		// InstancesCount > 0 menas that we use rednder instanced call
		virtual void Render(GLenum DrawType, bool UseEBO = true, int InstancesCount = 0);
        virtual void UpdateVertexArraysAndVAO(bool HalfFloat) {};
        template <typename T, typename U>
        void CreateVAOAndBuffers(int VertexCount);
		virtual void DeleteVAOAndBuffers();
		void SetWireframe(bool Wireframe)
		{
			m_Wireframe = Wireframe;
		}
		void SetStrideElemsCount(int StrideElemsCount)
		{
			m_StrideElemsCount = StrideElemsCount;
		}
		// Loads data structured in CW order
		void LoadUnitPlaneData16();
		void LoadUnitPlaneData();
		void LoadUnitCubeData16();
		void LoadUnitCubeData();
		GLuint AddExtraBuffer();
		void FillBufferWithFloatVector(GLuint Buffer, const std::vector<float>& Data)
		{
			glNamedBufferStorage(Buffer, Data.size() * sizeof(float), Data.data(), 0);
		}
        void PushTexturedShape(const std::initializer_list<float>& TexturedShapeData, const std::initializer_list<int>& TexturedShapeIndexData, int StartVertexIndex);
        void PushTexturedShape16(const std::initializer_list<float>& TexturedShapeData, const std::initializer_list<int8_t>& TexturedShapeIndexData, int StartVertexIndex);
        void PushTexturedShape(
            // 4 variables below set sprite position and size on screen
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap,
            const std::initializer_list<int>& TexturedShapeIndexData);
        void PushTexturedShape(
            // 4 variables below set sprite position and size on screen
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap);
        void PushTexturedShape(
            // 3 variables below set sprite position and size on screen, height is calculated proportionally based on width and original image within texture
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap);
        void PushTexturedShape16(
            // 4 variables below set sprite position and size on screen
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap,
            const std::initializer_list<int8_t>& TexturedShapeIndexData);
        void PushTexturedShape16(
            // 4 variables below set sprite position and size on screen
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap);
        void PushTexturedShape16(
            // 3 variables below set sprite position and size on screen, height is calculated proportionally based on width and original image within texture
            float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen,
            // variable below states which image we want to extract from text file data
            int ImageIndexWithinTextureMap);
        void ReadTextureMapInfoFromFile(const Tilc::TExtString& FilePath);
        void PrintVertexBuffer();
        void PrintVertexBuffer16();
        void SetStandard2DRenderConfig(const Tilc::TExtString& TextureMapInfoFilePath, const Tilc::TExtString& ResTextureId);
        void ClearData()
        {
            m_VertexData.clear();
            m_IndexData.clear();
            m_VertexData16.clear();
            m_IndexData8.clear();
        }
	protected:
		int GetIndexOfVertexComponent(EVertexComponent SearchType = EVertexComponent::EVC_Position);
		int GetLocationOfVertexComponent(EVertexComponent SearchType = EVertexComponent::EVC_Position);
		int GetStrideElemsCount();

	private:
		size_t m_IndexCount;
		size_t m_IndexCount8;
        std::vector<SDL_Rect> m_ImagesWithinTextureMapDescriptions;
        int TextureMapWidth;
        int TextureMapHeight;
	};

	template <typename T, typename U>
	void TMesh::CreateVAOAndBuffers(int VertexCount)
	{
		m_IndexCount = 0;
		m_IndexCount8 = 0;
		int ItemsComponentsCount = m_VertexComponents.size();
		int StrideElemsCount = GetStrideElemsCount();
		m_VertexCount = VertexCount;

		// Create VAO
		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create VBO
		glCreateBuffers(1, &m_VBO);
		if (sizeof(T) == 2)
		{
			glNamedBufferStorage(m_VBO, VertexCount * StrideElemsCount * sizeof(Tilc::float16), m_VertexData16.data(), 0);
		}
		else
		{
			glNamedBufferStorage(m_VBO, VertexCount * StrideElemsCount * sizeof(GLfloat), m_VertexData.data(), 0);
		}

		// Create EBO
		if (m_IndexData8.size() > 0 || m_IndexData.size() > 0)
		{
			glCreateBuffers(1, &m_EBO);
			if (sizeof(U) == 1)
			{
				glNamedBufferStorage(m_EBO, m_IndexData8.size() * sizeof(GLubyte), m_IndexData8.data(), 0);
				m_IndexCount8 = m_IndexData8.size();
			}
			else
			{
				glNamedBufferStorage(m_EBO, m_IndexData.size() * sizeof(GLuint), m_IndexData.data(), 0);
				m_IndexCount = m_IndexData.size();
			}

			// Attach Index Buffer (Element Buffer)
			glVertexArrayElementBuffer(m_VAO, m_EBO);
		}

		// Enable vertex attributes
		for (int i = 0; i < m_VertexComponents.size(); ++i)
		{
			if (m_VertexComponents[i].m_Location != -1)
			{
				glEnableVertexArrayAttrib(m_VAO, m_VertexComponents[i].m_Location);
				if (sizeof(T) == 2)
				{
					glVertexArrayAttribFormat(m_VAO, m_VertexComponents[i].m_Location, m_VertexComponents[i].m_ElemsCount, GL_HALF_FLOAT, GL_FALSE, 0);
					glVertexArrayVertexBuffer(m_VAO, m_VertexComponents[i].m_Location, m_VBO, m_VertexComponents[i].m_StartIndexInStride * sizeof(Tilc::float16), StrideElemsCount * sizeof(Tilc::float16));
				}
				else
				{
					glVertexArrayAttribFormat(m_VAO, m_VertexComponents[i].m_Location, m_VertexComponents[i].m_ElemsCount, GL_FLOAT, GL_FALSE, 0);
					glVertexArrayVertexBuffer(m_VAO, m_VertexComponents[i].m_Location, m_VBO, m_VertexComponents[i].m_StartIndexInStride * sizeof(GLfloat), StrideElemsCount * sizeof(GLfloat));
				}
				glVertexArrayAttribBinding(m_VAO, m_VertexComponents[i].m_Location, m_VertexComponents[i].m_Location);
			}
		}

		// Bind textures
		for (int i = 0; i < m_Textures.size(); ++i)
		{
			glBindTextureUnit(m_Textures[i].m_BindingUnit, m_Textures[i].m_Texture);
		}
		glBindVertexArray(0);

		m_VertexData16.clear();
		m_VertexData.clear();
		m_IndexData8.clear();
		m_IndexData.clear();
	}

}
