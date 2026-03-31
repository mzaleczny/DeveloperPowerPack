#include "Theme.h"
#include "Tilc/Game.h"
#include "Tilc/Resources/TextureManager.h"
#include "Tilc/Globals.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Graphics/GraphicsUtils.h"

Tilc::Gui::TTheme::TTheme(Tilc::TExtString name)
    : m_ThemeName(name)
{
}


Tilc::Gui::TTheme::~TTheme()
{
    Unload();
}

void Tilc::Gui::TTheme::Load(Tilc::TExtString name)
{
    if (name.empty())
    {
        name = m_ThemeName;
    }

    Tilc::TExtString ThemeDir{ "assets/themes/" + name};
    Tilc::TExtString Filename{ "GUI.png"};
    if (!FileExists(ThemeDir + "/" + Filename))
    {
        ThemeDir = "../assets/themes/" + name;
    }
    Filename = ThemeDir + "/GUI.png";
    if (!FileExists(Filename))
    {
        return;
    }

    m_ThemeDir = ThemeDir;
    m_ThemeName = name;

    SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
    GuiTextureMap1_TMP = Tilc::Graphics::LoadTextureFromFileAsRenderTarget(Renderer, Filename.c_str());
    if (!GuiTextureMap1_TMP) return;


    SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
    SDL_SetRenderTarget(Renderer, GuiTextureMap1_TMP);
  

    LayoutInputStream.open(Filename + ".txt", std::ios::in);

    //this->globalStandardFont = new CFont(NULL, RGB(0, 0, 0), "Verdana", 8);
    this->LoadWindowSkinResources(name);
    this->LoadPanelSkinResources(name);
    this->LoadMenuSkinResources(name);
    this->LoadPopupmenuSkinResources(name);
    this->LoadToolbar16SkinResources(name);
    this->LoadLabelSkinResources(name);
    this->LoadScrollBarSkinResources(name);
    this->LoadTextfieldSkinResources(name);
    this->LoadButtonSkinResources(name);
    this->LoadCheckboxSkinResources(name);
    this->LoadListboxSkinResources(name);
    this->LoadGridSkinResources(name);

    LayoutInputStream.close();

    SDL_SetRenderTarget(Renderer, OldRenderTarget);
    Tilc::Graphics::SaveTextureToFile(Renderer, GuiTextureMap1_TMP, (Tilc::GetTmpFolder() + "/GUI.png").c_str());

    Tilc::Resources::TTextureManager* tm = GameObject->GetContext()->m_TextureManager;
    GuiTextureMap1 = tm->LoadFromFile(Tilc::GetTmpFolder() + "/GUI.png", "SDL_Texture")->AsSDLTexture();
    if (!GuiTextureMap1) return;

    LoadFonts();
}

void Tilc::Gui::TTheme::LoadFonts()
{
    Tilc::GameObject->LoadFontsFromConfig(m_ThemeDir + "/Fonts.cfg");
    DefaultFont = Tilc::GameObject->GetFont("Default");
    /*
    this->globalStandardFont = new CFont(NULL, RGB(0, 0, 0), "Verdana", 8);
    this->windowCaptionFont = new CFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
    this->captionFont = new CFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
    this->commonTextControlFont = new CFont(NULL, this->commonTextControlNormalFontColor, "Verdana", 8);
    this->menuFont = this->globalStandardFont;
    this->commonLabelFont = this->globalStandardFont;
    this->commonButtonControlFont = this->globalStandardFont;
    this->commonCheckboxControlFont = this->globalStandardFont;
    this->commonListboxControlFont = this->globalStandardFont;
    this->commonGridControlFont = this->globalStandardFont;
    */
}

void Tilc::Gui::TTheme::LoadWindowSkinResources(Tilc::TExtString themeName)
{
    if (GameObject)
    {
        Tilc::TExtString Line;
        while (std::getline(LayoutInputStream, Line))
        {
            std::stringstream Keystream(Line);
            Tilc::TExtString Item, sx, sy, sw, sh;
            Keystream >> Item >> sx >> sy >> sw >> sh;
            Item = Item.substr(0, Item.length() - 1);
            sx = sx.substr(0, sx.length() - 1);
            sy = sy.substr(0, sy.length() - 1);
            sw = sw.substr(0, sw.length() - 1);
            //sh = sh.substr(0, sh.length() - 1);

            if (Item == "wnd_caption_left")
                wnd_caption_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_caption_middle")
                wnd_caption_middle_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_caption_right")
                wnd_caption_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_caption_inactive_left")
                wnd_caption_inactive_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_caption_inactive_middle")
                wnd_caption_inactive_middle_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_caption_inactive_right")
                wnd_caption_inactive_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_left")
                wnd_frame_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_right")
                wnd_frame_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_bottom")
                wnd_frame_bottom_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_bottom_left")
                wnd_frame_bottom_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_bottom_right")
                wnd_frame_bottom_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_inactive_left")
                wnd_frame_inactive_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_inactive_right")
                wnd_frame_inactive_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_inactive_right")
                wnd_frame_inactive_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_inactive_bottom")
                wnd_frame_inactive_bottom_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_frame_inactive_bottom_left")
                wnd_frame_inactive_bottom_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_close_button")
                wnd_close_button_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_close_button_active")
                wnd_close_button_active_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_close_button_hover")
                wnd_close_button_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_minimize_button")
                wnd_minimize_button_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_minimize_button_active")
                wnd_minimize_button_active_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_minimize_button_hover")
                wnd_minimize_button_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_maximize_button")
                wnd_maximize_button_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_maximize_button_active")
                wnd_maximize_button_active_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_maximize_button_hover")
                wnd_maximize_button_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_restore_button")
                wnd_restore_button_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_restore_button_active")
                wnd_restore_button_active_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_restore_button_hover")
                wnd_restore_button_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "wnd_leftmenu_button")
            {
                wnd_leftmenu_button_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
                break;
            }
        }

        /*
        // Fonty
        windowCaptionFont = new CFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
        captionFont = new CFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
        */
    }
}

