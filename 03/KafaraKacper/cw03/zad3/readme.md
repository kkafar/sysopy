## Uruchomienie

1. Kompilujemy poleceniem `make`.
2. Uruchamiamy program stosujac sie do skladni: ./main \<sciezka do katalogu poczatkowego przeszukiwania\> \<wzorzec\> \<glebokosc poszukiwan\>. Glebokosc poszukiwan == 0 ==> szukaj tylko w podanym katalogu. 


# Dostepne polecenia

1. `make test` testuje program na przygotowanej strukturze katalogowej 
2. `make clean` usuwa wszystkie pliki z rozszerzeniem .o
3. `make clean_all` usuwa wszystkie pliki z rozszerzeniem .o wraz z plikiem wykonywalnym `main`




## Uwaga

**Generowane ostrzezenie kompilacji nie wynika z "naginania regul jezyka". Jest ono skutkiem nie rozpoznawania przez kompilator skladni wyrazen regularnych.**