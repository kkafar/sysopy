1. Kompilacja poleceniem `make`
2. Przykladowe wywolanie programu dostepne pod poleceniem `make test` <-- wywolywane komendy znajduja sie w pliku `testcommands.txt`
3. Nie ma ograniczenia na liczbe komend w poleceniu (ograniczona jest tylko liczba znakow w linii). Tak samo z liczba argumentow przy danej komendzie.
4. Uzycie programu: ./main `sciezka do pliku z komendami`
5. Format pliku z komendami:
    1. Definicje polecen (linie w ktorych wystepuje znak `=`) musza byc oddzielone pojedyncza pusta linia od polecen przeznaczonych do wykonania
    2. Wszystkie slowa powinny byc oddzielone pojedyncza spacja.
    3. Przyklad w pliku testcommand.txt