void Tilc::Gui::TTheme::LoadPanelSkinResources(Tilc::TExtString themeName)
{
    /*
    this->panel_nocaption_left = new CBitmap(this->_hInst, panelDir + L"nocaption_left.bmp");
    this->panel_nocaption_middle = new CBitmap(this->_hInst, panelDir + L"nocaption_middle.bmp");
    this->panel_nocaption_right = new CBitmap(this->_hInst, panelDir + L"nocaption_right.bmp");
    this->panel_caption_left = new CBitmap(this->_hInst, panelDir + L"caption_left.bmp");
    this->panel_caption_middle = new CBitmap(this->_hInst, panelDir + L"caption_middle.bmp");
    this->panel_caption_right = new CBitmap(this->_hInst, panelDir + L"caption_right.bmp");
    this->panel_frame_left = new CBitmap(this->_hInst, panelDir + L"frame_left.bmp");
    this->panel_frame_right = new CBitmap(this->_hInst, panelDir + L"frame_right.bmp");
    this->panel_frame_bottom = new CBitmap(this->_hInst, panelDir + L"frame_bottom.bmp");
    this->panel_frame_bottom_left = new CBitmap(this->_hInst, panelDir + L"frame_bottom_left.bmp");
    this->panel_frame_bottom_right = new CBitmap(this->_hInst, panelDir + L"frame_bottom_right.bmp");
    this->panel_inner_bg = new CBitmap(this->_hInst, panelDir + L"inner_bg.bmp");
    // fulscreen bitmap for all panels to speedup panel redrawing
    if (this->panel_inner_bg && this->panel_inner_bg->hBitmap) {
        this->panel_inner_bg_fullscreen = this->_prepareScreenBg(this->panel_inner_bg);
    }
    */
}

void Tilc::Gui::TTheme::LoadMenuSkinResources(Tilc::TExtString themeName)
{
    /*
    this->menu_bg_left = new CBitmap(this->_hInst, menuDir + L"bg_left.bmp");
    this->menu_bg_middle = new CBitmap(this->_hInst, menuDir + L"bg_middle.bmp");
    this->menu_bg_right = new CBitmap(this->_hInst, menuDir + L"bg_right.bmp");
    this->menu_selected_mainmenu_item_bg_left = new CBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_left.bmp");
    this->menu_selected_mainmenu_item_bg_middle = new CBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_middle.bmp");
    this->menu_selected_mainmenu_item_bg_right = new CBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_right.bmp");
    this->menu_opened_mainmenu_item_bg_left = new CBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_left.bmp");
    this->menu_opened_mainmenu_item_bg_middle = new CBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_middle.bmp");
    this->menu_opened_mainmenu_item_bg_right = new CBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_right.bmp");
    this->menu_shadow_bottom_left = new CBitmap(this->_hInst, menuDir + L"shadow_bottom_left.bmp");
    this->menu_shadow_bottom_middle = new CBitmap(this->_hInst, menuDir + L"shadow_bottom_middle.bmp");
    this->menu_shadow_bottom_right = new CBitmap(this->_hInst, menuDir + L"shadow_bottom_right.bmp");
    this->menu_shadow_right_middle = new CBitmap(this->_hInst, menuDir + L"shadow_right_middle.bmp");
    this->menu_shadow_right_top = new CBitmap(this->_hInst, menuDir + L"shadow_right_top.bmp");

    // Fonty
    if (!this->menuFont) {
        this->menuFont = this->globalStandardFont;
    }
    */
}

void Tilc::Gui::TTheme::LoadPopupmenuSkinResources(Tilc::TExtString themeName)
{
    /*
    this->popupmenu_bg = new CBitmap(this->_hInst, menuDir + L"popupmenu_bg.bmp");
    this->popupmenu_frame_bottom = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom.bmp");
    this->popupmenu_frame_bottom_left = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom_left.bmp");
    this->popupmenu_frame_bottom_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom_right.bmp");
    this->popupmenu_frame_left = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_left.bmp");
    this->popupmenu_frame_left_top = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_left_top.bmp");
    this->popupmenu_frame_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_right.bmp");
    this->popupmenu_frame_top = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_top.bmp");
    this->popupmenu_frame_top_hilited = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_hilited.bmp");
    this->popupmenu_frame_top_hilited_end = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_hilited_end.bmp");
    this->popupmenu_frame_top_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_right.bmp");
    this->popupmenu_icon_area_left = new CBitmap(this->_hInst, menuDir + L"popupmenu_icon_area_left.bmp");
    this->popupmenu_submenu_arrow = new CBitmap(this->_hInst, menuDir + L"popupmenu_submenu_arrow.bmp");
    this->popupmenu_frame_shadow_bottom = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom.bmp");
    this->popupmenu_frame_shadow_bottom_left = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom_left.bmp");
    this->popupmenu_frame_shadow_bottom_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom_right.bmp");
    this->popupmenu_frame_shadow_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_right.bmp");
    this->popupmenu_frame_shadow_top_right = new CBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_top_right.bmp");
    this->popupmenu_divider = new CBitmap(this->_hInst, menuDir + L"popupmenu_divider.bmp");
    this->popupmenu_selected_item_bg_left = new CBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_left.bmp");
    this->popupmenu_selected_item_bg_middle = new CBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_middle.bmp");
    this->popupmenu_selected_item_bg_right = new CBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_right.bmp");
    this->popupmenu_opened_item_bg_left = new CBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_left.bmp");
    this->popupmenu_opened_item_bg_middle = new CBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_middle.bmp");
    this->popupmenu_opened_item_bg_right = new CBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_right.bmp");

    // Fonty
    if (!this->menuFont) {
        this->menuFont = this->globalStandardFont;
    }
    */
}

