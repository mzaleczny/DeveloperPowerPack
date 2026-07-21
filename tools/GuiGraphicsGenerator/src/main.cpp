#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <svg_raster.h>
namespace fs = std::filesystem;

bool UseSDLSvgRasterizer = false;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *OutputTexture = NULL;
static int texture_width = 0;
static int texture_height = 0;
int CurrentColumnX, CurrentColumnWidth, CurrentRowHeight;
int X, Y;
std::string OutputFile = "GUI.png";
std::string ThemeDir;// = "f:\\resources\\Gtk Themes\\Relax-Light-GTK\\assets";
std::ofstream of;

constexpr const int WINDOW_WIDTH = 1920;
constexpr const int WINDOW_HEIGHT = 1080;
constexpr const int TEXTURE_WIDTH = 1920;
constexpr const int TEXTURE_HEIGHT = 1080;
constexpr const int DEFAULT_COLUMN_WIDTH = 80;


void SaveTextureToFile(SDL_Texture* Texture);
std::string FindThemeDir();
SDL_Texture* LoadSVG(const char* Filename, int Width, int Height);
void RenderWindow();
void RenderFullNonScaledTexture(std::string FileName, float x, float y, int& TexWidth, int& TexHeight);
void RenderStretchedTexture(std::string FileName, float x, float y, float w, float h, int& TexWidth, int& TexHeight);
void RenderScrollBars(const int size, std::string postfix = "");
void RenderSliders();
void RenderSvgTile(std::string Name, SDL_Rect DestRect, bool Split, bool OutputTileToDescriptionFile = true, std::string OutputPostfix = "");
void RenderButtons(std::string Name, int width, int height, bool Split = true, std::string OutputPostfix = "");
void RenderButtonsByNamesHorizontal(std::vector<std::string> Names, int width, int height, int LineBreakAfterItemNumber = 3, std::string OutputPostfix = "");
void AddX(int Value, int NextItemWidth);
void AddY(int Value, int NextItemHeight);
std::ofstream& operator<<(std::ofstream& out, const SDL_Rect& rc);

std::ofstream& operator<<(std::ofstream& out, const SDL_Rect& rc)
{
    out << rc.x << ", " << rc.y << ", " << rc.w << ", " << rc.h;
    return out;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    ThemeDir = FindThemeDir();
    if (ThemeDir.empty())
    {
        return SDL_APP_FAILURE;
    }
    of.open(ThemeDir + "/" + OutputFile + ".txt", std::ios::out | std::ios::trunc);

    SDL_SetAppMetadata("GuiTextureGenerator", "1.0", "pl.net.progmar.gui-texture-generator");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/textures", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    OutputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    SDL_SetTextureBlendMode(OutputTexture, SDL_BLENDMODE_BLEND);
    if (!OutputTexture)
    {
        SDL_Log("Couldn't create output texture");
        return SDL_APP_FAILURE;
    }

    X = 0;
    Y = 0;
    CurrentColumnWidth = 0;
    CurrentRowHeight = 0;
    CurrentColumnX = X;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderTarget(renderer, OutputTexture);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, SDL_ALPHA_OPAQUE);  /* opaque */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);  /* transparent */
    SDL_RenderClear(renderer);

    RenderWindow();
    RenderScrollBars(16);
    RenderScrollBars(10, "small");
    RenderButtons("button", 75, 25);
    AddY(7 * 25, 0);
    X = CurrentColumnX;
    RenderSliders();
    RenderButtons("textfield", 75, 25);
    AddY(7 * 25, 0);
    X = CurrentColumnX;
    RenderSvgTile("textfield-selection", { X, Y, 1, 20 }, false, true);
    AddY(20, 0);
    RenderButtons("multiline-textfield", 75, 50);
    AddY(7 * 50, 0);
    X = CurrentColumnX;
    RenderButtonsByNamesHorizontal({"checkbox", "checkbox-checked", "checkbox-checked-disabled", "checkbox-checked-focused", "checkbox-checked-hover",
        "checkbox-disabled", "checkbox-hover", "checkbox-focused"}, 20, 20, 4);
    AddY(20 * 2, 0);
    RenderButtonsByNamesHorizontal({"option", "option-checked", "option-checked-disabled", "option-checked-focused", "option-checked-hover",
        "option-disabled", "option-hover", "option-focused"}, 20, 20, 4);
    AddY(20 * 2, 0);
    RenderButtonsByNamesHorizontal({ "grid_cell_inner_bg_normal", "grid_cell_inner_bg_selected", "grid_cell_inner_bg_selected_active", "grid_left_header_inner_bg_normal_cell",
        "grid_left_header_inner_bg_selected_cell", "grid_left_top_header_inner_bg", "grid_top_header_inner_bg_normal_cell", "grid_top_header_inner_bg_selected_cell" }, 4, 30, 8);
    AddX(8 * 4, 0);
    RenderButtonsByNamesHorizontal({ "grid_left_top_header_mark" }, 4, 4, 1);
    AddX(4, 0);
    RenderButtonsByNamesHorizontal({ "listbox_frame_top_left", "listbox_frame_top_right", "listbox_frame_bottom_left", "listbox_frame_bottom_right"}, 2, 2, 10);
    AddX(4*2, 0);
    RenderButtonsByNamesHorizontal({ "listbox_frame_left", "listbox_frame_right", "listbox_frame_top", "listbox_frame_bottom" }, 1, 1, 10);
    AddY(1, 0);
    RenderButtonsByNamesHorizontal({ "listbox_bg", "listbox_bg_selected" }, 1, 1, 10);

    SDL_SetRenderTarget(renderer, nullptr);

    //return SDL_APP_CONTINUE;
    // after first iteration close application
    SaveTextureToFile(OutputTexture);

    return SDL_APP_SUCCESS;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    of.close();
    if (OutputTexture)
    {
        SDL_DestroyTexture(OutputTexture);
    }
}


