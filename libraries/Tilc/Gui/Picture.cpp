#include "Tilc/Gui/Picture.h"
#include "Tilc/Gui/Theme.h"

Tilc::Gui::TPicture::TPicture(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, SDL_Texture* Texture)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Picture), m_Picture(Texture)
{
}

Tilc::Gui::TPicture::~TPicture()
{
}

void Tilc::Gui::TPicture::Draw()
{
    if (!m_Visible) return;
    m_NeedUpdate = ENeedUpdate::ENU_None;

    SDL_RenderTexture(Renderer, m_Picture, nullptr, &m_Position);
}