void Tilc::Gui::TTheme::LoadToolbar16SkinResources(Tilc::TExtString themeName)
{
    /*
    this->toolbar16_left = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_left.bmp");
    this->toolbar16_middle = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_middle.bmp");
    this->toolbar16_right = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_right.bmp");
    this->toolbar16_button_hilited = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_button_hilited.bmp");
    this->toolbar16_button_pressed = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_button_pressed.bmp");
    this->toolbar16_separator = new CBitmap(this->_hInst, toolbarDir + L"toolbar16_separator.bmp");
    */
}

void Tilc::Gui::TTheme::LoadLabelSkinResources(Tilc::TExtString themeName)
{
    // Fonty
    //this->commonLabelFont = this->globalStandardFont;
}

void Tilc::Gui::TTheme::LoadScrollBarSkinResources(Tilc::TExtString themeName)
{
    if (GameObject)
    {
        Tilc::TExtString Line;
        while (std::getline(LayoutInputStream, Line))
        {
            std::stringstream Keystream(Line);
            Tilc::TExtString Item, sx, sy, sw, sh;
            Keystream >> Item >> sx >> sy >> sw >> sh;
            Item = Item.substr(0, Item.length() - 1);
            sx = sx.substr(0, sx.length() - 1);
            sy = sy.substr(0, sy.length() - 1);
            sw = sw.substr(0, sw.length() - 1);
            //sh = sh.substr(0, sh.length() - 1);

            if (Item == "scrollbar-vertical-arrow-down_rc")
                scrollbar_vertical_arrow_down_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-down_disabled_rc")
                scrollbar_vertical_arrow_down_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-down_hover_rc")
                scrollbar_vertical_arrow_down_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-down_pushed_rc")
                scrollbar_vertical_arrow_down_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };

            else if (Item == "scrollbar-vertical-arrow-up_rc")
                scrollbar_vertical_arrow_up_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-up_disabled_rc")
                scrollbar_vertical_arrow_up_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-up_hover_rc")
                scrollbar_vertical_arrow_up_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-arrow-up_pushed_rc")
                scrollbar_vertical_arrow_up_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };

            else if (Item == "scrollbar-horizontal-arrow-left_rc")
                scrollbar_horizontal_arrow_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-left_disabled_rc")
                scrollbar_horizontal_arrow_left_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-left_hover_rc")
                scrollbar_horizontal_arrow_left_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-left_pushed_rc")
                scrollbar_horizontal_arrow_left_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };

            else if (Item == "scrollbar-horizontal-arrow-right_rc")
                scrollbar_horizontal_arrow_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-right_disabled_rc")
                scrollbar_horizontal_arrow_right_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-right_hover_rc")
                scrollbar_horizontal_arrow_right_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-arrow-right_pushed_rc")
                scrollbar_horizontal_arrow_right_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };

            else if (Item == "scrollbar-horizontal-bg_rc")
                scrollbar_horizontal_bg_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-bg_disabled_rc")
                scrollbar_horizontal_bg_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-bg_pushed_rc")
                scrollbar_horizontal_bg_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-thumb-left_rc")
                scrollbar_horizontal_thumb_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-thumb-middle_rc")
                scrollbar_horizontal_thumb_middle_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-thumb-right_rc")
                scrollbar_horizontal_thumb_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-horizontal-thumb-bg_rc")
                scrollbar_horizontal_thumb_bg_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };

            else if (Item == "scrollbar-vertical-bg_rc")
                scrollbar_vertical_bg_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-bg_disabled_rc")
                scrollbar_vertical_bg_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-bg_pushed_rc")
                scrollbar_vertical_bg_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-thumb-top_rc")
                scrollbar_vertical_thumb_top_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-thumb-middle_rc")
                scrollbar_vertical_thumb_middle_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-thumb-bottom_rc")
                scrollbar_vertical_thumb_bottom_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "scrollbar-vertical-thumb-bg_rc")
            {
                scrollbar_vertical_thumb_bg_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
                break;
            }
        }
    }
            /*
    this->scrollbar_horizontal_arrow_left = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_arrow_left.bmp").c_str());
    this->scrollbar_horizontal_arrow_left_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_arrow_left_clicked.bmp").c_str());
    this->scrollbar_horizontal_arrow_right = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_arrow_right.bmp").c_str());
    this->scrollbar_horizontal_arrow_right_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_arrow_right_clicked.bmp").c_str());
    this->scrollbar_horizontal_thumb_left = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_thumb_left.bmp").c_str());
    this->scrollbar_horizontal_thumb_middle = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_thumb_middle.bmp").c_str());
    this->scrollbar_horizontal_thumb_right = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_thumb_right.bmp").c_str());
    this->scrollbar_horizontal_thumb_bg = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_thumb_bg.bmp").c_str());
    this->scrollbar_horizontal_bg = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_bg.bmp").c_str());
    this->scrollbar_horizontal_bg_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "horizontal_bg_clicked.bmp").c_str());
    */
    /*
    this->scrollbar_vertical_arrow_up = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_arrow_up.png").c_str());
    this->scrollbar_vertical_arrow_up_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_arrow_up_clicked.png").c_str());
    this->scrollbar_vertical_arrow_down = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_arrow_down.png").c_str());
    this->scrollbar_vertical_arrow_down_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_arrow_down_clicked.png").c_str());
    this->scrollbar_vertical_thumb_top = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_thumb_top.png").c_str());
    this->scrollbar_vertical_thumb_middle = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_thumb_middle.png").c_str());
    this->scrollbar_vertical_thumb_bottom = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_thumb_bottom.png").c_str());
    this->scrollbar_vertical_thumb_bg = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_thumb_bg.png").c_str());
    this->scrollbar_vertical_bg = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_bg.png").c_str());
    this->scrollbar_vertical_bg_clicked = progmar::sdl3engine::TResources::LoadTexture(Application->Screen, DbResourcesFname, (ScrbarName + "vertical_bg_clicked.png").c_str());
    */
}