void SaveTextureToFile(SDL_Texture* Texture)
{
    SDL_SetRenderTarget(renderer, Texture);
    SDL_Surface* sf = SDL_RenderReadPixels(renderer, nullptr);
    if (sf)
    {
        SDL_SavePNG(sf, (ThemeDir + "/" + OutputFile).c_str());
        SDL_DestroySurface(sf);
    }
    SDL_SetRenderTarget(renderer, NULL);
}

std::string FindThemeDir()
{
    return "d:/repos/DeveloperPowerPack/examples/07_Gui/assets/themes/Blue";
    /*
    std::string Dir = fs::current_path().string();
    bool Found = false;
    for (int i = 0; i < 10; ++i)
    {
        if (!fs::exists(fs::path(Dir + "\\assets")))
        {
            Dir += "\\..";
        }
        else
        {
            Found = true;
            Dir = Dir + "\\assets\\Relax-Light-GTK\\assets";
            break;
        }
    }

    // Call below resolves symbolic link to real destination file
    //Dir = fs::canonical(Dir).string();
    // Call below only processes '..' and '.' to create real_path, not makes any requests to filesystem
    Dir = fs::path(Dir).lexically_normal().string();
    if (Found && fs::exists(Dir))
    {
        return Dir;
    }

    return {};
    */
}

