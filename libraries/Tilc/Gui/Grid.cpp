#include "Tilc/Gui/Grid.h"
/*
#include "mzstdguiGlobals.h"
#include "mzstd.h"
#include "mzFileUtils.h"
#include "mzImageList.h"
#include "mzTheme.h"
#include "mzBitmap.h"
#include "mzKeyboard.h"
#include "mzStyledWindow.h"
#include "mzMessages.h"
#include "mzApplication.h"
#include "mzArrayList.h"
#include "mzTextField.h"
#include "mzScrollbar.h"
#include "mzScrollbarVertical.h"
#include "mzScrollbarHorizontal.h"
#include "mzStdDialogs.h"
*/
Tilc::Gui::TGridCell::TGridCell(const Tilc::TExtString& value, SDL_Point size, bool isHeader, bool selected)
    : m_Value(value), m_Size(size), m_IsHeader(isHeader), m_Selected(selected)
{
}

Tilc::Gui::TGridCell::~TGridCell()
{
}


/*

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

    m_DefRowHeight = GetTheme()->grid_cell_inner_bg_normal->height() + 2;
    m_TopHeaderHeight = m_DefRowHeight;

    m_StartCellCoordX = 1;
    m_StartCellCoordY = 1;
    m_CoordX = 1;
    m_CoordY = 1;

    SetGridSize(columnCount, rowCount);

    //this->_backgroundColor = RGB(0xff, 0xff, 0xff);
    this->setSize(width, height);
    // Adding scrollbars and editor
    bool addVScr = static_cast<int>(this->getTotalRowsHeight()) > this->height;
    bool addHScr = static_cast<int>(this->getTotalColumnsWidth()) > this->width;
    this->addScrollbars(addVScr, addHScr, 0, 1000, 0, 1000);
    CScrollbar* scr = dynamic_cast<CScrollbar*>(this->getChild(this->name + "_VSB"));
    if (scr)
    {
        (dynamic_cast<CScrollbarVertical*>(scr))->setSteps(50, 100);
    }
    scr = dynamic_cast<CScrollbar*>(this->getChild(this->name + "_HSB"));
    if (scr)
    {
        (dynamic_cast<CScrollbarHorizontal*>(scr))->setSteps(50, 100);
    }
    this->_tabStop = true;
    if (editable) {
        this->addEditor();
    }

    this->_cachedFonts = new CmzArrayList(10);
    this->_updateGridParameters();

    if (this->_visibleWidthOfLastColumn < 4) {
        this->_xPageStep = this->getFullVisibleColumnCount();
    } else {
        this->_xPageStep = this->getVisibleColumnCount();
    }
    if (this->_visibleHeightOfLastRow < 4) {
        this->_yPageStep = this->getFullVisibleRowCount();
    } else {
        this->_yPageStep = this->getVisibleRowCount();
    }
}

Tilc::Gui::TGrid::~Tilc::Gui::TGrid() {
    this->_cleanGridContent();
    if (this->_cachedFonts->size() > 0) {
        this->cleanCachedFonts();
        delete this->_cachedFonts;
        this->_cachedFonts = NULL;
    }
}

VOID Tilc::Gui::TGrid::cleanData() {
    CPairExt* p;
    Tilc::Gui::TGridCell* c;
    Uintint size = this->_data.size();
    intint* items = this->_data.getItemsBuf();
    for (Uint i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p) {
            c = (Tilc::Gui::TGridCell*)p->second;
            delete c;
        }
    }
    // clearing below does delete on CPairExt objects
    this->_data.clear();
}

VOID Tilc::Gui::TGrid::cleanCachedFonts() {
    Uintint size = this->_cachedFonts->size();
    intint* items = this->_cachedFonts->getItemsBuf();
    CmzFont* font;
    for (Uintint i = 0; i < size; i++) {
        font = (CmzFont*)items[i];
        delete font;
    }
    this->_cachedFonts->clear();
}

CmzFont* Tilc::Gui::TGrid::getFont(const WCHAR* fontName, int fontSize, int fontColor) {
    CmzFont* defFont = this->_theme->commonGridControlFont;
    Uintint size = this->_cachedFonts->size();
    if (fontName == COMMON_EMPTY_STRING || size < 1) {
        return defFont;
    }

    intint* items = this->_cachedFonts->getItemsBuf();
    CmzFont* font;
    for (Uintint i = 0; i < size; i++) {
        font = (CmzFont*)items[i];
        if (font->name == fontName && font->size == fontSize && font->color == fontColor) {
            return font;
        }
    }

    font = new CmzFont(NULL, fontColor, fontName, fontSize);
    this->_cachedFonts->append((intint)font);

    return font;
}

VOID Tilc::Gui::TGrid::_updateCanvas() {
    this->_needUpdate = false;
    if (!this->canvas || !this->isVisible()) {
		return;
    }

	int x = 0;
    int y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    bool drawScrollbarsEmptySquare = false;
    if (this->_vscrollbar || this->_hscrollbar) {
        drawScrollbarsEmptySquare = true;
    }

    int w, h;
    Tilc::Gui::TGridHeaderCell *xheaderCell, *yheaderCell;
    SIZE cellSize;

	this->canvas->beginPaint(hdc);

    RECT clientRect, noheadersClientRect;
    int maxRightX = 0;
    int maxBottomY = 0;
    clientRect.left = 0;
    clientRect.top = 0;
    clientRect.right = this->width;
    clientRect.bottom = this->height;
    if (this->_showLeftHeader) {
        clientRect.left = this->_leftHeaderWidth - 1;
    }
    if (this->_showTopHeader) {
        clientRect.top = this->_topHeaderHeight - 1;
    }
    if (this->_vscrollbar) {
        clientRect.right -= this->_vscrollbar->width;
    }
    if (this->_hscrollbar) {
        clientRect.bottom -= this->_hscrollbar->height;
    }

    clientRect.right -= 1;      // apply border size
    clientRect.bottom -= 1;     // apply border size
    noheadersClientRect = clientRect;
    noheadersClientRect.left = 0;
    noheadersClientRect.top = 0;

    int theaderSize = this->_showTopHeader ? this->_topHeaderHeight : 0;
    int lheaderSize = this->_showLeftHeader ? this->_leftHeaderWidth : 0;

    // clear canvas
    this->canvas->fillRectangle(this->_backgroundColor, 0, 0, this->width, this->height);

    // ================================================================
    // Rysujemy lewą górną komórkę nagłówka
    // ================================================================
    if (this->_showLeftHeader && this->_showTopHeader) {
        w = this->_leftHeaderWidth;
        h = this->_topHeaderHeight;
        this->canvas->stretchBitmap(t->grid_left_top_header_inner_bg, x + 1, y + 1, w - 2, h - 2);
        this->canvas->drawBitmap(t->grid_left_top_header_mark, x + w - t->grid_left_top_header_mark->width() - 3, y + (h - t->grid_left_top_header_mark->height() - 2));
        this->canvas->drawRectangle(t->commonGridControlLeftTopHeaderCellBorderColor, x, y, x + w, y + h);
        x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
    }
    // ================================================================
    // Koniec rysowania lewej górnej komórki nagłówka
    // ================================================================

    bool selected = false, xHeaderColSelected = false, yHeaderColSelected = false;
    CmzBitmap *inner_bg;
    COLORREF border_color;
    CmzFont *f = this->getFont();
    int xIndex, yIndex;
    RECT rc;
    Uintint xheader_items_count, yheader_items_count;
    intint *xheader_items, *yheader_items;

    xheader_items_count = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    yheader_items_count = this->_rowData->size();
    yheader_items = this->_rowData->getItemsBuf();

    // ================================================================
    // Rysujemy górny nagłówek
    // ================================================================
    if (this->_showTopHeader) {
        h = this->_topHeaderHeight;
        xIndex = this->_startCellCoordX;
        while ((x < this->_dataCanvasWidth + lheaderSize) && (xIndex-1 < xheader_items_count)) {
            xheaderCell = (Tilc::Gui::TGridHeaderCell*)xheader_items[xIndex-1];
            w = xheaderCell->size;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (xIndex == this->_coordX) {
                xHeaderColSelected = true;
            } else {
                if (xheaderCell) {
                    xHeaderColSelected = xheaderCell->selected;
                } else {
                    xHeaderColSelected = false;
                }
            }

            if (xHeaderColSelected) {
                inner_bg = t->grid_top_header_inner_bg_selected_cell;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_top_header_inner_bg_normal_cell;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            this->canvas->drawRectangle(border_color, rc);
            if (xheaderCell && xIndex <= this->_maxColumnNumber) {
                this->canvas->drawText(f, xheaderCell->text, rc, true, 0, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0);
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
    if (this->_showLeftHeader) {
        x = 0;
        if (this->_showTopHeader) {
            y = t->grid_top_header_inner_bg_normal_cell->height() + 2 - 1;
        } else {
            y = 0;
        }
        w = this->_leftHeaderWidth;
        yIndex = this->_startCellCoordY;
        while ((y < this->_dataCanvasHeight + theaderSize) && (yIndex-1 < yheader_items_count)) {
            yheaderCell = (Tilc::Gui::TGridHeaderCell*)yheader_items[yIndex-1];
            h = yheaderCell->size;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (yIndex == this->_coordY) {
                yHeaderColSelected = true;
            } else {
                if (yheaderCell) {
                    yHeaderColSelected = yheaderCell->selected;
                } else {
                    yHeaderColSelected = false;
                }
            }

            if (yHeaderColSelected) {
                inner_bg = t->grid_left_header_inner_bg_selected_cell;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_left_header_inner_bg_normal_cell;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            this->canvas->drawRectangle(border_color, rc);
            if (yheaderCell && yIndex <= this->_maxRowNumber) {
                this->canvas->drawText(f, yheaderCell->text, rc, true, 0, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0);
            }

            y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            yIndex++;
        }
    }
    // ================================================================
    // Koniec rysowania lewego nagłówka
    // ================================================================


    Tilc::Gui::TGridCell *cellData;
    CPairExt *cellPair;
    intint coord;
    intint foundCellPairPosition;
    // ================================================================
    // Rysujemy komórki wewnętrzne (z danymi)
    // ================================================================
    x = lheaderSize ? lheaderSize - 1 : 0;
    y = theaderSize ? theaderSize - 1 : 0;
    w = this->_defColumnWidth;
    h = t->grid_cell_inner_bg_normal->height();

    xIndex = this->_startCellCoordX;
    yIndex = this->_startCellCoordY;
    int startX = x;

    while ((y <= this->_dataCanvasHeight + theaderSize) && (yIndex <= yheader_items_count)) {
        x = startX;
        xIndex = this->_startCellCoordX;
        while ((x <= this->_dataCanvasWidth + lheaderSize) && (xIndex <= xheader_items_count)) {
            xheaderCell = (Tilc::Gui::TGridHeaderCell*)xheader_items[xIndex-1];
            yheaderCell = (Tilc::Gui::TGridHeaderCell*)yheader_items[yIndex-1];
            w = xheaderCell->size;
            h = yheaderCell->size;

            coord = this->_cellCoordsToint(xIndex, yIndex);
            cellPair = this->_data.findFirstItemByFirst(coord, &foundCellPairPosition);
            if (cellPair) {
                cellData = (Tilc::Gui::TGridCell*)cellPair->second;
                selected = cellData->selected || (this->_entireRowSelect && (xIndex == this->_coordX)) || (this->_entireColumnSelect && (yIndex == this->_coordY));
            } else {
                selected = (this->_entireRowSelect && (xIndex == this->_coordX)) || (this->_entireColumnSelect && (yIndex == this->_coordY));
            }

            if (selected) {
                inner_bg = t->grid_cell_inner_bg_selected;
                border_color = t->commonGridControlCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_cell_inner_bg_normal;
                border_color = t->commonGridControlCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            if (this->_drawVerticalLines && this->_drawHorizontalLines) {
                this->canvas->drawRectangle(border_color, rc);
            } else {
                if (this->_drawHorizontalLines) {
                    this->canvas->drawHLine(border_color, rc.left, rc.top, w);
                    this->canvas->drawHLine(border_color, rc.left, rc.top + h - 1, w);
                } else if (yIndex == this->_maxRowNumber) {
                    // last (closing) grid horizontal line we draw always
                    this->canvas->drawHLine(border_color, rc.left, rc.top + h - 1, w);
                }
                if (this->_drawVerticalLines) {
                    this->canvas->drawVLine(border_color, rc.left, rc.top, h);
                    this->canvas->drawVLine(border_color, rc.left + w - 1, rc.top, h);
                } else if (xIndex == this->_maxColumnNumber) {
                    // last (closing) grid vertical line we draw always
                    this->canvas->drawVLine(border_color, rc.left + w - 1, rc.top, h);
                }
            }

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex++;
        }

        y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
    // ================================================================
    // Koniec rysowania komórek wewnętrznych (z danymi)
    // ================================================================


    int cellMarginLeft = 2;
    int cellMarginRight = 2;
    Tilc::TExtString cellValue;
    // ================================================================
    // Rysujemy zawartości (dane) komórek wewnętrznych
    // ================================================================
    x = this->_leftHeaderWidth - 1;
    y = t->grid_top_header_inner_bg_normal_cell->height() + 2 - 1;
    w = this->_defColumnWidth;
    h = t->grid_cell_inner_bg_normal->height();
    xIndex = this->_startCellCoordX;
    yIndex = this->_startCellCoordY;
    while (y < this->_dataCanvasHeight && (yIndex - this->_startCellCoordY < yheader_items_count)) {
        x = this->_leftHeaderWidth - 1;
        xIndex = this->_startCellCoordX;
        while (x < this->_dataCanvasWidth && (xIndex - this->_startCellCoordX < xheader_items_count)) {
            xheaderCell = (Tilc::Gui::TGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
            yheaderCell = (Tilc::Gui::TGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
            w = xheaderCell->size;
            h = yheaderCell->size - 2;

            if (selected) {
                inner_bg = t->grid_cell_inner_bg_selected;
                border_color = t->commonGridControlCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_cell_inner_bg_normal;
                border_color = t->commonGridControlCellBorderColor_Normal;
            }


            rc.left = x + cellMarginLeft;
            rc.top = y;
            rc.right = x + w - 1 - cellMarginRight;
            rc.bottom = y + h + 2;

            coord = this->_cellCoordsToint(xIndex, yIndex);
            cellPair = this->_data.findFirstItemByFirst(coord, &foundCellPairPosition);
            if (!cellPair) {
                cellValue = COMMON_EMPTY_STRING;
            } else {
                cellData = (Tilc::Gui::TGridCell*)cellPair->second;
                cellValue = cellData->value;
            }

            this->canvas->drawText(f, cellValue, rc, true, DT_VCENTER, -1, 0);

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex++;
        }

        y += h + 2 - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
    // ================================================================
    // Koniec rysowania zawartości (dane) komórek wewnętrznych
    // ================================================================

    // ================================================================
    // Rysowanie końcowych obramowań
    // ================================================================
    // Obramowanie całego Grida
    if (this->_state == Tilc::Gui::TGrid_STATE_FOCUSED) {
        this->canvas->drawRectangle(t->commonGridControlFocusedGridBorderColor, 0, 0, this->width, this->height);
    } else {
        this->canvas->drawRectangle(t->commonGridControlLeftTopHeaderCellBorderColor, 0, 0, this->width, this->height);
    }

    // Obramowanie dolne górnego nagłówka (gdyż zostało nadpisane przez komórki)
    if (this->_showTopHeader) {
        x = 1;
        y = this->_topHeaderHeight - 1;
        this->canvas->drawLine(t->commonGridControlTopHeaderCellBorderColor_Normal, x, y, this->_dataCanvasWidth + lheaderSize - 1, y);
    }
    // Obramowanie prawe lewego nagłówka (gdyż zostało nadpisane przez komórki)
    if (this->_showLeftHeader) {
        x = this->_leftHeaderWidth - 1;
        y = 1;
        this->canvas->drawLine(t->commonGridControlTopHeaderCellBorderColor_Normal, x, y, x, this->_dataCanvasHeight + theaderSize - 1);
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
    *-/
    // ================================================================
    // Rysowanie aktywnej komórki (a właściwie jej obramowania)
    // ================================================================
    if (this->cellVisible(this->_coordX, this->_coordY, true)) {
        this->cellCoordsToCanvasCoords(this->_coordX, this->_coordY, &x, &y);
        // Inkrementujemy obie współrzedne, bo prostokąt rysujemy pogrubioną linią
        x += 1;
        y += 1;
        cellSize = this->getCellSize(this->_coordX, this->_coordY);
        rc.left = x;
        rc.top = y;
        rc.right = x + cellSize.cx - 1; // -1, bo prostokąt rysujemy pogrubioną linią
        rc.bottom = y + cellSize.cy - 1; // -1, bo prostokąt rysujemy pogrubioną linią
        this->canvas->drawRectangle(t->commonGridControlCellBorderColor_Active, rc, 2);
        rc.left = rc.right - 4;
        rc.right += 3;
        rc.top = rc.bottom - 4;
        rc.bottom += 3;
        this->canvas->fillRectangle(RGB(0xff, 0xff, 0xff), t->commonGridControlCellBorderColor_Active, rc, 1);
    }
    // ================================================================
    // Koniec rysowania aktywnej komórki (a właściwie jej obramowania)
    // ================================================================

    if (drawScrollbarsEmptySquare) {
        int scw = 0;
        int sch = 0;
        if (this->_vscrollbar) {
            scw = this->_vscrollbar->width;
        }
        if (this->_hscrollbar) {
            sch = this->_hscrollbar->height;
        }
        this->canvas->fillRectangle(RGB(0xff, 0xff, 0xff), this->width - scw, this->height - sch, scw, sch);
    }

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID Tilc::Gui::TGrid::onDraw(CmzBitmap *dest) {
    if (!this->_visible) return;
	if (!this->canvas) return;

    if (this->_needUpdate) {
        this->invalidateChildSpritesCanvases();
        this->_updateCanvas();
    }

    // ================================================================
    // drawing child-sprites
    // ================================================================
	HDC hdc = GetDC(0);
	this->canvas->beginPaint(hdc);
    this->drawChildSprites(this->canvas);
	this->canvas->endPaint();
	ReleaseDC(0, hdc);
    // ================================================================
    // end of drawing child-sprites
    // ================================================================

    dest->drawBitmap(this->canvas, this->x, this->y, this->_currentFrame);
}


VOID Tilc::Gui::TGrid::focus() {
    CStyledWindow* wnd = this->getParentWindow();
    if (wnd && wnd->getActiveControl() != this) {
        wnd->setActiveControl(this);
        return;
    }
    this->setState(Tilc::Gui::TGrid_STATE_FOCUSED);
}

VOID Tilc::Gui::TGrid::looseFocus() {
    //CStyledWindow* wnd = this->getParentWindow();
    //int x = MININT, y = MININT;
    //wnd->setOnlyActiveControlPointer(NULL);
    // this->getCurrentMousePosition(x, y);

    this->setState(Tilc::Gui::TGrid_STATE_NORMAL);
}

Tilc::TExtString Tilc::Gui::TGrid::_indexToLetter(int index) {
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ - 26 liter
    WCHAR* letters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // dopuszczamy numery maksymalnie 3-literowe
    if (index < 1 || index > 17576) { // 17576 = 26 * 26 * 26
        return COMMON_EMPTY_STRING;
    }

    if (index <= 26) {
        return Tilc::TExtString(letters[index-1]);
    }
    if (index <= 676) { // 676 = 26 * 26
        return Tilc::TExtString(letters[index / 26 - 1]) + Tilc::TExtString(letters[index % 26]);
    }
    if (index <= 17576) { // 17576 = 26 * 26 * 26
        int tmp = index / 26;
        return Tilc::TExtString(letters[tmp / 26 - 1]) + Tilc::TExtString(letters[tmp % 26]) + Tilc::TExtString(letters[index % 26]);
    }

    return COMMON_EMPTY_STRING;
}

int Tilc::Gui::TGrid::_cellCoordsToint(int x, int y) {
    DWORD result = (((DWORD)y) << (sizeof(WORD) << 3)) | ((WORD)x);
    return (int)result;
}

VOID Tilc::Gui::TGrid::_intToCellCoords(int data, int* x, int* y) {
    *x = LOWORD((DWORD)data);
    *y = HIWORD((DWORD)data);
}

VOID Tilc::Gui::TGrid::cellCoordsToCanvasCoords(int cellCoordX, int cellCoordY, int* x, int* y) {
    *x = this->_showLeftHeader ? this->_leftHeaderWidth - 1 : 0;
    *y = this->_showTopHeader ? this->_topHeaderHeight - 1 : 0;

    int xIndex = this->_startCellCoordX;
    int yIndex = this->_startCellCoordY;
    Tilc::Gui::TGridHeaderCell *headerData;
    Uintint xheader_items_count, yheader_items_count;
    intint *xheader_items, *yheader_items;

    xheader_items_count = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    yheader_items_count = this->_rowData->size();
    yheader_items = this->_rowData->getItemsBuf();

    while (xIndex < cellCoordX) {
        headerData = (Tilc::Gui::TGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
        *x += headerData->size - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        xIndex++;
    }

    while (yIndex < cellCoordY) {
        headerData = (Tilc::Gui::TGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
        *y += headerData->size - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
}

SIZE Tilc::Gui::TGrid::getCellSize(int cellCoordX, int cellCoordY) {
    SIZE retval;
    retval.cx = 0;
    retval.cy = 0;
    Tilc::Gui::TGridHeaderCell* headerData;

    headerData = this->getHeaderCellX(cellCoordX);
    if (headerData) {
        retval.cx = headerData->size;
    }
    headerData = this->getHeaderCellY(cellCoordY);
    if (headerData) {
        retval.cy = headerData->size;
    }

    return retval;
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::getCell(int cellCoordX, int cellCoordY) {
    intint pos;
    intint first = this->_cellCoordsToint(cellCoordX, cellCoordY);
    CPairExt* p = this->_data.findFirstItemByFirst(first, &pos);
    if (p) {
        return (Tilc::Gui::TGridCell*)p->second;
    }
    return NULL;
}

Tilc::Gui::TGridCell* Tilc::Gui::TGrid::getCurrentCell() {
    return this->getCell(this->_coordX, this->_coordY);
}

Tilc::Gui::TGridHeaderCell* Tilc::Gui::TGrid::getHeaderCellX(int coordX) {
    return (Tilc::Gui::TGridHeaderCell*)this->_colData->getAt(coordX-1);
}

Tilc::Gui::TGridHeaderCell* Tilc::Gui::TGrid::getCurrentHeaderCellX() {
    return (Tilc::Gui::TGridHeaderCell*)this->_colData->getAt(this->_coordX-1);
}

Tilc::Gui::TGridHeaderCell* Tilc::Gui::TGrid::getHeaderCellY(int coordY) {
    return (Tilc::Gui::TGridHeaderCell*)this->_rowData->getAt(coordY-1);
}

Tilc::Gui::TGridHeaderCell* Tilc::Gui::TGrid::getCurrentHeaderCellY() {
    return (Tilc::Gui::TGridHeaderCell*)this->_rowData->getAt(this->_coordY-1);
}

VOID Tilc::Gui::TGrid::setStartCellCoordX(int coordX) {
    if (coordX <= 0) {
        return;
    }

    this->_startCellCoordX = coordX;

    if (this->_startCellCoordX < 1) {
        this->_startCellCoordX = 1;
        return;
    }

    if (this->_startCellCoordX > this->_maxAllowableStartCellCoordX) {
        this->_startCellCoordX = this->_maxAllowableStartCellCoordX;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, this->_startCellCoordX, this->_startCellCoordY));
}

VOID Tilc::Gui::TGrid::setStartCellCoordY(int coordY) {
    if (coordY <= 0) {
        return;
    }

    this->_startCellCoordY = coordY;

    if (this->_startCellCoordY < 1) {
        this->_startCellCoordY = 1;
        return;
    }

    if (this->_startCellCoordY > this->_maxAllowableStartCellCoordY) {
        this->_startCellCoordY = this->_maxAllowableStartCellCoordY;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, this->_startCellCoordX, this->_startCellCoordY));
}

bool Tilc::Gui::TGrid::moveRight(int count) {
    if (this->_coordX < this->_maxColumnNumber) {
        if (this->_coordX + count <= this->_maxColumnNumber) {
            this->_coordX += count;
        } else {
            count = this->_maxColumnNumber - this->_coordX;
            this->_coordX = this->_maxColumnNumber;
        }
        if (this->_coordX > this->_maxFullVisibleColumnNumber && this->_coordX < this->_maxColumnNumber) {
            this->setStartCellCoordX(this->_startCellCoordX + count);
            this->_updateGridParameters();
        } else if (this->_coordX == this->_maxColumnNumber) {
            this->setStartCellCoordX(this->_maxColumnNumber);
            this->_updateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::moveLeft(int count) {
    if (this->_coordX > 1) {
        if (this->_coordX - count >= 1) {
            this->_coordX -= count;
        } else {
            this->_coordX = 1;
        }
        if (this->_startCellCoordX > this->_coordX) {
            this->setStartCellCoordX(this->_coordX);
            this->_updateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::moveUp(int count) {
    if (this->_coordY > 1) {
        if (this->_coordY - count >= 1) {
            this->_coordY -= count;
        } else {
            this->_coordY = 1;
        }
        if (this->_startCellCoordY > this->_coordY) {
            this->setStartCellCoordY(this->_coordY);
            this->_updateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::moveDown(int count) {
    if (this->_coordY < this->_maxRowNumber) {
        if (this->_coordY + count <= this->_maxRowNumber) {
            this->_coordY += count;
        } else {
            count = this->_maxRowNumber - this->_coordY;
            this->_coordY = this->_maxRowNumber;
        }
        if (this->_coordY > this->_maxFullVisibleRowNumber) {
            this->setStartCellCoordY(this->_startCellCoordY + count);
            this->_updateGridParameters();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::movePageUp() {
    return this->moveUp(this->_yPageStep);
}

bool Tilc::Gui::TGrid::movePageDown() {
    return this->moveDown(this->_yPageStep);
}

bool Tilc::Gui::TGrid::movePageLeft() {
    return this->moveLeft(this->_xPageStep);
}

bool Tilc::Gui::TGrid::movePageRight() {
    return this->moveRight(this->_xPageStep);
}

bool Tilc::Gui::TGrid::moveHome() {
    return this->moveToCell(1, 1);
}

bool Tilc::Gui::TGrid::moveHomeVertical() {
    return this->moveToCell(this->_coordX, 1);
}

bool Tilc::Gui::TGrid::moveHomeHorizontal() {
    return this->moveToCell(1, this->_coordY);
}

bool Tilc::Gui::TGrid::moveEnd() {
    return this->moveToCell(this->_maxColumnNumber, this->_maxRowNumber);
}

bool Tilc::Gui::TGrid::moveEndVertical() {
    return this->moveToCell(this->_coordX, this->_maxRowNumber);
}

bool Tilc::Gui::TGrid::moveEndHorizontal() {
    return this->moveToCell(this->_maxColumnNumber, this->_coordY);
}

bool Tilc::Gui::TGrid::moveToCell(int coordX, int coordY) {
    if (
        coordX < 1 || coordX > this->_maxColumnNumber ||
        coordY < 1 || coordY > this->_maxRowNumber
       ) {
        return false;
    }
    int deltaX = coordX - this->_coordX;
    int deltaY = coordY - this->_coordY;
    if (this->cellVisible(this->_coordX + deltaX, this->_coordY + deltaY)) {
        this->_coordX = coordX;
        this->_coordY = coordY;
    } else {
        this->_coordX = coordX;
        this->_coordY = coordY;
        this->setStartCellCoordX(this->_startCellCoordX + deltaX > 0 ? this->_startCellCoordX + deltaX : 1);
        this->setStartCellCoordY(this->_startCellCoordY + deltaY > 0 ? this->_startCellCoordY + deltaY : 1);
        this->_updateGridParameters();
    }

    return deltaX != 0 || deltaY != 0;
}

bool Tilc::Gui::TGrid::cellVisible(int coordX, int coordY, bool acceptPartialVisibility) {
    int maxCoordX, maxCoordY;
    if (!acceptPartialVisibility) {
        maxCoordX = this->_maxFullVisibleColumnNumber;
        maxCoordY = this->_maxFullVisibleRowNumber;
    } else {
        maxCoordX = this->_maxVisibleColumnNumber;
        maxCoordY = this->_maxVisibleRowNumber;
    }

    if (
        (this->_startCellCoordX <= coordX) && (coordX <= maxCoordX) &&
        (this->_startCellCoordY <= coordY) && (coordY <= maxCoordY)) {
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::getCellCoordsAtMousePos(int mouseX, int mouseY, int* coordX, int* coordY) {
    *coordX = -1;
    *coordY = -1;

    int startMouseX;
    int startMouseY;
    this->cellCoordsToCanvasCoords(this->_startCellCoordX, this->_startCellCoordY, &startMouseX, &startMouseY);

    int xIndex = this->_startCellCoordX;
    int yIndex = this->_startCellCoordY;
    Tilc::Gui::TGridHeaderCell *headerData;
    intint *xheader_items, *yheader_items;

    if (mouseX < startMouseX) {
        mouseX = startMouseX;
    }
    if (mouseY < startMouseY) {
        mouseY = startMouseY;
    }

    bool coordSet = false;
    Uintint size = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    int curX = startMouseX;
    int curY = startMouseY;
    while ((xIndex - this->_startCellCoordX < size) && (curX <= this->width)) {
        headerData = (Tilc::Gui::TGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
        if (curX <= mouseX && mouseX <= curX + headerData->size) {
            coordSet = true;
            break;
        }
        curX += headerData->size - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
        xIndex += 1;
    }
    if (coordSet) {
        *coordX = xIndex;
        coordSet = false;
        size = this->_rowData->size();
        yheader_items = this->_rowData->getItemsBuf();
        while ((yIndex - this->_startCellCoordY < size) && (curY <= this->height)) {
            headerData = (Tilc::Gui::TGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
            if (curY <= mouseY && mouseY <= curY + headerData->size) {
                coordSet = true;
                break;
            }
            curY += headerData->size - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
            yIndex += 1;
        }
        if (coordSet) {
            *coordY = yIndex;
        } else {
            *coordX = -1;
        }
    }

    if (*coordX != -1 && *coordY != -1) {
        return true;
    }

    return false;
}

bool Tilc::Gui::TGrid::activateCellAtMousePos(int x, int y) {
    int coordX, coordY;
    if (this->getCellCoordsAtMousePos(x, y, &coordX, &coordY)) {
        this->_coordX = coordX;
        this->_coordY = coordY;
        return true;
    }

    return false;
}

VOID Tilc::Gui::TGrid::clearSelection(bool redraw) {
    bool had_selection = false;

    CPairExt *p;
    Tilc::Gui::TGridCell *gridCell;
    Uintint items_count = (int)this->_data.size();
    intint *items = this->_data.getItemsBuf();

    for (int i = 0; i < items_count; i++) {
        p = (CPairExt*)items[i];
        gridCell = (Tilc::Gui::TGridCell*)p->second;
        if (gridCell) {
            if (gridCell->selected) {
                had_selection = true;
                gridCell->selected = false;
            }
        }
    }

    if (!had_selection) {
        redraw = false;
    }

    if (redraw) {
        this->redraw();
    }
}

bool Tilc::Gui::TGrid::isSelection() {
    bool is_selection = false;

    CPairExt *p;
    Tilc::Gui::TGridCell *gridCell;
    Uintint items_count = (int)this->_data.size();
    intint *items = this->_data.getItemsBuf();

    for (int i = 0; i < items_count; i++) {
        p = (CPairExt*)items[i];
        gridCell = (Tilc::Gui::TGridCell*)p->second;
        if (gridCell) {
            if (gridCell->selected) {
                is_selection = true;
                break;
            }
        }
    }

    return is_selection;
}

bool Tilc::Gui::TGrid::_commonKeyProcessing(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch, bool& redraw) {
    redraw = false;
    return false;
}

bool Tilc::Gui::TGrid::onKeyDown(bool vkAlt, bool vkShift, bool vkControl,
    bool vkLAlt, bool vkRAlt,
    bool vkLShift, bool vkRShift,
    bool vkLControl, bool vkRControl,
    bool systemKey,
    Uint virtualCode, Uint scanCode, WCHAR ch) {

    bool redraw = false;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, redraw);
    }

    if (redraw) {
        this->redraw();
    }

    return true;
}

bool Tilc::Gui::TGrid::onKeyPressed(bool vkAlt, bool vkShift, bool vkControl,
    bool vkLAlt, bool vkRAlt,
    bool vkLShift, bool vkRShift,
    bool vkLControl, bool vkRControl,
    bool systemKey,
    Uint virtualCode, Uint scanCode, WCHAR ch) {

    bool redraw = false;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        bool process = !this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, redraw);

        if (process) {
            CKeyboard* kbd = this->getKbd();

            bool activeCellChanged = false;
            switch (virtualCode) {
            case VK_DELETE:
                this->clearCurrentCellValue();
                redraw = true;
                break;
            case VK_RIGHT:
                if (vkControl) {
                    activeCellChanged = this->moveEndHorizontal();
                } else {
                    activeCellChanged = this->moveRight();
                }
                break;
            case VK_LEFT:
                if (vkControl) {
                    activeCellChanged = this->moveHomeHorizontal();
                } else {
                    activeCellChanged = this->moveLeft();
                }
                break;
            case VK_UP:
                if (vkControl) {
                    activeCellChanged = this->moveHomeVertical();
                } else {
                    activeCellChanged = this->moveUp();
                }
                break;
            case VK_DOWN:
                if (vkControl) {
                    activeCellChanged = this->moveEndVertical();
                } else {
                    activeCellChanged = this->moveDown();
                }
                break;
            case VK_NEXT:
                activeCellChanged = this->movePageDown();
                break;
            case VK_PRIOR:
                activeCellChanged = this->movePageUp();
                break;
            case VK_HOME:
                activeCellChanged = this->moveHome();
                break;
            case VK_END:
                activeCellChanged = this->moveEnd();
                break;
            }

            if (activeCellChanged) {
                this->_updateScrollbars();
                redraw = activeCellChanged;
            }
        }
    }

    //InformationBox(FormatMessage(L"_coordX: %d, _coordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
    //        this->_coordX, this->_coordY, this->_startCellCoordX, this->_startCellCoordY));
    if (redraw) {
        this->redraw();
    }

    return true;
}

bool Tilc::Gui::TGrid::onKeyUp(bool vkAlt, bool vkShift, bool vkControl,
    bool vkLAlt, bool vkRAlt,
    bool vkLShift, bool vkRShift,
    bool vkLControl, bool vkRControl,
    bool systemKey,
    Uint virtualCode, Uint scanCode, WCHAR ch) {

    return true;
}

bool Tilc::Gui::TGrid::onMouseMove(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((intint)spriteThatCapturedMouse)) {
        return false;
    }

    if (this->pointIn(x, y)) {
        bool result = this->doChildOnMouseMove(x - this->x, y - this->y);
        if (result) {
            return true;
        }

        if (this->_lMouseButtonPressed) {
        }

        return true;
    } else {
        this->doChildOnMouseMove(-1, -1);
    }

    return false;
}

bool Tilc::Gui::TGrid::onMouseDown(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((intint)spriteThatCapturedMouse)) {
        return false;
    }

    if (this->pointIn(x, y)) {
        bool result = this->doChildOnMouseDown(x - this->x, y - this->y);
        if (result) {
            return true;
        }

        this->_lMouseButtonPressed = true;
        this->getParentWindow()->captureMouse(this);

        // pozycjonujemy karetkę na odpowiednim znaku
        this->activateCellAtMousePos(x - this->x, y - this->y);

        if (!(this->_state & Tilc::Gui::TGrid_STATE_FOCUSED)) {
            this->_state = Tilc::Gui::TGrid_STATE_FOCUSED;
            this->getParentWindow()->setActiveControl(this);
        }

        this->clearSelection(false);
        this->redraw();
        return true;
    } else {
        this->doChildOnMouseDown(-1, -1);
    }

    return false;
}

bool Tilc::Gui::TGrid::onMouseUp(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((intint)spriteThatCapturedMouse)) {
        return false;
    }

    this->_lMouseButtonPressed = false;
    if (spriteThatCapturedMouse == this) {
        this->getParentWindow()->captureMouse(NULL);
    }

    if (this->pointIn(x, y)) {
        bool result = this->doChildOnMouseUp(x - this->x, y - this->y);
        if (result) {
            return true;
        }
        return true;
    } else {
        this->doChildOnMouseUp(x - this->x, y - this->y);
    }

    return false;
}

VOID Tilc::Gui::TGrid::onThumbChange(int oldPosition, int curPosition, CScrollbar* scrollbar) {
    float CurPositionPercent = ((float)curPosition - scrollbar->getMinValue()) / (scrollbar->getMaxValue() - scrollbar->getMinValue());
    if (scrollbar == this->_hscrollbar) {
        this->_startCellCoordX = static_cast<int>(CurPositionPercent * (this->_maxAllowableStartCellCoordX-1)) + 1; //curPosition + 1;
        this->_updateGridParameters();
        this->redraw();
    }
    if (scrollbar == this->_vscrollbar) {
        this->_startCellCoordY = static_cast<int>(CurPositionPercent * (this->_maxAllowableStartCellCoordY-1)) + 1; //curPosition + 1;
        this->_updateGridParameters();
        this->redraw();
    }
}

Uint Tilc::Gui::TGrid::getTotalColumnsWidth() {
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    Tilc::Gui::TGridHeaderCell* headerData;
    Uint totalWidth = 0;
    for (i = 0; i < this->_maxColumnNumber; i++) {
        headerData = (Tilc::Gui::TGridHeaderCell*)this->_colData->getAt(i);
        totalWidth += headerData->size;
    }

    return totalWidth;
}

Uint Tilc::Gui::TGrid::getTotalRowsHeight() {
    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    Tilc::Gui::TGridHeaderCell* headerData;
    Uint totalHeight = 0;
    for (i = 0; i < this->_maxRowNumber; i++) {
        headerData = (Tilc::Gui::TGridHeaderCell*)this->_rowData->getAt(i);
        totalHeight += headerData->size;
    }

    return totalHeight;
}

int Tilc::Gui::TGrid::getVisibleColumnCount() {
    int count = (this->_maxVisibleColumnNumber - this->_startCellCoordX) + 1;
    //WriteLogMessage(L"_maxVisibleColumnNumber: %d, : _startCellCoordX: %d", this->_maxVisibleColumnNumber, this->_startCellCoordX);
    return count;
}

int Tilc::Gui::TGrid::getFullVisibleColumnCount() {
    int count = (this->_maxFullVisibleColumnNumber - this->_startCellCoordX) + 1;
    return count;
}

int Tilc::Gui::TGrid::getVisibleRowCount() {
    int count = (this->_maxVisibleRowNumber - this->_startCellCoordY) + 1;
    //WriteLogMessage(L"_maxVisibleRowNumber: %d, : _startCellCoordY: %d", this->_maxVisibleRowNumber, this->_startCellCoordY);
    return count;
}

int Tilc::Gui::TGrid::getFullVisibleRowCount() {
    int count = (this->_maxFullVisibleRowNumber - this->_startCellCoordY) + 1;
    return count;
}

VOID Tilc::Gui::TGrid::onApplyEditorChanges(CmzStdObject* value) {
    Tilc::TExtString v = value->getAsString(L"text");
    this->setValue(v);
}

CmzStdObject Tilc::Gui::TGrid::getValue() {
    Tilc::Gui::TGridCell* c = this->getCell(this->_coordX, this->_coordY);
    CmzStdObject v;

    if (c) {
        v.set(L"text", c->value);
    }

    return v;
}

VOID Tilc::Gui::TGrid::setValue(Tilc::TExtString value, bool redraw) {
    this->setCellValue(this->_coordX, this->_coordY, value, redraw);
}

VOID Tilc::Gui::TGrid::setCurrentCellValue(Tilc::TExtString value, bool redraw) {
    this->setCellValue(this->_coordX, this->_coordY, value, redraw);
}

VOID Tilc::Gui::TGrid::setCellValue(int cellCoordX, int cellCoordY, Tilc::TExtString value, bool redraw) {
    Tilc::Gui::TGridCell* c = this->getCell(cellCoordX, cellCoordY);
    if (c) {
        c->value = value;
    } else {
        c = new Tilc::Gui::TGridCell(value, false);
        intint first = this->_cellCoordsToint(cellCoordX, cellCoordY);
        this->_data.set(first, (intint)c);
    }

    if (redraw) {
        this->redraw();
    }
}

VOID Tilc::Gui::TGrid::clearCurrentCellValue(bool redraw) {
    this->setCellValue(this->_coordX, this->_coordY, COMMON_EMPTY_STRING, redraw);
}

VOID Tilc::Gui::TGrid::clearCellValue(int cellCoordX, int cellCoordY, bool redraw) {
    this->setCellValue(cellCoordX, cellCoordY, COMMON_EMPTY_STRING, redraw);
}

VOID Tilc::Gui::TGrid::onEditorShow() {
    int x, y, w;
    this->cellCoordsToCanvasCoords(this->_coordX, this->_coordY, &x, &y);
    CTextField* tf = (CTextField*)this->_editor;
    this->_editor->x = x;
    this->_editor->y = y;
    w = this->width - x;
    if (this->_vscrollbar) {
        w -= this->_vscrollbar->width;
    }
    this->_editor->setSize(w - 2, this->_editor->height);
    tf->selectAll();
}

VOID Tilc::Gui::TGrid::_updateScrollbars() {
    int pos;
    if (this->_vscrollbar) {
        pos = this->_startCellCoordY-1;
        this->_vscrollbar->setPosition(pos, false, false);
    }
    if (this->_hscrollbar) {
        pos = this->_startCellCoordX-1;
        this->_hscrollbar->setPosition(pos, false, false);
    }
}

VOID Tilc::Gui::TGrid::showTopHeader(bool showTopHeader, bool redraw) {
    this->_showTopHeader = showTopHeader;
    this->_updateGridParameters();
    if (redraw) {
        this->redraw();
    }
}

VOID Tilc::Gui::TGrid::showLeftHeader(bool showLeftHeader, bool redraw) {
    this->_showLeftHeader = showLeftHeader;
    this->_updateGridParameters();
    if (redraw) {
        this->redraw();
    }
}

VOID Tilc::Gui::TGrid::setGridSize(int columnCount, int rowCount) {
    this->_maxColumnNumber = columnCount;
    this->_maxRowNumber = rowCount;

    if (this->_colData && this->_rowData) {
        this->_compactData();
        return;
    }

    if (!this->_colData) {
        this->_colData = new CmzArrayList(this->_maxColumnNumber);
    }
    if (!this->_rowData) {
        this->_rowData = new CmzArrayList(this->_maxRowNumber);
    }

    int i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    Tilc::Gui::TGridHeaderCell *headerData;
    int index;
    for (i = 0; i < this->_maxColumnNumber; i++) {
        index = i + 1;
        headerData = new Tilc::Gui::TGridHeaderCell(this->_indexToLetter(index) + L"[" + index + L"]", this->_defColumnWidth, false);
        this->_colData->insert(i, (intint)headerData);
    }
    // Inicjalizujemy właściwości komórek nagłówka lewego
    for (i = 0; i < this->_maxRowNumber; i++) {
        index = i + 1;
        headerData = new Tilc::Gui::TGridHeaderCell(Tilc::TExtString(index), this->_theme->grid_top_header_inner_bg_normal_cell->height() + 2, false);
        this->_rowData->insert(i, (intint)headerData);
    }
}

VOID Tilc::Gui::TGrid::_cleanGridContent() {
    int i;
    Tilc::Gui::TGridHeaderCell *headerCell;
    intint *items;
    Uintint items_count;

    if (this->_colData) {
        items_count = this->_colData->size();
        items = this->_colData->getItemsBuf();
        for (i = 0; i < items_count; i++) {
            headerCell = (Tilc::Gui::TGridHeaderCell*)items[i];
            if (headerCell) {
                delete headerCell;
            }
        }
        this->_colData->clear();
        delete this->_colData;
        this->_colData = NULL;
    }

    if (this->_rowData) {
        items_count = (int)this->_rowData->size();
        items = this->_rowData->getItemsBuf();
        for (i = 0; i < items_count; i++) {
            headerCell = (Tilc::Gui::TGridHeaderCell*)items[i];
            if (headerCell) {
                delete headerCell;
            }
        }
        this->_rowData->clear();
        delete this->_rowData;
        this->_rowData = NULL;
    }

    if (this->_data.size() > 0) {
        this->cleanData();
    }
}

VOID Tilc::Gui::TGrid::_compactData() {
    CPairExt* p;
    Tilc::Gui::TGridCell* c;
    Uintint size = this->_data.size();
    intint* items = this->_data.getItemsBuf();
    int x,y;
    Uint i;

    for (i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p) {
            this->_intToCellCoords(static_cast<int>(p->first), &x, &y);
            if (x > this->_maxColumnNumber || y > this->_maxRowNumber) {
                c = (Tilc::Gui::TGridCell*)p->second;
                delete c;
                items[i] = NULL;
            }
        }
    }
    // throw off all NULL-s
    this->_data.remove(NULL);

    bool removeNulls = false;
    Tilc::Gui::TGridHeaderCell* hc;
    size = this->_colData->size();
    items = this->_colData->getItemsBuf();
    for (i = this->_maxColumnNumber; i < size; i++) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (hc) {
            items[i] = NULL;
            delete hc;
            removeNulls = true;
        }
    }
    if (removeNulls) {
        this->_colData->del(0);
    }

    removeNulls = false;
    size = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    for (i = this->_maxRowNumber; i < size; i++) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (hc) {
            items[i] = NULL;
            delete hc;
            removeNulls = true;
        }
    }
    if (removeNulls) {
        this->_rowData->del(0);
    }
}

VOID Tilc::Gui::TGrid::_updateGridParameters() {
    int size;
    int maxAllowableSize;

    // data width
    Tilc::Gui::TGridHeaderCell* hc;
    int i;
    Uintint itemsize;
    intint* items;
    int curX, curY;

    this->_maxFullVisibleColumnNumber = this->_startCellCoordX;
    this->_maxVisibleColumnNumber = this->_startCellCoordX;
    this->_maxFullVisibleRowNumber = this->_startCellCoordY;
    this->_maxVisibleRowNumber = this->_startCellCoordY;
    this->_visibleWidthOfLastColumn = 0;
    this->_visibleHeightOfLastRow = 0;

    // X-based parameters
    curX = this->_showLeftHeader ? this->_leftHeaderWidth - 1 : 0;
    itemsize = this->_colData->size();
    items = this->_colData->getItemsBuf();
    size = 0;
    maxAllowableSize = this->width;
    if (this->_showLeftHeader) {
        maxAllowableSize -= this->_leftHeaderWidth;
    }
    if (this->_vscrollbar) {
        maxAllowableSize -= this->_vscrollbar->width;
    }
    for (i = this->_startCellCoordX - 1; i < itemsize; i++) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (hc) {
            if (curX + hc->size <= maxAllowableSize) {
                this->_maxFullVisibleColumnNumber = i+1;
                this->_maxVisibleColumnNumber = i+1;
            } else {
                if (curX < maxAllowableSize) {
                    this->_maxVisibleColumnNumber = i+1;
                    this->_visibleWidthOfLastColumn = maxAllowableSize - curX;
                }
            }
            size += hc->size-1; // -1 because cells share borders
            curX += hc->size-1;
            if (size > this->width) {
                break;
            }
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize) {
        size = maxAllowableSize;
    }
    this->_dataCanvasWidth = size;




    // Y based parameters
    curY = this->_showTopHeader ? this->_topHeaderHeight - 1 : 0;
    itemsize = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    size = 0;
    maxAllowableSize = this->height;
    if (this->_showTopHeader) {
        maxAllowableSize -= this->_topHeaderHeight;
    }
    if (this->_hscrollbar) {
        maxAllowableSize -= this->_hscrollbar->height;
    }
    for (i = this->_startCellCoordY - 1; i < itemsize; i++) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (hc) {
            if (curY + hc->size <= maxAllowableSize) {
                this->_maxFullVisibleRowNumber = i+1;
                this->_maxVisibleRowNumber = i+1;
            } else {
                if (curY < maxAllowableSize) {
                    this->_maxVisibleRowNumber = i+1;
                    this->_visibleHeightOfLastRow = maxAllowableSize - curY;
                }
            }
            size += hc->size-1; // -1 because cells share borders
            curY += hc->size-1;
            if (size > this->height) {
                break;
            }
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize) {
        size = maxAllowableSize;
    }
    this->_dataCanvasHeight = size;


    itemsize = this->_colData->size();
    items = this->_colData->getItemsBuf();
    size = 0;
    i = this->_maxColumnNumber-1;
    this->_maxAllowableStartCellCoordX = this->_maxColumnNumber;
    while (i >= 0) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (size + hc->size <= this->_dataCanvasWidth) {
            this->_maxAllowableStartCellCoordX = i+1;
            size += (hc->size-1); // -1 because cells share borders
        } else {
            break;
        }
        i -= 1;
    }

    itemsize = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    size = 0;
    i = this->_maxRowNumber - 1;
    this->_maxAllowableStartCellCoordY = this->_maxRowNumber;
    while (i >= 0) {
        hc = (Tilc::Gui::TGridHeaderCell*)items[i];
        if (size + hc->size <= this->_dataCanvasHeight) {
            this->_maxAllowableStartCellCoordY = i+1;
            size += (hc->size-1); ; // -1 because cells share borders
        } else {
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
    *-/
}

VOID Tilc::Gui::TGrid::_setHeaderCaptions(CmzArrayList* headerCells, Tilc::TExtStringList* captions) {
    if (!headerCells || !captions) {
        return;
    }

    Uintint hsize = headerCells->size();
    Uintint csize = captions->size();
    Tilc::Gui::TGridHeaderCell* hc;
    if (!hsize || !csize) {
        return;
    }

    for (Uintint i = 0; i < hsize; i++) {
        hc = (Tilc::Gui::TGridHeaderCell*)headerCells->getAt(i);
        if (hc && i < csize) {
            hc->text = captions->getAt(i);
        }
    }
}

VOID Tilc::Gui::TGrid::setLeftHeaderCaptions(Tilc::TExtStringList* captions) {
    this->_setHeaderCaptions(this->_rowData, captions);
}

VOID Tilc::Gui::TGrid::setTopHeaderCaptions(Tilc::TExtStringList* captions) {
    this->_setHeaderCaptions(this->_colData, captions);
}

bool Tilc::Gui::TGrid::saveToFile(Tilc::TExtString fname) {
    Uint bufsize = 4096;
    BYTE* buffer = new BYTE[bufsize];

    if (buffer) {
        intint pos = 0;
        intint endpos = 0;
        Uintint size = this->_data.size();
        HANDLE fh;
        if (size > 0) {
            fh = CreateFile(fname.toWCHAR(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (fh == INVALID_HANDLE_VALUE) {
                delete[] buffer;
                return false;
            }

            bool result = true;
            CPairExt* p;
            Tilc::Gui::TGridCell* c;
            intint* items = this->_data.getItemsBuf();
            size_t len;
            Uint* plBuffer;
            for (Uint i = 0; i < size; i++) {
                p = (CPairExt*)items[i];
                if (p) {
                    c = (Tilc::Gui::TGridCell*)p->second;
                    len = c->value.length();
                    endpos = pos + sizeof(p->first) + sizeof(len) + len;
                    if (endpos >= bufsize) {
                        if (AppendToFile(fh, buffer, pos) != pos) {
                            result = false;
                            pos = 0;
                            break;
                        }
                        pos = 0;
                    }
                    plBuffer = (Uint*)&buffer[pos];
                    plBuffer[0] = static_cast<Uint>(p->first);
                    plBuffer[1] = static_cast<Uint>(len+1);
                    wcscpy_s((WCHAR*)&plBuffer[2], bufsize - pos - sizeof(p->first) - sizeof(len), c->value.toWCHAR());
                    pos += sizeof(p->first) + sizeof(len) + 2*len;
                    buffer[pos]   = 0;
                    buffer[pos+1] = 0;
                    pos += 2;
                }
            }

            if (pos > 0) {
                if (AppendToFile(fh, buffer, pos) != pos) {
                    result = false;
                }
            }
            CloseHandle(fh);
            delete[] buffer;
            return result;
        }
        delete[] buffer;
        fh = CreateFile(fname.toWCHAR(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fh != INVALID_HANDLE_VALUE) {
            CloseHandle(fh);
            return true;
        }
    }

    return false;
}

bool Tilc::Gui::TGrid::loadFromFile(Tilc::TExtString fname) {
    Uint bufsize = 4096;
    BYTE* buffer = new BYTE[bufsize];
    bool result = false;
    bool gridContentChanged = false;
    
    if (buffer) {
        HANDLE fh = CreateFile(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fh == INVALID_HANDLE_VALUE) {
            delete[] buffer;
            return 0;
        }

        DWORD read;
        DWORD offset;
        Uint* plBuffer;
        int first;
        int len;
        int hdrDataLen = sizeof(Uint) + sizeof(Uint);
        Uint dataToRead = bufsize;
        Uint readInDataAtOffset = 0;
        Uint dataProcessed = 0;
        do {
            ReadFile(fh, &buffer[readInDataAtOffset], dataToRead, &read, NULL);
            if (read > 0 || readInDataAtOffset > 0) {
                dataProcessed += read;
                offset = 0;
                while (offset + hdrDataLen <= read) {
                    plBuffer = (Uint*)&buffer[offset];
                    first = plBuffer[0];
                    len = plBuffer[1]*2;
                    if (offset + hdrDataLen + len > read) {
                        len = 0;
                    }
                    if (len > 0) {
                        Tilc::Gui::TGridCell* c = new Tilc::Gui::TGridCell((WCHAR*)&buffer[offset+hdrDataLen], false);
                        this->_data.set(first, (intint)c);
                        gridContentChanged = true;
                    }
                    offset += hdrDataLen + len;
                }
                if (offset < bufsize) {
                    CopyMemory(buffer, &buffer[offset], bufsize - offset);
                    readInDataAtOffset = bufsize - offset;
                    dataToRead = bufsize - readInDataAtOffset;
                } else {
                    readInDataAtOffset = 0;
                    dataToRead = bufsize;
                }
            }
        } while (read > 0);
        CloseHandle(fh);
        result = dataProcessed == GetFileSize(fname);
    }

    if (gridContentChanged) {
        this->redraw();
    }
    
    return result;
}
*/