void Tilc::Gui::TTheme::LoadTextfieldSkinResources(Tilc::TExtString themeName)
{
    /*
    this->textfield_frame_left = new CBitmap(this->_hInst, textfieldDir + L"frame_left.bmp");
    this->textfield_frame_right = new CBitmap(this->_hInst, textfieldDir + L"frame_right.bmp");
    this->textfield_frame_middle = new CBitmap(this->_hInst, textfieldDir + L"frame_middle.bmp");
    this->textfield_frame_left_hover = new CBitmap(this->_hInst, textfieldDir + L"frame_left_hover.bmp");
    this->textfield_frame_right_hover = new CBitmap(this->_hInst, textfieldDir + L"frame_right_hover.bmp");
    this->textfield_frame_middle_hover = new CBitmap(this->_hInst, textfieldDir + L"frame_middle_hover.bmp");
    this->textfield_frame_left_active = new CBitmap(this->_hInst, textfieldDir + L"frame_left_active.bmp");
    this->textfield_frame_right_active = new CBitmap(this->_hInst, textfieldDir + L"frame_right_active.bmp");
    this->textfield_frame_middle_active = new CBitmap(this->_hInst, textfieldDir + L"frame_middle_active.bmp");
    this->textfield_selection = new CBitmap(this->_hInst, textfieldDir + L"selection.bmp");

    // Pozostałe parametry i zasoby
    this->commonTextControlNormalFontColor = RGB(0, 0, 0);
    this->commonTextControlSelectedFontColor = RGB(0xff, 0xff, 0xff);
    this->commonTextControlFont = new CFont(NULL, this->commonTextControlNormalFontColor, "Verdana", 8);
    */
}

void Tilc::Gui::TTheme::LoadButtonSkinResources(Tilc::TExtString themeName)
{
    if (GameObject)
    {
        Tilc::TExtString Line;
        while (std::getline(LayoutInputStream, Line))
        {
            std::stringstream Keystream(Line);
            Tilc::TExtString Item, sx, sy, sw, sh;
            Keystream >> Item >> sx >> sy >> sw >> sh;
            Item = Item.substr(0, Item.length() - 1);
            sx = sx.substr(0, sx.length() - 1);
            sy = sy.substr(0, sy.length() - 1);
            sw = sw.substr(0, sw.length() - 1);
            //sh = sh.substr(0, sh.length() - 1);

            if (Item == "button_left_rc")
                button_left_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_rc")
                button_right_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_rc")
                button_middle_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_focused_rc")
                button_left_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_focused_rc")
                button_right_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_focused_rc")
                button_middle_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_disabled_rc")
                button_left_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_disabled_rc")
                button_right_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_diasbled_rc")
                button_middle_disabled_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_hover_rc")
                button_left_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_hover_rc")
                button_right_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_hover_rc")
                button_middle_hover_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_hover_focused_rc")
                button_left_hover_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_hover_focused_rc")
                button_right_hover_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_hover_focused_rc")
                button_middle_hover_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_pushed_rc")
                button_left_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_pushed_rc")
                button_right_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_pushed_rc")
                button_middle_pushed_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_left_pushed_focused_rc")
                button_left_pushed_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_right_pushed_focused_rc")
                button_right_pushed_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
            else if (Item == "button_middle_pushed_focused_rc")
            {
                button_middle_pushed_focused_rc = SDL_FRect{ sx.toFloat(), sy.toFloat(), sw.toFloat(), sh.toFloat() };
                break;
            }
        }
    }
    /*
    // Fonty
    this->commonButtonControlFont = this->globalStandardFont;
    */
}

void Tilc::Gui::TTheme::LoadCheckboxSkinResources(Tilc::TExtString themeName)
{
    /*
    this->checkbox = new CBitmap(this->_hInst, checkboxDir + L"checkbox.bmp");
    this->checkbox_disabled = new CBitmap(this->_hInst, checkboxDir + L"checkbox_disabled.bmp");
    this->checkbox_hover = new CBitmap(this->_hInst, checkboxDir + L"checkbox_hover.bmp");
    this->checkbox_checked = new CBitmap(this->_hInst, checkboxDir + L"checkbox_checked.bmp");
    this->checkbox_checked_disabled = new CBitmap(this->_hInst, checkboxDir + L"checkbox_checked_disabled.bmp");
    this->checkbox_checked_hover = new CBitmap(this->_hInst, checkboxDir + L"checkbox_checked_hover.bmp");

    // Fonty
    this->commonCheckboxControlFont = this->globalStandardFont;
    */
}

void Tilc::Gui::TTheme::LoadListboxSkinResources(Tilc::TExtString themeName)
{
    /*
    this->listbox_bg = new CBitmap(this->_hInst, listboxDir + L"bg.bmp");
    this->listbox_bg_fullscreen = this->_prepareScreenBg(this->listbox_bg);
    this->listbox_frame_top_left = new CBitmap(this->_hInst, listboxDir + L"frame_top_left.bmp");
    this->listbox_frame_top = new CBitmap(this->_hInst, listboxDir + L"frame_top.bmp");
    this->listbox_frame_top_right = new CBitmap(this->_hInst, listboxDir + L"frame_top_right.bmp");
    this->listbox_frame_bottom_left = new CBitmap(this->_hInst, listboxDir + L"frame_bottom_left.bmp");
    this->listbox_frame_bottom = new CBitmap(this->_hInst, listboxDir + L"frame_bottom.bmp");
    this->listbox_frame_bottom_right = new CBitmap(this->_hInst, listboxDir + L"frame_bottom_right.bmp");
    this->listbox_frame_left = new CBitmap(this->_hInst, listboxDir + L"frame_left.bmp");
    this->listbox_frame_right = new CBitmap(this->_hInst, listboxDir + L"frame_right.bmp");

    // Fonty
    this->commonListboxControlFont = this->globalStandardFont;
    */
}

