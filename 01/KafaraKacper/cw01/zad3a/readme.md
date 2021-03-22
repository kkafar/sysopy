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

W celu przeprowadzenia testów tylko na wybranej wersji progrmu:

1. `make test_static`
2. `make test_shared`
3. `make test_dynamic`

W razie potrzeby konfiguracji parametrów testu, należy zmodyfikować zwartość makefile targetu `test`. 
Składnia komend do przeprowadzania testu jest tam opisana. 

## Komentarz do otrzymanych wyników (znajduje się takżę w pliku report3.txt)

Generalna tendencja jest następujaca: najszybsze są biblioteki statyczne, następnie współdzielone, a po nich dynamicznie ładowane.
Nie jest to jednak twarda regułą, zdarzają się wyjątki np. w których wszystkie 3 sposoby osiągają zbliżone czasy. 
Jest to wynik zgodny z oczekiwaniami, ponieważ w przypadku statycznym proces nie musi tracić czasu na odnajdywanie potrzebnych funkcji w pamięci. 


## Uwaga

Ponowne uruchomienie testów usunie komentarz zawarty w pliku report3.txt (jednakowy z powyższym).