#include "SDL3_image/SDL_image.h"
#include "Tilc/Game.h"
#include "Tilc/Resources/TextureManager.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Graphics/GraphicsUtils.h"
#ifdef max
    #undef max
    #undef min
#endif
#include <algorithm>

Tilc::Resources::TResourceTexture::~TResourceTexture()
{
    if (m_Resource)
    {
        switch (m_TextureType)
        {
        case ETextureType::SDL_Surface:
            SDL_DestroySurface((SDL_Surface*)m_Resource);
            m_Resource = nullptr;
            break;
        case ETextureType::SDL_Texture:
            SDL_DestroyTexture((SDL_Texture*)m_Resource);
            m_Resource = nullptr;
            break;
        case ETextureType::GL_Texture:
            glDeleteTextures(1, (GLuint*)&m_Resource);
            m_Resource = nullptr;
            break;
        }
    }
}

Tilc::Resources::TResourceTexture* Tilc::Resources::TTextureManager::Load(const TResourceInfo& ResourceInfo)
{
    void* ResourcePointer = nullptr;
    int TexWidth = 0;
    int TexHeight = 0;
    int NumberOfChannels = 0;
    SDL_PixelFormat Format = (SDL_PixelFormat)0;
    SDL_Surface* Surface = nullptr;
    SDL_Texture* Texture = nullptr;
    GLuint GLTex = 0;
    Tilc::Resources::ETextureType TextureType = Tilc::Resources::ETextureType::None;

    if (ResourceInfo.m_SourceType == "File")
    {
        if (ResourceInfo.m_Source.ends_with(".svg"))
        {
            SDL_IOStream* stream = SDL_IOFromFile(ResourceInfo.m_Source.c_str(), "r");
            Surface = IMG_LoadSizedSVG_IO(stream, ResourceInfo.m_Width, ResourceInfo.m_Height);
            SDL_CloseIO(stream);
        }
        else
        {
            Surface = IMG_Load(ResourceInfo.m_Source.c_str());
        }
        if (!Surface)
        {
            SDL_Log("ERROR! Couldn't load image: %s", SDL_GetError());
            return nullptr;
        }
    }
    else
    {
        char* Buffer = nullptr;
        size_t BufferSize;
        Tilc::TExtString ResOrigPath;
        //SDL_Texture* Texture = nullptr;

        // Load Backgrounds from the disk
        Tilc::Data::Data->GetResourceByName(ResourceInfo.m_Source.c_str(), ResourceInfo.m_SourceName.c_str(), ResOrigPath, &Buffer, &BufferSize, ResourceInfo.m_SlotNumber);
        if (Buffer)
        {
            // Load Backgrounds from resource file
            Surface = IMG_Load_IO(SDL_IOFromConstMem(Buffer, BufferSize), true);
            //Texture = IMG_LoadTexture_IO(Tilc::GameObject->m_Window->GetRenderer(), SDL_IOFromConstMem(Buffer, BufferSize), true);
            delete[] Buffer;
            Buffer = nullptr;
            if (!Surface)
            {
                SDL_Log("ERROR! Couldn't load image: %s", SDL_GetError());
                return nullptr;
            }
        }
    }

    if (Surface)
    {
        TexWidth = Surface->w;
        TexHeight = Surface->h;
        NumberOfChannels = 0;
        Format = Surface->format;

        if (ResourceInfo.m_Type == "SDL_Surface" || ResourceInfo.m_Type == "SDL_Texture")
        {
            if (ResourceInfo.m_Type == "SDL_Surface")
            {
                TextureType = Tilc::Resources::ETextureType::SDL_Surface;
                ResourcePointer = Surface;
            }
            else if (ResourceInfo.m_Type == "SDL_Texture")
            {
                TextureType = Tilc::Resources::ETextureType::SDL_Texture;
                Texture = SDL_CreateTextureFromSurface(Tilc::GameObject->m_Window->GetRenderer(), Surface);
                SDL_DestroySurface(Surface);
                Surface = nullptr;
                if (!Texture)
                {
                    SDL_Log("Couldn't create static texture: %s", SDL_GetError());
                    return nullptr;
                }
                ResourcePointer = Texture;
            }
        }
        else if (ResourceInfo.m_Type == "GL_Texture")
        {
            SDL_FlipSurface(Surface, SDL_FLIP_VERTICAL);
            glCreateTextures(GL_TEXTURE_2D, 1, &GLTex);
            glTextureParameteri(GLTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(GLTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(GLTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(GLTex, GL_TEXTURE_WRAP_T, GL_REPEAT);
            /*
            float border[4]{ 1.0f, 0.0f, 0.0f, 1.0f };
            glTextureParameterfv(GLTex, GL_TEXTURE_BORDER_COLOR, border);
            glTextureParameteri(GLTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(GLTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            */
            GLenum GLInternalFormat = 0;
            GLenum GLFormat = 0;
            switch (Format)
            {
            case SDL_PIXELFORMAT_ABGR32:
                GLInternalFormat = GL_ABGR_EXT;
                GLFormat = GL_ABGR_EXT;
                break;
            case SDL_PIXELFORMAT_BGR24:
                GLInternalFormat = GL_BGR;
                GLFormat = GL_BGR;
                break;
            case SDL_PIXELFORMAT_RGBA32:
                GLInternalFormat = GL_RGBA8;
                GLFormat = GL_RGBA;
                break;
            case SDL_PIXELFORMAT_RGB24:
                GLInternalFormat = GL_RGB8;
                GLFormat = GL_RGB;
                break;
            }
            // Second parameter in function belows specifies number of mipmaps (Levels of Detail) to generate
            glTextureStorage2D(GLTex, std::floor(std::log2(std::max(TexWidth, TexHeight))), GLInternalFormat, TexWidth, TexHeight);
            glTextureSubImage2D(GLTex, 0, 0, 0, TexWidth, TexHeight, GLFormat, GL_UNSIGNED_BYTE, Surface->pixels);
            glGenerateTextureMipmap(GLTex);
            SDL_DestroySurface(Surface);
            Surface = nullptr;
            ResourcePointer = reinterpret_cast<void*>(static_cast<int64_t>(GLTex));
            TextureType = Tilc::Resources::ETextureType::GL_Texture;
        }
    }

    TResourceTexture* Resource = new (std::nothrow) TResourceTexture(ResourcePointer);
    if (!Resource)
    {
        if (Surface)
        {
            SDL_DestroySurface(Surface);
            Surface = nullptr;
        }
        if (Texture)
        {
            SDL_DestroyTexture(Texture);
            Texture = nullptr;
        }
        if (GLTex)
        {
            glDeleteTextures(1, &GLTex);
            GLTex = 0;
        }
        return nullptr;
    }
    else
    {
        Resource->m_TextureType = TextureType;
        Resource->m_TexWidth = TexWidth;
        Resource->m_TexHeight = TexHeight;
        Resource->m_Format = Format;
    }

    return Resource;
}

Tilc::Resources::TResourceTexture* Tilc::Resources::TTextureManager::LoadFromFile(const Tilc::TExtString& FileName, const Tilc::TExtString& ResourceType, int RasterizationWidth, int RasterizationHeight)
{
    TResourceInfo ResourceInfo;
    ResourceInfo.m_SlotNumber = -1;
    ResourceInfo.m_Source = FileName;
    ResourceInfo.m_SourceName = "";
    ResourceInfo.m_SourceType = "File";
    ResourceInfo.m_Type = ResourceType;
    ResourceInfo.m_Width = RasterizationWidth;
    ResourceInfo.m_Height = RasterizationHeight;
    return Load(ResourceInfo);
}

Tilc::Resources::TResourceTexture* Tilc::Resources::TTextureManager::CreateGLTextureResourceFromSDLSurface(SDL_Surface* Surface)
{
    void* ResourcePointer = nullptr;
    int TexWidth = 0;
    int TexHeight = 0;
    int NumberOfChannels = 0;
    SDL_PixelFormat Format = (SDL_PixelFormat)0;
    SDL_Texture* Texture = nullptr;
    GLuint GLTex = 0;
    Tilc::Resources::ETextureType TextureType = Tilc::Resources::ETextureType::GL_Texture;
    TResourceTexture* Resource = nullptr;

    if (Surface)
    {
        TexWidth = Surface->w;
        TexHeight = Surface->h;
        NumberOfChannels = 0;
        Format = Surface->format;

        glCreateTextures(GL_TEXTURE_2D, 1, &GLTex);
        glTextureParameteri(GLTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(GLTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(GLTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(GLTex, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GLenum GLInternalFormat = 0;
        GLenum GLFormat = 0;
        switch (Format)
        {
        case SDL_PIXELFORMAT_ABGR32:
            GLInternalFormat = GL_ABGR_EXT;
            GLFormat = GL_ABGR_EXT;
            break;
        case SDL_PIXELFORMAT_BGR24:
            GLInternalFormat = GL_BGR;
            GLFormat = GL_BGR;
            break;
        case SDL_PIXELFORMAT_RGBA32:
            GLInternalFormat = GL_RGBA8;
            GLFormat = GL_RGBA;
            break;
        case SDL_PIXELFORMAT_RGB24:
            GLInternalFormat = GL_RGB8;
            GLFormat = GL_RGB;
            break;
        }
        // Second parameter in function belows specifies number of mipmaps (Levels of Detail) to generate
        glTextureStorage2D(GLTex, std::floor(std::log2(std::max(TexWidth, TexHeight))), GLInternalFormat, TexWidth, TexHeight);
        glTextureSubImage2D(GLTex, 0, 0, 0, TexWidth, TexHeight, GLFormat, GL_UNSIGNED_BYTE, Surface->pixels);
        glGenerateTextureMipmap(GLTex);
        ResourcePointer = reinterpret_cast<void*>(static_cast<int64_t>(GLTex));
        TextureType = Tilc::Resources::ETextureType::GL_Texture;
        Resource = new (std::nothrow) TResourceTexture(ResourcePointer);
        if (!Resource)
        {
            if (GLTex)
            {
                glDeleteTextures(1, &GLTex);
                GLTex = 0;
            }
            return nullptr;
        }
        else
        {
            Resource->m_TextureType = TextureType;
            Resource->m_TexWidth = TexWidth;
            Resource->m_TexHeight = TexHeight;
            Resource->m_Format = Format;
        }
    }
    return Resource;
}
