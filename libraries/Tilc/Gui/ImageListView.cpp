#include "Tilc/Gui/ImageListView.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Globals.h"
#include "Tilc/Game.h"
#include "Tilc/Resources/TextureManager.h"
#include <chrono>

#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Picture.h"

Tilc::Gui::TImageListView::TImageListView(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, int ImageWidth, int ImageHeight)
	: Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_ImageListView), m_ImageWidth(ImageWidth), m_ImageHeight(ImageHeight)
{
}

Tilc::Gui::TImageListView::~TImageListView()
{
    if (m_PreviewPicture)
    {
        delete m_PreviewPicture;
    }
    RemoveAllImages();
}

void Tilc::Gui::TImageListView::Draw()
{
    if (!m_Visible) return;
    SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
    Tilc::Resources::TTextureManager* tmgr = Tilc::GameObject->GetContext()->m_TextureManager;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_SetRenderDrawColor(Renderer, 0, 0, 125, 255);
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
            SrcRect = {x + SrcRect.w - 20, y, 20.0f, 20.0f};
            SDL_SetRenderDrawColorFloat(Renderer, 1.0f, 0.0f, 0.0f, 1.0f);
            SDL_RenderFillRect(Renderer, &SrcRect);
            t->DefaultFontBold->SetColor({0xff, 0xff, 0xff, 0xff});
            t->DefaultFontBold->DrawString(Renderer, "X", SrcRect.x + 5.0f, SrcRect.y + 2.0f);
        }

        x += m_ImageWidth + 5;
        if (i > 0 && i % m_ImagesPerRow == 0)
        {
            y += m_ImageHeight + 5;
            x = m_Position.x;
        }
    }
}

