# Pliki 

1. `mainfork.c` zawiera kod realizujacy pierwsza czesc zadania -- "dziedziczenie sustawien sygnalow po wykonaniu funkcji fork"
2. `mainexec.c` oraz `toexec.c` zawieraja kod realizujacy druga czesc zadania; toexec jest wczytywany przez mainexec
3. `raport2` zawiera wyniki eksperymentu z krotkim podusmowaniem 

# Uruchamianie

1. kompilacja poleceniem `make` -- skompiluja sie wszystkie 3 pliki zrodlowe

2. `./<nazwa pliku wykonywalnego> <tryb>`. W przypadku pliku wykonywalnego `mainfork` tryb to jedno z:
    1. `ignore`
    2. `mask`
    3. `pending`
    4. `handler`

    Natomiast w przypadku `mainexec` to jedno z:
    1. `ignore`
    2. `mask`
    3. `pending`
