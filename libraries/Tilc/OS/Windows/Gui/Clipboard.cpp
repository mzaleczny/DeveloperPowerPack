#include "Tilc/OS/Windows/Gui/Clipboard.h"
#include <Windows.h>

Tilc::OS::Gui::TWindowsClipboard::TWindowsClipboard()
{
}

Tilc::OS::Gui::TWindowsClipboard::~TWindowsClipboard()
{
}

void Tilc::OS::Gui::TWindowsClipboard::GetClipboardFormatNameA(Tilc::TExtString& name, unsigned int format)
{
    int result;
    int buf_len = 255;
    char* formatName = new char[buf_len];
    result = ::GetClipboardFormatNameA(format, formatName, buf_len);

    if (result == 0)
    {
        name = "";
    }
    else
    {
        name = formatName;
    }

    delete[] formatName;
}

bool Tilc::OS::Gui::TWindowsClipboard::EmptyClipboard()
{
    return ::EmptyClipboard();
}

bool Tilc::OS::Gui::TWindowsClipboard::CopyTextToClipboard(void* Window, const TExtString& s)
{
    if (!Window)
    {
        return false;
    }

    if (!::OpenClipboard((HWND)Window))
    {
        return false;
    }

    size_t str_len = s.length();

    // jeśli mamy pusty tekst, to zamykamy schowek
    if (str_len < 1)
    {
        ::CloseClipboard();
        return true;
    }

    HGLOBAL hglbCopyUnicode = ::GlobalAlloc(GMEM_MOVEABLE, (str_len + 1) * sizeof(WCHAR));
    if (hglbCopyUnicode == NULL)
    {
        ::CloseClipboard();
        return false;
    }
    HGLOBAL hglbCopyAnsi = ::GlobalAlloc(GMEM_MOVEABLE, (str_len + 1) * sizeof(CHAR));
    if (hglbCopyAnsi == NULL)
    {
        ::GlobalFree(hglbCopyUnicode);
        ::CloseClipboard();
        return false;
    }

    // najpierw kopiujemy dane UNICODE
    LPVOID mem = ::GlobalLock(hglbCopyUnicode);
    // kopiujemy tekst razem ze znkiem NULL kończącym go
    memcpy(mem, s.data(), (str_len + 1) * sizeof(CHAR));
    ::GlobalUnlock(hglbCopyUnicode);

    // a następnie konwerujemy i kopiujemy dane ANSI
    mem = ::GlobalLock(hglbCopyAnsi);
    memcpy(mem, s.data(), (str_len + 1) * sizeof(CHAR));
    ::GlobalUnlock(hglbCopyAnsi);

    ::EmptyClipboard();
    // umieszczamy dane (a właściwie uchwyt HGLOBAL na przydzielony obszar pamięci) w schowku
    ::SetClipboardData(CF_TEXT, hglbCopyAnsi);
    ::SetClipboardData(CF_UNICODETEXT, hglbCopyUnicode);
    ::CloseClipboard();

    return true;
}

Tilc::TExtString Tilc::OS::Gui::TWindowsClipboard::GetTextFromClipboard(void* Window)
{
    if (!Window)
    {
        return "";
    }

    if (!::OpenClipboard((HWND)Window))
    {
        return "";
    }

    unsigned int clFormat = ::EnumClipboardFormats(0);
    if (!::IsClipboardFormatAvailable(CF_TEXT))
    {
        return "";
    }

    Tilc::TExtString retval = "";
    // W pierwszej kolejności próbujemy pobrać tekst UNICODE
    HGLOBAL hglb;

    hglb = ::GetClipboardData(CF_TEXT);
    if (hglb != NULL)
    {
        char* str = (char*)::GlobalLock(hglb);
        if (str != NULL)
        {
            retval = str;
            ::GlobalUnlock(hglb);
            ::CloseClipboard();

            return retval;
        }
    }

    hglb = ::GetClipboardData(CF_UNICODETEXT);
    if (hglb != NULL)
    {
        char* str = (char*)::GlobalLock(hglb);
        if (str != NULL)
        {
            retval = str;
            ::GlobalUnlock(hglb);
            ::CloseClipboard();

            return retval;
        }
    }

    ::CloseClipboard();
    return "";
}

bool Tilc::OS::Gui::TWindowsClipboard::GetAvailableFormats(void* Window)
{
    m_AvailableFormats.clear();
    if (!Window)
    {
        return false;
    }

    if (!::OpenClipboard((HWND)Window))
    {
        return false;
    }

    unsigned int format = ::EnumClipboardFormats(0);
    while (format != 0)
    {
        m_AvailableFormats.emplace_back(format);
        format = ::EnumClipboardFormats(format);
    }

    ::CloseClipboard();
    return TRUE;
}

