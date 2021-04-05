# Kompilacja 

1. Sam program kompilujemy poleceniem `make main`. Wykonanie `make` skutkuje rowniez skompilowaniem generatora testow. 
2. Generator testow mozemy skompilowac poleceniem `make testgen`.

# Uruchamianie

1. Kompilujemy.
2. Uruchamiamy poleceniem `./main <sciezka (para1, plik1)> <sciezka (para1, plik2)> <sciezka do pliku wynikowego z laczenia 1. pary> ...` itd. (deklarujemy zawsze trojkami. Mozna deklarowac dowolnie wiele trojek (ograniczone zasobnascia pamieci i rozmiarem zmiennej typu `size_t` na maszynie docelowej)).


# Testy

1. Testy generujemy polecniem `make generate_tests` (jest to krok konieczny do uruchomienia testow).
2. Uruchamiamy poleceniem `make test`. Wynik zostanie zapisany do pliku `report2.txt`.

3. Testy mozna przeprowadzic takze recznie uruchamiajac program z nastepujaca skladnia: `./main -t -d <sciezka do katalogu z plikami testowymi>`. W katalogu z plikami testowymi powinny znajdowac sie tylko pliki testowe oraz powinna byc ich parzysta ilosc. Pliki laczone sa parami w kolejnosci wpisow zwracanych przez `readdir`. Wynik mergowania w trybie testowym (opcja -t) nie jest zapisywany (poniewaz w lab2 czas mierzony byl takze bez operacji zapisu -- tylko mergowanie).
W przypadku nie korzystania z polecenia `make test` wyniki zostana zapisane do pliku `report.txt`.


# Dodatkowe polecenia

1. `make clean` -- usuwa wszystkie pliki z rozszerzeniem *.o
2. `make clean_all` -- usuwa wszystkie pliki z rozszerzeniem *.o oraz pliki wykonywalne `main` oraz `testgen` oraz cala zawartosc katalogu `tests/` (jezeli zostal wczesniej utworzony np. poleceniem `make generate_tests`).


## Komentarz 

Znajduje sie takze w pliku `main_report2.txt`.

1. Usertime jest we wszystkich przypadkach rowny 0, poniewaz z procesu glownego (pierwszego) nie sa wykonywane zadne operacje. 
2. Ogolnie zauwazamy ogromne skrocenie czasu wykonania programu z punktu widzenia uzytkownika (realtime) -- ok. 4.1 razy krotszy czas wykonania dla najwiekszego 
przypadku testowego. (W ogolnosci takze zauwazamy ogromne przyspieszenie).
Jest to spowodowane zrownolegleniem wykonywania operacji (testowanie z narzedziem gnome-system-monitor (system: Linux Mint 20) wykazuje ze obliczenia 
wykonywane sa rownolegle na rownych rdzeniach procesora). Ogolnie jednak czas poswiecony przez procesor jest dluzszy. (czas naliczany rownolegle na roznych watkach + czas poswiecony na kopiowanie obszarów pamieci procesu (fork)).