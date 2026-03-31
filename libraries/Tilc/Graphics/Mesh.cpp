#include "Tilc/Game.h"
#include "Tilc/Graphics/Mesh.h"
#include "Tilc/Resources/TextureManager.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include <stack>
#include <sstream>
#include "Tilc/Utils/FileUtils.h"
#include "TextRenderer.h"

Tilc::Graphics::TMesh::TMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline)
	: m_Pipeline(Pipeline)
{
}

Tilc::Graphics::TMesh::~TMesh()
{
	DeleteVAOAndBuffers();
}

void Tilc::Graphics::TMesh::Render(GLenum DrawType, bool UseEBO, int InstancesCount)
{
    // Jesli trzeba to przed wyrenderowaniem wywolujemy funkcje UpdateVertexArraysAndVAO(), ktora odpowiada za utworzenie wektorow vertexow
    // i buforow opengl. Jest to wykorzystywane przez klasy pochodne od TMesh, ktore oferuja dynamiczne wyliczanie obiektow na teksturze jak np.
    // TTextRenderer.
    if (m_NeedRegenerateBuffers)
    {
        UpdateVertexArraysAndVAO(m_IndexCount8 > 0);
    }

	if (m_DoRender)
	{
		m_Pipeline->Bind();

		if (m_Wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(m_VAO);

		// Bind textures
		for (int i = 0; i < m_Textures.size(); ++i)
		{
			glBindTextureUnit(m_Textures[i].m_BindingUnit, m_Textures[i].m_Texture);
		}

        if (m_UseAlpha)
        {
            m_Pipeline->SetFloat(m_Pipeline->m_FragmentShader, "Alpha", m_Alpha);
        }
		if (UseEBO)
		{
			if (m_IndexCount > 0)
			{
				if (InstancesCount < 1)
				{
					glDrawElements(DrawType, m_IndexCount, GL_UNSIGNED_INT, NULL);
				}
				else
				{
					glDrawElementsInstanced(DrawType, m_IndexCount, GL_UNSIGNED_INT, NULL, InstancesCount);
				}
			}
			else if (m_IndexCount8 > 0)
			{
				if (InstancesCount < 1)
				{
					glDrawElements(DrawType, m_IndexCount8, GL_UNSIGNED_BYTE, NULL);
				}
				else
				{
					glDrawElementsInstanced(DrawType, m_IndexCount8, GL_UNSIGNED_BYTE, NULL, InstancesCount);
				}
			}
		}
		else
		{
			if (InstancesCount < 1)
			{
				glDrawArrays(DrawType, 0, m_VertexCount);
			}
			else
			{
				glDrawArraysInstanced(DrawType, 0, m_VertexCount, InstancesCount);
			}
		}
		glBindVertexArray(0);
	}
}

void Tilc::Graphics::TMesh::DeleteVAOAndBuffers()
{
	if (m_ExtraBuffers.size() > 0)
	{
		glDeleteBuffers(m_ExtraBuffers.size(), m_ExtraBuffers.data());
	}
	if (m_EBO)
	{
		glDeleteBuffers(1, &m_EBO);
	}
	if (m_VBO)
	{
		glDeleteBuffers(1, &m_VBO);
	}
	if (m_VAO)
	{
		glDeleteVertexArrays(1, &m_VAO);
	}
}

void Tilc::Graphics::TMesh::LoadUnitPlaneData16()
{
	constexpr const uint16_t Padding = 0.0f;
	// Because buffer data must be aligned to offset of size = 4, then after data for each element type we adding padding to ensure then next element data
	// starts at offest of 4 aligned
	// m_VertexData already ensures it but we convert it to 2 bytes half float, so padding added below ensures 4 byte alignment in half float buffer
	// DATA: Position, Color, TexCoords, Normal
	m_VertexData.assign({
		-0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
		0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
		0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
		-0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
	});
	m_VertexData16.resize(m_VertexData.size());
	for (size_t i = 0u; i < m_VertexData.size(); ++i)
	{
		m_VertexData16[i] = Tilc::f32tof16(m_VertexData[i]);
	}
	m_VertexData.clear();

	m_IndexData8.assign({
		/* // Clockwise direction
		0, 1, 2,
		0, 2, 3
		*/
		// Counter-Clockwise direction
		2, 1, 0,
		3, 2, 0
	});
}

void Tilc::Graphics::TMesh::LoadUnitPlaneData()
{
    constexpr const uint16_t Padding = 0.0f;
    // Because buffer data must be aligned to offset of size = 4, then after data for each element type we adding padding to ensure then next element data
    // starts at offest of 4 aligned
    // m_VertexData already ensures it but we convert it to 2 bytes half float, so padding added below ensures 4 byte alignment in half float buffer
    // DATA: Position, Color, TexCoords, Normal
    m_VertexData.assign({
        -0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
        0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
        0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
        -0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
    });

    m_IndexData.assign({
        /* // Clockwise direction
        0, 1, 2,
        0, 2, 3
        */
        // Counter-Clockwise direction
        2, 1, 0,
        3, 2, 0
    });
}

void Tilc::Graphics::TMesh::LoadUnitCubeData16()
{
	constexpr const uint16_t Padding = 0.0f;
	// Because buffer data must be aligned to offset of size = 4, then after data for each element type we adding padding to ensure then next element data
	// starts at offest of 4 aligned
	// m_VertexData already ensures it but we convert it to 2 bytes half float, so padding added below ensures 4 byte alignment in half float buffer
	// DATA: Position, Color, TexCoords, Normal
	m_VertexData.assign({
		// front
		-0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
		0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 1.f,		0.0f, 0.0f, 1.0f,		Padding,
		0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	1.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
		-0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 0.f,		Padding,	0.f, 0.f,		0.0f, 0.0f, 1.0f,		Padding,
		// right
		0.5f, 0.5f, 0.5f,		Padding,	0.f, 1.f, 0.f,		Padding,	0.f, 1.f,		1.0f, 0.0f, 0.0f,		Padding,
		0.5f, 0.5f, -0.5f,		Padding,	0.f, 1.f, 0.f,		Padding,	1.f, 1.f,		1.0f, 0.0f, 0.0f,		Padding,
		0.5f, -0.5f, -0.5f,		Padding,	0.f, 1.f, 0.f,		Padding,	1.f, 0.f,		1.0f, 0.0f, 0.0f,		Padding,
		0.5f, -0.5f, 0.5f,		Padding,	0.f, 1.f, 0.f,		Padding,	0.f, 0.f,		1.0f, 0.0f, 0.0f,		Padding,
		//top
		-0.5f, 0.5f, -0.5f,		Padding,	0.f, 0.f, 1.f,		Padding,	0.f, 1.f,		0.0f, 1.0f, 0.0f,		Padding,
		0.5f, 0.5f, -0.5f,		Padding,	0.f, 0.f, 1.f,		Padding,	1.f, 1.f,		0.0f, 1.0f, 0.0f,		Padding,
		0.5f, 0.5f, 0.5f,		Padding,	0.f, 0.f, 1.f,		Padding,	1.f, 0.f,		0.0f, 1.0f, 0.0f,		Padding,
		-0.5f, 0.5f, 0.5f,		Padding,	0.f, 0.f, 1.f,		Padding,	0.f, 0.f,		0.0f, 1.0f, 0.0f,		Padding,
		// back
		-0.5f, 0.5f, -0.5f,		Padding,	1.f, 1.f, 0.f,		Padding,	1.f, 1.f,		0.0f, 0.0f, -1.0f,		Padding,
		0.5f, 0.5f, -0.5f,		Padding,	1.f, 1.f, 0.f,		Padding,	0.f, 1.f,		0.0f, 0.0f, -1.0f,		Padding,
		0.5f, -0.5f, -0.5f,		Padding,	1.f, 1.f, 0.f,		Padding,	0.f, 0.f,		0.0f, 0.0f, -1.0f,		Padding,
		-0.5f, -0.5f, -0.5f,	Padding,	1.f, 1.f, 0.f,		Padding,	1.f, 0.f,		0.0f, 0.0f, -1.0f,		Padding,
		// left
		-0.5f, 0.5f, 0.5f,		Padding,	1.f, 0.f, 1.f,		Padding,	1.f, 1.f,		-1.0f, 0.0f, 0.0f,		Padding,
		-0.5f, 0.5f, -0.5f,		Padding,	1.f, 0.f, 1.f,		Padding,	0.f, 1.f,		-1.0f, 0.0f, 0.0f,		Padding,
		-0.5f, -0.5f, -0.5f,	Padding,	1.f, 0.f, 1.f,		Padding,	0.f, 0.f,		-1.0f, 0.0f, 0.0f,		Padding,
		-0.5f, -0.5f, 0.5f,		Padding,	1.f, 0.f, 1.f,		Padding,	1.f, 0.f,		-1.0f, 0.0f, 0.0f,		Padding,
		//bottom
		-0.5f, -0.5f, -0.5f,	Padding,	0.f, 1.f, 1.f,		Padding,	0.f, 1.f,		0.0f, -1.0f, 1.0f,		Padding,
		0.5f, -0.5f, -0.5f,		Padding,	0.f, 1.f, 1.f,		Padding,	1.f, 1.f,		0.0f, -1.0f, 1.0f,		Padding,
		0.5f, -0.5f, 0.5f,		Padding,	0.f, 1.f, 1.f,		Padding,	1.f, 0.f,		0.0f, -1.0f, 1.0f,		Padding,
		-0.5f, -0.5f, 0.5f,		Padding,	0.f, 1.f, 1.f,		Padding,	0.f, 0.f,		0.0f, -1.0f, 1.0f,		Padding
		});
	m_VertexData16.resize(m_VertexData.size());
	for (size_t i = 0u; i < m_VertexData.size(); ++i)
	{
		m_VertexData16[i] = Tilc::f32tof16(m_VertexData[i]);
	}
	m_VertexData.clear();

	m_IndexData8.assign({
		/* // Clockwise direction
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		14, 13, 12,
		15, 14, 12,
		18, 17, 16,
		19, 18, 16,
		22, 21, 20,
		23, 22, 20
		*/
		// Counter-Clockwise direction
		2, 1, 0,
		3, 2, 0,
		6, 5, 4,
		7, 6, 4,
		10, 9, 8,
		11, 10, 8,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	});
}

void Tilc::Graphics::TMesh::LoadUnitCubeData()
{
	// DATA: Position, Color, TexCoords, Normal
	m_VertexData.assign({
		// front
		-0.5f, 0.5f, 0.5f,		1.f, 0.f, 0.f,		0.f, 1.f,		0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f, 0.f,		1.f, 1.f,		0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f, 0.f,		1.f, 0.f,		0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		1.f, 0.f, 0.f,		0.f, 0.f,		0.0f, 0.0f, 1.0f,
		// right
		0.5f, 0.5f, 0.5f,		0.f, 1.f, 0.f,		0.f, 1.f,		1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		0.f, 1.f, 0.f,		1.f, 1.f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f, 0.f,		1.f, 0.f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		0.f, 1.f, 0.f,		0.f, 0.f,		1.0f, 0.0f, 0.0f,
		//top
		-0.5f, 0.5f, -0.5f,		0.f, 0.f, 1.f,		0.f, 1.f,		0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		0.f, 0.f, 1.f,		1.f, 1.f,		0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		0.f, 0.f, 1.f,		1.f, 0.f,		0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,		0.f, 0.f, 1.f,		0.f, 0.f,		0.0f, 1.0f, 0.0f,
		// back
		-0.5f, 0.5f, -0.5f,		1.f, 1.f, 0.f,		1.f, 1.f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f, 0.f,		0.f, 1.f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,		1.f, 1.f, 0.f,		0.f, 0.f,		0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,	1.f, 1.f, 0.f,		1.f, 0.f,		0.0f, 0.0f, -1.0f,
		// left
		-0.5f, 0.5f, 0.5f,		1.f, 0.f, 1.f,		1.f, 1.f,		-1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		1.f, 0.f, 1.f,		0.f, 1.f,		-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	1.f, 0.f, 1.f,		0.f, 0.f,		-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		1.f, 0.f, 1.f,		1.f, 0.f,		-1.0f, 0.0f, 0.0f,
		//bottom
		-0.5f, -0.5f, -0.5f,	0.f, 1.f, 1.f,		0.f, 1.f,		0.0f, -1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f, 1.f,		1.f, 1.f,		0.0f, -1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		0.f, 1.f, 1.f,		1.f, 0.f,		0.0f, -1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		0.f, 1.f, 1.f,		0.f, 0.f,		0.0f, -1.0f, 1.0f
		});

	m_IndexData.assign({
		/* // Clockwise direction
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		14, 13, 12,
		15, 14, 12,
		18, 17, 16,
		19, 18, 16,
		22, 21, 20,
		23, 22, 20
		*/
		// Counter-Clockwise direction
		2, 1, 0,
		3, 2, 0,
		6, 5, 4,
		7, 6, 4,
		10, 9, 8,
		11, 10, 8,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	});
}

GLuint Tilc::Graphics::TMesh::AddExtraBuffer()
{
	GLuint buf;
	glCreateBuffers(1, &buf);
	m_ExtraBuffers.push_back(buf);
	return buf;
}

void Tilc::Graphics::TMesh::PushTexturedShape(const std::initializer_list<float>& TexturedShapeData, const std::initializer_list<int>& TexturedShapeIndexData, int StartVertexIndex)
{
    for (float Elem : TexturedShapeData)
    {
        m_VertexData.push_back(Elem);
    }
    for (int Index : TexturedShapeIndexData)
    {
        m_IndexData.push_back(StartVertexIndex + Index);
    }
}

void Tilc::Graphics::TMesh::PushTexturedShape16(const std::initializer_list<float>& TexturedShapeData, const std::initializer_list<int8_t>& TexturedShapeIndexData, int StartVertexIndex)
{
    for (float Elem : TexturedShapeData)
    {
        //std::cout << "Push " << Elem << std::endl;
        m_VertexData16.push_back(Tilc::f32tof16(Elem));
    }
    for (int8_t Index : TexturedShapeIndexData)
    {
        m_IndexData8.push_back(static_cast<int8_t>(StartVertexIndex) + Index);
    }
}

void Tilc::Graphics::TMesh::PushTexturedShape(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen, int ImageIndexWithinTextureMap, const std::initializer_list<int>& TexturedShapeIndexData)
{
    int StartVertexIndex = static_cast<int>(m_VertexData.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    PushTexturedShape({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
    }, TexturedShapeIndexData, StartVertexIndex);
}

void Tilc::Graphics::TMesh::PushTexturedShape(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen, int ImageIndexWithinTextureMap)
{
    int StartVertexIndex = static_cast<int>(m_VertexData.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    PushTexturedShape({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
        }, {
            // Counter-Clockwise direction
            0, 1, 2,
            2, 3, 0,
        }, StartVertexIndex);
}

void Tilc::Graphics::TMesh::PushTexturedShape(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, int ImageIndexWithinTextureMap)
{
    int StartVertexIndex = static_cast<int>(m_VertexData.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    Tilc::TVector2D<int> WindowSize = Tilc::GameObject->GetContext()->m_Window->GetWindowSize();
    float Aspect = static_cast<float>(WindowSize.x) / static_cast<float>(WindowSize.y);
    float HeightOnScreen = static_cast<float>(r.h) / static_cast<float>(r.w) * WidthOnScreen * Aspect;
    PushTexturedShape({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
        }, {
            // Counter-Clockwise direction
            0, 1, 2,
            2, 3, 0,
        }, StartVertexIndex);
}

void Tilc::Graphics::TMesh::PushTexturedShape16(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen, int ImageIndexWithinTextureMap, const std::initializer_list<int8_t>& TexturedShapeIndexData)
{
    int StartVertexIndex = static_cast<int>(m_VertexData16.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    PushTexturedShape16({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
        }, TexturedShapeIndexData, StartVertexIndex);
}

void Tilc::Graphics::TMesh::PushTexturedShape16(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, float HeightOnScreen, int ImageIndexWithinTextureMap)
{
    int StartVertexIndex = static_cast<int>(m_VertexData16.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    PushTexturedShape16({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
        }, {
            // Counter-Clockwise direction
            0, 1, 2,
            2, 3, 0,
        }, StartVertexIndex);
}

void Tilc::Graphics::TMesh::PushTexturedShape16(float ShapeXonScreen, float ShapeYonScreen, float WidthOnScreen, int ImageIndexWithinTextureMap)
{
    int StartVertexIndex = static_cast<int>(m_VertexData16.size()) / 4;
    SDL_Rect r = m_ImagesWithinTextureMapDescriptions[ImageIndexWithinTextureMap];
    Tilc::TVector2D<int> WindowSize = Tilc::GameObject->GetContext()->m_Window->GetWindowSize();
    float Aspect = static_cast<float>(WindowSize.x) / static_cast<float>(WindowSize.y);
    float HeightOnScreen = static_cast<float>(r.h) / static_cast<float>(r.w) * WidthOnScreen * Aspect;
    PushTexturedShape16({
        ShapeXonScreen, ShapeYonScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight,
        ShapeXonScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen - HeightOnScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y + r.h) / TextureMapHeight,
        ShapeXonScreen + WidthOnScreen, ShapeYonScreen, static_cast<float>(r.x + r.w) / TextureMapWidth, static_cast<float>(r.y) / TextureMapHeight
        }, {
            // Counter-Clockwise direction
            0, 1, 2,
            2, 3, 0,
        }, StartVertexIndex);
}

void Tilc::Graphics::TMesh::ReadTextureMapInfoFromFile(const Tilc::TExtString& FilePath)
{
    std::vector<Tilc::TExtString> DataFileLines;
    Tilc::TFile DataFile(FilePath.c_str());
    DataFile.ReadLines(DataFileLines);
    m_ImagesWithinTextureMapDescriptions.clear();

    std::stringstream Keystream(DataFileLines[0]);
    Keystream >> TextureMapWidth >> TextureMapHeight;

    for (int i = 1; i < DataFileLines.size(); ++i)
    {
        std::stringstream Keystream(DataFileLines[i].substr(DataFileLines[i].find(':') + 1));
        SDL_Rect Rect;
        Keystream >> Rect.x;
        if (Keystream.fail()) break;
        Keystream >> Rect.y;
        if (Keystream.fail()) break;
        Keystream >> Rect.w;
        if (Keystream.fail()) break;
        Keystream >> Rect.h;
        if (Keystream.fail()) break;
        // Adjustment below fixes problem with border around images that should not be present.
        // If we skip lines below then next image may draw last line of previous image.
        Rect.x += 1;
        Rect.y += 1;
        Rect.w -= 2;
        Rect.h -= 2;
        m_ImagesWithinTextureMapDescriptions.push_back(Rect);
    }
}

void Tilc::Graphics::TMesh::PrintVertexBuffer()
{
    for (int i = 0; i < m_VertexData.size(); ++i)
    {
        std::cout << m_VertexData[i] << "  ";
        if (i % 4 == 3) std::cout << std::endl;
    }
}

void Tilc::Graphics::TMesh::PrintVertexBuffer16()
{
    for (int i = 0; i < m_VertexData16.size(); ++i)
    {
        std::cout << Tilc::f16tof32(m_VertexData16[i]) << "  ";
        if (i % 4 == 3) std::cout << std::endl;
    }
}

void Tilc::Graphics::TMesh::SetStandard2DRenderConfig(const Tilc::TExtString& TextureMapInfoFilePath, const Tilc::TExtString& ResTextureId)
{
    if (TextureMapInfoFilePath != "")
    {
        ReadTextureMapInfoFromFile(TextureMapInfoFilePath);
    }
    m_VertexComponents.clear();
    m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 2, glGetAttribLocation(m_Pipeline->m_VertexShader, "VertexPos"), 0 });
    m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_TexCoord, 2, glGetAttribLocation(m_Pipeline->m_VertexShader, "VertexTexCoord"), 2 });
    SetStrideElemsCount(4);

    // jesli byly ustawione jakies tekstury, to je zwalniamy, zeby nie zwiekszac referencji
    if (m_Textures.size() > 0)
    {
        m_Textures.clear();
    }

    if (ResTextureId != "")
    {
        if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource(ResTextureId))
        {
            m_Textures.push_back({ 0, Tilc::GameObject->GetContext()->m_TextureManager->GetResource(ResTextureId)->AsGLTexture() });
        }
    }
    m_UseAlpha = true;
    m_Alpha = 1.0f;
}