void RenderWindow()
{
    SDL_FRect dr;
    int FrameWidth{}, FrameHeight{};
    int CaptionWidth{}, CaptionHeight{};
    int CloseButtonWidth{}, CloseButtonHeight{};
    int MaximizeButtonWidth{}, MaximizeButtonHeight{};
    int RestoreButtonWidth{}, RestoreButtonHeight{};
    int MinimizeButtonWidth{}, MinimizeButtonHeight{};
    int BgWidth{}, BgHeight{};
    int top, left;
    float TitleButtonSpacing = 8.0f;

    RenderSvgTile("line-v", { X, Y, 1, 25 }, false, false);
    ++X;
    RenderSvgTile("window_caption_active", { X, Y, 20, 25 }, false, false);
    X += 20;
    RenderSvgTile("line-v", { X, Y, 1, 25 }, false, false);

    ++X;
    RenderSvgTile("line-v-inactive", { X, Y, 1, 25 }, false, false);
    ++X;
    RenderSvgTile("window_caption_inactive", { X, Y, 20, 25 }, false, false);
    X += 20;
    RenderSvgTile("line-v-inactive", { X, Y, 1, 25 }, false, false);


    X = 0;
    RenderSvgTile("line-h", { X, Y, 22, 1 }, false, false);
    Y += 25;
    RenderSvgTile("line-h", { X, Y, 22, 1 }, false, false);

    Y = 0;
    X += 22;
    RenderSvgTile("line-h-inactive", { X, Y, 22, 1 }, false, false);
    Y += 25;
    RenderSvgTile("line-h-inactive", { X, Y, 22, 1 }, false, false);

    X = 44;
    Y = 0;
    RenderSvgTile("frame_bg", { X, Y, 1, 1 }, false, false);

    FrameWidth = 1;
    FrameHeight = 1;
    CaptionWidth = 20;
    CaptionHeight = 24;
    of << "wnd_caption_left: 0, 0, 1, " << CaptionHeight + 2*FrameHeight << std::endl;
    of << "wnd_caption_middle: " << FrameWidth << ", 0, 1, " << CaptionHeight + 2*FrameHeight << std::endl;
    of << "wnd_caption_right: " << FrameWidth + CaptionWidth << ", 0, 1, " << CaptionHeight + 2*FrameHeight << std::endl;
    of << "wnd_caption_inactive_left: " << 2*FrameWidth + CaptionWidth << ", 0, " << CaptionWidth << ", " << CaptionHeight + 2*FrameHeight << std::endl;
    of << "wnd_caption_inactive_middle: " << 2 * FrameWidth + CaptionWidth + 1 << ", 0, 1, " << CaptionHeight + 2*FrameHeight << std::endl;
    of << "wnd_caption_inactive_right: " << 2*FrameWidth + 2*CaptionWidth + FrameWidth << ", 0, " << CaptionWidth << ", " << CaptionHeight + 2*FrameHeight << std::endl;

    of << "wnd_frame_left: 0, 1, " << FrameWidth << ", 24" << std::endl;
    of << "wnd_frame_right: " << FrameWidth + CaptionWidth << ", 1, " << FrameWidth << ", 24" << std::endl;
    of << "wnd_frame_bottom: " << FrameWidth << ", " << FrameHeight + CaptionHeight<< ", 20, " << FrameHeight << std::endl;
    of << "wnd_frame_bottom_left: 0, " << FrameHeight + CaptionHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;
    of << "wnd_frame_bottom_right: " << FrameWidth + CaptionWidth << ", " << FrameHeight + CaptionHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;

    of << "wnd_frame_inactive_left: " << 2*FrameWidth + CaptionWidth << ", 0, " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_inactive_right: " << 2*FrameWidth + CaptionWidth + FrameWidth << ", 0, " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_inactive_bottom: " << 2*FrameWidth + CaptionWidth << ", " << FrameHeight + CaptionHeight << ", 1, " << FrameHeight << std::endl;
    of << "wnd_frame_inactive_bottom_left: " << 2 * FrameWidth + CaptionWidth << ", " << FrameHeight + CaptionHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;
    of << "wnd_frame_inactive_bottom_right: " << 2*FrameWidth + 2*CaptionWidth + FrameWidth << ", " << FrameHeight + CaptionHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;

    top = FrameHeight + 2 * CaptionHeight + 2;

    X = 0;
    Y = 26;
    CloseButtonWidth = 20;
    CloseButtonHeight = 20;
    RenderButtons("wnd_close_button", 20, 20, false);
    X += 20;
    RenderButtons("wnd_maximize_button", 20, 20, false);
    X += 20;
    RenderButtons("wnd_minimize_button", 20, 20, false);
    X += 20;
    RenderButtons("wnd_restore_button", 20, 20, false);

    X = 0;
    AddY(3 * 20, 25);
    of << "wnd_leftmenu_button: 0, 0, 0, 0" << std::endl;
}

void RenderFullNonScaledTexture(std::string FileName, float x, float y, int& TexWidth, int& TexHeight)
{
    TexWidth = 0.0f;
    TexHeight = 0.0f;
    SDL_Surface* s = IMG_Load(FileName.c_str());
    if (s)
    {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            TexWidth = s->w;
            TexHeight = s->h;
            SDL_FRect dr = { x, y, static_cast<float>(s->w), static_cast<float>(s->h) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
        }
        SDL_DestroySurface(s);
    }
}

void RenderStretchedTexture(std::string FileName, float x, float y, float w, float h, int& TexWidth, int& TexHeight)
{
    TexWidth = 0.0f;
    TexHeight = 0.0f;
    SDL_Surface* s = IMG_Load(FileName.c_str());
    if (s)
    {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            TexWidth = s->w;
            TexHeight = s->h;
            if (w < 0.0f) w = TexWidth;
            if (h < 0.0f) h = TexHeight;
            SDL_FRect dr = { x, y, w, h };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
        }
        SDL_DestroySurface(s);
    }
}

