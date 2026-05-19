#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    Tilc::GameObject->AddFont("Label", t->GetThemeDir() +  "/fonts/Aller_Bd.ttf", 36.0f);
    Tilc::GameObject->InitGuiMode();
    
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    float X = 20.0f;
    float Y = 30.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TTextField* txt;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 1", { X, Y, 1400, 25 },
        "Wartość 1 zawierająca długaśny tekst mający nam ułatwić testowanie zaznaczenia w kontrolce tekstowej. I trochę polskich literek: ąćżęńćłó ĄĆĘŁÓŃŚŹŻ "
        "Wartość 1 zawierająca długaśny tekst mający nam ułatwić testowanie zaznaczenia w kontrolce tekstowej. I trochę polskich literek: ąćżęńćłó ĄĆĘŁÓŃŚŹŻ"
    );
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 2", { X, Y, 220, 25 }, "Wartość 2");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 3", { X, Y, 220, 25 }, "Wartość 3");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 4", { X, Y, 220, 25 }, "Wartość 4");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 4", { X, Y, 220, 25 }, "Wartość 4");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 5", { X, Y, 220, 25 }, "Wartość 5");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 6", { X, Y, 220, 25 }, "Wartość 6");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 7", { X, Y, 220, 25 }, "Wartość 7");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 8", { X, Y, 220, 25 }, "Wartość 8");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 9", { X, Y, 220, 25 }, "Wartość 9");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 10", { X, Y, 220, 25 }, "Wartość 10");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 11", { X, Y, 220, 25 }, "Linijka 11");
    Y += 30.0f;
    
    Tilc::Gui::TMultilineTextField* memo;
    memo = new Tilc::Gui::TMultilineTextField(wnd, "Multiline Text Field 1", { 260, 60, 1500, 825 },
        R"##(Materiał z wikipedii: https://pl.wikipedia.org/wiki/Polska
Polska, Rzeczpospolita Polska (RP) – państwo unitarne[12] w Europie Środkowej[13], położone między Morzem Bałtyckim na północy a Sudetami i Karpatami na południu, w przeważającej części w dorzeczu Wisły i Odry. Od północy Polska graniczy z Rosją (z jej obwodem królewieckim) i Litwą, od wschodu z Białorusią i Ukrainą, od południa ze Słowacją i Czechami, od zachodu z Niemcami[14]. Większość północnej granicy Polski wyznacza wybrzeże Morza Bałtyckiego. Polska Wyłączna Strefa Ekonomiczna na Bałtyku graniczy ze strefami Danii i Szwecji. Granice z Ukrainą, Białorusią i Rosją stanowią równocześnie granicę zewnętrzną NATO, Unii Europejskiej i strefy Schengen.

Powierzchnia administracyjna Polski wynosi 313 933 km²[a][4], co daje jej 69. miejsce na świecie[g][15] i 9. w Europie. Zamieszkana przez 37 563 071 osób (30 VI 2024 r.)[5], zajmuje pod względem liczby ludności 38. miejsce na świecie (wg danych za 2020 rok)[16], a 5. w Unii Europejskiej. Polska podzielona jest na 16 województw. Jej miastem z największą ludnością i jednocześnie stolicą jest Warszawa. Inne metropolie to Kraków, Wrocław, Łódź, Poznań, Gdańsk, Szczecin. Największą polską konurbacją jest konurbacja górnośląska. Polska jest krajem jednolitym etnicznie – 97% ludności deklaruje narodowość polską.

Pierwszą historycznie potwierdzoną datą opisującą dzieje Polski jest rok 966, gdy książę Mieszko I, władca obszarów mieszczących się współcześnie w większości w granicach Polski, przyjął chrzest. W 1025 powstało Królestwo Polskie, którego pierwszym królem był syn Mieszka I, Bolesław I Chrobry. W 1385 Polska zawarła porozumienie z Wielkim Księstwem Litewskim, podpisując akt unii w Krewie; związek ten zacieśniła unia lubelska z 1569, w wyniku czego powstała Rzeczpospolita Obojga Narodów, jedno z największych i najludniejszych państw na mapie szesnasto- i siedemnastowiecznej Europy[17][18], które w latach 1618–1621, po rozejmie w Dywilinie, miało powierzchnię około 1 mln km². Jej ustrój wewnętrzny określany jest jako demokracja szlachecka, zaś monarchę wybierano w systemie tak zwanej wolnej elekcji. Rzeczpospolita przestała istnieć w wyniku III rozbioru, w 1795, kiedy to jej terytorium podzielone zostało między Prusy, Rosję i Austrię.

Po 123 latach, pod koniec I wojny światowej, w 1918, Polska odzyskała niepodległość (odrodzona wówczas państwowość nazywana jest II Rzecząpospolitą). 1 września 1939 atakiem Niemiec na Polskę rozpoczęła się II wojna światowa, zaś 17 września 1939 nastąpiła agresja ZSRR; wobec tego w październiku 1939 całe terytorium kraju znalazło się pod okupacją niemiecką i sowiecką. W wyniku II wojny światowej życie straciło ponad sześć milionów obywateli Polski[19][20]. Konferencja jałtańska w lutym 1945 umieściła Polskę w strefie wpływów ZSRR. W lipcu 1945 postanowieniami konferencji poczdamskiej granice Polski przesunięto na zachód (między linię Odry i Nysy Łużyckiej, a linię Curzona), przyłączając Ziemie Odzyskane w miejsce Kresów Wschodnich. Polska Rzeczpospolita Ludowa (nazwa państwa przyjęta w 1952) była państwem satelickim ZSRR w formule tzw. demokracji ludowej. Rządy w systemie monopartyjnym sprawowała komunistyczna Polska Partia Robotnicza, a następnie Polska Zjednoczona Partia Robotnicza (przy formalnym istnieniu ugrupowań satelickich). W okresie zimnej wojny PRL należała do Układu Warszawskiego, stanowiła zatem część tzw. bloku wschodniego.

Po przemianach politycznych zapoczątkowanych w konsekwencji Okrągłego Stołu i częściowo demokratycznych wyborów parlamentarnych w czerwcu 1989 został powołany pierwszy rząd z premierem spoza partii komunistycznej. Polska zmodyfikowała konstytucję, zmieniając 31 grudnia 1989 nazwę państwa na Rzeczpospolita Polska i stając się krajem demokratycznym.

Mimo olbrzymich strat w ludziach oraz znacznego zniszczenia kraju w wyniku II wojny światowej w Polsce udało się zachować wiele bogactwa kulturowego. Znajduje się tu 17 miejsc wpisanych na listę światowego dziedzictwa UNESCO (15 obiektów dziedzictwa kulturowego i 2 o charakterze przyrodniczym)[21], 123 pomniki historii[22][23] oraz duża liczba zarejestrowanych zabytków.

Od początku transformacji ustrojowej w gospodarkę rynkową, Polska utrzymuje bardzo wysoki wskaźnik rozwoju społecznego (HDI). W kraju stopniowo zwiększa się wolność ekonomiczna[24]. Polska jest demokratycznym państwem z rozwiniętą, wysokodochodową gospodarką i wysokim wskaźnikiem jakości życia[25]; większość Polaków (57%) pracuje w sektorze usług. Ponadto rocznie Polskę odwiedza około 21,4 mln turystów (2025), dzięki czemu jest jednym z najczęściej odwiedzanych krajów świata[26]. Polska jest szóstą co do wielkości gospodarką w Unii Europejskiej[27] i jedną z najszybciej rozwijających się gospodarek europejskich[28]. W badaniu wskaźnika demokracji, przeprowadzanym przez zespół związany z tygodnikiem The Economist, Polska zajęła w 2024 roku 39. miejsce (na 167 przebadanych państw i terytoriów), co oznacza wzrost od poprzedniego roku o dwa miejsca, a demokracja kraju jest określana jako wadliwa[29][30]. Według wskaźnika Global Peace Index z 2017 roku Polska zajmuje 33. miejsce na świecie (22. w Europie) pod względem poziomu bezpieczeństwa, natomiast wg raportu firmy Underwriters Laboratories z 2018 roku, Polska pod względem bezpieczeństwa zajmuje miejsce 35. (27. w Europie)[31][32]. Wskaźnik wolności prasy klasyfikuje Polskę na 31. pozycji w świecie w 2025 roku (wolność prasy gwarantuje art. 14 Konstytucji)[33].

Polska należy m.in. do UE, ONZ, NATO, OBWE, WTO, OECD i Rady Europy, zaś Warszawa stanowi siedzibę Frontexu. 
)##"
    );

    Tilc::Gui::TButton* btnSave = new Tilc::Gui::TButton(wnd, "SaveButton", { X, Y, 100, 25 }, "Zapisz");
    Tilc::Gui::TButton* btnCancel = new Tilc::Gui::TButton(wnd, "CancelButton", { X + 120.0f, Y, 100, 25 }, "Anuluj");
}

TStateGame::~TStateGame()
{
	if (m_TextBox)
	{
		delete m_TextBox;
		m_TextBox = nullptr;
	}
}

void TStateGame::OnCreate()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Escape", &TStateGame::KeyEscapePressed, this);
	}
	m_Elapsed = 0.0f;

    Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
    if (Window->m_TopmostWindow)
    {
        Window->m_TopmostWindow->AllowResizing(true);
    }
}

void TStateGame::OnDestroy()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Escape");
	}
}

void TStateGame::Update(float DeltaTime)
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;

	m_Elapsed += DeltaTime;
}

void TStateGame::Draw()
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
	static SDL_Renderer* Renderer = Window->GetRenderer();

    SDL_RenderClear(Renderer);
    Window->m_TopmostWindow->Draw();
}

void TStateGame::KeyEscapePressed(Tilc::TEventDetails* Details)
{
}
