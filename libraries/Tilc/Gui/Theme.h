#pragma once

#include "Tilc/Utils/ExtString.h"
#include "SDL3/SDL.h"
#include <fstream>
#include <sstream>

namespace Tilc {
    namespace Gui {

        class TFont;

        class TTheme
        {
        public:
            TTheme(Tilc::TExtString name);
            virtual ~TTheme();
            Tilc::TExtString GetThemeName() const { return m_ThemeName; }
            Tilc::TExtString GetThemeDir() const { return m_ThemeDir; }

            // TextureMap of all possible controls in all possible states inside window
            SDL_Texture* GuiTextureMap1{};
            SDL_Texture* GuiTextureMap1_TMP{};

            SDL_FRect wnd_caption_left_rc;
            SDL_FRect wnd_caption_middle_rc;
            SDL_FRect wnd_caption_right_rc;
            SDL_FRect wnd_caption_inactive_left_rc;
            SDL_FRect wnd_caption_inactive_middle_rc;
            SDL_FRect wnd_caption_inactive_right_rc;
            SDL_FRect wnd_frame_left_rc;
            SDL_FRect wnd_frame_right_rc;
            SDL_FRect wnd_frame_bottom_rc;
            SDL_FRect wnd_frame_bottom_left_rc;
            SDL_FRect wnd_frame_bottom_right_rc;
            SDL_FRect wnd_frame_inactive_left_rc;
            SDL_FRect wnd_frame_inactive_right_rc;
            SDL_FRect wnd_frame_inactive_bottom_rc;
            SDL_FRect wnd_frame_inactive_bottom_left_rc;
            SDL_FRect wnd_frame_inactive_bottom_right_rc;
            SDL_FRect wnd_leftmenu_button_rc;
            SDL_FRect wnd_minimize_button_rc;
            SDL_FRect wnd_minimize_button_active_rc;
            SDL_FRect wnd_minimize_button_hover_rc;
            SDL_FRect wnd_maximize_button_rc;
            SDL_FRect wnd_maximize_button_active_rc;
            SDL_FRect wnd_maximize_button_hover_rc;
            SDL_FRect wnd_restore_button_rc;
            SDL_FRect wnd_restore_button_active_rc;
            SDL_FRect wnd_restore_button_hover_rc;
            SDL_FRect wnd_close_button_rc;
            SDL_FRect wnd_close_button_active_rc;
            SDL_FRect wnd_close_button_hover_rc;

