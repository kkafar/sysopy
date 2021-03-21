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