int Tilc::Graphics::TMesh::GetIndexOfVertexComponent(EVertexComponent SearchType)
{
	for (int i = 0; i < m_VertexComponents.size(); ++i)
	{
		if (m_VertexComponents[i].m_Type == SearchType)
		{
			return i;
		}
	}
	return -1;
}

int Tilc::Graphics::TMesh::GetLocationOfVertexComponent(EVertexComponent SearchType)
{
	for (int i = 0; i < m_VertexComponents.size(); ++i)
	{
		if (m_VertexComponents[i].m_Type == SearchType)
		{
			return m_VertexComponents[i].m_Location;
		}
	}
	return -1;
}

int Tilc::Graphics::TMesh::GetStrideElemsCount()
{
	/*
	int Count = 0;
	for (int i = 0; i < m_VertexComponents.size(); ++i)
	{
		Count += m_VertexComponents[i].m_ElemsCount;
	}
	return Count;
	*/
	return m_StrideElemsCount;
}




void LoadMaterialTextures(Tilc::Graphics::TMesh* Mesh, aiMaterial* Material, aiTextureType TextureType, const Tilc::TExtString& TextureTypeName)
{
    if (Material->GetTextureCount(TextureType) > 0u)
    {
        aiString TextureName;
        Material->GetTexture(TextureType, 0u, &TextureName);

        if (!Tilc::GameObject->GetContext()->m_TextureManager->GetResource(TextureName.C_Str()))
        {
            Tilc::GameObject->GetContext()->m_TextureManager->RequireResource(TextureName.C_Str());
        }

        if (TextureType == aiTextureType_DIFFUSE)
        {
            Mesh->m_Textures.push_back({ Mesh->DiffuseTexturesCount, Tilc::GameObject->GetContext()->m_TextureManager->GetResource(TextureName.C_Str())->AsGLTexture(), TextureTypeName });
            ++Mesh->DiffuseTexturesCount;
        }
        else if (TextureType == aiTextureType_SPECULAR)
        {
            Mesh->m_Textures.push_back({ 2 + Mesh->SpecularTexturesCount, Tilc::GameObject->GetContext()->m_TextureManager->GetResource(TextureName.C_Str())->AsGLTexture(), TextureTypeName });
            ++Mesh->SpecularTexturesCount;
        }
    }
}