            /*
            // Bitmapy panelu
            SDL_FRect panel_caption_left_rc;
            SDL_FRect panel_caption_middle_rc;
            SDL_FRect panel_caption_right_rc;
            SDL_FRect panel_nocaption_left_rc;
            SDL_FRect panel_nocaption_middle_rc;
            SDL_FRect panel_nocaption_right_rc;
            SDL_FRect panel_frame_left_rc;
            SDL_FRect panel_frame_right_rc;
            SDL_FRect panel_frame_bottom_rc;
            SDL_FRect panel_frame_bottom_left_rc;
            SDL_FRect panel_frame_bottom_right_rc;
            SDL_FRect panel_inner_bg_rc;
            SDL_FRect panel_inner_bg_fullscreen_rc;

            // Bitmapy menu
            SDL_FRect menu_bg_left_rc;
            SDL_FRect menu_bg_middle_rc;
            SDL_FRect menu_bg_right_rc;
            SDL_FRect menu_selected_mainmenu_item_bg_left_rc;
            SDL_FRect menu_selected_mainmenu_item_bg_middle_rc;
            SDL_FRect menu_selected_mainmenu_item_bg_right_rc;
            SDL_FRect menu_opened_mainmenu_item_bg_left_rc;
            SDL_FRect menu_opened_mainmenu_item_bg_middle_rc;
            SDL_FRect menu_opened_mainmenu_item_bg_right_rc;
            SDL_FRect menu_shadow_bottom_left_rc;
            SDL_FRect menu_shadow_bottom_middle_rc;
            SDL_FRect menu_shadow_bottom_right_rc;
            SDL_FRect menu_shadow_right_middle_rc;
            SDL_FRect menu_shadow_right_top_rc;

            // Bitmapy popupmenu
            SDL_FRect popupmenu_bg_rc;
            SDL_FRect popupmenu_frame_bottom_rc;
            SDL_FRect popupmenu_frame_bottom_left_rc;
            SDL_FRect popupmenu_frame_bottom_right_rc;
            SDL_FRect popupmenu_frame_left_rc;
            SDL_FRect popupmenu_frame_left_top_rc;
            SDL_FRect popupmenu_frame_right_rc;
            SDL_FRect popupmenu_frame_top_rc;
            SDL_FRect popupmenu_frame_top_hilited_rc;
            SDL_FRect popupmenu_frame_top_hilited_end_rc;
            SDL_FRect popupmenu_frame_top_right_rc;
            SDL_FRect popupmenu_icon_area_left_rc;
            SDL_FRect popupmenu_submenu_arrow_rc;
            SDL_FRect popupmenu_frame_shadow_bottom_rc;
            SDL_FRect popupmenu_frame_shadow_bottom_left_rc;
            SDL_FRect popupmenu_frame_shadow_bottom_right_rc;
            SDL_FRect popupmenu_frame_shadow_right_rc;
            SDL_FRect popupmenu_frame_shadow_top_right_rc;
            SDL_FRect popupmenu_divider_rc;
            SDL_FRect popupmenu_selected_item_bg_left_rc;
            SDL_FRect popupmenu_selected_item_bg_middle_rc;
            SDL_FRect popupmenu_selected_item_bg_right_rc;
            SDL_FRect popupmenu_opened_item_bg_left_rc;
            SDL_FRect popupmenu_opened_item_bg_middle_rc;
            SDL_FRect popupmenu_opened_item_bg_right_rc;

            // Bitmapy toolbara przeznaczonego na ikony o rozmiarze 16x16
            SDL_FRect toolbar16_left_rc;
            SDL_FRect toolbar16_middle_rc;
            SDL_FRect toolbar16_right_rc;
            SDL_FRect toolbar16_button_hilited_rc;
            SDL_FRect toolbar16_button_pressed_rc;
            SDL_FRect toolbar16_separator_rc;
            */

            // Bitmapy scrollbara standardowego 16px
            SDL_FRect scrollbar_horizontal_arrow_left_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_disabled_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_hover_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_pushed_rc;

            SDL_FRect scrollbar_horizontal_arrow_right_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_disabled_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_hover_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_pushed_rc;

            SDL_FRect scrollbar_horizontal_thumb_left_rc;
            SDL_FRect scrollbar_horizontal_thumb_middle_rc;
            SDL_FRect scrollbar_horizontal_thumb_right_rc;
            SDL_FRect scrollbar_horizontal_thumb_bg_rc;

            SDL_FRect scrollbar_horizontal_bg_rc;
            SDL_FRect scrollbar_horizontal_bg_disabled_rc;
            SDL_FRect scrollbar_horizontal_bg_pushed_rc;

            SDL_FRect scrollbar_vertical_arrow_up_rc;
            SDL_FRect scrollbar_vertical_arrow_up_disabled_rc;
            SDL_FRect scrollbar_vertical_arrow_up_hover_rc;
            SDL_FRect scrollbar_vertical_arrow_up_pushed_rc;

            SDL_FRect scrollbar_vertical_arrow_down_rc;
            SDL_FRect scrollbar_vertical_arrow_down_disabled_rc;
            SDL_FRect scrollbar_vertical_arrow_down_hover_rc;
            SDL_FRect scrollbar_vertical_arrow_down_pushed_rc;

            SDL_FRect scrollbar_vertical_thumb_top_rc;
            SDL_FRect scrollbar_vertical_thumb_middle_rc;
            SDL_FRect scrollbar_vertical_thumb_bottom_rc;
            SDL_FRect scrollbar_vertical_thumb_bg_rc;

