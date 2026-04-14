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
std::string OutputFile = "GUI.png";
std::string ThemeDir;// = "f:\\resources\\Gtk Themes\\Relax-Light-GTK\\assets";
std::ofstream of;

constexpr const int WINDOW_WIDTH = 1920;
constexpr const int WINDOW_HEIGHT = 1080;

void SaveTextureToFile(SDL_Texture* Texture);
std::string FindThemeDir();
SDL_Texture* LoadSVG(const char* Filename, int Width, int Height);
void RenderWindow();
void RenderFullNonScaledTexture(std::string FileName, float x, float y, int& TexWidth, int& TexHeight);
void RenderStretchedTexture(std::string FileName, float x, float y, float w, float h, int& TexWidth, int& TexHeight);
void RenderScrollBars();
void RenderSvgTile(std::string Name, SDL_Rect StartRect, bool Split, bool OutputTileToDescriptionFile = true);
void RenderButtons(std::string Name, SDL_Rect StartRect, bool Split = true);

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

    OutputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureBlendMode(OutputTexture, SDL_BLENDMODE_BLEND);
    if (!OutputTexture)
    {
        SDL_Log("Couldn't create output texture");
        return SDL_APP_FAILURE;
    }

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
    RenderButtons("button", { 5, 86, 75, 25 });

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
    SDL_Rect rc;
    int FrameWidth{}, FrameHeight{};
    int CaptionWidth{}, CaptionHeight{};
    int CloseButtonWidth{}, CloseButtonHeight{};
    int MaximizeButtonWidth{}, MaximizeButtonHeight{};
    int RestoreButtonWidth{}, RestoreButtonHeight{};
    int MinimizeButtonWidth{}, MinimizeButtonHeight{};
    int BgWidth{}, BgHeight{};
    int top, left;
    float TitleButtonSpacing = 8.0f;

    rc = { 0, 1, 1, 24 };
    RenderSvgTile("line-v", rc, false, false);
    rc.x += 1;
    rc.w = 20;
    RenderSvgTile("window_caption_active", rc, false, false);
    rc.x += 20;
    rc.w = 1;
    RenderSvgTile("line-v", rc, false, false);

    rc.x += 1;
    RenderSvgTile("line-v-inactive", rc, false, false);
    rc.x += 1;
    rc.w = 20;
    RenderSvgTile("window_caption_inactive", rc, false, false);
    rc.x += 20;
    rc.w = 1;
    RenderSvgTile("line-v-inactive", rc, false, false);


    rc.y = 0;
    rc.x = 0;
    rc.w = 22;
    rc.h = 1;
    RenderSvgTile("line-h", rc, false, false);
    rc.y += 25;
    RenderSvgTile("line-h", rc, false, false);

    rc.y = 0;
    rc.x += 22;
    RenderSvgTile("line-h-inactive", rc, false, false);
    rc.y += 25;
    RenderSvgTile("line-h-inactive", rc, false, false);

    rc.x = 44;
    rc.y = 0;
    rc.w = 1;
    rc.h = 1;
    RenderSvgTile("frame_bg", rc, false, false);

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

    rc.x = FrameWidth + 1 + BgWidth;
    rc.y = 26;
    rc.w = CloseButtonWidth = 20;
    rc.h = CloseButtonHeight = 20;
    RenderButtons("wnd_close_button", rc, false);
    rc.x += 20;
    RenderButtons("wnd_maximize_button", rc, false);
    rc.x += 20;
    RenderButtons("wnd_minimize_button", rc, false);
    rc.x += 20;
    RenderButtons("wnd_restore_button", rc, false);

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
    SDL_Rect rc{ 84, 26, size, size }, LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* Texture;
    int x, y;

    RenderButtons("scrollbar-vertical-arrow-down", rc, false);
    rc.x += size;
    RenderButtons("scrollbar-vertical-arrow-up", rc, false);
    rc.x += size;
    RenderButtons("scrollbar-horizontal-arrow-left", rc, false);
    rc.x += size;
    RenderButtons("scrollbar-horizontal-arrow-right", rc, false);

    rc.x += size;
    rc.w = 1;
    RenderButtons("scrollbar-horizontal-bg", rc, false);
    rc.x += 1;
    RenderButtons("scrollbar-horizontal-bg_disabled", rc, false);
    rc.x += 1;
    RenderButtons("scrollbar-horizontal-bg_pushed", rc, false);
    rc.x += 1;
    rc.w = 3;
    RenderButtons("scrollbar-horizontal-thumb-left", rc, false);
    rc.x += rc.w;
    rc.w = size - 2 * rc.w;
    RenderButtons("scrollbar-horizontal-thumb-middle", rc, false);
    rc.x += rc.w;
    rc.w = 3;
    RenderButtons("scrollbar-horizontal-thumb-right", rc, false);
    rc.x += rc.w;
    rc.w = size - 2 * rc.w;
    RenderButtons("scrollbar-horizontal-thumb-bg", rc, false);

    rc.x += rc.w;
    x = rc.x;
    y = rc.y;
    rc.w = size;
    rc.h = 1;
    RenderButtons("scrollbar-vertical-bg", rc, false);
    rc.y += 1;
    RenderButtons("scrollbar-vertical-bg_disabled", rc, false);
    rc.y += 1;
    RenderButtons("scrollbar-vertical-bg_pushed", rc, false);
    rc.y += 1;
    rc.h = 3;
    RenderButtons("scrollbar-vertical-thumb-top", rc, false);
    rc.y += rc.h;
    rc.h = size - 2 * rc.h;
    RenderButtons("scrollbar-vertical-thumb-middle", rc, false);
    rc.x = x + rc.w;
    rc.y = y;
    rc.h = 3;
    RenderButtons("scrollbar-vertical-thumb-bottom", rc, false);
    rc.y += rc.h;
    rc.h = size - 2 * rc.h;
    RenderButtons("scrollbar-vertical-thumb-bg", rc, false);
    /*
    Texture = LoadSVG((ThemeDir + "/button.svg").c_str(), 75, 25);
    if (Texture)
    {
        SDL_RectToFRect(&rc, &frc);
        SDL_RenderTexture(renderer, ButtonTexture, nullptr, &frc);

        LeftRC = rc;
        LeftRC.w = static_cast<int>(rc.w / 3.0f);
        of << "button_left_rc: ";
        of << LeftRC << std::endl;

        RightRC = rc;
        RightRC.x = static_cast<int>(rc.x + rc.w - LeftRC.w);
        RightRC.w = static_cast<int>(rc.w / 3.0f);
        of << "button_right_rc: ";
        of << RightRC << std::endl;

        MiddleRC = { rc.x + LeftRC.w + 1, rc.y, 1, rc.h };
        of << "button_middle_rc: ";
        of << MiddleRC << std::endl;

        SDL_DestroyTexture(ButtonTexture);
    }
    */
}

void RenderSvgTile(std::string Name, SDL_Rect StartRect, bool Split, bool OutputTileToDescriptionFile)
{
    SDL_Rect rc = StartRect, LeftRC, MiddleRC, RightRC;
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
        rc.y += StartRect.h;
    }
}

void RenderButtons(std::string Name, SDL_Rect StartRect, bool Split)
{
    SDL_Rect rc = StartRect, LeftRC, MiddleRC, RightRC;
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
        rc.y += StartRect.h;
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
        rc.y += StartRect.h;
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
        rc.y += StartRect.h;
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
        rc.y += StartRect.h;
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
        rc.y += StartRect.h;
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
            of << Name + "_left_hover_pushed_rc: ";
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
        rc.y += StartRect.h;
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