bool Tilc::Gui::TImageListView::Update(float DeltaTime)
{
    TGuiControl::Update(DeltaTime);
    if (m_PreviewImage)
    {
        if (m_IsPreviewAnimating)
        {
            float speed = 1.0f / m_PreviewAnimDuration;
            m_PreviewPicture->Show();
            if (m_IsExpanded)
            {
                m_PreviewAnimProgress += DeltaTime * speed;
                if (m_PreviewAnimProgress >= 1.0f)
                {
                    m_PreviewAnimProgress = 1.0f;
                    m_IsPreviewAnimating = false;
                }
                Invalidate();
            }
            else
            {
                m_PreviewAnimProgress -= DeltaTime * speed;
                if (m_PreviewAnimProgress <= 0.0f)
                {
                    m_PreviewAnimProgress = 0.0f;
                    m_IsPreviewAnimating = false;
                    m_PreviewPicture->RemoveFromPrivilegedList();
                    CaptureMouse(this);
                    m_PreviewPicture->Hide();
                }
                Invalidate();
            }
        }

        // Calculate interpolated current rectangle using smoothstep
        float t = Tilc::Smoothstep(0.0f, 1.0f, m_PreviewAnimProgress);
        m_PreviewCurrentRect = {
            m_PreviewStartRect.x + (m_PreviewTargetRect.x - m_PreviewStartRect.x) * t,
            m_PreviewStartRect.y + (m_PreviewTargetRect.y - m_PreviewStartRect.y) * t,
            m_PreviewStartRect.w + (m_PreviewTargetRect.w - m_PreviewStartRect.w) * t,
            m_PreviewStartRect.h + (m_PreviewTargetRect.h - m_PreviewStartRect.h) * t
        };
        m_PreviewPicture->m_Position = m_PreviewCurrentRect;
    }
    return true;
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

bool Tilc::Gui::TImageListView::OnMouseButtonUp(const SDL_Event& event)
{
    bool Result = TGuiControl::OnMouseButtonUp(event);
    m_LastCickedImageIndex = -1;

    if (m_AllowRemoveImage)
    {
        if (PointIn(event.button.x, event.button.y))
        {
            Tilc::Resources::TTextureManager* tmgr = Tilc::GameObject->GetContext()->m_TextureManager;
            SDL_FRect Position{ GetRealPosition() };
            float x = Position.x;
            float y = Position.y;
            SDL_FRect SrcRect;
            for (int i = 0; i < m_Images.size(); ++i)
            {
                if (event.button.x >= x && event.button.x <= x + m_ImageWidth && event.button.y >= y && event.button.y <= y + m_ImageHeight)
                {
                    m_LastCickedImageIndex = i;
                }

                if (event.button.x >= x + m_ImageWidth - 20.0f && event.button.x <= x + m_ImageWidth && event.button.y >= y && event.button.y <= y + 20.0f)
                {
                    auto pos = m_Images[i].find("Pic_");
                    if (pos != std::string::npos)
                    {
                        tmgr->ReleaseResource(m_Images[i]);
                        if (OnRemoveImage)
                        {
                            OnRemoveImage(this, i, m_Images[i]);
                        }
                        m_Images.erase(m_Images.begin() + i);
                        m_LastCickedImageIndex = -1;
                        break;
                    }
                }

                if (m_LastCickedImageIndex != -1)
                {
                    if (OnImageClicked) OnImageClicked(this, m_LastCickedImageIndex);
                    break;
                }

                x += m_ImageWidth + 5;
                if (i > 0 && i % m_ImagesPerRow == 0)
                {
                    y += m_ImageHeight + 5;
                    x = m_Position.x;
                }
            }
        }
    }

    return Result;
}

void Tilc::Gui::TImageListView::StartPreview()
{
    if (!m_PreviewImage) return;

    m_IsExpanded = true;
    m_IsPreviewAnimating = true;
    m_PreviewAnimProgress = 0.0f; // 0.0 (thumbnail) to 1.0 (original)

    // Thumbnail initial position and size
    m_PreviewStartRect = GetLastClickedImageRect();

    unsigned int WindowWidth = Tilc::GameObject->m_Window->GetWindowWidth();
    unsigned int WindowHeight = Tilc::GameObject->m_Window->GetWindowHeight();

    // Target position and size (centered in the window, keeping original aspect ratio and fitting window bounds)
    float maxTargetWidth = static_cast<float>(WindowWidth) - 50.0f;
    float maxTargetHeight = static_cast<float>(WindowHeight) - 50.0f;

    float targetW = static_cast<float>(m_PreviewImage->w);
    float targetH = static_cast<float>(m_PreviewImage->h);

    // Scale down if it exceeds window bounds while preserving aspect ratio
    if (targetW > maxTargetWidth)
    {
        float scale = maxTargetWidth / targetW;
        targetW *= scale;
        targetH *= scale;
    }
    if (targetH > maxTargetHeight)
    {
        float scale = maxTargetHeight / targetH;
        targetW *= scale;
        targetH *= scale;
    }

    m_PreviewTargetRect = {
        (WindowWidth - targetW) / 2.0f,
        (WindowHeight - targetH) / 2.0f,
        targetW,
        targetH
    };

    // if PreviewPicture control was not created then create it now
    if (!m_PreviewPicture)
    {
        m_PreviewPicture = new Tilc::Gui::TPicture(GetParentWindow(), m_Name + "_PreviewPicture", m_Position, m_PreviewImage);
        if (m_PreviewPicture)
        {
            m_PreviewPicture->OnClick = Tilc::Gui::TImageListView::OnPreviewPictureClick;
            m_PreviewPicture->Hide();
        }
    }
    if (m_PreviewPicture)
    {
        m_PreviewPicture->AddToPrivilegedList();
        m_PreviewPicture->m_Position = m_PreviewStartRect;
        m_PreviewPicture->Show();
        CaptureMouse(m_PreviewPicture);
    }
}

SDL_FRect Tilc::Gui::TImageListView::GetLastClickedImageRect()
{
    SDL_FRect Rect{};
    if (m_Images.size() == 0)
    {
        return Rect;
    }

    SDL_FRect Position{ GetRealPosition() };
    Rect.x = Position.x;
    Rect.y = Position.y;
    Rect.w = m_ImageWidth;
    Rect.h = m_ImageHeight;
    for (int i = 0; i < m_LastCickedImageIndex; ++i)
    {
        Rect.x += m_ImageWidth + 5;
        if (i > 0 && i % m_ImagesPerRow == 0)
        {
            Rect.y += m_ImageHeight + 5;
            Rect.x = m_Position.x;
        }
    }

    return Rect;
}

int Tilc::Gui::TImageListView::OnPreviewPictureClick(float x, float y, int MouseButton, TGuiControl* Self)
{
    Tilc::TExtString ListNameControl = Self->GetName().substr(0, Self->GetName().length() - std::strlen("_PreviewPicture"));
    Tilc::Gui::TImageListView* lst = dynamic_cast<Tilc::Gui::TImageListView*>(Self->GetParentWindow()->GetChildIncludingAllSubWindows(ListNameControl));
    std::cout << ListNameControl << ": " << lst << std::endl;
    if (lst)
    {
        lst->TogglePreview();
    }
    return 1;
}