void RenderScrollBars(const int size, std::string postfix)
{
    SDL_Rect LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* Texture;

    X = 0;
    std::string name = "scrollbar-vertical-arrow-down";
    RenderButtons(name, size, size, false, postfix);
    AddX(size, size);
    name = "scrollbar-vertical-arrow-up";
    RenderButtons(name, size, size, false, postfix);
    AddX(size, size);
    name = "scrollbar-horizontal-arrow-left";
    RenderButtons(name, size, size, false, postfix);
    AddX(size, size);
    name = "scrollbar-horizontal-arrow-right";
    RenderButtons(name, size, size, false, postfix);

    X = 0;
    AddY(4*size, 0);
    name = "scrollbar-horizontal-bg";
    RenderSvgTile(name, {X, Y, 1, size}, false, true, postfix);
    AddX(1, 1);
    name = "scrollbar-horizontal-bg_disabled";
    RenderSvgTile(name, { X, Y, 1, size }, false, true, postfix);
    AddX(1, 1);
    name = "scrollbar-horizontal-bg_pushed";
    RenderSvgTile(name, { X, Y, 1, size }, false, true, postfix);
    AddX(1, 3);
    name = "scrollbar-horizontal-thumb-left";
    RenderSvgTile(name, { X, Y, 3, size }, false, true, postfix);
    AddX(3, size - 2 * 3);
    name = "scrollbar-horizontal-thumb-middle";
    RenderSvgTile(name, { X, Y, size - 2 * 3, size }, false, true, postfix);
    AddX(size - 2 * 3, 3);
    name = "scrollbar-horizontal-thumb-right";
    RenderSvgTile(name, { X, Y, 3, size }, false, true, postfix);
    AddX(3, size);
    name = "scrollbar-horizontal-thumb-bg";
    RenderSvgTile(name, { X, Y, size, size }, false, true, postfix);

    AddX(size, 0);
    name = "scrollbar-vertical-bg";
    RenderSvgTile(name, { X, Y, size, 1 }, false, true, postfix);
    AddY(1, 1);
    name = "scrollbar-vertical-bg_disabled";
    RenderSvgTile(name, { X, Y, size, 1 }, false, true, postfix);
    AddY(1, 1);
    name = "scrollbar-vertical-bg_pushed";
    RenderSvgTile(name, { X, Y, size, 1 }, false, true, postfix);
    AddY(1, 3);
    AddX(size, 0);
    Y -= 3;
    name = "scrollbar-vertical-thumb-top";
    RenderSvgTile(name, { X, Y, size, 3 }, false, true, postfix);
    AddY(3, size - 2 * 3);
    name = "scrollbar-vertical-thumb-middle";
    RenderSvgTile(name, { X, Y, size, size - 2 * 3 }, false, true, postfix);
    AddY(size - 2 * 3, 3);
    name = "scrollbar-vertical-thumb-bottom";
    RenderSvgTile(name, { X, Y, size, 3 }, false, true, postfix);
    AddX(size, size);
    Y -= size - 2 * 3 + 3;
    name = "scrollbar-vertical-thumb-bg";
    RenderSvgTile(name, { X, Y, size, size }, false, true, postfix);
    X = 0;
    AddY(size, 0);
}

void RenderSliders()
{
    constexpr const int ThumbSize = 20;
    SDL_Rect LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* Texture;

    float OldY = Y;
    X = 75;
    Y -= 3 * ThumbSize;
    RenderSvgTile("slider-vertical-rail", { X, Y, 5, 3 * ThumbSize }, false);
    X = 0;
    Y += 3 * ThumbSize;
    RenderSvgTile("slider-vertical-thumb", { X, Y, ThumbSize, ThumbSize }, false);
    X += ThumbSize;
    RenderSvgTile("slider-horizontal-thumb", { X, Y, ThumbSize, ThumbSize }, false);
    AddX(ThumbSize, 0);
    RenderSvgTile("slider-horizontal-rail", { X, Y, 2 * ThumbSize, 5 }, false);
    Y = OldY;
    AddY(ThumbSize, 0);
    X = 0;
}