bool Tilc::OS::Gui::TWindowsClipboard::IsClipboardFormatAvailable(void* Window, unsigned int format)
{
    m_AvailableFormats.clear();
    if (!Window)
    {
        return false;
    }

    if (!::OpenClipboard((HWND)Window))
    {
        return FALSE;
    }

    bool retval = ::IsClipboardFormatAvailable(format);

    ::CloseClipboard();

    return retval;
}

Tilc::TExtString Tilc::OS::Gui::TWindowsClipboard::GetAllAvailableFormats(void* Window, Tilc::TExtString separator)
{
    Tilc::TExtString retval = " ";

    GetAllAvailableFormats(Window);
    size_t formatsCount = m_AvailableFormats.size();
    unsigned int format;
    if (formatsCount > 0)
    {
        int i = 0;
        for (auto it = m_AvailableFormats.begin(); it != m_AvailableFormats.end(); ++it)
        {
            if (i > 0)
            {
                retval += separator;
            }
            format = *it;
            retval += IntToHex(format);
            ++i;
        }
    }

    return retval;
}

Tilc::TExtString Tilc::OS::Gui::TWindowsClipboard::GetAllAvailableFormatsDefines(void* Window, const Tilc::TExtString& separator, bool withNumbers)
{
    Tilc::TExtString retval = " ";

    GetAllAvailableFormats(Window);
    size_t formatsCount = m_AvailableFormats.size();
    unsigned int format;
    if (formatsCount > 0)
    {
        int i = 0;
        for (auto it = m_AvailableFormats.begin(); it != m_AvailableFormats.end(); ++it)
        {
            if (i > 0)
            {
                retval += separator;
            }

            format = *it;

            switch (format)
            {
            case CF_TEXT:
                retval += "CF_TEXT";
                break;
            case CF_BITMAP:
                retval += "CF_BITMAP";
                break;
            case CF_METAFILEPICT:
                retval += "CF_METAFILEPICT";
                break;
            case CF_SYLK:
                retval += "CF_SYLK";
                break;
            case  CF_DIF:
                retval += "CF_DIF";
                break;
            case  CF_TIFF:
                retval += "CF_TIFF";
                break;
            case  CF_OEMTEXT:
                retval += "CF_OEMTEXT";
                break;
            case  CF_DIB:
                retval += "CF_DIB";
                break;
            case  CF_PALETTE:
                retval += "CF_PALETTE";
                break;
            case  CF_PENDATA:
                retval += "CF_PENDATA";
                break;
            case  CF_RIFF:
                retval += "CF_RIFF";
                break;
            case  CF_WAVE:
                retval += "CF_WAVE";
                break;
            case  CF_UNICODETEXT:
                retval += "CF_UNICODETEXT";
                break;
            case  CF_ENHMETAFILE:
                retval += "CF_ENHMETAFILE";
                break;
#if defined CF_HDROP
            case  CF_HDROP:
                retval += "CF_HDRP";
                break;
#endif /* if defined CF_HDROP */
#if defined CF_LOCALE
            case  CF_LOCALE:
                retval += "CF_LOCALE";
                break;
#endif /* if defined CF_LOCALE */
#if defined CF_DIBV5
            case  CF_DIBV5:
                retval += "CF_DIBV5";
                break;
#endif /* if defined CF_DIBV5 */
            case CF_MAX:
                retval += "CF_MAX";
                break;
            case CF_OWNERDISPLAY:
                retval += "CF_OWNERDISPLAY";
                break;
            case CF_DSPTEXT:
                retval += "CF_DSPTEXT";
                break;
            case CF_DSPBITMAP:
                retval += "CF_DSPBITMAP";
                break;
            case CF_DSPMETAFILEPICT:
                retval += "CF_DSPMETAFILEPICT";
                break;
            case CF_DSPENHMETAFILE:
                retval += "CF_DSPENHMETAFILE";
                break;
            case CF_PRIVATEFIRST:
                retval += "Nieznany";
                break;
            case CF_PRIVATELAST:
                retval += "CF_PRIVATELAST";
                break;
            case CF_GDIOBJFIRST:
                retval += "CF_GDIOBJFIRST";
                break;
            case CF_GDIOBJLAST:
                retval += "CF_GDIOBJLAST";
                break;
            default:
                retval += "Nieznany";
                break;
            }

            if (withNumbers)
            {
                retval += Tilc::TExtString(" [") + IntToHex(format) + "]";
            }
            ++i;
        }
    }

    return retval;
}
