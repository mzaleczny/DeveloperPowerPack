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
void RenderScrollBars();
void RenderSliders();
void RenderSvgTile(std::string Name, SDL_Rect DestRect, bool Split, bool OutputTileToDescriptionFile = true);
void RenderButtons(std::string Name, int width, int height, bool Split = true);
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
    RenderScrollBars();
    RenderButtons("button", 75, 25);
    AddY(7 * 25, 0);
    X = CurrentColumnX;
    RenderSliders();
    RenderButtons("textfield", 75, 25);
    AddY(7 * 25, 0);
    X = CurrentColumnX;
    RenderSvgTile("textfield-selection", { X, Y, 1, 20 }, false, true);
    AddY(20, 0);

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
    return "f:/sources/vs/cpp/_commercial_/DeveloperPowerPack/examples/07_Gui/assets/themes/Blue";
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

void RenderScrollBars()
{
    constexpr const int size = 16;
    SDL_Rect LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* Texture;

    X = 0;
    RenderButtons("scrollbar-vertical-arrow-down", size, size, false);
    AddX(size, size);
    RenderButtons("scrollbar-vertical-arrow-up", size, size, false);
    AddX(size, size);
    RenderButtons("scrollbar-horizontal-arrow-left", size, size, false);
    AddX(size, size);
    RenderButtons("scrollbar-horizontal-arrow-right", size, size, false);

    X = 0;
    AddY(4*size, 0);
    RenderSvgTile("scrollbar-horizontal-bg", {X, Y, 1, 16}, false);
    AddX(1, 1);
    RenderSvgTile("scrollbar-horizontal-bg_disabled", { X, Y, 1, 16 }, false);
    AddX(1, 1);
    RenderSvgTile("scrollbar-horizontal-bg_pushed", { X, Y, 1, 16 }, false);
    AddX(1, 3);
    RenderSvgTile("scrollbar-horizontal-thumb-left", { X, Y, 3, 16 }, false);
    AddX(3, size - 2 * 3);
    RenderSvgTile("scrollbar-horizontal-thumb-middle", { X, Y, size - 2 * 3, 16 }, false);
    AddX(size - 2 * 3, 3);
    RenderSvgTile("scrollbar-horizontal-thumb-right", { X, Y, 3, 16 }, false);
    AddX(3, size);
    RenderSvgTile("scrollbar-horizontal-thumb-bg", { X, Y, size, size }, false);

    AddX(size, 0);
    RenderSvgTile("scrollbar-vertical-bg", { X, Y, size, 1 }, false);
    AddY(1, 1);
    RenderSvgTile("scrollbar-vertical-bg_disabled", { X, Y, size, 1 }, false);
    AddY(1, 1);
    RenderSvgTile("scrollbar-vertical-bg_pushed", { X, Y, size, 1 }, false);
    AddY(1, 3);
    AddX(size, 0);
    Y -= 3;
    RenderSvgTile("scrollbar-vertical-thumb-top", { X, Y, size, 3 }, false);
    AddY(3, size - 2 * 3);
    RenderSvgTile("scrollbar-vertical-thumb-middle", { X, Y, size, size - 2 * 3 }, false);
    AddY(size - 2 * 3, 3);
    RenderSvgTile("scrollbar-vertical-thumb-bottom", { X, Y, size, 3 }, false);
    AddX(size, size);
    Y -= size - 2 * 3 + 3;
    RenderSvgTile("scrollbar-vertical-thumb-bg", { X, Y, size, size }, false);
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

void RenderSvgTile(std::string Name, SDL_Rect DestRect, bool Split, bool OutputTileToDescriptionFile)
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
                of << Name + "_left_rc: ";
                of << LeftRC << std::endl;

                RightRC = rc;
                RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
                RightRC.w = static_cast<int>(rc.w / 3.0f);
                of << Name + "_right_rc: ";
                of << RightRC << std::endl;

                MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
                of << Name + "_middle_rc: ";
                of << MiddleRC << std::endl;
            }
            else
            {
                of << Name + "_rc: ";
                of << rc << std::endl;
            }
        }
        SDL_DestroyTexture(Texture);
        rc.y += DestRect.h;
    }
}

void RenderButtons(std::string Name, int width, int height, bool Split)
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
            of << Name + "_left_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_rc: ";
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
            of << Name + "_left_focused_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_focused_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_focused_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_focused_rc: ";
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
            of << Name + "_left_disabled_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_disabled_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_disabled_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_disabled_rc: ";
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
            of << Name + "_left_hover_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_hover_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_hover_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_hover_rc: ";
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
            of << Name + "_left_hover_focused_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_hover_focused_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_hover_focused_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_focused_rc: ";
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
            of << Name + "_left_pushed_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_pushed_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_pushed_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_pushed_rc: ";
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
            of << Name + "_left_pushed_focused_rc: ";
            of << LeftRC << std::endl;

            RightRC = rc;
            RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
            RightRC.w = static_cast<int>(rc.w / 3.0f);
            of << Name + "_right_pushed_focused_rc: ";
            of << RightRC << std::endl;

            MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
            of << Name + "_middle_pushed_focused_rc: ";
            of << MiddleRC << std::endl;
        }
        else
        {
            of << Name + "_pushed_focused_rc: ";
            of << rc << std::endl;
        }
        SDL_DestroyTexture(ButtonTexture);
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