void RenderSvgTile(std::string Name, SDL_Rect DestRect, bool Split, bool OutputTileToDescriptionFile, std::string OutputPostfix)
{
    SDL_Rect rc = DestRect, LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* Texture;
    Texture = LoadSVG((ThemeDir + "/" + Name + ".svg").c_str(), rc.w, rc.h);
    if (Texture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, Texture, nullptr, &frc);

        if (OutputTileToDescriptionFile)
        {
            if (Split)
            {
                LeftRC = rc;
                LeftRC.w = static_cast<int>(rc.w / 3.0f);
                if (OutputPostfix.empty())
                {
                    of << Name + "_left_rc: ";
                }
                else
                {
                    of << Name + "_left_rc_" << OutputPostfix << ": ";
                }
                of << LeftRC << std::endl;

                RightRC = rc;
                RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
                RightRC.w = static_cast<int>(rc.w / 3.0f);
                if (OutputPostfix.empty())
                {
                    of << Name + "_right_rc: ";
                }
                else
                {
                    of << Name + "_right_rc_" << OutputPostfix << ": ";
                }
                of << RightRC << std::endl;

                MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
                if (OutputPostfix.empty())
                {
                    of << Name + "_middle_rc: ";
                }
                else
                {
                    of << Name + "_middle_rc_" << OutputPostfix << ": ";
                }
                of << MiddleRC << std::endl;
            }
            else
            {
                if (OutputPostfix.empty())
                {
                    of << Name + "_rc: ";
                }
                else
                {
                    of << Name + "_rc_" << OutputPostfix << ": ";
                }
                of << rc << std::endl;
            }
        }
        SDL_DestroyTexture(Texture);
        rc.y += DestRect.h;
    }
}

void RenderButtons(std::string Name, int width, int height, bool Split, std::string OutputPostfix)
{
    if (X + width > CurrentColumnX + CurrentColumnWidth)
    {
        CurrentColumnWidth = X + width - CurrentColumnX;
    }
    SDL_Rect rc = {X, Y, width, height}, LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* ButtonTexture;
    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + ".svg").c_str(), rc.w, rc.h);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_rc: ";
            }
            else
            {
                of << Name + "_left_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_rc: ";
            }
            else
            {
                of << Name + "_right_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_rc: ";
            }
            else
            {
                of << Name + "_middle_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_rc: ";
            }
            else
            {
                of << Name + "_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-focused.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_focused_rc: ";
            }
            else
            {
                of << Name + "_left_focused_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_focused_rc: ";
            }
            else
            {
                of << Name + "_right_focused_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_focused_rc: ";
            }
            else
            {
                of << Name + "_middle_focused_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_focused_rc: ";
            }
            else
            {
                of << Name + "_focused_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-disabled.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_disabled_rc: ";
            }
            else
            {
                of << Name + "_left_disabled_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_disabled_rc: ";
            }
            else
            {
                of << Name + "_right_disabled_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_disabled_rc: ";
            }
            else
            {
                of << Name + "_middle_disabled_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_disabled_rc: ";
            }
            else
            {
                of << Name + "_disabled_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-hover.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_hover_rc: ";
            }
            else
            {
                of << Name + "_left_hover_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_hover_rc: ";
            }
            else
            {
                of << Name + "_right_hover_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_hover_rc: ";
            }
            else
            {
                of << Name + "_middle_hover_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_hover_rc: ";
            }
            else
            {
                of << Name + "_hover_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-hover-focused.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_hover_focused_rc: ";
            }
            else
            {
                of << Name + "_left_hover_focused_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_hover_focused_rc: ";
            }
            else
            {
                of << Name + "_right_hover_focused_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_hover_focused_rc: ";
            }
            else
            {
                of << Name + "_middle_hover_focused_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_focused_rc: ";
            }
            else
            {
                of << Name + "_focused_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-pushed.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_pushed_rc: ";
            }
            else
            {
                of << Name + "_left_pushed_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_pushed_rc: ";
            }
            else
            {
                of << Name + "_right_pushed_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_pushed_rc: ";
            }
            else
            {
                of << Name + "_middle_pushed_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_pushed_rc: ";
            }
            else
            {
                of << Name + "_pushed_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
        rc.y += height;
    }

    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + "-pushed-focused.svg").c_str(), 75, 25);
    if (ButtonTexture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        if (Split)
        {
            LeftRC = rc;
            LeftRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_left_pushed_focused_rc: ";
            }
            else
            {
                of << Name + "_left_pushed_focused_rc_" << OutputPostfix << ": ";
            }
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            if (OutputPostfix.empty())
            {
                of << Name + "_right_pushed_focused_rc: ";
            }
            else
            {
                of << Name + "_right_pushed_focused_rc_" << OutputPostfix << ": ";
            }
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            if (OutputPostfix.empty())
            {
                of << Name + "_middle_pushed_focused_rc: ";
            }
            else
            {
                of << Name + "_middle_pushed_focused_rc_" << OutputPostfix << ": ";
            }
            of << MiddleRC << std::endl;
        }
        else
        {
            if (OutputPostfix.empty())
            {
                of << Name + "_pushed_focused_rc: ";
            }
            else
            {
                of << Name + "_pushed_focused_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
    }
}

