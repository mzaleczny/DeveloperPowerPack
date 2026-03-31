#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

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
        SDL_Log("Couldn't create output texture", SDL_GetError());
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
    RenderButtons("button", { 5, 180, 75, 25 });

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
    SDL_Surface* s = IMG_Load((ThemeDir + "\\line-v.png").c_str());
    SDL_Texture* t{ nullptr };
    if (s)
    {
        t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            FrameWidth = s->w;
            dr = { 0, 0, static_cast<float>(FrameWidth), WINDOW_HEIGHT };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            dr = { static_cast<float>(WINDOW_WIDTH - FrameWidth), 0, static_cast<float>(FrameWidth), WINDOW_HEIGHT };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
            t = nullptr;
        }
        SDL_DestroySurface(s);
        s = nullptr;
    }

    s = IMG_Load((ThemeDir + "\\line-h.png").c_str());
    if (s)
    {
        t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            FrameHeight = s->h;
            dr = { 0, 0, WINDOW_WIDTH, static_cast<float>(FrameHeight) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            dr = { 0, static_cast<float>(WINDOW_HEIGHT - FrameHeight), WINDOW_WIDTH, static_cast<float>(FrameHeight) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
            t = nullptr;
        }
        SDL_DestroySurface(s);
        s = nullptr;
    }

    s = IMG_Load((ThemeDir + "\\frame_bg.png").c_str());
    if (s)
    {
        t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            BgWidth = s->w;
            BgHeight = s->h;
            dr = { static_cast<float>(FrameWidth), static_cast<float>(FrameHeight), static_cast<float>(s->w), static_cast<float>(s->h) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
            t = nullptr;
        }
        SDL_DestroySurface(s);
        s = nullptr;
    }

    RenderStretchedTexture(ThemeDir + "\\window-caption-active.png", FrameWidth, FrameHeight, WINDOW_WIDTH - 2 * FrameWidth, -1.0f, CaptionWidth, CaptionHeight);
    RenderStretchedTexture(ThemeDir + "\\window-caption-inactive.png", 0, FrameHeight + CaptionHeight, WINDOW_WIDTH, -1.0f, CaptionWidth, CaptionHeight);

    s = IMG_Load((ThemeDir + "\\line-v-inactive.png").c_str());
    if (s)
    {
        t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            float y = FrameHeight + CaptionHeight;
            dr = { 1, y, static_cast<float>(FrameWidth), WINDOW_HEIGHT - y - FrameHeight};
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            dr = { static_cast<float>(WINDOW_WIDTH - 2*FrameWidth), y, static_cast<float>(FrameWidth), WINDOW_HEIGHT - y - FrameHeight };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
            t = nullptr;
        }
        SDL_DestroySurface(s);
        s = nullptr;
    }

    s = IMG_Load((ThemeDir + "\\line-h-inactive.png").c_str());
    if (s)
    {
        t = SDL_CreateTextureFromSurface(renderer, s);

        if (t)
        {
            float x = FrameWidth;
            float y = FrameHeight + CaptionHeight;
            dr = { x, y, static_cast<float>(WINDOW_WIDTH - 2 * FrameWidth), static_cast<float>(FrameHeight) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            dr = { x, static_cast<float>(WINDOW_HEIGHT - 2*FrameHeight), static_cast<float>(WINDOW_WIDTH - 2 * FrameWidth), static_cast<float>(FrameHeight) };
            SDL_RenderTexture(renderer, t, nullptr, &dr);
            SDL_DestroyTexture(t);
            t = nullptr;
        }
        SDL_DestroySurface(s);
        s = nullptr;
    }

    of << "wnd_caption_left: 0, 0, " << CaptionWidth << ", " << CaptionHeight + FrameHeight << std::endl;
    of << "wnd_caption_middle: " << FrameWidth << ", 0, 1, " << CaptionHeight + FrameHeight << std::endl;
    of << "wnd_caption_right: " << WINDOW_WIDTH - CaptionWidth << ", 0, " << CaptionWidth << ", " << CaptionHeight + FrameHeight << std::endl;
    of << "wnd_caption_inactive_left: 0, " << FrameHeight + CaptionHeight << ", " << CaptionWidth << ", " << CaptionHeight + FrameHeight << std::endl;
    of << "wnd_caption_inactive_middle: " << CaptionWidth + 1 << ", " << FrameHeight + CaptionHeight << ", 1, " << CaptionHeight + FrameHeight << std::endl;
    of << "wnd_caption_inactive_right: " << WINDOW_WIDTH - CaptionWidth << ", " << FrameHeight + CaptionHeight << ", " << CaptionWidth << ", " << CaptionHeight + FrameHeight << std::endl;

    of << "wnd_frame_left: 0, " << 2 * CaptionHeight + FrameHeight << ", " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_right: " << WINDOW_WIDTH - FrameWidth << ", " << 2 * CaptionHeight + FrameHeight << ", " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_bottom: " << FrameWidth << ", " << WINDOW_HEIGHT - FrameHeight << ", 1, " << FrameHeight << std::endl;
    of << "wnd_frame_bottom_left: 0, " << WINDOW_HEIGHT - FrameHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;
    of << "wnd_frame_bottom_right: " << WINDOW_WIDTH - FrameWidth << ", " << WINDOW_HEIGHT - FrameHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;

    of << "wnd_frame_inactive_left: 1, " << 2 * CaptionHeight + FrameHeight << ", " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_inactive_right: " << WINDOW_WIDTH - 2 * FrameWidth << ", " << 2 * CaptionHeight + FrameHeight << ", " << FrameWidth << ", 1" << std::endl;
    of << "wnd_frame_inactive_bottom: " << 2 * FrameWidth << ", " << WINDOW_HEIGHT - 2 * FrameHeight << ", 1, " << FrameHeight << std::endl;
    of << "wnd_frame_inactive_bottom_left: 0, " << WINDOW_HEIGHT - 2 * FrameHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;
    of << "wnd_frame_inactive_bottom_right: " << WINDOW_WIDTH - 2 * FrameWidth << ", " << WINDOW_HEIGHT - 2 * FrameHeight << ", " << FrameWidth << ", " << FrameHeight << std::endl;

    top = FrameHeight + 2 * CaptionHeight + 2;

    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-close.png", FrameWidth + 1 + BgWidth, top, CloseButtonWidth, CloseButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-close-active.png", FrameWidth + 1 + BgWidth + 4 + CloseButtonWidth, top, CloseButtonWidth, CloseButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-close-hover.png", FrameWidth + 1 + BgWidth + 2 * 4 + 2 * CloseButtonWidth, top, CloseButtonWidth, CloseButtonHeight);
    of << "wnd_close_button: " << FrameWidth + 1 + BgWidth << ", " << top << ", " << CloseButtonWidth << ", " << CloseButtonHeight << std::endl;
    of << "wnd_close_button_active: " << FrameWidth + 1 + BgWidth + 4 + CloseButtonWidth << ", " << top << ", " << CloseButtonWidth << ", " << CloseButtonHeight << std::endl;
    of << "wnd_close_button_hover: " << FrameWidth + 1 + BgWidth + 2 * 4 + 2 * CloseButtonWidth << ", " << top << ", " << CloseButtonWidth << ", " << CloseButtonHeight << std::endl;

    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-maximize.png", FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth, top, MaximizeButtonWidth, MaximizeButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-maximize-active.png", FrameWidth + 1 + BgWidth + 4 * 4 + 4 * CloseButtonWidth, top, MaximizeButtonWidth, MaximizeButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-maximize-hover.png", FrameWidth + 1 + BgWidth + 5 * 4 + 5 * CloseButtonWidth, top, MaximizeButtonWidth, MaximizeButtonHeight);
    of << "wnd_maximize_button: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth << ", " << top << ", " << MaximizeButtonWidth << ", " << MaximizeButtonHeight << std::endl;
    of << "wnd_maximize_button_active: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth + 4 + MinimizeButtonWidth << ", " << top << ", " << MaximizeButtonWidth << ", " << MaximizeButtonHeight << std::endl;
    of << "wnd_maximize_button_hover: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 2 * 4 + 2 * MaximizeButtonWidth << ", " << top << ", " << MaximizeButtonWidth << ", " << MaximizeButtonHeight << std::endl;

    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-restore.png", FrameWidth + 1 + BgWidth + 6 * 4 + 6 * CloseButtonWidth, top, RestoreButtonWidth, RestoreButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-restore-active.png", FrameWidth + 1 + BgWidth + 7 * 4 + 7 * CloseButtonWidth, top, RestoreButtonWidth, RestoreButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-restore-hover.png", FrameWidth + 1 + BgWidth + 8 * 4 + 8 * CloseButtonWidth, top, RestoreButtonWidth, RestoreButtonHeight);
    of << "wnd_restore_button: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth << ", " << top << ", " << RestoreButtonWidth << ", " << RestoreButtonHeight << std::endl;
    of << "wnd_restore_button_active: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth + 4 + RestoreButtonWidth << ", " << top << ", " << RestoreButtonWidth << ", " << RestoreButtonHeight << std::endl;
    of << "wnd_restore_button_hover: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth + 2 * 4 + 2 * RestoreButtonWidth << ", " << top << ", " << RestoreButtonWidth << ", " << RestoreButtonHeight << std::endl;
    
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-minimize.png", FrameWidth + 1 + BgWidth + 9 * 4 + 9 * CloseButtonWidth, top, MinimizeButtonWidth, MinimizeButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-minimize-active.png", FrameWidth + 1 + BgWidth + 10 * 4 + 10 * CloseButtonWidth, top, MinimizeButtonWidth, MinimizeButtonHeight);
    RenderFullNonScaledTexture(ThemeDir + "\\titlebutton-minimize-hover.png", FrameWidth + 1 + BgWidth + 11 * 4 + 11 * CloseButtonWidth, top, MinimizeButtonWidth, MinimizeButtonHeight);
    of << "wnd_minimize_button: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth + 3 * 4 + 3 * RestoreButtonWidth << ", " << top << ", " << MinimizeButtonWidth << ", " << MinimizeButtonHeight << std::endl;
    of << "wnd_minimize_button_active: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth + 3 * 4 + 3 * RestoreButtonWidth + 4 + MinimizeButtonWidth << ", " << top << ", " << MinimizeButtonWidth << ", " << MinimizeButtonHeight << std::endl;
    of << "wnd_minimize_button_hover: " << FrameWidth + 1 + BgWidth + 3 * 4 + 3 * CloseButtonWidth
        + 3 * 4 + 3 * MaximizeButtonWidth + 3 * 4 + 3 * RestoreButtonWidth + 2 * 4 + 2 * MinimizeButtonWidth << ", " << top << ", " << MinimizeButtonWidth << ", " << MinimizeButtonHeight << std::endl;

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
    constexpr const int size = 15;
    SDL_Rect rc{ 5, 80, size, size }, LeftRC, MiddleRC, RightRC;
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

void RenderButtons(std::string Name, SDL_Rect StartRect, bool Split)
{
    SDL_Rect rc = StartRect, LeftRC, MiddleRC, RightRC;
    SDL_FRect frc;
    SDL_Texture* ButtonTexture;
    ButtonTexture = LoadSVG((ThemeDir + "/" + Name + ".svg").c_str(), 75, 25);
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
    SDL_IOStream* stream = SDL_IOFromFile(Filename, "r");
    SDL_Surface* Surface = IMG_LoadSizedSVG_IO(stream, Width, Height);
    SDL_CloseIO(stream);

    SDL_Texture* Texture{};
    if (Surface)
    {
        Texture = SDL_CreateTextureFromSurface(renderer, Surface);
        SDL_DestroySurface(Surface);
    }
    return Texture;
}