            SDL_FRect scrollbar_vertical_bg_rc;
            SDL_FRect scrollbar_vertical_bg_disabled_rc;
            SDL_FRect scrollbar_vertical_bg_pushed_rc;

            // Bitmapy scrollbara małego 10px - ten jest używany w kontrolkach różnych od okna, dla odróżnienia i nie przytłaczania kontrolki swoim rozmiarem
            SDL_FRect small_scrollbar_horizontal_arrow_left_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_left_disabled_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_left_hover_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_left_pushed_rc;

            SDL_FRect small_scrollbar_horizontal_arrow_right_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_right_disabled_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_right_hover_rc;
            SDL_FRect small_scrollbar_horizontal_arrow_right_pushed_rc;

            SDL_FRect small_scrollbar_horizontal_thumb_left_rc;
            SDL_FRect small_scrollbar_horizontal_thumb_middle_rc;
            SDL_FRect small_scrollbar_horizontal_thumb_right_rc;
            SDL_FRect small_scrollbar_horizontal_thumb_bg_rc;

            SDL_FRect small_scrollbar_horizontal_bg_rc;
            SDL_FRect small_scrollbar_horizontal_bg_disabled_rc;
            SDL_FRect small_scrollbar_horizontal_bg_pushed_rc;

            SDL_FRect small_scrollbar_vertical_arrow_up_rc;
            SDL_FRect small_scrollbar_vertical_arrow_up_disabled_rc;
            SDL_FRect small_scrollbar_vertical_arrow_up_hover_rc;
            SDL_FRect small_scrollbar_vertical_arrow_up_pushed_rc;

            SDL_FRect small_scrollbar_vertical_arrow_down_rc;
            SDL_FRect small_scrollbar_vertical_arrow_down_disabled_rc;
            SDL_FRect small_scrollbar_vertical_arrow_down_hover_rc;
            SDL_FRect small_scrollbar_vertical_arrow_down_pushed_rc;

            SDL_FRect small_scrollbar_vertical_thumb_top_rc;
            SDL_FRect small_scrollbar_vertical_thumb_middle_rc;
            SDL_FRect small_scrollbar_vertical_thumb_bottom_rc;
            SDL_FRect small_scrollbar_vertical_thumb_bg_rc;

            SDL_FRect small_scrollbar_vertical_bg_rc;
            SDL_FRect small_scrollbar_vertical_bg_disabled_rc;
            SDL_FRect small_scrollbar_vertical_bg_pushed_rc;


            // Bitmapy sliderów
            SDL_FRect slider_horizontal_thumb_rc;
            SDL_FRect slider_horizontal_rail_left_rc;
            SDL_FRect slider_horizontal_rail_middle_rc;
            SDL_FRect slider_horizontal_rail_right_rc;

            SDL_FRect slider_vertical_thumb_rc;
            SDL_FRect slider_vertical_rail_top_rc;
            SDL_FRect slider_vertical_rail_middle_rc;
            SDL_FRect slider_vertical_rail_bottom_rc;


            // Bitmapy textfield
            SDL_FRect textfield_left_rc;
            SDL_FRect textfield_right_rc;
            SDL_FRect textfield_middle_rc;

            SDL_FRect textfield_left_focused_rc;
            SDL_FRect textfield_right_focused_rc;
            SDL_FRect textfield_middle_focused_rc;

            SDL_FRect textfield_left_disabled_rc;
            SDL_FRect textfield_right_disabled_rc;
            SDL_FRect textfield_middle_disabled_rc;

            SDL_FRect textfield_left_hover_rc;
            SDL_FRect textfield_right_hover_rc;
            SDL_FRect textfield_middle_hover_rc;

            SDL_FRect textfield_left_hover_focused_rc;
            SDL_FRect textfield_right_hover_focused_rc;
            SDL_FRect textfield_middle_hover_focused_rc;


            SDL_FRect textfield_left_pushed_rc;
            SDL_FRect textfield_right_pushed_rc;
            SDL_FRect textfield_middle_pushed_rc;

