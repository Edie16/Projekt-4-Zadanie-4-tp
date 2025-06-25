//Autorzy: Edie Kulik 203578 i Aleksander Rorbach 203749 ACiR 3

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif


//biblioteki
#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>

//Przestrzenie nazw
using namespace Gdiplus;
using namespace std;

//Przyciski (3 do generowania kształtów i 3 do podnoszenia tylko danego kształtu), pole do wpisania wagi i reset towarów
HWND PrzyciskKolo;
HWND PrzyciskTrojkat;
HWND PrzyciskKwadrat;
HWND TylkoKwadrat;
HWND TylkoTrojkat;
HWND TylkoKolo;
HWND Waga;
HWND Reset;

int MaxWaga = 10; //Maksymalna waga, którą może unieść dźwig

//klasa towarów, mają współrzędne, wagę, kształt (chyba się nie przyda) konstruktor i metody, każda rysuje konkretny kształt
class Towary
{
public:
    int wspx, wspy;
    int waga;
    string ksztalt;
    Towary(int x, int y, int w, string k)
    {
        wspx=x;
        wspy=y;
        waga=w;
        ksztalt=k;
    }
    void rysujkolo(Graphics &kolo)
    {
        SolidBrush brush(Color(255, 241, 52, 52));
        kolo.FillEllipse(&brush, wspx-15, wspy-15, 30, 30);
    }
    void rysujtrojkat(Graphics &trojkat)
    {
        SolidBrush brush(Color(255, 191, 78, 230));
        Point myPointArray[] = {Point(wspx, wspy-15), Point(wspx-15, wspy+15), Point(wspx+15, wspy+15)};
        trojkat.FillPolygon(&brush, myPointArray, 3);
    }
    void rysujkwadrat(Graphics &kwadrat)
    {
        SolidBrush brush(Color(255, 96, 216, 152));
        kwadrat.FillRectangle(&brush, wspx-15, wspy-15, 30, 30);
    }
};

//klasa liny, ma współrzędne początku i długość oraz konstruktor i metodę rysowania liny
class Lina
{
public:
    int wspx, wspy;
    int dlugosc;
    Lina(int x, int y, int d)
    {
        wspx=x;
        wspy=y;
        dlugosc=d;
    }
    void rysuj (Graphics &lina)
    {
        Pen pen(Color(255, 0, 0, 0), 2);
        lina.DrawRectangle(&pen, wspx-10, wspy-5, 20, 5);
        lina.DrawLine(&pen, wspx, wspy, wspx, wspy+dlugosc);
        lina.DrawEllipse(&pen, wspx-5, wspy-5+dlugosc, 10, 10);
    }
};

//Podniesiony towar, narazie go nie ma
Towary* podniesiony = nullptr;

//Trzy pudełka dla kształtów
vector<Towary> kola;
vector<Towary> trojkaty;
vector<Towary> kwadraty;

//Funkcja, w której sprawdzamy, czy współrzędne x i y znajdują się w odrębie jakiegoś kształtu
bool czyJestTowar (int x, int y)
{
    for (const auto& kolo : kola)
    {
        if (podniesiony == &kolo) continue;
        int dx = x - kolo.wspx;
        int dy = y - kolo.wspy;
        if (dx*dx + dy*dy <= 15*15)
        {
            return true;
        }
    }
    for (const auto& trojkat : trojkaty)
    {
        if (podniesiony == &trojkat) continue;
        if (x >= trojkat.wspx - 15 && x <= trojkat.wspx + 15 &&
                y >= trojkat.wspy - 15 && y <= trojkat.wspy + 15)
        {
            return true;
        }
    }
    for (const auto& kwadrat : kwadraty)
    {
        if (podniesiony == &kwadrat) continue;
        if (x >= kwadrat.wspx - 15 && x <= kwadrat.wspx + 15 &&
                y >= kwadrat.wspy - 15 && y <= kwadrat.wspy + 15)
        {
            return true;
        }
    }
    return false;
}

Lina lina (500, 180, 300);

//Znajdujemy konkretny kształt, w którym są współrzędne x i y
Towary* znajdzKolo(int x, int y)
{
    for (auto& kolo : kola)
    {
        int dx = x - kolo.wspx;
        int dy = y - kolo.wspy;
        if (dx*dx + dy*dy <= 15*15)
        {
            return &kolo;
        }
    }
    return nullptr;
}
Towary* znajdzTrojkat(int x, int y)
{
    for (auto& trojkat : trojkaty)
    {
        if (x >= trojkat.wspx - 15 && x <= trojkat.wspx + 15 &&
                y >= trojkat.wspy - 15 && y <= trojkat.wspy + 15)
        {
            return &trojkat;
        }
    }
    return nullptr;
}
Towary* znajdzKwadrat(int x, int y)
{
    for (auto& kwadrat : kwadraty)
    {
        if (x >= kwadrat.wspx - 15 && x <= kwadrat.wspx + 15 &&
                y >= kwadrat.wspy - 15 && y <= kwadrat.wspy + 15)
        {
            return &kwadrat;
        }
    }
    return nullptr;
}

