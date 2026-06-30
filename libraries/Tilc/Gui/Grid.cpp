#include "Tilc/Gui/Grid.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/Scrollbar.h"
#include "Tilc/Gui/ScrollbarVertical.h"
#include "Tilc/Gui/ScrollbarHorizontal.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/StyledWIndow.h"
#include "Tilc/Utils/FileUtils.h"
/*
#include "mzImageList.h"
#include "mzMessages.h"
#include "mzStdDialogs.h"
*/
#include <ranges>

Tilc::Gui::TGridCell::TGridCell(const Tilc::TExtString& value, SDL_Point size, bool isHeader, bool selected)
    : m_Value(value), m_Size(size), m_IsHeader(isHeader), m_Selected(selected)
{
}

Tilc::Gui::TGridCell::~TGridCell()
{
}



Tilc::Gui::TGrid::TGrid(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int columnCount, int rowCount, bool editable)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Grid, editable)
{
    CommonInit(columnCount, rowCount, editable, true, true, false, false, true, true);
}

Tilc::Gui::TGrid::TGrid(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position,
                        int columnCount, int rowCount,
                        bool editable,
                        bool showLeftHeader, bool showTopHeader, bool entireRowSelect, bool entireColumnSelect,
                        bool drawVerticalLines, bool drawHorizontalLines)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Grid, editable)
{
    CommonInit(columnCount, rowCount, editable, showLeftHeader, showTopHeader, entireRowSelect, entireColumnSelect, drawVerticalLines, drawHorizontalLines);
}

void Tilc::Gui::TGrid::CommonInit(int columnCount, int rowCount, bool editable, bool showLeftHeader, bool showTopHeader, bool entireRowSelect, bool entireColumnSelect, bool drawVerticalLines, bool drawHorizontalLines)
{
//    m_ImageList = NULL;

    m_ShowTopHeader = showTopHeader;
    m_ShowLeftHeader = showLeftHeader;
    m_DrawVerticalLines = drawVerticalLines;
    m_DrawHorizontalLines = drawHorizontalLines;
    m_EntireRowSelect = entireRowSelect;
    m_EntireColumnSelect = entireColumnSelect;

    m_LeftHeaderWidth = Tilc::Gui::GRID_DEFAULT_LEFT_HEADER_WIDTH;
    m_DefColumnWidth = Tilc::Gui::GRID_DEFAULT_COLUMN_WIDTH;

    m_DefRowHeight = GetTheme()->grid_cell_inner_bg_normal_rc.h + 2;
    m_TopHeaderHeight = m_DefRowHeight;

    m_StartCellCoordX = 1;
    m_StartCellCoordY = 1;
    m_CoordX = 1;
    m_CoordY = 1;

    SetGridSize(columnCount, rowCount);

    //this->_backgroundColor = RGB(0xff, 0xff, 0xff);
    // Adding scrollbars and editor
    bool addVScr = static_cast<int>(GetTotalRowsHeight()) > m_Position.h;
    bool addHScr = static_cast<int>(GetTotalColumnsWidth()) > m_Position.w;
    AddScrollBars(addVScr, addHScr, 0, 1000, 0, 1000, false);
    Tilc::Gui::TScrollBar* scr = dynamic_cast<Tilc::Gui::TScrollBar*>(GetChild(m_Name + "_VSB"));
    if (scr)
    {
        (dynamic_cast<Tilc::Gui::TScrollBarVertical*>(scr))->SetSteps(50, 100);
    }
    scr = dynamic_cast<Tilc::Gui::TScrollBar*>(GetChild(m_Name + "_HSB"));
    if (scr)
    {
        (dynamic_cast<Tilc::Gui::TScrollBarHorizontal*>(scr))->SetSteps(50, 100);
    }
    m_TabStop = true;
    if (editable)
    {
        m_Editable = true;
        AddEditor();
    }

    UpdateGridParameters();

    if (m_VisibleWidthOfLastColumn < 4)
    {
        m_XPageStep = GetFullVisibleColumnCount();
    }
    else
    {
        m_XPageStep = GetVisibleColumnCount();
    }
    if (m_VisibleHeightOfLastRow < 4)
    {
        m_YPageStep = GetFullVisibleRowCount();
    }
    else
    {
        m_YPageStep = GetVisibleRowCount();
    }
}

Tilc::Gui::TGrid::~TGrid()
{
    CleanGridContent();
}

void Tilc::Gui::TGrid::CleanData() {
    m_Data.clear();
}

