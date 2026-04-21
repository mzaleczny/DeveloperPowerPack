#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"


#define TLABEL_BORDER_TYPE_NONE                 0L
#define TLABEL_BORDER_TYPE_SOLID                1L
#define TLABEL_BORDER_TYPE_BEVEL_RAISEDINNER    2L
#define TLABEL_BORDER_TYPE_BEVEL_SUNKENINNER    3L
#define TLABEL_BORDER_TYPE_BEVEL_RAISEDOUTER    4L
#define TLABEL_BORDER_TYPE_BEVEL_SUNKENOUTER    5L
#define TLABEL_BORDER_TYPE_BEVEL_BUMP           6L
#define TLABEL_BORDER_TYPE_BEVEL_ETCHED         7L
#define TLABEL_BORDER_TYPE_BEVEL_RAISED         8L
#define TLABEL_BORDER_TYPE_BEVEL_SUNKEN         9L


namespace Tilc::Gui {

    class DECLSPEC TLabel : public TGuiControl
    {
    public:
        // LineHeight = 0 menas default LineHeight
        TLabel(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, Tilc::TExtString text = "", bool autosize = true, int LineHeight = 0);
        virtual ~TLabel();

        virtual void Draw() override;
        //virtual bool Update(float DeltaTime) override;

        void ResizeLabelIfAutoSize();
        inline void SetText(TExtString& text)
        {
            m_Text = text;
            ResizeLabelIfAutoSize();
            Invalidate();
        }
        TExtString GetText() const { return m_Text; }
        inline void SetTransparentBg(bool SetTransparent = true)
        {
            m_IsTransparentBg = SetTransparent;
            Invalidate();
        }
        inline void SetColor(SDL_Color color)
        {
            m_Color = color;
            Invalidate();
        }
        inline void SetBgColor(SDL_Color color)
        {
            m_BgColor = color;
            Invalidate();
        }
        inline void SetBorderColor(SDL_Color color)
        {
            m_BorderColor = color;
            Invalidate();
        }
        inline void SetBorder(EControlBorderType BorderType)
        {
            m_BorderType = BorderType;
        }
        void SetPadding(unsigned int padding);
        void SetPadding(unsigned int paddingLeftRight, unsigned int paddingTopBottom);
        inline void SetFontToUse(const TExtString& FontNameToUse)
        {
            m_FontNameToUse = FontNameToUse;
            ResizeLabelIfAutoSize();
            Invalidate();
        }
        void SetWrap(bool DoWrap)
        {
            m_Wrap = DoWrap;
            Invalidate();
        }
    protected:
        SDL_Color m_BgColor{};
        int m_LineHeight{};
        EControlBorderType m_BorderType{};
        SDL_Color m_Color{};
        SDL_Color m_BorderColor{};
        TExtString m_Text;
        TExtString m_FontNameToUse{ "Default" };
        // Paddingi od krawędzi
        unsigned int m_PaddingLeft{};
        unsigned int m_PaddingRight{};
        unsigned int m_PaddingTop{};
        unsigned int m_PaddingBottom{};
        // Szerokość i wysokość labelki niezbędna do wyświeltenia wszystkich potencjalnych
        // linii ustawionego tekstu. Wartości te są ustawiane przy każdorazowym ustawianiu tekstu.
        int m_BaseWidth;
        int m_BaseHeight;
        bool m_IsTransparentBg{ true };
        bool m_AutoSize{ true };
        bool m_Wrap{};
    };

}
