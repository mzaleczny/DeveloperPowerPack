#include "Tilc/Gui/ImageListView.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Game.h"
#include "Tilc/Resources/TextureManager.h"
#include <chrono>

#include "Font.h"

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
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    SDL_RenderRect(Renderer, &m_Position);

    float x = m_Position.x;
    float y = m_Position.y;
    SDL_FRect SrcRect;
    for (int i = 0; i < m_Images.size(); ++i)
    {
        SDL_Texture* img = tmgr->GetResource(m_Images[i])->AsSDLTexture();
        SrcRect = {0, 0, static_cast<float>(m_ImageWidth), static_cast<float>(m_ImageHeight) };
        RenderTexture(img, &SrcRect, x, y, m_ImageWidth, m_ImageHeight);

        // Jeśli pozwalamy na usunięcie obrazka, to wyświetlamy białego X na czerwonym kwadracie w prawym górnym rogu obrazka
        if (m_AllowRemoveImage)
        {
            SrcRect = {SrcRect.x + SrcRect.w - 20, SrcRect.y, 20.0f, 20.0f};
            SDL_SetRenderDrawColorFloat(Renderer, 1.0f, 0.0f, 0.0f, 1.0f);
            SDL_RenderFillRect(Renderer, &SrcRect);
            t->DefaultFontBold->SetColor({0xff, 0xff, 0xff, 0xff});
            t->DefaultFontBold->DrawString(Renderer, "X", SrcRect.x + 5.0f, SrcRect.y + 5.0f);
        }

        x += m_ImageWidth + 5;
        if (i > 0 && i % m_ImagesPerRow == 0)
        {
            y += m_ImageHeight + 5;
            x = m_Position.x;
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
    UpdateHeight();
}

void Tilc::Gui::TImageListView::AddImage(const std::string_view ImageBlob, const std::string_view ImageName)
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    Tilc::TExtString Id = m_Name + "_Picture_" + std::to_string(milliseconds.count());
    bool AddResource = false;
    if (ImageName.empty())
    {
        AddResource = true;
    }
    else
    {
        Id = m_Name + "_Picture_" + ImageName.data();
        if (!Tilc::GameObject->GetContext()->m_TextureManager->GetResource(Id))
        {
            AddResource = true;
        }
    }
    if (AddResource)
    {
        Tilc::GameObject->GetContext()->m_TextureManager->AddResourceFromMemory(Id, reinterpret_cast<const unsigned char*>(ImageBlob.data()), ImageBlob.size(), "SDL_Texture", true);
    }
    m_Images.push_back(Id);
    UpdateHeight();
}

void Tilc::Gui::TImageListView::RemoveImage(const std::string_view ImageId)
{
    m_Images.erase(std::find(m_Images.begin(), m_Images.end(), ImageId));
    // decreases ref counter and if dropped to 0 then deletes resource totally
    Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(ImageId.data());
    UpdateHeight();
}

void Tilc::Gui::TImageListView::UpdateHeight()
{
    m_ImagesPerRow = m_Position.w / (m_ImageWidth+5);
    m_ImagesPerColumn = m_Images.size() / m_ImagesPerRow;
    if (m_Images.size() % m_ImagesPerRow > 0)
    {
        ++m_ImagesPerColumn;
    }
    if (m_ImagesPerColumn < 1) m_ImagesPerColumn = 1;
    m_Position.h = m_ImagesPerColumn * (m_ImageHeight + 5);
    Invalidate();
}
