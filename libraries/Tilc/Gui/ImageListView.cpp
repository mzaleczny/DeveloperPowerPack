#include "Tilc/Gui/ImageListView.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Game.h"
#include "Tilc/Resources/TextureManager.h"
#include <chrono>

Tilc::Gui::TImageListView::TImageListView(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, int ImageWidth, int ImageHeight)
	: Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_ImageListView), m_ImageWidth(ImageWidth), m_ImageHeight(ImageHeight)
{
}

Tilc::Gui::TImageListView::~TImageListView()
{
    RemoveAllImages();
}

void Tilc::Gui::TImageListView::Draw()
{
    if (!m_Visible) return;
    SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
    Tilc::Resources::TTextureManager* tmgr = Tilc::GameObject->GetContext()->m_TextureManager;
    float y = m_Position.y;
    for (int i = 0; i < m_Images.size(); ++i)
    {
        RenderTexture(tmgr->GetResource(m_Images[i])->AsSDLTexture(), nullptr,
            m_Position.x + i * (m_ImageWidth+5), y, m_ImageWidth, m_ImageHeight);
        if (i > 0 && i % m_ImagesPerRow == 0)
        {
            y += m_ImageHeight + 5;
        }
    }
}

void Tilc::Gui::TImageListView::RemoveAllImages()
{
    while (!m_Images.empty())
    {
        auto it = m_Images.begin();
        // decreases ref counter and if dropped to 0 then deletes resource totally
        Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(*it);
        m_Images.erase(it, it + 1);
    }
}

void Tilc::Gui::TImageListView::AddImage(const std::string_view ImageBlob)
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    Tilc::TExtString Id = m_Name + "_Picture_" + std::to_string(milliseconds.count());
    if (Tilc::GameObject->GetContext()->m_TextureManager->AddResourceFromMemory(Id, reinterpret_cast<const unsigned char*>(ImageBlob.data()), ImageBlob.size(), "SDL_Texture", true))
    {
        m_Images.push_back(Id);
        UpdateHeight();
    }
}

void Tilc::Gui::TImageListView::RemoveImage(const std::string_view ImageId)
{
    m_Images.erase(std::find(m_Images.begin(), m_Images.end(), ImageId));
    // decreases ref counter and if dropped to 0 then deletes resource totally
    Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(ImageId.data());
}

void Tilc::Gui::TImageListView::UpdateHeight()
{
    m_ImagesPerRow = m_Position.w / (m_ImageWidth+5);
    m_ImagesPerColumn = m_Images.size() / m_ImagesPerRow;
    if (m_Images.size() % m_ImagesPerRow > 0)
    {
        ++m_ImagesPerColumn;
    }
    m_Position.h = m_ImagesPerColumn * (m_ImageHeight + 5);
}