            SDL_FRect textfield_left_pushed_focused_rc;
            SDL_FRect textfield_right_pushed_focused_rc;
            SDL_FRect textfield_middle_pushed_focused_rc;

            SDL_FRect textfield_selection_rc;


            // Bitmapy multiline_textfield
            SDL_FRect multiline_textfield_top_left_rc;
            SDL_FRect multiline_textfield_top_right_rc;
            SDL_FRect multiline_textfield_top_middle_rc;
            SDL_FRect multiline_textfield_inner_left_rc;
            SDL_FRect multiline_textfield_inner_right_rc;
            SDL_FRect multiline_textfield_bottom_left_rc;
            SDL_FRect multiline_textfield_bottom_right_rc;
            SDL_FRect multiline_textfield_bottom_middle_rc;

            SDL_FRect multiline_textfield_top_left_focused_rc;
            SDL_FRect multiline_textfield_top_right_focused_rc;
            SDL_FRect multiline_textfield_top_middle_focused_rc;
            SDL_FRect multiline_textfield_inner_left_focused_rc;
            SDL_FRect multiline_textfield_inner_right_focused_rc;
            SDL_FRect multiline_textfield_bottom_left_focused_rc;
            SDL_FRect multiline_textfield_bottom_right_focused_rc;
            SDL_FRect multiline_textfield_bottom_middle_focused_rc;

            SDL_FRect multiline_textfield_top_left_disabled_rc;
            SDL_FRect multiline_textfield_top_right_disabled_rc;
            SDL_FRect multiline_textfield_top_middle_disabled_rc;
            SDL_FRect multiline_textfield_inner_left_disabled_rc;
            SDL_FRect multiline_textfield_inner_right_disabled_rc;
            SDL_FRect multiline_textfield_bottom_left_disabled_rc;
            SDL_FRect multiline_textfield_bottom_right_disabled_rc;
            SDL_FRect multiline_textfield_bottom_middle_disabled_rc;

            SDL_FRect multiline_textfield_top_left_hover_rc;
            SDL_FRect multiline_textfield_top_right_hover_rc;
            SDL_FRect multiline_textfield_top_middle_hover_rc;
            SDL_FRect multiline_textfield_inner_left_hover_rc;
            SDL_FRect multiline_textfield_inner_right_hover_rc;
            SDL_FRect multiline_textfield_bottom_left_hover_rc;
            SDL_FRect multiline_textfield_bottom_right_hover_rc;
            SDL_FRect multiline_textfield_bottom_middle_hover_rc;

            SDL_FRect multiline_textfield_top_left_hover_focused_rc;
            SDL_FRect multiline_textfield_top_right_hover_focused_rc;
            SDL_FRect multiline_textfield_top_middle_hover_focused_rc;
            SDL_FRect multiline_textfield_inner_left_hover_focused_rc;
            SDL_FRect multiline_textfield_inner_right_hover_focused_rc;
            SDL_FRect multiline_textfield_bottom_left_hover_focused_rc;
            SDL_FRect multiline_textfield_bottom_right_hover_focused_rc;
            SDL_FRect multiline_textfield_bottom_middle_hover_focused_rc;

            SDL_FRect multiline_textfield_top_left_pushed_rc;
            SDL_FRect multiline_textfield_top_right_pushed_rc;
            SDL_FRect multiline_textfield_top_middle_pushed_rc;
            SDL_FRect multiline_textfield_inner_left_pushed_rc;
            SDL_FRect multiline_textfield_inner_right_pushed_rc;
            SDL_FRect multiline_textfield_bottom_left_pushed_rc;
            SDL_FRect multiline_textfield_bottom_right_pushed_rc;
            SDL_FRect multiline_textfield_bottom_middle_pushed_rc;

            SDL_FRect multiline_textfield_top_left_pushed_focused_rc;
            SDL_FRect multiline_textfield_top_right_pushed_focused_rc;
            SDL_FRect multiline_textfield_top_middle_pushed_focused_rc;
            SDL_FRect multiline_textfield_inner_left_pushed_focused_rc;
            SDL_FRect multiline_textfield_inner_right_pushed_focused_rc;
            SDL_FRect multiline_textfield_bottom_left_pushed_focused_rc;
            SDL_FRect multiline_textfield_bottom_right_pushed_focused_rc;
            SDL_FRect multiline_textfield_bottom_middle_pushed_focused_rc;

