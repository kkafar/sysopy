# Standardowe uruchomienie programu 

1. Kompilacja poprzez `make`
2. `./main`

LUB

1. Wykonanie polecenia `make run`

W trybie standardowym (nietestowym) dostępne są komendy: 

1. `create_table` (+ dostarczenie informacji zgodnie z instrukcjami programu)
2. `merge_files` (+ dostarczenie informacji zgodnie z instrukcjami programu)
3. `remove_block` (+ ---------------------------//-------------------------)
4. `remove_row` (+ ---------------------------//-------------------------)
5. `print_blocks` 
6. `exit`


# Przeprowadzenie testów

W tym celu należy wykonać polecenie `make test_all`.

W razie potrzeby konfiguracji parametrów testu, należy zmodyfikować zwartość makefile targetu `test`. 
Składnia komend do przeprowadzania testu jest tam opisana. 


## Komentarz do uzyskanych wyników (znajduje się także w pliku report3b.txt)

Niezgodnie z oczekiwaniami najlepsze rezultaty daje flaga -O2 (spodziewałem się -O3, gdyż flagi przez nią stosowane są nadzbiorem -O2), 
dla każdego rodzaju biblioteki (statyczna, współdzielona, dynamiczna). 

W przypadku flagi -Os ograniczenie rozmiaru programu było nieznaczące - ok. 2kB (z 24kB) -- prawdopodobnie dla większych programów (większej ilości kodu) 
efekt byłby bardziej znaczący. Czas wykonania testów jest jednak dłuższy niż dla flagi -O2.

Generalnie flagi -O1 i -O3 dawały rezultaty porównywalne z -O0 (opcją domyślną) lub były gorsze! (Wpływ na takie zachowanie może mieć duża ilość operacji 
zapisu i odczytu z dysku twardego wykonywana przez moj program (operacja kosztowna i obarczona różnymi kosztami czasowymi)). ==> rezultaty mogą być zaburzone (duża ilość procesów w tle).
