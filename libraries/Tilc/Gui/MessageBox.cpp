#include "Tilc/Gui/MessageBox.h"
#include "SDL3/SDL.h"

DECLSPEC int Tilc::Gui::MessageBox(Tilc::TExtString caption, Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), caption.getBuffer(), MB_OK | MB_ICONINFORMATION);
    return result;
}

DECLSPEC int Tilc::Gui::AlertBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Uwaga", MB_OK | MB_ICONWARNING);
    return result;
}

DECLSPEC int Tilc::Gui::WarningBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Uwaga", MB_OK | MB_ICONWARNING);
    return result;
}

DECLSPEC int Tilc::Gui::QuestionBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Pytanie", MB_YESNO | MB_ICONQUESTION);
    return result;
}

DECLSPEC int Tilc::Gui::QuestionCancelBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Pytanie", MB_YESNOCANCEL | MB_ICONQUESTION);
    return result;
}

DECLSPEC int Tilc::Gui::ErrorBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Błąd", MB_OK | MB_ICONERROR);
    return result;
}

DECLSPEC int Tilc::Gui::InformationBox(Tilc::TExtString message)
{
    int result = 0;//::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Informacja", MB_OK | MB_ICONINFORMATION);
    return result;
}

DECLSPEC bool Tilc::Gui::IsMessageBoxOpened()
{
    return false;
}

DECLSPEC void Tilc::Gui::ShowSystemError(const Tilc::TExtString PrependString)
{
	ErrorBox(PrependString + SDL_GetError());
}