            SDL_Color multiline_textfield_inner_bg{ 255, 255, 255, 255 };

            // Bitmapy button
            SDL_FRect button_left_rc;
            SDL_FRect button_right_rc;
            SDL_FRect button_middle_rc;

            SDL_FRect button_left_focused_rc;
            SDL_FRect button_right_focused_rc;
            SDL_FRect button_middle_focused_rc;

            SDL_FRect button_left_disabled_rc;
            SDL_FRect button_right_disabled_rc;
            SDL_FRect button_middle_disabled_rc;

            SDL_FRect button_left_hover_rc;
            SDL_FRect button_right_hover_rc;
            SDL_FRect button_middle_hover_rc;
            SDL_FRect button_left_hover_focused_rc;
            SDL_FRect button_right_hover_focused_rc;
            SDL_FRect button_middle_hover_focused_rc;

            SDL_FRect button_left_pushed_rc;
            SDL_FRect button_right_pushed_rc;
            SDL_FRect button_middle_pushed_rc;
            SDL_FRect button_left_pushed_focused_rc;
            SDL_FRect button_right_pushed_focused_rc;
            SDL_FRect button_middle_pushed_focused_rc;


            // checkbox
            SDL_FRect checkbox_rc;
            SDL_FRect checkbox_disabled_rc;
            SDL_FRect checkbox_focused_rc;
            SDL_FRect checkbox_hover_rc;
            SDL_FRect checkbox_checked_rc;
            SDL_FRect checkbox_checked_disabled_rc;
            SDL_FRect checkbox_checked_focused_rc;
            SDL_FRect checkbox_checked_hover_rc;


            // option
            SDL_FRect option_rc;
            SDL_FRect option_disabled_rc;
            SDL_FRect option_focused_rc;
            SDL_FRect option_hover_rc;
            SDL_FRect option_checked_rc;
            SDL_FRect option_checked_disabled_rc;
            SDL_FRect option_checked_focused_rc;
            SDL_FRect option_checked_hover_rc;

            // listbox bitmaps
            SDL_FRect listbox_frame_top_left_rc;
            SDL_FRect listbox_frame_top_rc;
            SDL_FRect listbox_frame_top_right_rc;
            SDL_FRect listbox_frame_bottom_left_rc;
             SDL_FRect listbox_frame_bottom_rc;
            SDL_FRect listbox_frame_bottom_right_rc;
            SDL_FRect listbox_frame_left_rc;
            SDL_FRect listbox_frame_right_rc;
            SDL_FRect listbox_bg_rc;
            SDL_FRect listbox_bg_selected_rc;
            // fill colors for use by SDL_RenderFilledRect, because SDL_RenderTextureTiled cause much much much memory use from one pixel source rect and large destination rect
            // no idea why
            SDL_Color listbox_bg{0x52, 0x90, 0xfb, 0xff};
            SDL_Color listbox_bg_selected{0xb0, 0xcd, 0xfe, 0xff};

            // Grid
            SDL_FRect grid_cell_inner_bg_normal_rc;
            SDL_FRect grid_cell_inner_bg_selected_rc;
            SDL_FRect grid_cell_inner_bg_selected_active_rc;
            SDL_FRect grid_left_header_inner_bg_normal_cell_rc;
            SDL_FRect grid_left_header_inner_bg_selected_cell_rc;
            SDL_FRect grid_left_top_header_inner_bg_rc;
            SDL_FRect grid_left_top_header_mark_rc;
            SDL_FRect grid_top_header_inner_bg_normal_cell_rc;
            SDL_FRect grid_top_header_inner_bg_selected_cell_rc;