void RenderButtonsByNamesHorizontal(std::vector<std::string> Names, int width, int height, int LineBreakAfterItemNumber, std::string OutputPostfix)
{
    if (X + width > CurrentColumnX + CurrentColumnWidth)
    {
        CurrentColumnWidth = X + width - CurrentColumnX;
    }
    SDL_Rect rc = { X, Y, width, height }, LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* ButtonTexture;

    for (int i = 0; i < Names.size(); ++i)
    {
        ButtonTexture = LoadSVG((ThemeDir + "/" + Names[i] + ".svg").c_str(), rc.w, rc.h);
        if (ButtonTexture)
        {
            SDL_RectToFRect(&rc, &frc);
            SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

            if (OutputPostfix.empty())
            {
                of << Names[i] + "_rc: ";
            }
            else
            {
                of << Names[i] + "_rc_" << OutputPostfix << ": ";
            }
            of << rc << std::endl;

            SDL_DestroyTexture(ButtonTexture);
            if (i + 1 == LineBreakAfterItemNumber)
            {
                rc.y += height;
                rc.x = 0;
            }
            else
            {
                rc.x += width;
            }
        }
    }
}

SDL_Texture* LoadSVG(const char* Filename, int Width, int Height)
{
    if (X + Width > CurrentColumnX + CurrentColumnWidth)
    {
        CurrentColumnWidth = X + Width - CurrentColumnX;
    }
    SDL_Texture* Texture{};
    if (UseSDLSvgRasterizer)
    {
        SDL_IOStream* stream = SDL_IOFromFile(Filename, "r");
        SDL_Surface* Surface = IMG_LoadSizedSVG_IO(stream, Width, Height);
        SDL_CloseIO(stream);

        if (Surface)
        {
            Texture = SDL_CreateTextureFromSurface(renderer, Surface);
            SDL_DestroySurface(Surface);
        }
    }
    else
    {
        SvgBitmap bmp{};
        if (render_svg_to_rgba_scaled(Filename, Width, Height, &bmp) != 0)
        {
            return nullptr;
        }

        SDL_Surface* surf = SDL_CreateSurfaceFrom(
            bmp.width,
            bmp.height,
            SDL_PIXELFORMAT_RGBA32,
            bmp.data,
            bmp.width * 4
        );

        // używasz surf - tworzysz teksturę:
        Texture = SDL_CreateTextureFromSurface(renderer, surf);

        // po użyciu:
        SDL_DestroySurface(surf);

        // UWAGA: SDL nie zwalnia bmp.data — musisz zrobić to sam:
        free_svg_bitmap(bmp.data, bmp.len);
    }

    return Texture;
}

void AddX(int Value, int NextItemWidth)
{
    if (X + Value + NextItemWidth >= CurrentColumnWidth)
    {
        X = 0;
        Y += CurrentRowHeight;
        CurrentRowHeight = 0;
        if (Y + 25 > TEXTURE_HEIGHT)
        {
            X = CurrentColumnX + CurrentColumnWidth;
            CurrentColumnX = X;
            Y = 0;
            CurrentColumnWidth = DEFAULT_COLUMN_WIDTH;
            CurrentRowHeight = 0;
        }
        return;
    }

    X += Value;
}

void AddY(int Value, int NextItemHeight)
{
    if (Y + NextItemHeight > TEXTURE_HEIGHT)
    {
        X = CurrentColumnX + CurrentColumnWidth;
        CurrentColumnX = X;
        Y = 0;
        CurrentColumnWidth = DEFAULT_COLUMN_WIDTH;
        CurrentRowHeight = 0;
        return;
    }
    Y += Value;
}