//Deklaracja procedury Windowsa
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

//Nazwa klasy
TCHAR szClassName[ ] = _T("AplikacjaDzwig");

//Odpowiednik main dla aplikacji okienkowej
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    //Uruchomienie GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    //Stworzenie okna, używam code::blocks i przy stworzeniu projektu sam to dodał, zmieniłem kolor tła i nazwę okienka
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush(RGB(22, 79, 83));
    if (!RegisterClassEx (&wincl))
        return 0;
    hwnd = CreateWindowEx (0, szClassName, _T("Dzwig"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800, HWND_DESKTOP, NULL, hThisInstance, NULL);
    ShowWindow (hwnd, nCmdShow);

    //Kontynuacja prrzycisków, są opisane cyframi
    PrzyciskKolo = CreateWindowEx( 0, "BUTTON", "Kolo", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 900, 200, 150, 30, hwnd, (HMENU)1, hThisInstance, NULL );
    PrzyciskTrojkat = CreateWindowEx( 0, "BUTTON", "Trojkat", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 900, 250, 150, 30, hwnd, (HMENU)2, hThisInstance, NULL );
    PrzyciskKwadrat = CreateWindowEx( 0, "BUTTON", "Kwadrat", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 900, 300, 150, 30, hwnd, (HMENU)3, hThisInstance, NULL );
    TylkoKolo = CreateWindow( "BUTTON", "Wieza tylko z kol",WS_CHILD | WS_VISIBLE |  BS_AUTOCHECKBOX | BS_PUSHLIKE, 110, 30, 300, 30, hwnd, (HMENU)4, hThisInstance, NULL);
    TylkoTrojkat = CreateWindow( "BUTTON", "Wieza tylko z trojkatow",WS_CHILD | WS_VISIBLE |  BS_AUTOCHECKBOX | BS_PUSHLIKE, 420, 30, 300, 30, hwnd, (HMENU)5, hThisInstance, NULL);
    TylkoKwadrat = CreateWindow( "BUTTON", "Wieza tylko z kwadratow",WS_CHILD | WS_VISIBLE |  BS_AUTOCHECKBOX | BS_PUSHLIKE, 730, 30, 300, 30, hwnd, (HMENU)6, hThisInstance, NULL);
    Waga = CreateWindowEx(0, "EDIT", "Waga (usun i wpisz)", WS_CHILD | WS_VISIBLE | WS_BORDER, 900, 350, 150, 30, hwnd, (HMENU)7, hThisInstance, NULL);
    Reset = CreateWindowEx( 0, "BUTTON", "Resetuj", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 900, 400, 150, 30, hwnd, (HMENU)8, hThisInstance, NULL );


    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    //Zamnięcie GDI+
    GdiplusShutdown(gdiplusToken);

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Graphics dzwig(hdc);
        Graphics ziemia(hdc);

        Pen penz(Color(255, 252, 255, 74), 2);
        //Rysowanie dźwigu i podłoża
        dzwig.DrawRectangle(&penz, 400, 120, 20, 400);
        dzwig.DrawLine(&penz, 400, 120, 420, 100);
        dzwig.DrawLine(&penz, 420, 100, 420, 120);
        for (int i = 0; i < 20; i++)
        {
            dzwig.DrawRectangle(&penz, 400, 120 + i*20, 20, 20);
            dzwig.DrawLine(&penz, 420, 120 + i*20, 400, 140 + i*20);
        }

        dzwig.DrawRectangle(&penz, 320, 160, 480, 20);
        dzwig.DrawLine(&penz, 800, 160, 820, 180);
        dzwig.DrawLine(&penz, 820, 180, 800, 180);
        for (int i = 0; i < 24; i++)
        {
            dzwig.DrawRectangle(&penz, 320 + i*20, 160, 20, 20);
            dzwig.DrawLine(&penz, 320 + i*20, 180, 340 + i*20, 160);
        }

        Pen penc(Color(255, 0, 0, 0), 2);
        dzwig.DrawLine(&penc, 420, 100, 320, 160);
        dzwig.DrawLine(&penc, 420, 100, 540, 160);
        dzwig.DrawLine(&penc, 420, 100, 660, 160);

        dzwig.DrawRectangle(&penc, 310, 150, 10, 40);
        dzwig.DrawRectangle(&penc, 300, 150, 10, 40);
        dzwig.DrawRectangle(&penc, 290, 150, 10, 40);
        dzwig.DrawRectangle(&penc, 280, 150, 10, 40);

        dzwig.DrawLine(&penc, 400, 520, 380, 580);
        dzwig.DrawLine(&penc, 420, 520, 440, 580);
        dzwig.DrawRectangle(&penc, 420, 580, 40, 10);
        dzwig.DrawRectangle(&penc, 360, 580, 40, 10);

        dzwig.DrawRectangle(&penc, 370, 520, 80, 10);
        dzwig.DrawRectangle(&penc, 370, 530, 80, 10);
        dzwig.DrawRectangle(&penc, 370, 540, 80, 10);
        dzwig.DrawRectangle(&penc, 370, 550, 80, 10);

        dzwig.DrawRectangle(&penc, 390, 480, 40, 80);
        dzwig.DrawRectangle(&penc, 380, 470, 60, 10);

        dzwig.DrawRectangle(&penc, 420, 182, 20, 40);
        Point myPointArray[] = {Point(400, 210), Point(420, 210), Point(440, 190), Point(440, 230), Point(400, 230)};
        dzwig.DrawPolygon(&penc, myPointArray, 5);
        SolidBrush brush(Color(255, 252, 255, 74));
        dzwig.FillPolygon(&brush, myPointArray, 5);

        ziemia.DrawLine(&penc, 320, 590, 820, 590);

        lina.rysuj(dzwig);

        //Rysowanie wszystkich kształtów w vektorach
        if (kola.size() > 0)
        {
            for (auto& kolo : kola)
            {
                kolo.rysujkolo(dzwig);
            }
        }
        if (trojkaty.size() > 0)
        {
            for (auto& trojkat : trojkaty)
            {
                trojkat.rysujtrojkat(dzwig);
            }
        }
        if (kwadraty.size() > 0)
        {
            for (auto& kwadrat : kwadraty)
            {
                kwadrat.rysujkwadrat(dzwig);
            }
        }

        Graphics napis(hdc);

        //"Rysowanie" napisu
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
        SolidBrush text(Color(255, 252, 255, 74));
        PointF pointF(240, 600);
        napis.DrawString(L"Przesuwaj linke strzalkami, towar podnos enterem i upuszczaj spacja", -1, &font, pointF, &text);

        Graphics przycisk(hdc);
        SolidBrush brushp(Color(255, 250, 95, 33));

        //Rysowanie obramowania dla przycisków jeśli są wciśnięte, żeby było widać, że są wciśnięte
        if(IsDlgButtonChecked(hwnd, 4) == BST_CHECKED)
        {
            przycisk.FillRectangle(&brushp, 105, 25, 310, 40);
        }
        if(IsDlgButtonChecked(hwnd, 5) == BST_CHECKED)
        {
            przycisk.FillRectangle(&brushp, 415, 25, 310, 40);
        }
        if(IsDlgButtonChecked(hwnd, 6) == BST_CHECKED)
        {
            przycisk.FillRectangle(&brushp, 725, 25, 310, 40);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_COMMAND:
        //Instrukcje po naciśnięciu przycisków
        switch (LOWORD(wParam))
        {
        //Dodawanie kształtów, przypisywanie im wagi i sprawdzanie, czy kształt może być dodany
        case 1:
        {
            char napis[6];
            int waga = 10;
            GetWindowText(Waga, napis, 6);
            waga = atoi(napis);
            if (!czyJestTowar(500, 575)&&!podniesiony)
            {
                kola.push_back(Towary(500, 575, waga, "kolo"));
                InvalidateRect(hwnd, NULL, TRUE); //Odświerzanie
            }
            else
            {
                MessageBox(hwnd, _T("Nie mozna dodac kolejnego kolka, cos juz jest w pozycji startowej lub dzwig podnosi inny towar (nie chcemy go przeciez rozpraszac)"), _T("No sorka"), MB_OK | MB_ICONWARNING);
            }
            SendMessage(PrzyciskKolo, BM_SETSTATE, FALSE, 0); //Program skupiał się na przycisku i nie pozwalał poruszać się linką
            SetFocus(hwnd);
            break;
        }
        case 2:
        {
            char napis[6];
            int waga = 10;
            GetWindowText(Waga, napis, 6);
            waga = atoi(napis);
            if (!czyJestTowar(550, 575)&&!podniesiony)
            {
                trojkaty.push_back(Towary(550, 575, waga, "trojkat"));
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else
            {
                MessageBox(hwnd, _T("Nie mozna dodac kolejnego trojkata, cos juz jest w pozycji startowej lub dzwig podnosi inny towar (nie chcemy go przeciez rozpraszac)"), _T("No sorka"), MB_OK | MB_ICONWARNING);
            }
            SendMessage(PrzyciskTrojkat, BM_SETSTATE, FALSE, 0);
            SetFocus(hwnd);
            break;
        }
        case 3:
        {
            char napis[6];
            int waga = 10;
            GetWindowText(Waga, napis, 6);
            waga = atoi(napis);
            if (!czyJestTowar(600, 575)&&!podniesiony)
            {
                kwadraty.push_back(Towary(600, 575, waga, "kwadrat"));
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else
            {
                MessageBox(hwnd, _T("Nie mozna dodac kolejnego kwadratu, cos juz jest w pozycji startowej lub dzwig podnosi inny towar (nie chcemy go przeciez rozpraszac)"), _T("No sorka"), MB_OK | MB_ICONWARNING);
            }
            SendMessage(PrzyciskKwadrat, BM_SETSTATE, FALSE, 0);
            SetFocus(hwnd);
            break;
        }
        //Odznaczanie innych przycisków, clearowanie vectorów, żeby nie było innej wieży niż ta, która ma być
        case 4:
            CheckDlgButton(hwnd, 5, BST_UNCHECKED);
            CheckDlgButton(hwnd, 6, BST_UNCHECKED);
            kola.clear();
            trojkaty.clear();
            kwadraty.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case 5:
            CheckDlgButton(hwnd, 4, BST_UNCHECKED);
            CheckDlgButton(hwnd, 6, BST_UNCHECKED);
            kola.clear();
            trojkaty.clear();
            kwadraty.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case 6:
            CheckDlgButton(hwnd, 4, BST_UNCHECKED);
            CheckDlgButton(hwnd, 5, BST_UNCHECKED);
            kola.clear();
            trojkaty.clear();
            kwadraty.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case 8:
            kola.clear();
            trojkaty.clear();
            kwadraty.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        //Przesuwanie liny o 5px i, jeżeli istnieje podniesiony towar, przesuwanie jego w zależności od strzałek
        case VK_LEFT:
            if(lina.wspx>=325)
            {
                lina.wspx = lina.wspx-5;
                if(podniesiony) podniesiony->wspx = podniesiony->wspx-5;
            }
            break;
        case VK_RIGHT:
            if(lina.wspx<=795)
            {
                lina.wspx = lina.wspx+5;
                if(podniesiony) podniesiony->wspx = podniesiony->wspx+5;
            }
            break;
        case VK_UP:
            if(lina.dlugosc>=5)
            {
                lina.dlugosc = lina.dlugosc-5;
                if(podniesiony) podniesiony->wspy = podniesiony->wspy-5;
            }
            break;
        case VK_DOWN:
            if(lina.dlugosc<=405)
            {
                lina.dlugosc = lina.dlugosc+5;
                if(podniesiony) podniesiony->wspy = podniesiony->wspy+5;
            }
            break;
        //Podnoszenie towaru enterem, sprawdzanie, czy już istnieje podniesiony przedmiot, czy jakiś przycisk od podnoszenia konkretnego kształu jest wciśnięty i czy ma odpowiednią wage, przesunięcie towaru do końca linki
        case VK_RETURN:
            if(!podniesiony&&czyJestTowar(lina.wspx,lina.wspy+lina.dlugosc))
            {
                if(IsDlgButtonChecked(hwnd, 4) == BST_UNCHECKED&&IsDlgButtonChecked(hwnd, 5) == BST_UNCHECKED&&IsDlgButtonChecked(hwnd, 6) == BST_UNCHECKED)
                {
                    podniesiony = znajdzKolo(lina.wspx, lina.wspy + lina.dlugosc);
                    if (!podniesiony) podniesiony = znajdzTrojkat(lina.wspx, lina.wspy + lina.dlugosc);
                    if (!podniesiony) podniesiony = znajdzKwadrat(lina.wspx, lina.wspy + lina.dlugosc);
                    if(podniesiony->waga<=MaxWaga)
                    {
                        podniesiony->wspx = lina.wspx;
                        podniesiony->wspy = lina.wspy + lina.dlugosc;
                    }
                    else
                    {
                        MessageBox(hwnd, _T("Towar jest za ciezki, nie moge go podniesc!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                        podniesiony = nullptr;
                    }

                }
                if(IsDlgButtonChecked(hwnd, 4) == BST_CHECKED)
                {
                    podniesiony = znajdzKolo(lina.wspx, lina.wspy + lina.dlugosc);
                    if(podniesiony)
                    {
                        if(podniesiony->waga<=MaxWaga)
                        {
                            podniesiony->wspx = lina.wspx;
                            podniesiony->wspy = lina.wspy + lina.dlugosc;
                        }
                        else
                        {
                            MessageBox(hwnd, _T("To kolko jest za ciezkie, nie moge go podniesc!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                            podniesiony = nullptr;
                        }
                    }
                    else
                    {
                        MessageBox(hwnd, _T("To nie kolo!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }
                }
                if(IsDlgButtonChecked(hwnd, 5) == BST_CHECKED)
                {
                    podniesiony = znajdzTrojkat(lina.wspx, lina.wspy + lina.dlugosc);
                    if(podniesiony)
                    {
                        if(podniesiony->waga<=MaxWaga)
                        {
                            podniesiony->wspx = lina.wspx;
                            podniesiony->wspy = lina.wspy + lina.dlugosc;
                        }
                        else
                        {
                            MessageBox(hwnd, _T("Ten trojkat jest za ciezki, nie moge go podniesc!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                            podniesiony = nullptr;
                        }
                    }
                    else
                    {
                        MessageBox(hwnd, _T("To nie trojkat!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }
                }
                if(IsDlgButtonChecked(hwnd, 6) == BST_CHECKED)
                {
                    podniesiony = znajdzKwadrat(lina.wspx, lina.wspy + lina.dlugosc);
                    if(podniesiony)
                    {
                        if(podniesiony->waga<=MaxWaga)
                        {
                            podniesiony->wspx = lina.wspx;
                            podniesiony->wspy = lina.wspy + lina.dlugosc;
                        }
                        else
                        {
                            MessageBox(hwnd, _T("Ten kwadrat jest za ciezki, nie moge go podniesc!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                            podniesiony = nullptr;
                        }
                    }
                    else
                    {
                        MessageBox(hwnd, _T("To nie kwadrat!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }
                }
            }
            break;
        //Upuszczanie towaru spacją, jak jest już towar pod, to można zrobić wieże, wysoką na max.3 towary
        case VK_SPACE:
            if (podniesiony)
            {
                if(!czyJestTowar(podniesiony->wspx,575))
                {
                    podniesiony->wspy = 575;
                    podniesiony = nullptr;
                }
                else if(czyJestTowar(podniesiony->wspx,515))
                {
                    MessageBox(hwnd, _T("Wieza jest juz duza"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                }
                else if(czyJestTowar(podniesiony->wspx,545))
                {
                    podniesiony->wspy = 515;
                    podniesiony = nullptr;
                }
                else if(czyJestTowar(podniesiony->wspx,575)&&IsDlgButtonChecked(hwnd, 4) == BST_UNCHECKED&&IsDlgButtonChecked(hwnd, 5) == BST_UNCHECKED&&IsDlgButtonChecked(hwnd, 6) == BST_UNCHECKED)
                {
                    podniesiony->wspy = 545;
                    podniesiony = nullptr;
                }
                else if(czyJestTowar(podniesiony->wspx,575)&&IsDlgButtonChecked(hwnd, 4) == BST_CHECKED)
                {
                    if(znajdzKolo(podniesiony->wspx,575))
                    {
                        podniesiony->wspy = 545;
                        podniesiony = nullptr;
                    }
                    else
                    {
                        MessageBox(hwnd, _T("Nie mozesz tutaj tego polozyc, to nie kolo"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }

                }
                else if(czyJestTowar(podniesiony->wspx,575)&&IsDlgButtonChecked(hwnd, 5) == BST_CHECKED)
                {
                    if(znajdzTrojkat(podniesiony->wspx,575))
                    {
                        podniesiony->wspy = 545;
                        podniesiony = nullptr;
                    }
                    else
                    {
                        MessageBox(hwnd, _T("Nie mozesz tutaj tego polozyc, to nie trojkat"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }
                }
                else if(czyJestTowar(podniesiony->wspx,575)&&IsDlgButtonChecked(hwnd, 6) == BST_CHECKED)
                {
                    if(znajdzKwadrat(podniesiony->wspx,575))
                    {
                        podniesiony->wspy = 545;
                        podniesiony = nullptr;
                    }
                    else
                    {
                        MessageBox(hwnd, _T("Nie mozesz tutaj tego polozyc, to nie kwadrat"), _T("No sorka"), MB_OK | MB_ICONWARNING);
                    }
                }
            }
            break;
        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage (0);
        break;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