void Tilc::Gui::TTheme::LoadGridSkinResources(Tilc::TExtString themeName)
{
    /*
    this->grid_cell_inner_bg_normal = new CBitmap(this->_hInst, gridDir + L"cell_inner_bg_normal.bmp");
    this->grid_cell_inner_bg_selected = new CBitmap(this->_hInst, gridDir + L"cell_inner_bg_selected.bmp");
    this->grid_cell_inner_bg_selected_active = new CBitmap(this->_hInst, gridDir + L"cell_inner_bg_selected_active.bmp");
    this->grid_left_header_inner_bg_normal_cell = new CBitmap(this->_hInst, gridDir + L"left_header_inner_bg_normal_cell.bmp");
    this->grid_left_header_inner_bg_selected_cell = new CBitmap(this->_hInst, gridDir + L"left_header_inner_bg_selected_cell.bmp");
    this->grid_left_top_header_inner_bg = new CBitmap(this->_hInst, gridDir + L"left_top_header_inner_bg.bmp");
    this->grid_left_top_header_mark = new CBitmap(this->_hInst, gridDir + L"left_top_header_mark.bmp");
    this->grid_top_header_inner_bg_normal_cell = new CBitmap(this->_hInst, gridDir + L"top_header_inner_bg_normal_cell.bmp");
    this->grid_top_header_inner_bg_selected_cell = new CBitmap(this->_hInst, gridDir + L"top_header_inner_bg_selected_cell.bmp");

    // Fonty i pozostałe parametry
    this->commonGridControlFont = this->globalStandardFont;

    Tilc::TExtString settings_fname = gridDir + L"\\_settings.ini";

    this->commonGridControlTopHeaderNormalFontColor = this->_getIniColorValue(settings_fname, L"top_header_font_color_normal");
    this->commonGridControlTopHeaderSelectedFontColor = this->_getIniColorValue(settings_fname, L"top_header_font_color_selected");
    this->commonGridControlLeftHeaderNormalFontColor = this->_getIniColorValue(settings_fname, L"left_header_font_color_normal");
    this->commonGridControlLeftHeaderSelectedFontColor = this->_getIniColorValue(settings_fname, L"left_header_font_color_selected");
    this->commonGridControlTopHeaderCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"top_header_cell_border_color_normal");
    this->commonGridControlTopHeaderCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"top_header_cell_border_color_selected");
    this->commonGridControlLeftTopHeaderCellBorderColor = this->_getIniColorValue(settings_fname, L"left_top_header_cell_border_color");
    this->commonGridControlLeftHeaderCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"left_header_cell_border_color_normal");
    this->commonGridControlLeftHeaderCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"left_header_cell_border_color_selected");
    this->commonGridControlCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"cell_border_color_normal");
    this->commonGridControlCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"cell_border_color_selected");
    this->commonGridControlCellBorderColor_Active = this->_getIniColorValue(settings_fname, L"cell_border_color_active");
    this->commonGridControlSelectionBorderColor = this->_getIniColorValue(settings_fname, L"border_color_selection");
    this->commonGridControlFocusedGridBorderColor = this->_getIniColorValue(settings_fname, L"border_color_focused_grid");
    */
}

void Tilc::Gui::TTheme::Unload()
{
    if (GuiTextureMap1)
    {
        SDL_DestroyTexture(GuiTextureMap1);
        GuiTextureMap1 = nullptr;
    }

    /*
    if (this->globalStandardFont) {
        delete this->globalStandardFont;
        this->globalStandardFont = NULL;
    }
    */
}

