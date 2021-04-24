1. Numeracja linii w plikach jest od 0
2. Kompilacja poleceniem `make`
3. W katalogu ./testdata znajduja sie dane wykorzystywane przy przykladowych wywolaniach programu. Zapisywane sa tam rowniez wyniki dzialania programu. 
4. W celu sprawdzenia jakie parametry przyjmuje program nalezy wywolac go bez zadnych parametrow -- zostanie wtedy wypisany komunikat z skladnia uzycia.
5. main (uruchomienie ./main) tworzy potok nazwany, po czym uruchamia konsumenta i 5-ciu roznych producentow.
6. dostepne jest przykladowe wywolanie programu: `make test2`; wyniki zostana zapisane do pliku testdata/result.txt
7. dostepny jest takze polecenie wykonujace testy dla wielu producentow i jednego konsumenta: `make test`
8. `make clean` usuwa wszyskie pliki z rozszerzeniem .o
9. `make clean_all` usuwa wszystkie pliki z rozszerzeniem .o oraz wszystkie pliki wykonywalne
