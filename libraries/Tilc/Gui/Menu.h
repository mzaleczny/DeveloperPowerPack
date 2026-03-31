#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Gui/GuiControl.h"
#include <vector>
#include <SDL3/SDL.h>

namespace Tilc::Audio
{
	class TMusic;
}
namespace Tilc::Graphics
{
	class TTextRenderer;
}

namespace Tilc::Gui
{

	class TFont;

	class DECLSPEC TMenuItem
	{
	public:
		TMenuItem(const char* Caption, bool Available, bool DrawIcon = true);
		~TMenuItem() {};
		Tilc::TExtString m_Caption;
		bool m_Available;
		bool m_DrawIcon;
	};

	class DECLSPEC TMenu : public TGuiControl
	{
	public:
		TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, TFont* MenuFont, float SizeSelected);
		TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, TFont* MenuFont, bool OpenGLVersion, int SurfaceWidth, int SurfaceHeight);
		TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const char *MenuResourceName, const char* FontResourceName, float Size, float SizeSelected = -1);
		~TMenu();
		void AddItem(const char* MenuItem, bool Available = true, bool DrawIcon = false);
		void SetItem(int AtPos, const char* MenuItem, bool Available = true, bool DrawIcon = false);
		inline void SetItemAvailable(int AtPos, bool Available)
        {
            m_MenuItems[AtPos].m_Available = Available;
        };
		inline void SetItemState(int AtPos, bool Available, bool DrawIcon)
        {
            m_MenuItems[AtPos].m_Available = Available;
            m_MenuItems[AtPos].m_DrawIcon = DrawIcon;
        };
		void InsertItem(int AtPos, const char* MenuItem, bool Available = true, bool DrawIcon = false);
		inline void SetItemDrawIcon(int AtPos, bool DrawIcon)
        {
            m_MenuItems[AtPos].m_DrawIcon = DrawIcon;
        };
        inline void Render();
		inline void Draw() { Draw(m_PosX, m_PosY); }
		void Draw(float x, float y, bool CenterHorizontally = false, bool CenterVertically = false);
		inline void SelectNextItem()
        {
            m_SelectedIndex = (m_SelectedIndex + 1) % m_MenuItems.size();
        }
		inline void SelectPrevItem()
        {
            m_SelectedIndex = (m_SelectedIndex + static_cast<int>(m_MenuItems.size()) - 1) % static_cast<int>(m_MenuItems.size());
        }
		inline void SetPositionX(float x)
        {
            m_PosX = x;
            m_NeedRegenerateSurface = false;
        }
		inline void SetPositionY(float y)
        {
            m_PosY = y;
            m_NeedRegenerateSurface = false;
        }
		inline void SetPosition(float x, float y)
        {
            m_PosX = x;
            m_PosY = y;
            m_NeedRegenerateSurface = false;
        }
		inline void GetPosition(float* x, float* y) const
        {
            *x = m_PosX;
            *y = m_PosY;
        }
		inline float GetPositionX() const { return m_PosX; }
		inline float GetPositionY() const { return m_PosY; }
		inline void SetAlpha(int Alpha) { m_Alpha = Alpha; }
		inline int GetAlpha() const { return m_Alpha; }
		inline size_t GetItemsCount() const { return m_MenuItems.size(); }
		inline void MoveByDeltaX(float DeltaX) { m_PosX += DeltaX; }
		inline void SetColor(int r, int g, int b, int a)
        {
            m_Color.r = r;
            m_Color.g = g;
            m_Color.b = b;
            m_Color.a = a;
        }
		inline void SetColorSelected(int r, int g, int b, int a)
        {
            m_ColorSelected.r = r;
            m_ColorSelected.g = g;
            m_ColorSelected.b = b;
            m_ColorSelected.a = a;
        }
		inline int GetSelectedIndex() const { return m_SelectedIndex; }
		inline void SetSelectedIndex(int Index) { m_SelectedIndex = Index; }
		int GetMenuItemIndexAtPoint(float x, float y);
		TMenuItem& GetMenuItem(int Index) { return m_MenuItems[Index]; };
		SDL_FRect GetBoundingBox();
		TFont* GetFont() const { return m_Font; }

        virtual bool OnMouseMove(const SDL_Event& Event) override;
		virtual bool OnMouseButtonDown(const SDL_Event& Event) override;
		//virtual bool OnMouseButtonUp(const SDL_Event& Event) override;
		virtual bool OnKeyDown(const SDL_Event& Event);

        void SetVSpacing(int Value) { m_VSpacing = Value; }
        TMenu* GetParentMenu() const { return m_ParentMenu; }

        static int MenuCounter;
	private:
        TMenu* m_ParentMenu{ nullptr };
        SDL_Texture* m_MenuBackground{ nullptr };
        SDL_Texture* m_MenuEnabledImage{ nullptr };
        SDL_Texture* m_MenuDisabledImage{ nullptr };
		SDL_FRect m_MenuEnabledImageRect;
		SDL_FRect m_MenuDisabledImageRect;
        Tilc::Audio::TMusic* m_WavMenuActivated{ nullptr };
        Tilc::Audio::TMusic* m_WavMenuSelected{ nullptr };
        bool HasBackground{ false };
		std::vector<TMenuItem> m_MenuItems;
        TFont* m_Font{ nullptr };
		SDL_Color m_Color { 255, 255, 255, 0 };
		SDL_Color m_ColorSelected { 255, 255, 0, 0 };
		// Szerokość menu na potrzeby centrowania, uaktualniana po każdorazowym dodaniu nowej opcji. Jeśli ta nowa opcja jes szersza niż bieżąca szerokość menu, to szerokość menu jest
		// ustawiana na tą nową większą wartość.
        int m_MenuWidth{ 0 };
		// Wysokość menu na potrzeby centrowania, uaktualniana po każdorazowym dodaniu nowej opcji. Dodawana jest wysokość nowej opcji z uwzględnieniem atrybutu VSpacing.
        int m_MenuHeight{ 0 };
		// Wysokość najwyższej z dotychczas dodanych opcji. Uaktualniana po każdorazowym dodaniu nowej opcji.
        int m_ItemHeight{ 0 };
		// Dodatkowy oddstęp w pionie pomiędzy dwiema sąsiadującymi opcjami.
        int m_VSpacing{ 20 };
		// Index aktualnie wybranej i podświetlonej opcji 0 - pierwsza opcja.
        int m_SelectedIndex{ 0 };
		// Współrzędne menu brane pod uwagę przy rysowaniu menu za pomocą funkcji draw użytej bez parametrów x i y. Są one aktualizowane przy każdym wywołaniu metody Draw(x, y, ...)
		// Umożliwiają wraz z powyższymi zmiennym rozpoznanie czy dany punkt należy do menu i jesli tak - którą pozycję wskazuje.
		float m_PosX;
		float m_PosY;
		int m_Alpha; // 0%-100%, 100% - pelna przezroczystosc
        Tilc::Graphics::TTextRenderer* m_TextContentForOpenGL{ nullptr };
        bool m_NeedRegenerateSurface{ false };
	};
}
