#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Globals.h"
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

        TGridCell() = default;
        TGridCell(const Tilc::TExtString& value, SDL_Point size, bool isHeader = false, bool selected = false);
        virtual ~TGridCell();
    };
    using TCells = std::unordered_map<int, TGridCell>;
    using TCellData = std::pair<int, TGridCell>;

    class TScrollBar;

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
        void CleanData();

        virtual void Draw();

        // Funkcje obsługi zdarzeń
        virtual bool OnMouseButtonDown(const SDL_Event& event) override;
        virtual bool OnKeyDown(const SDL_Event& event) override;
        virtual void OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar);

        // Move active cell to the next cell to the right and returns TRUE if active cell was changed.
        bool MoveRight(int count = 1);
        // Move active cell to the next cell to the left and returns TRUE if active cell was changed.
        bool MoveLeft(int count = 1);
        // Move active cell to the next cell to the up and returns TRUE if active cell was changed.
        bool MoveUp(int count = 1);
        // Move active cell to the next cell to the down and returns TRUE if active cell was changed.
        bool MoveDown(int count = 1);
        bool MovePageUp();
        bool MovePageDown();
        bool MovePageLeft();
        bool MovePageRight();
        bool MoveHome();
        bool MoveHomeVertical();
        bool MoveHomeHorizontal();
        bool MoveEnd();
        bool MoveEndVertical();
        bool MoveEndHorizontal();
        bool MoveToCell(int coordX, int coordY);
        void SetStartCellCoordX(int coordX);
        void SetStartCellCoordY(int coordX);
        // Gets coords of the cell under specified mouse coordinates. Returns TRUE if cell was found.
        bool GetCellCoordsAtMousePos(int mouseX, int mouseY, int* coordX, int* coordY);
        // Move active cell to mouse-clicked cell returns TRUE if active cell was changed.
        bool ActivateCellAtMousePos(int x, int y);
        // Usuwa zaznaczenie (bez usunięcia zawartości komórek, które były zaznaczone)
        // i ewentualnie odrysowuje kontrolkę
        void ClearSelection(bool redraw = true);
        bool IsSelection();

        // Returns total summary width of all columns
        unsigned int GetTotalColumnsWidth();
        // Returns total summary height of all rows
        unsigned int GetTotalRowsHeight();
        // Returns number of visible columns
        int GetVisibleColumnCount();
        // Returns number of fully visible columns (if there is column partialy visible then it is not counted)
        int GetFullVisibleColumnCount();
        // Returns number of visible rows
        int GetVisibleRowCount();
        // Returns number of fully visible rows (if there is row partialy visible then it is not counted)
        int GetFullVisibleRowCount();

        virtual void OnEditorShow();
        virtual Tilc::TExtString GetValue();
        virtual void SetValue(const Tilc::TExtString& value, bool redraw = true);
        void SetCellValue(int cellCoordX, int cellCoordY, const Tilc::TExtString& value, bool redraw = true);
        void SetCurrentCellValue(const Tilc::TExtString& value, bool redraw = true);
        void ClearCellValue(int cellCoordX, int cellCoordY, bool redraw = true);
        void ClearCurrentCellValue(bool redraw = true);
        virtual void OnApplyEditorChanges(const Tilc::TExtString& value) override;

        TGridCell* GetCell(int cellCoordX, int cellCoordY);
        TGridCell* GetCurrentCell();
        TGridCell* GetHeaderCellX(int coordX);
        TGridCell* GetCurrentHeaderCellX();
        TGridCell* GetHeaderCellY(int coordY);
        TGridCell* GetCurrentHeaderCellY();

        // Zwraca wymiary podane komórki grida.
        SDL_Point GetCellSize(int cellCoordX, int cellCoordY);
        // Zwraca współrzędne lewego górnego rogu komórki grida o podany współrzędnych logicznych.
        // Współrzędne te są współrzędnymi (x, y) bitmapy na której grid jest rysowany.
        void CellCoordsToCanvasCoords(int cellCoordX, int cellCoordY, int* x, int* y);
        bool CellVisible(int coordX, int coordY, bool acceptPartialVisibility = false);

        void ShowTopHeader(bool showTopHeader, bool redraw = false);
        void ShowLeftHeader(bool showTopHeader, bool redraw = false);
        void SetGridSize(int columnCount, int rowCount);

        inline void SetEntireRowSelect(bool value) { m_EntireRowSelect = value; }
        inline void SetEntireColumnSelect(bool value) { m_EntireColumnSelect = value; }
        inline void SetDrawVerticalLines(bool value) { m_DrawVerticalLines = value; }
        inline void SetDrawHorizontalLines(bool value) { m_DrawHorizontalLines = value; }

        void SetLeftHeaderCaptions(TStringVector* captions);
        void SetTopHeaderCaptions(TStringVector* captions);

        inline void SetLeftHeaderWidth(int value) { m_LeftHeaderWidth = value; }
        inline TCells& GetData() { return m_Data; }

        bool SaveToFile(const Tilc::TExtString& fname);
        bool LoadFromFile(const Tilc::TExtString& fname);

    protected:
        // Dane grida przechowywane są jako pary std::pair(int, TGridCell). gdzie int definiuje
        // współrzędne w gridzie:
        //    first:
        //      HiWord(int) = cellYCoord
        //      LoWord(int) = cellCoordX
        //    second:
        //      TGridCell obiekt komórki (wraz z zawartością).
        // Zatem przy aktualnej implementacji grid obsługuje max. siatkę o polach 1..65536 w pionie i poziomie.
        TCells m_Data;
        // Array containing header objects (CGridHeaderCellData*) for the columns
        std::vector<TGridCell> m_ColData;
        // Array containing header objects (CGridHeaderCellData*) for the rows
        std::vector<TGridCell> m_RowData;

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

        // Funkcja zwraca string literowy odpowiadający podanemu indeksowi. Np. dla indeks=1 zwracane
        // jest 'A', dla 2 => 'B', itd.
        Tilc::TExtString IndexToLetter(int index);

        // Zwraca LONGa przechowującego podane współrzędne komórki grida. Kodowanie współrzędnych
        // określone jest przy opisie danych grida.
        int CellCoordsToInt(int x, int y);
        void IntToCellCoords(int data, int* x, int* y);

        // Ustawia indeksy maksymalnych widocznych w całości/częściowo kolumn i wierszy.
        void UpdateGridParameters();
        void UpdateScrollBars();
        void CleanGridContent();
        void SetHeaderCaptions(std::vector<TGridCell>* headerCells, TStringVector* captions);
        // if size of grid changed, then data for cells that are outside grid col/row dimensions are removed.
        void CompactData();
    };

}