/*
VOID CTheme::_initAttributes(HINSTANCE hInst) {
    this->_hInst = hInst;
    
    this->_iniSectionColors = L"colors";

    this->wnd_caption_left = NULL;
    this->wnd_caption_middle = NULL;
    this->wnd_caption_right = NULL;
    this->wnd_frame_left = NULL;
    this->wnd_frame_right = NULL;
    this->wnd_frame_bottom = NULL;
    this->wnd_frame_bottom_left = NULL;
    this->wnd_frame_bottom_right = NULL;
    this->wnd_leftmenu_button = NULL;
    this->wnd_minimize_button = NULL;
    this->wnd_maximize_button = NULL;
    this->wnd_restore_button = NULL;
    this->wnd_close_button = NULL;

    // Bitmapy panelu
    this->panel_caption_left = NULL;
    this->panel_caption_middle = NULL;
    this->panel_caption_right = NULL;
    this->panel_nocaption_left = NULL;
    this->panel_nocaption_middle = NULL;
    this->panel_nocaption_right = NULL;
    this->panel_frame_left = NULL;
    this->panel_frame_right = NULL;
    this->panel_frame_bottom = NULL;
    this->panel_frame_bottom_left = NULL;
    this->panel_frame_bottom_right = NULL;
    this->panel_inner_bg = NULL;
    this->panel_inner_bg_fullscreen = NULL;

	// Bitmapy menu
	this->menu_bg_left = NULL;
	this->menu_bg_middle = NULL;
	this->menu_bg_right = NULL;
	this->menu_selected_mainmenu_item_bg_left = NULL;
	this->menu_selected_mainmenu_item_bg_middle = NULL;
	this->menu_selected_mainmenu_item_bg_right = NULL;
	this->menu_opened_mainmenu_item_bg_left = NULL;
	this->menu_opened_mainmenu_item_bg_middle = NULL;
	this->menu_opened_mainmenu_item_bg_right = NULL;
	this->menu_shadow_bottom_left = NULL;
	this->menu_shadow_bottom_middle = NULL;
	this->menu_shadow_bottom_right = NULL;
	this->menu_shadow_right_middle = NULL;
	this->menu_shadow_right_top = NULL;

    // Bitmapy popupmenu
    this->popupmenu_bg = NULL;
    this->popupmenu_frame_bottom = NULL;
    this->popupmenu_frame_bottom_left = NULL;
    this->popupmenu_frame_bottom_right = NULL;
    this->popupmenu_frame_left = NULL;
    this->popupmenu_frame_left_top = NULL;
    this->popupmenu_frame_right = NULL;
    this->popupmenu_frame_top = NULL;
    this->popupmenu_frame_top_hilited = NULL;
    this->popupmenu_frame_top_hilited_end = NULL;
    this->popupmenu_frame_top_right = NULL;
    this->popupmenu_icon_area_left = NULL;
    this->popupmenu_submenu_arrow = NULL;
    this->popupmenu_frame_shadow_bottom = NULL;
    this->popupmenu_frame_shadow_bottom_left = NULL;
    this->popupmenu_frame_shadow_bottom_right = NULL;
    this->popupmenu_frame_shadow_right = NULL;
    this->popupmenu_frame_shadow_top_right = NULL;
    this->popupmenu_divider = NULL;
	this->popupmenu_selected_item_bg_left = NULL;
	this->popupmenu_selected_item_bg_middle = NULL;
	this->popupmenu_selected_item_bg_right = NULL;
	this->popupmenu_opened_item_bg_left = NULL;
	this->popupmenu_opened_item_bg_middle = NULL;
	this->popupmenu_opened_item_bg_right = NULL;

    // Bitmapy toolbara przeznaczonego na ikony o rozmiarze 16x16
    this->toolbar16_left = NULL;
    this->toolbar16_middle = NULL;
    this->toolbar16_right = NULL;
    this->toolbar16_button_hilited = NULL;
    this->toolbar16_button_pressed = NULL;
    this->toolbar16_separator = NULL;

    // Bitmapy scrollbara
    this->scrollbar_horizontal_arrow_left = NULL;
    this->scrollbar_horizontal_arrow_left_clicked = NULL;
    this->scrollbar_horizontal_arrow_right = NULL;
    this->scrollbar_horizontal_arrow_right_clicked = NULL;
    this->scrollbar_horizontal_thumb_left = NULL;
    this->scrollbar_horizontal_thumb_middle = NULL;
    this->scrollbar_horizontal_thumb_right = NULL;
    this->scrollbar_horizontal_thumb_bg = NULL;
    this->scrollbar_horizontal_bg = NULL;
    this->scrollbar_horizontal_bg_clicked = NULL;
    this->scrollbar_vertical_arrow_up = NULL;
    this->scrollbar_vertical_arrow_up_clicked = NULL;
    this->scrollbar_vertical_arrow_down = NULL;
    this->scrollbar_vertical_arrow_down_clicked = NULL;
    this->scrollbar_vertical_thumb_top = NULL;
    this->scrollbar_vertical_thumb_middle = NULL;
    this->scrollbar_vertical_thumb_bottom = NULL;
    this->scrollbar_vertical_thumb_bg = NULL;
    this->scrollbar_vertical_bg = NULL;
    this->scrollbar_vertical_bg_clicked = NULL;

    // Bitmapy Textfield
    this->textfield_frame_left = NULL;
    this->textfield_frame_right = NULL;
    this->textfield_frame_middle = NULL;
    this->textfield_frame_left_hover = NULL;
    this->textfield_frame_right_hover = NULL;
    this->textfield_frame_middle_hover = NULL;
    this->textfield_frame_left_active = NULL;
    this->textfield_frame_right_active = NULL;
    this->textfield_frame_middle_active = NULL;
    this->textfield_selection = NULL;

    // Bitmapy Button
    this->button_left = NULL;
    this->button_right = NULL;
    this->button_middle = NULL;
    this->button_left_disabled = NULL;
    this->button_right_disabled = NULL;
    this->button_middle_disabled = NULL;
    this->button_left_hover = NULL;
    this->button_right_hover = NULL;
    this->button_middle_hover = NULL;
    this->button_left_pushed = NULL;
    this->button_right_pushed = NULL;
    this->button_middle_pushed = NULL;

    // Checkbox bitmaps
    this->checkbox = NULL;
    this->checkbox_disabled = NULL;
    this->checkbox_hover = NULL;
    this->checkbox_checked = NULL;
    this->checkbox_checked_disabled = NULL;
    this->checkbox_checked_hover = NULL;

    // Listbox bitmaps
    this->listbox_bg_fullscreen = NULL;
    this->listbox_bg = NULL;
    this->listbox_frame_top_left = NULL;
    this->listbox_frame_top = NULL;
    this->listbox_frame_top_right = NULL;
    this->listbox_frame_bottom_left = NULL;
    this->listbox_frame_bottom = NULL;
    this->listbox_frame_bottom_right = NULL;
    this->listbox_frame_left = NULL;
    this->listbox_frame_right = NULL;

    // Bitmapy Grid
    this->grid_cell_inner_bg_normal = NULL;
    this->grid_cell_inner_bg_selected = NULL;
    this->grid_cell_inner_bg_selected_active = NULL;
    this->grid_left_header_inner_bg_normal_cell = NULL;
    this->grid_left_header_inner_bg_selected_cell = NULL;
    this->grid_left_top_header_mark = NULL;
    this->grid_left_top_header_inner_bg = NULL;
    this->grid_top_header_inner_bg_normal_cell = NULL;
    this->grid_top_header_inner_bg_selected_cell = NULL;

    // Fonty
    this->globalStandardFont = NULL;
    this->windowCaptionFont = NULL;
    this->captionFont = NULL;
    this->menuFont = NULL;
    this->commonLabelFont = NULL;
    this->commonTextControlFont = NULL;
    this->commonButtonControlFont = NULL;
    this->commonGridControlFont = NULL;
}

SDL_Color CTheme::_getIniColorValue(Tilc::TExtString settings_fname, WCHAR *ini_varname) {
    DWORD buf_size = 16;
    WCHAR input[16];
    WCHAR *colors_section = this->_iniSectionColors.getBuffer();

    GetPrivateProfileString(colors_section, ini_varname, L"#000000", input, buf_size, settings_fname.getBuffer());
    return HexToRGB(input);
}


bool CTheme::isLoaded() {
    return this->isWindowSkinLoaded() &&
           this->isPanelSkinLoaded() &&
           this->isMenuSkinLoaded() &&
           this->isPopupmenuSkinLoaded() &&
           this->isToolbar16SkinLoaded() &&
           this->isLabelSkinLoaded() &&
           this->isScrollbarSkinLoaded() &&
           this->isScrollbarHorizontalSkinLoaded() &&
           this->isScrollbarVerticalSkinLoaded() &&
           this->isTextfieldSkinLoaded() &&
           this->isButtonSkinLoaded() &&
           this->isCheckboxSkinLoaded() &&
           this->isListboxSkinLoaded() &&
           this->isGridSkinLoaded();
}

bool CTheme::isWindowSkinLoaded() {
    return  this->wnd_caption_left != NULL &&
            this->wnd_caption_middle != NULL &&
            this->wnd_caption_right != NULL &&
            this->wnd_caption_inactive_left != NULL &&
            this->wnd_caption_inactive_middle != NULL &&
            this->wnd_caption_inactive_right != NULL &&
            this->wnd_frame_left != NULL &&
            this->wnd_frame_right != NULL &&
            this->wnd_frame_bottom != NULL &&
            this->wnd_frame_bottom_left != NULL &&
            this->wnd_frame_bottom_right != NULL &&
            this->wnd_frame_inactive_left != NULL &&
            this->wnd_frame_inactive_right != NULL &&
            this->wnd_frame_inactive_bottom != NULL &&
            this->wnd_frame_inactive_bottom_left != NULL &&
            this->wnd_frame_inactive_bottom_right != NULL &&
            //this->wnd_leftmenu_button != NULL &&
            this->wnd_minimize_button != NULL &&
            this->wnd_maximize_button != NULL &&
            this->wnd_restore_button != NULL &&
            this->wnd_close_button != NULL;
}

bool CTheme::isPanelSkinLoaded() {
    return  this->panel_caption_left != NULL &&
            this->panel_caption_middle != NULL &&
            this->panel_caption_right != NULL &&
            this->panel_nocaption_left != NULL &&
            this->panel_nocaption_middle != NULL &&
            this->panel_nocaption_right != NULL &&
            this->panel_frame_left != NULL &&
            this->panel_frame_right != NULL &&
            this->panel_frame_bottom != NULL &&
            this->panel_frame_bottom_left != NULL &&
            this->panel_frame_bottom_right != NULL &&
            this->panel_inner_bg != NULL &&
            this->panel_inner_bg_fullscreen != NULL;
}

bool CTheme::isMenuSkinLoaded() {
	return  this->menu_bg_left != NULL &&
            this->menu_bg_middle != NULL &&
            this->menu_bg_right != NULL &&
            this->menu_selected_mainmenu_item_bg_left != NULL &&
            this->menu_selected_mainmenu_item_bg_middle != NULL &&
            this->menu_selected_mainmenu_item_bg_right != NULL &&
            this->menu_opened_mainmenu_item_bg_left != NULL &&
            this->menu_opened_mainmenu_item_bg_middle != NULL &&
            this->menu_opened_mainmenu_item_bg_right != NULL &&
            this->menu_shadow_bottom_left != NULL &&
            this->menu_shadow_bottom_middle != NULL &&
            this->menu_shadow_bottom_right != NULL &&
            this->menu_shadow_right_middle != NULL &&
            this->menu_shadow_right_top != NULL;
}

bool CTheme::isPopupmenuSkinLoaded() {
    return  this->popupmenu_bg != NULL &&
            this->popupmenu_frame_bottom != NULL &&
            this->popupmenu_frame_bottom_left != NULL &&
            this->popupmenu_frame_bottom_right != NULL &&
            this->popupmenu_frame_left != NULL &&
            this->popupmenu_frame_left_top != NULL &&
            this->popupmenu_frame_right != NULL &&
            this->popupmenu_frame_top != NULL &&
            this->popupmenu_frame_top_hilited != NULL &&
            this->popupmenu_frame_top_hilited_end != NULL &&
            this->popupmenu_frame_top_right != NULL &&
            this->popupmenu_icon_area_left != NULL &&
            this->popupmenu_submenu_arrow != NULL &&
            this->popupmenu_frame_shadow_bottom != NULL &&
            this->popupmenu_frame_shadow_bottom_left != NULL &&
            this->popupmenu_frame_shadow_bottom_right != NULL &&
            this->popupmenu_frame_shadow_right != NULL &&
            this->popupmenu_frame_shadow_top_right != NULL &&
            this->popupmenu_divider != NULL &&
            this->popupmenu_selected_item_bg_left != NULL &&
            this->popupmenu_selected_item_bg_middle != NULL &&
            this->popupmenu_selected_item_bg_right != NULL &&
            this->popupmenu_opened_item_bg_left != NULL &&
            this->popupmenu_opened_item_bg_middle != NULL &&
            this->popupmenu_opened_item_bg_right != NULL;
}

bool CTheme::isToolbar16SkinLoaded() {
    return  this->toolbar16_left != NULL &&
            this->toolbar16_middle != NULL &&
            this->toolbar16_right != NULL &&
            this->toolbar16_button_hilited != NULL &&
            this->toolbar16_button_pressed != NULL &&
            this->toolbar16_separator != NULL;
}

bool CTheme::isLabelSkinLoaded() {
    return TRUE;
}

bool CTheme::isScrollbarSkinLoaded() {
    return this->isScrollbarHorizontalSkinLoaded() && this->isScrollbarVerticalSkinLoaded();
}

bool CTheme::isScrollbarHorizontalSkinLoaded() {
    return  this->scrollbar_horizontal_arrow_left != NULL &&
            this->scrollbar_horizontal_arrow_left_clicked != NULL &&
            this->scrollbar_horizontal_arrow_right != NULL &&
            this->scrollbar_horizontal_arrow_right_clicked != NULL &&
            this->scrollbar_horizontal_thumb_left != NULL &&
            this->scrollbar_horizontal_thumb_middle != NULL &&
            this->scrollbar_horizontal_thumb_right != NULL &&
            this->scrollbar_horizontal_thumb_bg != NULL &&
            this->scrollbar_horizontal_bg != NULL &&
            this->scrollbar_horizontal_bg_clicked != NULL;
}

bool CTheme::isScrollbarVerticalSkinLoaded() {
    return  this->scrollbar_vertical_arrow_up != NULL &&
            this->scrollbar_vertical_arrow_up_clicked != NULL &&
            this->scrollbar_vertical_arrow_down != NULL &&
            this->scrollbar_vertical_arrow_down_clicked != NULL &&
            this->scrollbar_vertical_thumb_top != NULL &&
            this->scrollbar_vertical_thumb_middle != NULL &&
            this->scrollbar_vertical_thumb_bottom != NULL &&
            this->scrollbar_vertical_thumb_bg != NULL &&
            this->scrollbar_vertical_bg != NULL &&
            this->scrollbar_vertical_bg_clicked != NULL;
}

bool CTheme::isTextfieldSkinLoaded() {
    return  this->textfield_frame_left != NULL &&
            this->textfield_frame_right != NULL &&
            this->textfield_frame_middle != NULL &&
            this->textfield_frame_left_hover != NULL &&
            this->textfield_frame_right_hover != NULL &&
            this->textfield_frame_middle_hover != NULL &&
            this->textfield_frame_left_active != NULL &&
            this->textfield_frame_right_active != NULL &&
            this->textfield_frame_middle_active != NULL &&
            this->textfield_selection != NULL;
}

bool CTheme::isButtonSkinLoaded() {
    return  this->button_left != NULL &&
            this->button_right != NULL &&
            this->button_middle != NULL &&
            this->button_left_disabled != NULL &&
            this->button_right_disabled != NULL &&
            this->button_middle_disabled != NULL &&
            this->button_left_hover != NULL &&
            this->button_right_hover != NULL &&
            this->button_middle_hover != NULL &&
            this->button_left_pushed != NULL &&
            this->button_right_pushed != NULL &&
            this->button_middle_pushed != NULL;
}

bool CTheme::isCheckboxSkinLoaded() {
    return  this->checkbox != NULL &&
            this->checkbox_disabled != NULL &&
            this->checkbox_hover != NULL &&
            this->checkbox_checked != NULL &&
            this->checkbox_checked_disabled != NULL &&
            this->checkbox_checked_hover != NULL;
}

bool CTheme::isListboxSkinLoaded() {
    return  this->listbox_bg_fullscreen != NULL &&
            this->listbox_bg != NULL &&
            this->listbox_frame_top_left != NULL &&
            this->listbox_frame_top != NULL &&
            this->listbox_frame_top_right != NULL &&
            this->listbox_frame_bottom_left != NULL &&
            this->listbox_frame_bottom != NULL &&
            this->listbox_frame_bottom_right != NULL &&
            this->listbox_frame_left != NULL &&
            this->listbox_frame_right != NULL;
}

bool CTheme::isGridSkinLoaded() {
    return  this->grid_cell_inner_bg_normal != NULL &&
            this->grid_cell_inner_bg_selected != NULL &&
            this->grid_cell_inner_bg_selected_active != NULL &&
            this->grid_left_header_inner_bg_normal_cell != NULL &&
            this->grid_left_header_inner_bg_selected_cell != NULL &&
            this->grid_left_top_header_inner_bg != NULL &&
            this->grid_left_top_header_mark != NULL &&
            this->grid_top_header_inner_bg_normal_cell != NULL &&
            this->grid_top_header_inner_bg_selected_cell != NULL;
}

bool CTheme::themeExists(Tilc::TExtString dir) {
    if (DirectoryExists(dir) &&
        DirectoryExists(dir + L"\\button") &&
        DirectoryExists(dir + L"\\checkbox") &&
        DirectoryExists(dir + L"\\listbox") &&
        DirectoryExists(dir + L"\\grid") &&
        DirectoryExists(dir + L"\\menu") &&
        DirectoryExists(dir + L"\\panel") &&
        DirectoryExists(dir + L"\\scrollbar") &&
        DirectoryExists(dir + L"\\textfield") &&
        DirectoryExists(dir + L"\\toolbar") &&
        DirectoryExists(dir + L"\\windows")) {
        return TRUE;
    }
    
    return FALSE;
}
*/