Tilc::Graphics::TMesh* ReadMesh(aiMesh* AssimpMesh, const aiScene* Scene, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis = false)
{
	uint32_t i;

	Tilc::Graphics::TMesh* Mesh = new Tilc::Graphics::TMesh(Pipeline);
    if (!Mesh) return nullptr;
	Mesh->m_VertexData.reserve(AssimpMesh->mNumVertices * (3 + 2 + 3)); // Vertices Pos Num + Texture Coords Num + Normal Coords Num
	for (i = 0u; i < AssimpMesh->mNumVertices; ++i)
	{
		Mesh->m_VertexData.push_back(AssimpMesh->mVertices[i].x);
		Mesh->m_VertexData.push_back(AssimpMesh->mVertices[i].y);
		Mesh->m_VertexData.push_back(AssimpMesh->mVertices[i].z);

        // Assimp allows a model to have up to 8 different texture coordinates per vertex, but we care only for the first one
		if (AssimpMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			Mesh->m_VertexData.push_back(AssimpMesh->mTextureCoords[0][i].x);
            if (ReverseTexturesOnYAxis)
            {
                Mesh->m_VertexData.push_back(1.0f - AssimpMesh->mTextureCoords[0][i].y);
            }
            else
            {
                Mesh->m_VertexData.push_back(AssimpMesh->mTextureCoords[0][i].y);
            }
		}
		else
		{
			Mesh->m_VertexData.push_back(0.0f);
			Mesh->m_VertexData.push_back(0.0f);
		}

        if (AssimpMesh->HasNormals())
        {
            Mesh->m_VertexData.push_back(AssimpMesh->mNormals[i].x);
            Mesh->m_VertexData.push_back(AssimpMesh->mNormals[i].y);
            Mesh->m_VertexData.push_back(AssimpMesh->mNormals[i].z);
        }
        else
        {
            Mesh->m_VertexData.push_back(0.0f);
            Mesh->m_VertexData.push_back(0.0f);
            Mesh->m_VertexData.push_back(0.0f);
        }
	}

	int IndexCount = 0;
	for (i = 0u; i < AssimpMesh->mNumFaces; ++i)
	{
		aiFace Face = AssimpMesh->mFaces[i];
		IndexCount += Face.mNumIndices;
	}
	Mesh->m_IndexData.reserve(IndexCount);
	for (i = 0u; i < AssimpMesh->mNumFaces; ++i)
	{
		aiFace Face = AssimpMesh->mFaces[i];
		for (uint32_t j = 0u; j < Face.mNumIndices; ++j)
		{
			Mesh->m_IndexData.push_back(Face.mIndices[j]);
		}
	}

	Mesh->m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0});
	Mesh->m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_TexCoord, 2, glGetAttribLocation(Pipeline->m_VertexShader, "VertexTexCoord"), 3});
	Mesh->m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Normal, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexNormal"), 5});
	Mesh->SetStrideElemsCount(8);

    if (AssimpMesh->mMaterialIndex >= 0)
    {
        aiMaterial* Material = Scene->mMaterials[AssimpMesh->mMaterialIndex];
        if (Material)
        {
            LoadMaterialTextures(Mesh, Material, aiTextureType_DIFFUSE, "TextureDiffuse");
            LoadMaterialTextures(Mesh, Material, aiTextureType_SPECULAR, "TextureSpecular");
        }
    }

	Mesh->CreateVAOAndBuffers<float, GLuint>(AssimpMesh->mNumVertices);

	return Mesh;
}

