1. W programie najpierw musi być uruchomiony serwer. 



## Serwer

1. Musi przechowywać listę klientów. 
    1. Jej rozmiar może być ograniczony z góry, więc to robimy (stała MAX_CLIENTS)
    2. Pytanie jeszcze w jaki sposób reprezentować klineta? Jakie informacje potrzebujemy z nim stowarzyszyc? Zastanówmy się najpierw nad tym jakie informacje będą potrzebne.

    W odpowiedzi na polecenie LIST będziemy musieli wypisać listę klientów, to nie powinien być problem. 

    Każdego klienta musimy mieć stowarzyszonego z kluczem kolejki mu odpowiadającej! W takim razie będzie potrzebna struktura reprezentująca klienta.


1. Serwer po uruchomieniu tworzy nową kolejkę komunikatów (w systemie), za pomocą której klienci mają wysyłać do niego zapytania. W takim razie będzie potrzebne wypisanie na stdout klucza stworzonej przez serwer publicznej kolejki. 
2. Po otrzymaniu od klienta komunikatu INIT otwiera indywidualną kolejkę klienta, przydziela klientowi jakieś unikalne ID (proponuję indesk z listy klientów) i odsyła to id klientowi (przez kolejkę klienta).


Jeżeli 



## Klient

1. Po uruchomieniu tworzy kolejkę z **unikalnym** kluczem IPC i przsyła ten klucz do serwera. 
2. Po otrzymaniu odpowiedzi od serwera klient może przesyłać do publicznej kolejki polecenia odczytane z stdin. 