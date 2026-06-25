#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <unordered_map>

namespace Tilc::Gui
{
    constexpr const char* GRID_DATA_FILE_EXTENSION = "tilc";
    constexpr const int GRID_DEFAULT_COLUMN_COUNT = 10;
    constexpr const int GRID_DEFAULT_ROW_COUNT = 10;
    constexpr const int GRID_DEFAULT_COLUMN_WIDTH = 90;
    constexpr const int GRID_DEFAULT_LEFT_HEADER_WIDTH = 30;


    struct DECLSPEC TGridCell
    {
        Tilc::TExtString m_Value;
        SDL_Point m_Size;
        bool m_IsHeader;
        bool m_Selected;

        TGridCell(const Tilc::TExtString& value, SDL_Point size, bool isHeader = false, bool selected = false);
        virtual ~TGridCell();
    };

    /*
    class DECLSPEC TGrid : public TGuiControl
    {
    public:
        TGrid(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position,
            int columnCount = GRID_DEFAULT_COLUMN_COUNT, int rowCount = GRID_DEFAULT_ROW_COUNT,
            bool editable = true);
        TGrid(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position,
            int columnCount, int rowCount,
            bool editable,
            bool showLeftHeader, bool showTopHeader, bool entireRowSelect, bool entireColumnSelect,
            bool drawVerticalLines, bool drawHorizontalLines);
        virtual ~TGrid();

        // Returns CmzFont* out of the this->_cachedFonts attriubute. If fontName == "", the default font for grid
        // defined in current theme is returned. Else if fontName != "" and font does not exists in this->_cachedFonts
        // then it is created added tho the attribute and returned.
        CmzFont* getFont(const WCHAR* fontName = L"", int fontSize = 0, int fontColor = 0);
        void cleanCachedFonts();
        void cleanData();

        // Umieszcza focus na tej kontrolce a dokładnie:
        //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
        //      ParentWnd->setActiveControl(this) tego okna.
        //      Przełącza stan kontrolki w tryb CGRID_STATE_FOCUSED.
        virtual void focus();
        // Usuwa focus z tej kontrolki a dokładnie:
        //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
        //      Przełącza kontrolkę w stan CGRID_STATE_NORMAL
        //      w zależności od aktualnego położenia myszy.
        virtual void looseFocus();
        // funkcja obsługi kliknięcia
    //    virtual void onClick();

        // Funkcje obsługi zdarzeń
        virtual void onDraw(CmzBitmap* dest);

        // Funkcje obsługi kliknięć
        virtual bool onMouseMove(int x, int y);
        virtual bool onMouseDown(int x, int y);
        virtual bool onMouseUp(int x, int y);

        virtual bool onKeyDown(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch);
        virtual bool onKeyPressed(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch);
        virtual bool onKeyUp(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch);
        virtual void onThumbChange(int oldPosition, int curPosition, CScrollbar* scrollbar);

        // Move active cell to the next cell to the right and returns TRUE if active cell was changed.
        bool moveRight(int count = 1);
        // Move active cell to the next cell to the left and returns TRUE if active cell was changed.
        bool moveLeft(int count = 1);
        // Move active cell to the next cell to the up and returns TRUE if active cell was changed.
        bool moveUp(int count = 1);
        // Move active cell to the next cell to the down and returns TRUE if active cell was changed.
        bool moveDown(int count = 1);
        bool movePageUp();
        bool movePageDown();
        bool movePageLeft();
        bool movePageRight();
        bool moveHome();
        bool moveHomeVertical();
        bool moveHomeHorizontal();
        bool moveEnd();
        bool moveEndVertical();
        bool moveEndHorizontal();
        bool moveToCell(int coordX, int coordY);
        void setStartCellCoordX(int coordX);
        void setStartCellCoordY(int coordX);
        // Gets coords of the cell under specified mouse coordinates. Returns TRUE if cell was found.
        bool getCellCoordsAtMousePos(int mouseX, int mouseY, LONG* coordX, LONG* coordY);
        // Move active cell to mouse-clicked cell returns TRUE if active cell was changed.
        bool activateCellAtMousePos(int x, int y);
        // Usuwa zaznaczenie (bez usunięcia zawartości komórek, które były zaznaczone)
        // i ewentualnie odrysowuje kontrolkę
        void clearSelection(bool redraw = TRUE);
        bool isSelection();

        // Returns total summary width of all columns
        Uint getTotalColumnsWidth();
        // Returns total summary height of all rows
        Uint getTotalRowsHeight();
        // Returns number of visible columns
        int getVisibleColumnCount();
        // Returns number of fully visible columns (if there is column partialy visible then it is not counted)
        int getFullVisibleColumnCount();
        // Returns number of visible rows
        int getVisibleRowCount();
        // Returns number of fully visible rows (if there is row partialy visible then it is not counted)
        int getFullVisibleRowCount();

        virtual void onEditorShow();
        virtual mzstd::CmzStdObject getValue();
        virtual void setValue(mzstd::CmzString value, bool redraw = TRUE);
        void setCellValue(int cellCoordX, int cellCoordY, mzstd::CmzString value, bool redraw = TRUE);
        void setCurrentCellValue(mzstd::CmzString value, bool redraw = TRUE);
        void clearCellValue(int cellCoordX, int cellCoordY, bool redraw = TRUE);
        void clearCurrentCellValue(bool redraw = TRUE);
        virtual void onApplyEditorChanges(mzstd::CmzStdObject* value);

        CGridCell* getCell(int cellCoordX, int cellCoordY);
        CGridCell* getCurrentCell();
        CGridHeaderCell* getHeaderCellX(int coordX);
        CGridHeaderCell* getCurrentHeaderCellX();
        CGridHeaderCell* getHeaderCellY(int coordY);
        CGridHeaderCell* getCurrentHeaderCellY();

        // Zwraca wymiary podane komórki grida.
        SIZE getCellSize(int cellCoordX, int cellCoordY);
        // Zwraca współrzędne lewego górnego rogu komórki grida o podany współrzędnych logicznych.
        // Współrzędne te są współrzędnymi (x, y) bitmapy na której grid jest rysowany.
        void cellCoordsToCanvasCoords(int cellCoordX, int cellCoordY, LONG* x, LONG* y);
        bool cellVisible(int coordX, int coordY, bool acceptPartialVisibility = FALSE);

        void showTopHeader(bool showTopHeader, bool redraw = FALSE);
        void showLeftHeader(bool showTopHeader, bool redraw = FALSE);
        void setGridSize(int columnCount, int rowCount);

        inline void setEntireRowSelect(bool value) { this->_entireRowSelect = value; }
        inline void setEntireColumnSelect(bool value) { this->_entireColumnSelect = value; }
        inline void setDrawVerticalLines(bool value) { this->_drawVerticalLines = value; }
        inline void setDrawHorizontalLines(bool value) { this->_drawHorizontalLines = value; }

        void setLeftHeaderCaptions(mzstd::CmzStringList* captions);
        void setTopHeaderCaptions(mzstd::CmzStringList* captions);

        inline void setLeftHeaderWidth(int value) { this->_leftHeaderWidth = value; }
        inline mzstd::CmzMap* getData() { return &this->_data; }

        bool saveToFile(mzstd::CmzString fname);
        bool loadFromFile(mzstd::CmzString fname);

    protected:
        // Dane grida przechowywane są jako pary std::pair(int, TGridCell). gdzie int definiuje
        // współrzędne w gridzie:
        //    first:
        //      HiWord(int) = cellYCoord
        //      LoWord(int) = cellCoordX
        //    second:
        //      TGridCell obiekt komórki (wraz z zawartością).
        // Zatem przy aktualnej implementacji grid obsługuje max. siatkę o polach 1..65536 w pionie i poziomie.
        std::unordered_map<int, TGridCell> m_Data;
        // Przechowuje wskaźnik na listę obrazków z której pobierane są obrazki, tgo wskaźnika nie
        // zwalniamy
//        CmzImageList* _imageList;
        // if top header is visible
        bool m_ShowTopHeader;
        // if left header is visible
        bool m_ShowLeftHeader;
        bool m_DrawHorizontalLines;
        bool m_DrawVerticalLines;
        // do entire row selected (not just only active cell)
        bool m_EntireRowSelect;
        // do entire column selected (not just only active cell)
        bool m_EntireColumnSelect;

        // width of grid's canvas which contain cells with data (not headers and scrollbars)
        int m_DataCanvasWidth;
        // height of grid's canvas which contain cells with data (not headers and scrollbars)
        int m_DataCanvasHeight;

        int m_LeftHeaderWidth;
        int m_TopHeaderHeight;
        int m_DefColumnWidth;
        int m_DefRowHeight;
        // maksymalne dopuszczalne wartości dla indeksów kolumny i wiersza
        int m_MaxColumnNumber;
        int m_MaxRowNumber;
        // indeksy maksymalnej widocznej w całosci kolumny i wiersza
        int m_MaxFullVisibleColumnNumber;
        int m_MaxFullVisibleRowNumber;
        // indeksy maksymalnej widocznej przynajmniej częściowo kolumny i wiersza
        int m_MaxVisibleColumnNumber;
        int m_MaxVisibleRowNumber;
        // width in pixels of the last column's visible part
        int m_VisibleWidthOfLastColumn;
        // width in pixels of the last row's visible part
        int m_VisibleHeightOfLastRow;

        int m_XPageStep;
        int m_YPageStep;

        // Współrzędna od której zaczyna się indeks nagłówka górnego, liczona od 1. Wyświetlana
        // jest natomiast litera. Literze 'A' odpowiada indeks 1, literze 'B' indeks 2 itd.
        // Wyświetlane są tylko litery A-Z a po ich wyczerpaniu AA-ZZ.
        int m_StartCellCoordX;
        // Współrzędna od której zaczyna się indeks nagłówka lewego, liczony od 1. Wyświetlany
        // jest numer tej współrzednej.
        int m_StartCellCoordY;
        // max allowable value for this->_startCellCoordX
        int m_MaxAllowableStartCellCoordX;
        // max allowable value for this->_startCellCoordY
        int m_MaxAllowableStartCellCoordY;
        // Współrzędna X (1.._maxColumnNumber NOT a coord on Canvas) aktywnej komórki.
        int m_CoordX;
        // Współrzędna Y (1.._maxRowNumber NOT a coord on Canvas) aktywnej komórki.
        int m_CoordY;

        virtual void CommonInit(int columnCount, int rowCount, bool editable, bool showLeftHeader, bool showTopHeader, bool entireRowSelect, bool entireColumnSelect, bool drawVerticalLines, bool drawHorizontalLines);
        // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
        virtual bool _commonKeyProcessing(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch, bool& redraw);

        // Funkcja zwraca string literowy odpowiadający podanemu indeksowi. Np. dla indeks=1 zwracane
        // jest 'A', dla 2 => 'B', itd.
        mzstd::CmzString _indexToLetter(int index);

        // Zwraca LONGa przechowującego podane współrzędne komórki grida. Kodowanie współrzędnych
        // określone jest przy opisie danych grida.
        inline int _cellCoordsToLONG(int x, int y);
        inline void _LONGToCellCoords(int data, LONG* x, LONG* y);

        // Ustawia indeksy maksymalnych widocznych w całości/częściowo kolumn i wierszy.
        void _updateGridParameters();
        void _updateScrollbars();
        void _cleanGridContent();
        void _setHeaderCaptions(mzstd::CmzArrayList* headerCells, mzstd::CmzStringList* lst);
        // if size of grid changed, then data for cells that are outside grid col/row dimensions are deleted.
        void _compactData();

    private:
        virtual void _updateCanvas();

    };
    */
}