int ReadNode(std::vector<Tilc::Graphics::TMesh*>& Meshes, aiNode* Node, const aiScene* Scene, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis = false)
{
	for (uint32_t i = 0u; i < Node->mNumMeshes; ++i)
	{
		aiMesh* AssimpMesh = Scene->mMeshes[Node->mMeshes[i]];
		Tilc::Graphics::TMesh* Mesh = ReadMesh(AssimpMesh, Scene, Pipeline, ReverseTexturesOnYAxis);
		if (Mesh)
		{
			Meshes.push_back(Mesh);
		}
	}
	return 0;
}

int Tilc::Graphics::LoadMeshesFromFile(const std::string& ModelFileName, std::vector<TMesh*>& Meshes, Tilc::Graphics::OpenGL::TPipeline* Pipeline, bool ReverseTexturesOnYAxis)
{
	Assimp::Importer importer;

	if (Meshes.size() > 0)
	{
		for (auto it = Meshes.begin(); it != Meshes.end(); ++it)
		{
			delete *it;
		}
		Meshes.clear();
	}

	const aiScene* Scene = importer.ReadFile(ModelFileName, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
	if (!Scene || !Scene->mRootNode || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cerr << "Error while loading meshes from file " << ModelFileName << ": " << importer.GetErrorString() << std::endl;
		return -1;
	}

	std::stack<aiNode*> Nodes;
	Nodes.push(Scene->mRootNode);
	while (!Nodes.empty())
	{
		aiNode* Node = Nodes.top();
		Nodes.pop();
		for (uint32_t i = 0u; i < Node->mNumChildren; ++i)
		{
			Nodes.push(Node->mChildren[i]);
		}

		ReadNode(Meshes, Node, Scene, Pipeline, ReverseTexturesOnYAxis);
	}

	return 0;
}
