# SPRAWOZDANIE
### Edie Kulik 203578, Aleksander Rorbach 203749
1. **Podstawowa funkcjonalność**

Przesuwanie liny jest za pomocą strzałek, w kodzie jest klasa Lina i dodany element lina, ma współrzędne początkowe i długość, strzałki przesuwają linę o 5px, strzałka w lewo przesuwa współrzędną x w lewo (odejmuje 5px), strzałka w prawo przesuwa w prawo, strzałka w górę skraca długość, a w dół wydłuża, następnie jest odświeżenie ekranu, żeby linka się przesunęła. Przykład (strzałka w lewo):
```c++
case VK_LEFT:
            if(lina.wspx>=325)
            {
                lina.wspx = lina.wspx-5;
                if(podniesiony) podniesiony->wspx = podniesiony->wspx-5;
            }
            break;
```
Dźwig podnosi towar, kiedy wciśnie się enter, sprawdza czy już jest podniesiony towar i jak go nie ma oraz znajduje się w obszarze jakiegoś towaru to go podnosi, współrzędne towaru zmieniają się na współrzędne końca linki. Kod dla podstawowej funkcjonalności (później dodanie sprawdzania kształtów i wagi):
```c++
case VK_RETURN:
            if(!podniesiony&&czyJestTowar(lina.wspx,lina.wspy+lina.dlugosc))
            {
                    podniesiony = znajdzKolo(lina.wspx, lina.wspy + lina.dlugosc);
                    if (!podniesiony) podniesiony = znajdzTrojkat(lina.wspx, lina.wspy + lina.dlugosc);
                    if (!podniesiony) podniesiony = znajdzKwadrat(lina.wspx, lina.wspy + lina.dlugosc);
                    podniesiony->wspx = lina.wspx;
                    podniesiony->wspy = lina.wspy + lina.dlugosc;
            }
            break;
```
Do sprawdzania, czy towar znajduje się na końcu linki jest funkcja przeszukująca każdy vektor (kola, trojkaty i kwadraty) i sprawdzjąca, czy x i y znajdują się w obszarze współrzędnych towarów, dla ułatwienia trójkąt jest sprawdzany jak kwadrat. Dla koła: (pętla ignoruje podniesiony towar, żeby nie brać go pod uwagę przy jego odkładaniu)
```c++
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
```
Jeżeli istnieje podniesiony towar, ma zmieniane współrzędne razem z końcem linki, jest to pokazane w kodzie od strzałki w lewo.  
Upuszczanie jest za pomocą spacji, współrzędna y podniesionego zmienia się i podniesiony znowu nie istnieje, sprawdzane jest czy pod podniesionym towarem jest inny towar i jeżeli jest, to robi się wieża, która może mieć maksymalnie 3 towary, jak użytkownik chce upuścić 4 towar pojawia się okienko mówiące, że się nie da.
```c++
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
else if(czyJestTowar(podniesiony->wspx,575))
{
    podniesiony->wspy = 545;
    podniesiony = nullptr;
}
```
Dodane są też 3 przyciski, które jak się wciśnie dodają dany kształt do vektora na pozycje startową i odświeżają ekran, żeby się pojawił, program sprawdza też, czy już jest jakiś towar, tam gdzie ma się pojawić kolejny.

2. **Sprawdzanie, czy podnoszony towar ma kształt koła/trójkąta/kwadratu**

Dodałem 3 przyciski typu checkbox, wyglądające jak zwykłe *BS_PUSHLIKE* i rysowanie pomarańczowego obramowania jak przycisk jest wciśnięty. Przykład koła:
```c++
TylkoKolo = CreateWindow( "BUTTON", "Wieza tylko z kol",WS_CHILD | WS_VISIBLE |  BS_AUTOCHECKBOX | BS_PUSHLIKE, 110, 30, 300, 30, hwnd, (HMENU)4, hThisInstance, NULL);
```
```c++
if(IsDlgButtonChecked(hwnd, 4) == BST_CHECKED)
{
    przycisk.FillRectangle(&brushp, 105, 25, 310, 40);
}
```
Kiedy sie jeden wciśnie inne są *odciskiwane*.  
W kodzie podnoszenia dodany jest if sprawdzający, czy któryś z tych przycisków jest zaznaczony, jeżeli jest to sprawdza, czy koniec liny znajduje się w kształcie, który może podnieść, jeżeli jest w innym pojawia się komunikat.
```c++
if(IsDlgButtonChecked(hwnd, 4) == BST_CHECKED)
{
    podniesiony = znajdzKolo(lina.wspx, lina.wspy + lina.dlugosc);
    if(podniesiony)
    {
        podniesiony->wspx = lina.wspx;
        podniesiony->wspy = lina.wspy + lina.dlugosc;
    }
    else
    {
        MessageBox(hwnd, _T("To nie kolo!"), _T("No sorka"), MB_OK | MB_ICONWARNING);
    }
}
```
Upuszczanie jest takie jak w podstawowej funkcjonalności, bo już dało się ułożyć wieże z maksymalnie 3 elementów, ale dodatkowo sprawdzany jest kształt na podłożu, jeżeli jest inny, nie można upuścić towaru.
```c++
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
```
3. **Sprawdzanie masy**

Maksymalna waga jest ustawiona globalnie na 10, to czy jest to kg, czy ton, jest do własnej interpretacji.
```c++
int MaxWaga = 10;
```
Dodałem pole do wpisania wagi.
```c++
Waga = CreateWindowEx(0, "EDIT", "Waga (usun i wpisz)", WS_CHILD | WS_VISIBLE | WS_BORDER, 900, 350, 150, 30, hwnd, (HMENU)7, hThisInstance, NULL);
```
Przy dodawaniu kształtu przypisuje wpisaną wagę, domyślnie obiekty mają wagę 10.
```c++
char napis[6];
int waga = 10;
GetWindowText(Waga, napis, 6);
waga = atoi(napis);
```
Przy podnoszeniu program sprawdza, czy podniesiony towar ma odpowiednią wagę, jeśli jest za ciężki pojawia się komunikat i dźwig *upuszcza* towar.
```c++
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
```