void Tilc::Gui::TGrid::Draw()
{
    if (!m_Visible) return;
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };
    Tilc::Gui::TFont* f = t->DefaultFont;
    int x = 0;
    int y = 0;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    SDL_Color border_color{ 0, 0, 0, 255 };
    int w, h;
    Tilc::Gui::TGridCell* xheaderCell, * yheaderCell;
    SDL_Point cellSize;
    SDL_FRect clientRect = m_RealPosition, noheadersClientRect, rc, rc2;
    int maxRightX = 0;
    int maxBottomY = 0;
    bool drawScrollbarsEmptySquare = true;

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }


    if (m_ShowLeftHeader)
    {
        clientRect.x += m_LeftHeaderWidth - 1;
    }
    if (m_ShowTopHeader)
    {
        clientRect.y = m_TopHeaderHeight - 1;
    }
    if (m_VScrollBar)
    {
        clientRect.w -= m_VScrollBar->m_Position.w;
    }
    if (m_HScrollBar)
    {
        clientRect.h -= m_HScrollBar->m_Position.h;
    }

    clientRect.w -= 1;      // apply border size
    clientRect.h -= 1;     // apply border size
    noheadersClientRect = clientRect;
    noheadersClientRect.x = m_RealPosition.x;
    noheadersClientRect.y = m_RealPosition.y;

    int theaderSize = m_ShowTopHeader ? m_TopHeaderHeight : 0;
    int lheaderSize = m_ShowLeftHeader ? m_LeftHeaderWidth : 0;

    // clear canvas
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(Renderer, &m_RealPosition);

    // ================================================================
    // Rysujemy lewą górną komórkę nagłówka
    // ================================================================
    x = 0;
    y = 0;
    if (m_ShowLeftHeader && m_ShowTopHeader)
    {
        w = m_LeftHeaderWidth;
        h = m_TopHeaderHeight;
        rc.x = x + m_RealPosition.x;
        rc.y = y + m_RealPosition.y;
        rc.w = w;
        rc.h = h;
        RenderTiledTexture(TextureMap, &t->grid_left_top_header_inner_bg_rc, &rc);
        rc2.x = x + m_RealPosition.x + w - t->grid_left_top_header_mark_rc.w - 1;
        rc2.y = y + m_RealPosition.y + h - t->grid_left_top_header_mark_rc.h - 1;
        rc2.w = t->grid_left_top_header_mark_rc.w;
        rc2.h = t->grid_left_top_header_mark_rc.h;
        RenderTiledTexture(TextureMap, &t->grid_left_top_header_mark_rc, &rc2);
        SDL_SetRenderDrawColor(Renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderRect(Renderer, &rc);
        x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
    }
    // ================================================================
    // Koniec rysowania lewej górnej komórki nagłówka
    // ================================================================
    SDL_FRect inner_bg_rc;
    bool selected = false, xHeaderColSelected = false, yHeaderColSelected = false;
    int xIndex, yIndex;
    unsigned int xheader_items_count, yheader_items_count;
    std::vector<TGridCell>& xheader_items = m_ColData;
    std::vector<TGridCell>& yheader_items = m_RowData;
    float vscrw = 0, hscrh = 0;
    if (m_VScrollBar)
    {
        vscrw = m_VScrollBar->m_Position.w;
    }
    if (m_HScrollBar)
    {
        hscrh = m_HScrollBar->m_Position.h;
    }

    xheader_items_count = xheader_items.size();
    yheader_items_count = yheader_items.size();

    // ================================================================
    // Rysujemy górny nagłówek
    // ================================================================
    if (m_ShowTopHeader)
    {
        h = m_TopHeaderHeight;
        xIndex = m_StartCellCoordX;
        while ((x < m_RealPosition.x + m_RealPosition.w - vscrw) && (xIndex-1 < xheader_items_count))
        {
            Tilc::Gui::TGridCell& xheaderCell = xheader_items[xIndex-1];
            w = xheaderCell.m_Size.x;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (xIndex == m_CoordX)
            {
                xHeaderColSelected = true;
            }
            else
            {
                xHeaderColSelected = xheaderCell.m_Selected;
            }

            if (xHeaderColSelected)
            {
                inner_bg_rc = t->grid_top_header_inner_bg_selected_cell_rc;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Selected;
            }
            else
            {
                inner_bg_rc = t->grid_top_header_inner_bg_normal_cell_rc;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Normal;
            }

            rc.x = x + m_RealPosition.x;
            rc.y = y + m_RealPosition.y;
            rc.w = w;
            rc.h = h;
            if (rc.x + rc.w > m_RealPosition.x + m_RealPosition.w - vscrw)
            {
                rc.w = m_RealPosition.x + m_RealPosition.w - vscrw - rc.x;
            }
            RenderTiledTexture(TextureMap, &inner_bg_rc, &rc);
            SDL_SetRenderDrawColor(Renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderRect(Renderer, &rc);
            if (xIndex <= m_MaxColumnNumber)
            {
                f->DrawString(Renderer, xheaderCell.m_Value.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
            }

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex += 1;
        }
    }
    // ================================================================
    // Koniec rysowania górnego nagłówka
    // ================================================================


    // ================================================================
    // Rysujemy lewy nagłówek
    // ================================================================
    if (m_ShowLeftHeader)
    {
        x = m_RealPosition.x;
        if (m_ShowTopHeader)
        {
            y = m_RealPosition.y + t->grid_top_header_inner_bg_normal_cell_rc.h + 2 - 1;
        }
        w = m_LeftHeaderWidth;
        yIndex = m_StartCellCoordY;
        while ((y < m_RealPosition.y + m_RealPosition.h - hscrh) && (yIndex-1 < yheader_items_count))
        {
            Tilc::Gui::TGridCell& yheaderCell = yheader_items[yIndex-1];
            h = yheaderCell.m_Size.y;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (yIndex == m_CoordY)
            {
                yHeaderColSelected = true;
            }
            else
            {
                yHeaderColSelected = yheaderCell.m_Selected;
            }

            if (yHeaderColSelected)
            {
                inner_bg_rc = t->grid_left_header_inner_bg_selected_cell_rc;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Selected;
            } else {
                inner_bg_rc = t->grid_left_header_inner_bg_normal_cell_rc;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Normal;
            }

            rc.x = x;
            rc.y = y;
            rc.w = w;
            rc.h = h;
            if (rc.y + rc.h > m_RealPosition.y + m_RealPosition.h - hscrh)
            {
                rc.h = m_RealPosition.y + m_RealPosition.h - hscrh - rc.y;
            }
            RenderTiledTexture(TextureMap, &inner_bg_rc, &rc);
            SDL_SetRenderDrawColor(Renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderRect(Renderer, &rc);
            if (yIndex <= m_MaxRowNumber)
            {
                f->DrawString(Renderer, yheaderCell.m_Value.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
            }

            y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            ++yIndex;
        }
    }
    // ================================================================
    // Koniec rysowania lewego nagłówka
    // ================================================================


    int coord;
    int foundCellPairPosition;
    // ================================================================
    // Rysujemy komórki wewnętrzne (z danymi)
    // ================================================================
    x = lheaderSize ? lheaderSize - 1 : 0;
    y = theaderSize ? theaderSize - 1 : 0;
    w = m_DefColumnWidth;
    h = t->grid_cell_inner_bg_normal_rc.h;
    x += m_RealPosition.x;
    y += m_RealPosition.y;

    xIndex = m_StartCellCoordX;
    yIndex = m_StartCellCoordY;
    int startX = x;

    while ((y < m_RealPosition.y + m_RealPosition.h - hscrh) && (yIndex <= yheader_items_count))
    {
        x = startX;
        xIndex = m_StartCellCoordX;
        while ((x < m_RealPosition.x + m_RealPosition.w - vscrw) && (xIndex <= xheader_items_count))
        {
            Tilc::Gui::TGridCell& xheaderCell = xheader_items[xIndex-1];
            Tilc::Gui::TGridCell& yheaderCell = yheader_items[yIndex-1];
            w = xheaderCell.m_Size.x;
            h = yheaderCell.m_Size.y;

            coord = CellCoordsToInt(xIndex, yIndex);
            auto cellPair = m_Data.find(coord);
            // rysujemy tylko te komórki, które są puste, bo te z tekstem są renderowane poniżej
            if (cellPair == m_Data.end())
            {
                selected = (m_EntireRowSelect && (xIndex == m_CoordX)) || (m_EntireColumnSelect && (yIndex == m_CoordY));

                if (selected)
                {
                    inner_bg_rc = t->grid_cell_inner_bg_selected_rc;
                    border_color = t->commonGridControlCellBorderColor_Selected;
                }
                else
                {
                    inner_bg_rc = t->grid_cell_inner_bg_normal_rc;
                    border_color = t->commonGridControlCellBorderColor_Normal;
                }

                rc.x = x;
                rc.y = y;
                rc.w = w;
                rc.h = h;
                if (rc.x + rc.w > m_RealPosition.x + m_RealPosition.w - vscrw)
                {
                    rc.w = m_RealPosition.x + m_RealPosition.w - vscrw - rc.x;
                }
                if (rc.y + rc.h > m_RealPosition.y + m_RealPosition.h - hscrh)
                {
                    rc.h = m_RealPosition.y + m_RealPosition.h - hscrh - rc.y;
                }
                RenderTiledTexture(TextureMap, &inner_bg_rc, &rc);
                if (m_DrawVerticalLines && m_DrawHorizontalLines)
                {
                    SDL_SetRenderDrawColor(Renderer, border_color.r, border_color.g, border_color.b, border_color.a);
                    SDL_RenderRect(Renderer, &rc);
                }
                else
                {
                    if (m_DrawHorizontalLines)
                    {
                        rc.h = 1;
                        SDL_RenderRect(Renderer, &rc);
                        rc.y += h - 1;
                        SDL_RenderRect(Renderer, &rc);
                    }
                    else if (yIndex == m_MaxRowNumber)
                    {
                        // last (closing) grid horizontal line we draw always
                        rc.h = 1;
                        rc.y += h - 1;
                        SDL_RenderRect(Renderer, &rc);
                    }
                    rc.x = x;
                    rc.y = y;
                    if (m_DrawVerticalLines)
                    {
                        rc.w = 1;
                        SDL_RenderRect(Renderer, &rc);
                        rc.x += w - 1;
                        SDL_RenderRect(Renderer, &rc);
                    }
                    else if (xIndex == m_MaxColumnNumber)
                    {
                        // last (closing) grid vertical line we draw always
                        rc.h = 1;
                        rc.x += w - 1;
                        SDL_RenderRect(Renderer, &rc);
                    }
                }
            }

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            ++xIndex;
        }

        y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        ++yIndex;
    }
    // ================================================================
    // Koniec rysowania komórek wewnętrznych (z danymi)
    // ================================================================

    // ================================================================
    // Rysowanie aktywnej komórki (a właściwie jej obramowania)
    // ================================================================
    if (CellVisible(m_CoordX, m_CoordY, true))
    {
        CellCoordsToCanvasCoords(m_CoordX, m_CoordY, &x, &y);
        // Inkrementujemy obie współrzedne, bo prostokąt rysujemy pogrubioną linią
        x += 0;
        y += 0;
        cellSize = GetCellSize(m_CoordX, m_CoordY);
        rc.x = x + m_RealPosition.x;
        rc.w = cellSize.x - 2;
        rc.y = y + m_RealPosition.y;
        rc.h = cellSize.y - 2;
        if (rc.x + rc.w > m_RealPosition.x + m_RealPosition.w - vscrw)
        {
            rc.w = m_RealPosition.x + m_RealPosition.w - vscrw - rc.x - 1;
        }
        if (rc.y + rc.h > m_RealPosition.y + m_RealPosition.h - hscrh)
        {
            rc.h = m_RealPosition.y + m_RealPosition.h - hscrh - rc.y - 1;
        }
        SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(Renderer, &rc);
        SDL_SetRenderDrawColor(Renderer, t->commonGridControlCellBorderColor_Active.r, t->commonGridControlCellBorderColor_Active.g, t->commonGridControlCellBorderColor_Active.b, t->commonGridControlCellBorderColor_Active.a);
        rc.w += 2;
        rc.h += 2;
        SDL_RenderRect(Renderer, &rc);
    }
    // ================================================================
    // Koniec rysowania aktywnej komórki (a właściwie jej obramowania)
    // ================================================================


    int cellMarginLeft = 2;
    int cellMarginRight = 2;
    // ================================================================
    // Rysujemy zawartości (dane) komórek wewnętrznych
    // ================================================================
    Tilc::TExtString cellValue;
    x = m_RealPosition.x + m_LeftHeaderWidth - 1;
    y = m_RealPosition.y + t->grid_top_header_inner_bg_normal_cell_rc.h + 1;
    w = m_DefColumnWidth;
    h = t->grid_cell_inner_bg_normal_rc.h;
    xIndex = m_StartCellCoordX;
    yIndex = m_StartCellCoordY;
    while ((y < m_RealPosition.y + m_RealPosition.h - hscrh) && (yIndex - m_StartCellCoordY < yheader_items_count))
    {
        Tilc::Gui::TGridCell& yheaderCell = yheader_items[yIndex - m_StartCellCoordY];
        x = m_RealPosition.x + m_LeftHeaderWidth - 1;
        xIndex = m_StartCellCoordX;
        while ((x < m_RealPosition.x + m_RealPosition.w - vscrw) && (xIndex - m_StartCellCoordX < xheader_items_count))
        {
            Tilc::Gui::TGridCell& xheaderCell = xheader_items[xIndex - m_StartCellCoordX];
            w = xheaderCell.m_Size.x;
            h = yheaderCell.m_Size.y;

            coord = CellCoordsToInt(xIndex, yIndex);
            auto cellPair = m_Data.find(coord);
            if (cellPair == m_Data.end())
            {
                cellValue = "";
            }
            else
            {
                cellValue = cellPair->second.m_Value;
            }

            if (!cellValue.empty())
            {
                if (selected)
                {
                    inner_bg_rc = t->grid_cell_inner_bg_selected_rc;
                    border_color = t->commonGridControlCellBorderColor_Selected;
                }
                else
                {
                    inner_bg_rc = t->grid_cell_inner_bg_normal_rc;
                    border_color = t->commonGridControlCellBorderColor_Normal;
                }


                // Najpierw tło
                rc.x = x;
                rc.y = y;
                rc.w = w;
                rc.h = h;
                // Jesli to aktywna komórka, to ryzujemy na żółto
                if (xIndex == m_CoordX && yIndex == m_CoordY)
                {
                    SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
                    SDL_RenderFillRect(Renderer, &rc);
                    SDL_SetRenderDrawColor(Renderer, t->commonGridControlCellBorderColor_Active.r, t->commonGridControlCellBorderColor_Active.g, t->commonGridControlCellBorderColor_Active.b, t->commonGridControlCellBorderColor_Active.a);
                }
                // w przeciwnym razie kolor bieżemy z theme
                else
                {
                    if (cellPair != m_Data.end())
                    {
                        selected = cellPair->second.m_Selected || (m_EntireRowSelect && (xIndex == m_CoordX)) || (m_EntireColumnSelect && (yIndex == m_CoordY));
                    }
                    else
                    {
                        selected = (m_EntireRowSelect && (xIndex == m_CoordX)) || (m_EntireColumnSelect && (yIndex == m_CoordY));
                    }

                    if (selected)
                    {
                        inner_bg_rc = t->grid_cell_inner_bg_selected_rc;
                        border_color = t->commonGridControlCellBorderColor_Selected;
                    }
                    else
                    {
                        inner_bg_rc = t->grid_cell_inner_bg_normal_rc;
                        border_color = t->commonGridControlCellBorderColor_Normal;
                    }
                    RenderTiledTexture(TextureMap, &inner_bg_rc, &rc);
                }
                // Obramowanie
                SDL_RenderRect(Renderer, &rc);

                // Teraz zawartość
                rc.x = x + cellMarginLeft;
                rc.y = y;
                rc.w = w - 1 - cellMarginLeft - cellMarginRight;
                rc.h = h;
                f->SetColor({ 0, 0, 0, 255 });
                f->DrawString(Renderer, cellValue.c_str(), &rc, Align_CenterVertical | Align_Left);
            }
            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            ++xIndex;
        }

        y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        ++yIndex;
    }
    // ================================================================
    // Koniec rysowania zawartości (dane) komórek wewnętrznych
    // ================================================================

    // ================================================================
    // Rysowanie końcowych obramowań
    // ================================================================
    // Obramowanie całego Grida
    if (m_State == Tilc::Gui::CONTROL_STATE_FOCUSED)
    {
        SDL_SetRenderDrawColor(Renderer, t->commonGridControlFocusedGridBorderColor.r, t->commonGridControlFocusedGridBorderColor.g, t->commonGridControlFocusedGridBorderColor.b, t->commonGridControlFocusedGridBorderColor.a);
        SDL_RenderRect(Renderer, &m_RealPosition);
    }
    else
    {
        SDL_SetRenderDrawColor(Renderer, t->commonGridControlLeftTopHeaderCellBorderColor.r, t->commonGridControlLeftTopHeaderCellBorderColor.g, t->commonGridControlLeftTopHeaderCellBorderColor.b, t->commonGridControlLeftTopHeaderCellBorderColor.a);
        SDL_RenderRect(Renderer, &m_RealPosition);
    }

    // Obramowanie dolne górnego nagłówka (gdyż zostało nadpisane przez komórki)
    if (m_ShowTopHeader)
    {
        rc = m_RealPosition;
        rc.x += 1;
        rc.y += m_TopHeaderHeight - 1;
        rc.w = 1;
        rc.h = m_DataCanvasWidth + lheaderSize - 1;
        SDL_SetRenderDrawColor(Renderer, t->commonGridControlTopHeaderCellBorderColor_Normal.r, t->commonGridControlTopHeaderCellBorderColor_Normal.g, t->commonGridControlTopHeaderCellBorderColor_Normal.b, t->commonGridControlTopHeaderCellBorderColor_Normal.a);
        SDL_RenderRect(Renderer, &m_RealPosition);
    }
    // Obramowanie prawe lewego nagłówka (gdyż zostało nadpisane przez komórki)
    if (m_ShowLeftHeader)
    {
        rc = m_RealPosition;
        rc.x += m_LeftHeaderWidth - 1;
        rc.y += 1;
        rc.w = m_DataCanvasHeight + theaderSize - 1;
        rc.h = 1;
        SDL_SetRenderDrawColor(Renderer, t->commonGridControlTopHeaderCellBorderColor_Normal.r, t->commonGridControlTopHeaderCellBorderColor_Normal.g, t->commonGridControlTopHeaderCellBorderColor_Normal.b, t->commonGridControlTopHeaderCellBorderColor_Normal.a);
        SDL_RenderRect(Renderer, &m_RealPosition);
    }
    // ================================================================
    // Koniec rysowania końcowych obramowań
    // ================================================================
    /*
    // If grid data cells occupy smaller area than entire grid control then fill remained regions with bg color
    if (maxRightX < clientRect.right) {
        this->canvas->fillRectangle(this->_backgroundColor, maxRightX, clientRect.top, clientRect.right - maxRightX, this->_dataCanvasHeight);
    }
    if (maxBottomY < clientRect.bottom) {
        this->canvas->fillRectangle(this->_backgroundColor, clientRect.left, maxBottomY, clientRect.right - clientRect.left, clientRect.bottom - maxBottomY);
    }
    */

    if (drawScrollbarsEmptySquare)
    {
        rc.x = m_RealPosition.x + m_RealPosition.w - vscrw;
        rc.y = m_RealPosition.y + m_RealPosition.h - hscrh;
        rc.w = vscrw;
        rc.h = hscrh;
        if (rc.x + rc.w > m_RealPosition.x + m_RealPosition.w - vscrw)
        {
            rc.w = m_RealPosition.x + m_RealPosition.w - vscrw - rc.x;
        }
        if (rc.y + rc.h > m_RealPosition.y + m_RealPosition.h - hscrh)
        {
            rc.h = m_RealPosition.y + m_RealPosition.h - hscrh - rc.y;
        }
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(Renderer, &rc);
    }


    DrawChildren(false);

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

Tilc::TExtString Tilc::Gui::TGrid::IndexToLetter(int index)
{
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ - 26 liter
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // dopuszczamy numery maksymalnie 3-literowe
    if (index < 1 || index > 17576) { // 17576 = 26 * 26 * 26
        return "";
    }

    if (index <= 26)
    {
        return Tilc::TExtString({ letters[index - 1] });
    }
    if (index <= 676) { // 676 = 26 * 26
        return Tilc::TExtString({ letters[index / 26 - 1], letters[index % 26] });
    }
    if (index <= 17576) { // 17576 = 26 * 26 * 26
        int tmp = index / 26;
        return Tilc::TExtString({ letters[tmp / 26 - 1], letters[tmp % 26], letters[index % 26] });
    }

    return "";
}

int Tilc::Gui::TGrid::CellCoordsToInt(int x, int y)
{
    int result = ((y & 0x0000ffff) << 16) | (x & 0x0000ff);
    return result;
}

void Tilc::Gui::TGrid::IntToCellCoords(int data, int* x, int* y)
{
    *x = Tilc::LoWord(data);
    *y = Tilc::HiWord(data);
}

void Tilc::Gui::TGrid::CellCoordsToCanvasCoords(int cellCoordX, int cellCoordY, int* x, int* y)
{
    *x = m_ShowLeftHeader ? m_LeftHeaderWidth - 1 : 0;
    *y = m_ShowTopHeader ? m_TopHeaderHeight - 1 : 0;

    int xIndex = m_StartCellCoordX;
    int yIndex = m_StartCellCoordY;
    size_t xheader_items_count, yheader_items_count;
    std::vector<TGridCell>& xheader_items = m_ColData;
    std::vector<TGridCell>& yheader_items = m_RowData;

    xheader_items_count = m_ColData.size();
    yheader_items_count = m_RowData.size();

    while (xIndex < cellCoordX)
    {
        *x += xheader_items[xIndex - m_StartCellCoordX].m_Size.x - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        ++xIndex;
    }

    while (yIndex < cellCoordY)
    {
        *y += yheader_items[yIndex - m_StartCellCoordY].m_Size.y - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        ++yIndex;
    }
}

SDL_Point Tilc::Gui::TGrid::GetCellSize(int cellCoordX, int cellCoordY)
{
    SDL_Point retval;
    retval.x = 0;
    retval.y = 0;
    Tilc::Gui::TGridCell* headerData;

    headerData = GetHeaderCellX(cellCoordX);
    if (headerData)
    {
        retval.x = headerData->m_Size.x;
    }
    headerData = GetHeaderCellY(cellCoordY);
    if (headerData)
    {
        retval.y = headerData->m_Size.y;
    }

    return retval;
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetCell(int cellCoordX, int cellCoordY)
{
    int pos;
    int first = CellCoordsToInt(cellCoordX, cellCoordY);
    auto p = m_Data.find(first);
    if (p != m_Data.end())
    {
        return &p->second;
    }
    return nullptr;
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetCurrentCell()
{
    return GetCell(m_CoordX, m_CoordY);
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetHeaderCellX(int coordX)
{
    return &m_ColData[coordX-1];
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetCurrentHeaderCellX()
{
    return &m_ColData[m_CoordX-1];
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetHeaderCellY(int coordY)
{
    return &m_RowData[coordY-1];
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::GetCurrentHeaderCellY()
{
    return &m_RowData[m_CoordY-1];
}

void Tilc::Gui::TGrid::SetStartCellCoordX(int coordX)
{
    if (coordX <= 0)
    {
        return;
    }

    m_StartCellCoordX = coordX;

    if (m_StartCellCoordX < 1)
    {
        m_StartCellCoordX = 1;
        return;
    }

    if (m_StartCellCoordX > m_MaxAllowableStartCellCoordX)
    {
        m_StartCellCoordX = m_MaxAllowableStartCellCoordX;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, m_StartCellCoordX, m_StartCellCoordY));
}

void Tilc::Gui::TGrid::SetStartCellCoordY(int coordY)
{
    if (coordY <= 0)
    {
        return;
    }

    m_StartCellCoordY = coordY;

    if (m_StartCellCoordY < 1)
    {
        m_StartCellCoordY = 1;
        return;
    }

    if (m_StartCellCoordY > m_MaxAllowableStartCellCoordY)
    {
        m_StartCellCoordY = m_MaxAllowableStartCellCoordY;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, m_StartCellCoordX, m_StartCellCoordY));
}

bool Tilc::Gui::TGrid::MoveRight(int count)
{
    if (m_CoordX < m_MaxColumnNumber)
    {
        if (m_CoordX + count <= m_MaxColumnNumber)
        {
            m_CoordX += count;
        }
        else
        {
            count = m_MaxColumnNumber - m_CoordX;
            m_CoordX = m_MaxColumnNumber;
        }
        if (m_CoordX > m_MaxFullVisibleColumnNumber && m_CoordX < m_MaxColumnNumber)
        {
            SetStartCellCoordX(m_StartCellCoordX + count);
            UpdateGridParameters();
        }
        else if (m_CoordX == m_MaxColumnNumber)
        {
            SetStartCellCoordX(m_MaxColumnNumber);
            UpdateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::MoveLeft(int count)
{
    if (m_CoordX > 1)
    {
        if (m_CoordX - count >= 1)
        {
            m_CoordX -= count;
        }
        else
        {
            m_CoordX = 1;
        }
        if (m_StartCellCoordX > m_CoordX)
        {
            SetStartCellCoordX(m_CoordX);
            UpdateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::MoveUp(int count)
{
    if (m_CoordY > 1)
    {
        if (m_CoordY - count >= 1)
        {
            m_CoordY -= count;
        }
        else
        {
            m_CoordY = 1;
        }
        if (m_StartCellCoordY > m_CoordY)
        {
            SetStartCellCoordY(m_CoordY);
            UpdateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::MoveDown(int count)
{
    if (m_CoordY <= m_MaxRowNumber)
    {
        if (m_CoordY + count <= m_MaxRowNumber)
        {
            m_CoordY += count;
        }
        else
        {
            count = m_MaxRowNumber - m_CoordY;
            m_CoordY = m_MaxRowNumber;
        }
        if (m_CoordY > m_MaxFullVisibleRowNumber)
        {
            SetStartCellCoordY(m_StartCellCoordY + count);
            UpdateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::MovePageUp()
{
    return MoveUp(m_YPageStep);
}

bool Tilc::Gui::TGrid::MovePageDown()
{
    return MoveDown(m_YPageStep);
}

bool Tilc::Gui::TGrid::MovePageLeft()
{
    return MoveLeft(m_XPageStep);
}

bool Tilc::Gui::TGrid::MovePageRight()
{
    return MoveRight(m_XPageStep);
}

bool Tilc::Gui::TGrid::MoveHome()
{
    return MoveToCell(1, 1);
}

bool Tilc::Gui::TGrid::MoveHomeVertical()
{
    return MoveToCell(m_CoordX, 1);
}

bool Tilc::Gui::TGrid::MoveHomeHorizontal()
{
    return MoveToCell(1, m_CoordY);
}

bool Tilc::Gui::TGrid::MoveEnd()
{
    return MoveToCell(m_MaxColumnNumber, m_MaxRowNumber);
}

bool Tilc::Gui::TGrid::MoveEndVertical()
{
    return MoveToCell(m_CoordX, m_MaxRowNumber);
}

bool Tilc::Gui::TGrid::MoveEndHorizontal()
{
    return MoveToCell(m_MaxColumnNumber, m_CoordY);
}

bool Tilc::Gui::TGrid::MoveToCell(int coordX, int coordY)
{
    if (
        coordX < 1 || coordX > m_MaxColumnNumber ||
        coordY < 1 || coordY > m_MaxRowNumber
       ) {
        return false;
    }
    int deltaX = coordX - m_CoordX;
    int deltaY = coordY - m_CoordY;
    if (CellVisible(m_CoordX + deltaX, m_CoordY + deltaY))
    {
        m_CoordX = coordX;
        m_CoordY = coordY;
    }
    else
    {
        m_CoordX = coordX;
        m_CoordY = coordY;
        SetStartCellCoordX(m_StartCellCoordX + deltaX > 0 ? m_StartCellCoordX + deltaX : 1);
        SetStartCellCoordY(m_StartCellCoordY + deltaY > 0 ? m_StartCellCoordY + deltaY : 1);
        UpdateGridParameters();
    }

    return deltaX != 0 || deltaY != 0;
}

bool Tilc::Gui::TGrid::CellVisible(int coordX, int coordY, bool acceptPartialVisibility)
{
    int maxCoordX, maxCoordY;
    if (!acceptPartialVisibility)
    {
        maxCoordX = m_MaxFullVisibleColumnNumber;
        maxCoordY = m_MaxFullVisibleRowNumber;
    }
    else
    {
        maxCoordX = m_MaxVisibleColumnNumber;
        maxCoordY = m_MaxVisibleRowNumber;
    }

    if (
        (m_StartCellCoordX <= coordX) && (coordX <= maxCoordX) &&
        (m_StartCellCoordY <= coordY) && (coordY <= maxCoordY)) {
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::GetCellCoordsAtMousePos(int mouseX, int mouseY, int* coordX, int* coordY)
{
    *coordX = -1;
    *coordY = -1;

    int startMouseX;
    int startMouseY;
    CellCoordsToCanvasCoords(m_StartCellCoordX, m_StartCellCoordY, &startMouseX, &startMouseY);

    int xIndex = m_StartCellCoordX;
    int yIndex = m_StartCellCoordY;
    Tilc::Gui::TGridCell* headerData;
    std::vector<Tilc::Gui::TGridCell>& xheader_items = m_ColData;
    std::vector<Tilc::Gui::TGridCell>& yheader_items = m_RowData;

    if (mouseX < startMouseX)
    {
        mouseX = startMouseX;
    }
    if (mouseY < startMouseY)
    {
        mouseY = startMouseY;
    }

    bool coordSet = false;
    size_t size = m_ColData.size();
    int curX = startMouseX;
    int curY = startMouseY;
    while ((xIndex - m_StartCellCoordX < size) && (curX <= m_Position.w))
    {
        headerData = &xheader_items[xIndex - m_StartCellCoordX];
        if (curX <= mouseX && mouseX <= curX + headerData->m_Size.x)
        {
            coordSet = true;
            break;
        }
        curX += headerData->m_Size.x - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
        xIndex += 1;
    }
    if (coordSet)
    {
        *coordX = xIndex;
        coordSet = false;
        size = m_RowData.size();
        while ((yIndex - m_StartCellCoordY < size) && (curY <= m_Position.h))
        {
            headerData = &yheader_items[yIndex - m_StartCellCoordY];
            if (curY <= mouseY && mouseY <= curY + headerData->m_Size.y)
            {
                coordSet = true;
                break;
            }
            curY += headerData->m_Size.y - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
            yIndex += 1;
        }
        if (coordSet)
        {
            *coordY = yIndex;
        }
        else
        {
            *coordX = -1;
        }
    }

    if (*coordX != -1 && *coordY != -1)
    {
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::ActivateCellAtMousePos(int x, int y)
{
    int coordX, coordY;
    if (GetCellCoordsAtMousePos(x, y, &coordX, &coordY))
    {
        m_CoordX = coordX;
        m_CoordY = coordY;
        return true;
    }

    return false;
}

void Tilc::Gui::TGrid::ClearSelection(bool redraw)
{
    bool had_selection = false;

    Tilc::Gui::TGridCell *gridCell;
    size_t items_count = m_Data.size();

    for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
    {
        gridCell = &(it->second);
        if (gridCell)
        {
            if (gridCell->m_Selected)
            {
                had_selection = true;
                gridCell->m_Selected = false;
            }
        }
    }

    if (!had_selection)
    {
        redraw = false;
    }

    if (redraw)
    {
        Invalidate();
    }
}

bool Tilc::Gui::TGrid::IsSelection()
{
    bool is_selection = false;

    Tilc::Gui::TGridCell *gridCell;
    size_t items_count = m_Data.size();

    for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
    {
        gridCell = &(it->second);
        if (gridCell)
        {
            if (gridCell->m_Selected)
            {
                is_selection = true;
                break;
            }
        }
    }

    return is_selection;
}

bool Tilc::Gui::TGrid::OnKeyDown(const SDL_Event& event)
{
    bool redraw = false;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    bool activeCellChanged = false;

    const bool* Keys = SDL_GetKeyboardState(nullptr);
    bool vkControl = Keys[SDL_SCANCODE_LCTRL] || Keys[SDL_SCANCODE_RCTRL];

    switch (event.key.key)
    {
    case SDLK_F2:
    case SDLK_RETURN:
        Edit();
        redraw = true;
        break;
    case SDLK_DELETE:
        ClearCurrentCellValue();
        redraw = true;
        break;
    case SDLK_RIGHT:
        if (vkControl)
        {
            activeCellChanged = MoveEndHorizontal();
        }
        else
        {
            activeCellChanged = MoveRight();
        }
        break;
    case SDLK_LEFT:
        if (vkControl)
        {
            activeCellChanged = MoveHomeHorizontal();
        }
        else
        {
            activeCellChanged = MoveLeft();
        }
        break;
    case SDLK_UP:
        if (vkControl)
        {
            activeCellChanged = MoveHomeVertical();
        }
        else
        {
            activeCellChanged = MoveUp();
        }
        break;
    case SDLK_DOWN:
        if (vkControl)
        {
            activeCellChanged = MoveEndVertical();
        }
        else
        {
            activeCellChanged = MoveDown();
        }
        break;
    case SDLK_PAGEDOWN:
        activeCellChanged = MovePageDown();
        break;
    case SDLK_PAGEUP:
        activeCellChanged = MovePageUp();
        break;
    case SDLK_HOME:
        activeCellChanged = MoveHome();
        break;
    case SDLK_END:
        activeCellChanged = MoveEnd();
        break;
    }

    if (activeCellChanged)
    {
        UpdateScrollBars();
        redraw = activeCellChanged;
    }

    //InformationBox(FormatMessage(L"_coordX: %d, _coordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
    //        m_CoordX, m_CoordY, m_StartCellCoordX, m_StartCellCoordY));
    if (redraw)
    {
        Invalidate();
    }

    return true;
}

bool Tilc::Gui::TGrid::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonDown(event);

    if (PointIn(event.button.x, event.button.y))
    {
        CaptureMouse(this);

        ActivateCellAtMousePos(event.button.x - m_RealPosition.x, event.button.y - m_RealPosition.y);

        ClearSelection(false);
        Invalidate();

        return true;
    }

    return false;
}

void Tilc::Gui::TGrid::OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar)
{
    float CurPositionPercent = ((float)curPosition - scrollbar->GetMinValue()) / (scrollbar->GetMaxValue() - scrollbar->GetMinValue());
    if (scrollbar == m_HScrollBar)
    {
        m_StartCellCoordX = static_cast<int>(CurPositionPercent * (m_MaxAllowableStartCellCoordX-1)) + 1; //curPosition + 1;
        UpdateGridParameters();
        Invalidate();
    }
    if (scrollbar == m_VScrollBar)
    {
        m_StartCellCoordY = static_cast<int>(CurPositionPercent * (m_MaxAllowableStartCellCoordY-1)) + 1; //curPosition + 1;
        UpdateGridParameters();
        Invalidate();
    }
}

unsigned int Tilc::Gui::TGrid::GetTotalColumnsWidth()
{
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    unsigned int totalWidth = 0;
    for (i = 0; i < m_MaxColumnNumber; ++i)
    {
        totalWidth += m_ColData[i].m_Size.x;
    }

    return totalWidth;
}

unsigned int Tilc::Gui::TGrid::GetTotalColumnsWidthFromStartTo(int ToCell)
{
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    unsigned int totalWidth = 0;
    for (i = 0; i < m_MaxColumnNumber && ToCell > 0; ++i, --ToCell)
    {
        totalWidth += m_ColData[i].m_Size.x;
    }

    return totalWidth;
}

unsigned int Tilc::Gui::TGrid::GetTotalRowsHeight()
{
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    unsigned int totalHeight = 0;
    for (i = 0; i < m_MaxRowNumber; ++i)
    {
        totalHeight += m_RowData[i].m_Size.y;
    }

    return totalHeight;
}

unsigned int Tilc::Gui::TGrid::GetTotalRowsHeightFromStartTo(int ToRow)
{
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    unsigned int totalHeight = 0;
    for (i = 0; i < m_MaxRowNumber && ToRow > 0; ++i, --ToRow)
    {
        totalHeight += m_RowData[i].m_Size.y;
    }

    return totalHeight;
}

int Tilc::Gui::TGrid::GetVisibleColumnCount()
{
    int count = (m_MaxVisibleColumnNumber - m_StartCellCoordX) + 1;
    //WriteLogMessage(L"_maxVisibleColumnNumber: %d, : _startCellCoordX: %d", this->_maxVisibleColumnNumber, m_StartCellCoordX);
    return count;
}

int Tilc::Gui::TGrid::GetFullVisibleColumnCount()
{
    int count = (m_MaxFullVisibleColumnNumber - m_StartCellCoordX) + 1;
    return count;
}

int Tilc::Gui::TGrid::GetVisibleRowCount()
{
    int count = (m_MaxVisibleRowNumber - m_StartCellCoordY) + 1;
    //WriteLogMessage(L"_maxVisibleRowNumber: %d, : _startCellCoordY: %d", this->_maxVisibleRowNumber, m_StartCellCoordY);
    return count;
}

int Tilc::Gui::TGrid::GetFullVisibleRowCount()
{
    int count = (m_MaxFullVisibleRowNumber - m_StartCellCoordY) + 1;
    return count;
}

void Tilc::Gui::TGrid::OnApplyEditorChanges(const Tilc::TExtString& value)
{
    SetValue(value);
}


Tilc::TExtString Tilc::Gui::TGrid::GetValue()
{
    Tilc::Gui::TGridCell* c = GetCell(m_CoordX, m_CoordY);
    if (c)
    {
        return c->m_Value;
    }
    return "";
}

void Tilc::Gui::TGrid::SetValue(const Tilc::TExtString& value, bool redraw)
{
    SetCellValue(m_CoordX, m_CoordY, value, redraw);
}

void Tilc::Gui::TGrid::SetCurrentCellValue(const Tilc::TExtString& value, bool redraw)
{
    SetCellValue(m_CoordX, m_CoordY, value, redraw);
}

void Tilc::Gui::TGrid::SetCellValue(int cellCoordX, int cellCoordY, const Tilc::TExtString& value, bool redraw)
{
    Tilc::Gui::TGridCell* c = GetCell(cellCoordX, cellCoordY);
    if (c)
    {
        c->m_Value = value;
    }
    else
    {
        int first = CellCoordsToInt(cellCoordX, cellCoordY);
        m_Data[first] = Tilc::Gui::TGridCell(value, { 0, 0 }, false, false);
    }

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TGrid::ClearCurrentCellValue(bool redraw)
{
    SetCellValue(m_CoordX, m_CoordY, "", redraw);
}

void Tilc::Gui::TGrid::ClearCellValue(int cellCoordX, int cellCoordY, bool redraw)
{
    SetCellValue(cellCoordX, cellCoordY, "", redraw);
}

void Tilc::Gui::TGrid::OnEditorShow()
{
    int x, y, w;
    CellCoordsToCanvasCoords(m_CoordX, m_CoordY, &x, &y);
    Tilc::Gui::TTextField* tf = (Tilc::Gui::TTextField*)m_Editor;
    m_Editor->m_Position.x = x;
    m_Editor->m_Position.y = y;
    w = m_Position.w - x;
    if (m_VScrollBar)
    {
        w -= m_VScrollBar->m_Position.w;
    }
    m_Editor->SetSize(w - 2, m_Editor->m_Position.h);
    tf->SelectAll();
}

void Tilc::Gui::TGrid::UpdateScrollBars()
{
    int TotalSize, ToCellSize;
    float percent;
    if (m_VScrollBar)
    {
        TotalSize = GetTotalRowsHeight() - m_Position.h;
        ToCellSize = GetTotalRowsHeightFromStartTo(m_StartCellCoordY-1);
        percent = static_cast<float>(ToCellSize) / static_cast<float>(TotalSize);
        m_VScrollBar->SetPosition(static_cast<int>(m_VScrollBar->GetMinValue() + std::floor(percent * (m_VScrollBar->GetMaxValue() - m_VScrollBar->GetMinValue()))), false, false);
    }
    if (m_HScrollBar)
    {
        TotalSize = GetTotalColumnsWidth() - m_Position.w;
        ToCellSize = GetTotalColumnsWidthFromStartTo(m_StartCellCoordX-1);
        percent = static_cast<float>(ToCellSize) / static_cast<float>(TotalSize);
        m_HScrollBar->SetPosition(static_cast<int>(m_HScrollBar->GetMinValue() + std::floor(percent * (m_HScrollBar->GetMaxValue() - m_HScrollBar->GetMinValue()))), false, false);
    }
}

void Tilc::Gui::TGrid::ShowTopHeader(bool showTopHeader, bool redraw)
{
    m_ShowTopHeader = showTopHeader;
    UpdateGridParameters();
    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TGrid::ShowLeftHeader(bool showLeftHeader, bool redraw)
{
    m_ShowLeftHeader = showLeftHeader;
    UpdateGridParameters();
    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TGrid::SetGridSize(int columnCount, int rowCount)
{
    m_MaxColumnNumber = columnCount;
    m_MaxRowNumber = rowCount;

    if (m_ColData.size() > 0 && m_RowData.size() > 0)
    {
        CompactData();
        return;
    }

    if (m_ColData.size() < 1)
    {
        m_ColData.reserve(m_MaxColumnNumber);
    }
    if (m_RowData.size() < 1)
    {
        m_RowData.reserve(m_MaxRowNumber);
    }

    int i;
    int index;
    for (i = 0; i < m_MaxColumnNumber; i++)
    {
        index = i + 1;
        m_ColData.emplace_back(IndexToLetter(index) + "[" + std::to_string(index) + "]", SDL_Point({ m_DefColumnWidth, 0 }), true, false);
    }
    // Inicjalizujemy właściwości komórek nagłówka lewego
    for (i = 0; i < m_MaxRowNumber; i++)
    {
        index = i + 1;
        m_RowData.emplace_back(std::to_string(index), SDL_Point({0, static_cast<int>(GetTheme()->grid_top_header_inner_bg_normal_cell_rc.h + 2) }), true, false);
    }
}

void Tilc::Gui::TGrid::CleanGridContent()
{
    int i;
    Tilc::Gui::TGridCell *headerCell;
    int items_count;

    m_ColData.clear();
    m_RowData.clear();
    CleanData();
}

void Tilc::Gui::TGrid::CompactData()
{
    std::vector<int> ItemsDoDelete;
    std::for_each(m_Data.begin(), m_Data.end(), [this, &ItemsDoDelete](auto& item) {
        int x, y;
        IntToCellCoords(static_cast<int>(item.first), &x, &y);
        if (x > m_MaxColumnNumber || y > m_MaxRowNumber)
        {
            ItemsDoDelete.push_back(item.first);
        }
    });
    std::ranges::for_each(ItemsDoDelete, [this](int item) {
        m_Data.erase(item);
    });

    while (m_ColData.size() > m_MaxColumnNumber)
    {
        m_ColData.pop_back();
    }

    while (m_RowData.size() > m_MaxRowNumber)
    {
        m_RowData.pop_back();
    }
}

void Tilc::Gui::TGrid::UpdateGridParameters()
{
    int size;
    int maxAllowableSize;

    // data width
    int i;
    int curX, curY;

    m_MaxFullVisibleColumnNumber = m_StartCellCoordX;
    m_MaxVisibleColumnNumber = m_StartCellCoordX;
    m_MaxFullVisibleRowNumber = m_StartCellCoordY;
    m_MaxVisibleRowNumber = m_StartCellCoordY;
    m_VisibleWidthOfLastColumn = 0;
    m_VisibleHeightOfLastRow = 0;

    // X-based parameters
    curX = m_ShowLeftHeader ? m_LeftHeaderWidth - 1 : 0;
    curX += m_RealPosition.x;
    size = 0;
    maxAllowableSize = m_Position.w;
    if (m_VScrollBar)
    {
        maxAllowableSize -= m_VScrollBar->m_Position.w;
    }
    for (i = m_StartCellCoordX - 1; i < m_ColData.size(); ++i)
    {
        TGridCell& hc = m_ColData[i];
        if (curX + hc.m_Size.x <= m_RealPosition.x + maxAllowableSize)
        {
            m_MaxFullVisibleColumnNumber = i + 1;
            m_MaxVisibleColumnNumber = i + 1;
        }
        else
        {
            if (curX < m_RealPosition.x + maxAllowableSize)
            {
                m_MaxVisibleColumnNumber = i + 1;
                m_VisibleWidthOfLastColumn = maxAllowableSize - curX;
            }
        }
        size += hc.m_Size.x - 1; // -1 because cells share borders
        curX += hc.m_Size.x - 1;
        if (size > m_Position.w)
        {
            break;
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize)
    {
        size = maxAllowableSize;
    }
    m_DataCanvasWidth = size;




    // Y based parameters
    curY = m_ShowTopHeader ? m_TopHeaderHeight - 1 : 0;
    curY += m_RealPosition.y;
    size = 0;
    maxAllowableSize = m_Position.h;
    if (m_HScrollBar)
    {
        maxAllowableSize -= m_HScrollBar->m_Position.h;
    }
    for (i = m_StartCellCoordY - 1; i < m_RowData.size(); ++i)
    {
        TGridCell& hc = m_RowData[i];
        if (curY + hc.m_Size.y <= m_RealPosition.y + maxAllowableSize)
        {
            m_MaxFullVisibleRowNumber = i + 1;
            m_MaxVisibleRowNumber = i + 1;
        }
        else
        {
            if (curY < m_RealPosition.y + maxAllowableSize)
            {
                m_MaxVisibleRowNumber = i + 1;
                m_VisibleHeightOfLastRow = maxAllowableSize - curY;
            }
        }
        size += hc.m_Size.y - 1; // -1 because cells share borders
        curY += hc.m_Size.y - 1;
        if (size > m_Position.h)
        {
            break;
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize)
    {
        size = maxAllowableSize;
    }
    m_DataCanvasHeight = size;


    size = 0;
    i = m_MaxColumnNumber - 1;
    m_MaxAllowableStartCellCoordX = m_MaxColumnNumber;
    while (i >= 0)
    {
        TGridCell& hc = m_ColData[i];
        if (size + hc.m_Size.x <= m_DataCanvasWidth)
        {
            m_MaxAllowableStartCellCoordX = i + 1;
            size += (hc.m_Size.x - 1); // -1 because cells share borders
        }
        else
        {
            break;
        }
        i -= 1;
    }

    int TopHeaderHeight = 0;
    if (m_ShowTopHeader)
    {
        TopHeaderHeight = m_TopHeaderHeight;
    }
    size = 0;
    i = m_MaxRowNumber - 1;
    m_MaxAllowableStartCellCoordY = m_MaxRowNumber;
    while (i >= 0)
    {
        TGridCell& hc = m_RowData[i];
        if (size + hc.m_Size.y + TopHeaderHeight <= m_DataCanvasHeight)
        {
            m_MaxAllowableStartCellCoordY = i + 1;
            size += (hc.m_Size.y - 1); ; // -1 because cells share borders
        }
        else
        {
            break;
        }
        i -= 1;
    }

    /*
    if (this->_hscrollbar) {
        this->_hscrollbar->setMaxValue(this->_maxAllowableStartCellCoordX-1);
    }
    if (this->_vscrollbar) {
        this->_vscrollbar->setMaxValue(this->_maxAllowableStartCellCoordY-1);
    }
    */
}

void Tilc::Gui::TGrid::SetHeaderCaptions(std::vector<TGridCell>* headerCells, TStringVector* captions)
{
    if (!headerCells || !captions)
    {
        return;
    }

    size_t hsize = headerCells->size();
    size_t csize = captions->size();
    if (hsize < 1 || csize < 1)
    {
        return;
    }

    for (size_t i = 0; i < hsize; ++i)
    {
        Tilc::Gui::TGridCell& hc = (*headerCells)[i];
        if (i < csize)
        {
            hc.m_Value = (*captions)[i];
        }
    }
}

void Tilc::Gui::TGrid::SetLeftHeaderCaptions(TStringVector* captions)
{
    SetHeaderCaptions(&m_RowData, captions);
}

void Tilc::Gui::TGrid::SetTopHeaderCaptions(TStringVector* captions)
{
    SetHeaderCaptions(&m_ColData, captions);
}

bool Tilc::Gui::TGrid::SaveToFile(const Tilc::TExtString& fname)
{
    TFile File(fname.c_str(), std::ios::out | std::ios::trunc);
    Tilc::TExtString Content;

    std::for_each(m_Data.begin(), m_Data.end(), [&Content, &File](auto& item) {
        Tilc::Gui::TGridCell& c = item.second;
        Content = "";
        Content += std::to_string(item.first) + "\n";
        Content += std::to_string(c.m_Value.length()) + "\n";
        Content += c.m_Value;
        File.AppendContent(Content);
    });

    return false;
}

bool Tilc::Gui::TGrid::LoadFromFile(const Tilc::TExtString& fname)
{
    TFile File(fname.c_str());
    Tilc::TExtString Content;
    bool result = false;
    bool gridContentChanged = false;

    m_Data.clear();

    File.ReadContent(Content);
    int pos = 0;
    while (pos < Content.length())
    {
        int first;
        int len;

        int EndPos = Content.find('\n', pos);
        Tilc::TExtString s = Content.substr(pos, EndPos - pos);
        try
        {
            first = s.toInt();
            pos = EndPos + 1;
        }
        catch (...)
        {
            result = false;
            gridContentChanged = false;
            break;
        }


        if (pos >= Content.length())
        {
            result = false;
            gridContentChanged = false;
            break;
        }

        EndPos = Content.find('\n', pos);
        s = Content.substr(pos, EndPos - pos);
        try
        {
            len = s.toInt();
            pos = EndPos + 1;
        }
        catch (...)
        {
            result = false;
            gridContentChanged = false;
            break;
        }


        if (pos + len >= Content.length())
        {
            result = false;
            gridContentChanged = false;
            break;
        }
        s = Content.substr(pos, len);
        pos += len;

        m_Data[first] = Tilc::Gui::TGridCell(s, { 0, 0 }, false, false);
        gridContentChanged = true;
        result = true;
    }

    if (gridContentChanged)
    {
        Invalidate();
    }
    
    return result;
}