            SDL_Color commonGridControlTopHeaderNormalFontColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlTopHeaderSelectedFontColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlLeftHeaderNormalFontColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlLeftHeaderSelectedFontColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlTopHeaderCellBorderColor_Normal{ 0, 0, 0, 255 };
            SDL_Color commonGridControlTopHeaderCellBorderColor_Selected{ 0, 0, 0, 255 };
            SDL_Color commonGridControlLeftTopHeaderCellBorderColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlLeftHeaderCellBorderColor_Normal{ 0, 0, 0, 255 };
            SDL_Color commonGridControlLeftHeaderCellBorderColor_Selected{ 0, 0, 0, 255 };
            SDL_Color commonGridControlCellBorderColor_Normal{ 0, 0, 0, 255 };
            SDL_Color commonGridControlCellBorderColor_Selected{ 0, 0, 0, 255 };
            SDL_Color commonGridControlCellBorderColor_Active{ 0, 0, 0, 255 };
            SDL_Color commonGridControlSelectionBorderColor{ 0, 0, 0, 255 };
            SDL_Color commonGridControlFocusedGridBorderColor{ 0, 0, 0, 255 };

            /*
            // Dostępne fonty
            CFont* globalStandardFont;
            CFont* windowCaptionFont;
            CFont* captionFont;
            CFont* menuFont;
            CFont* commonLabelFont;
            CFont* commonTextControlFont;
            CFont* commonButtonControlFont;
            CFont* commonGridControlFont;
            CFont* commonCheckboxControlFont;
            CFont* commonListboxControlFont;
            */

            // kolory fontu this->commonTextControlFont wyświetlonego w trybie normalnym i zaznaczonym
            SDL_Color commonTextControlNormalFontColor;
            SDL_Color commonTextControlSelectedFontColor;

            TFont* DefaultFont;

            void Load(Tilc::TExtString name = "");
            void LoadFonts();
            void Unload();

            void LoadWindowSkinResources(Tilc::TExtString themeName);
            void LoadPanelSkinResources(Tilc::TExtString themeName);
            void LoadMenuSkinResources(Tilc::TExtString themeName);
            void LoadPopupmenuSkinResources(Tilc::TExtString themeName);
            void LoadToolbar16SkinResources(Tilc::TExtString themeName);
            void LoadLabelSkinResources(Tilc::TExtString themeName);
            void LoadScrollBarSkinResources(Tilc::TExtString themeName, Tilc::TExtString Postfix = "");
            void LoadSliderSkinResources(Tilc::TExtString themeName);
            void LoadTextFieldSkinResources(Tilc::TExtString themeName);
            void LoadMultilineTextFieldSkinResources(Tilc::TExtString themeName);
            void LoadButtonSkinResources(Tilc::TExtString themeName);
            void LoadCheckboxSkinResources(Tilc::TExtString themeName);
            void LoadOptionSkinResources(Tilc::TExtString themeName);
            void LoadListboxSkinResources(Tilc::TExtString themeName);
            void LoadGridSkinResources(Tilc::TExtString themeName);
        protected:
            Tilc::TExtString m_ThemeName;
            Tilc::TExtString m_ThemeDir;
            SDL_Renderer* m_Renderer;

        private:
            // auxiliary variable storing layout data, for the puropse of reading all rectangles of all subtextures. The variable is cleared after initialization is
            // complete.
            std::ifstream LayoutInputStream;
            std::stringstream m_Data;
            /*
            void initAttributes();
            SDL_Color getIniColorValue(Tilc::TExtString settings_fname, WCHAR_T* ini_varname);
            */
            void CreateComplexRects(const SDL_FRect& LeftRc, const SDL_FRect& MiddleRc, const SDL_FRect& RightRc,
                SDL_FRect& TopLeftRc, SDL_FRect& TopMiddleRc, SDL_FRect& TopRightRc,
                SDL_FRect& InnerLeftRc, SDL_FRect& InnerRightRc,
                SDL_FRect& BottomLeftRc, SDL_FRect& BottomMiddleRc, SDL_FRect& BottomRightRc
            );
        };
    }
}
