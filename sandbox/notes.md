
# Specyfikacja:

## Sender 

1. Przyjmuje 3 argumenty wywołania:
       1. pid procesu catcher
       2. liczbę sygnałów do wysłania - N
       3. tryb wysyłania sygnałów (?)
2. Po uruchomieniu wysyła N sygnałów SIGUSR1 do catcher'a, wysyła do niego dodatkowy sygnał SIGUSR2 oznaczający dla niego koniec nadawania sygnałów.   
3. Zliczamy sygnały odesłane przez catchera, czekając na SIGUSR2. 
4. Jeżeli dostajemy SIGUSR2, to przestajmey nasłuchiwać, wypisujemy ile sygnałów otrzymaliśmy, ile powinniśmy byli otrzymać i kończymy działanie. 
## Catcher

1. Uruchamiany najpierw.
2. Wypisuje swój pid do stdout.
3. Blokuje wszystkie sygnały poza SIGUSR1 oraz SIGUSR2
4. Każdy otrzymany sygnał SIGUSR1 zlicza (liczy ile ich odebrał).
5. Po otrzymaniu SIGUSR2 przesyła do Sendera tyle sygnałów SIGUSR1 ile zarejestrował, a następnie przesyła sygnał SIGUSR2 oznaczający koniec przesyłania. 
6. Po przesłaniu wygnałow wypisuje ile SIGUSR1 otrzymał i kończy działania.


Tryby wysyłania sygnałów 

1. KILL - za pomocą funkcji kill
2. SIGQUEUE - za pomocą funkcji sigqueue -- wraz z przesłanym sygnałem catcher wysyła numer kolejnego odsyłanego sygnału, dzięki czemu sender wie, ile dokładnie catcher odebrał, a tym sammym wysłał do niego sygnałów. <-- w tym przypadku, należy tą dodatkową informację wypisać w senderze
3. SIGRT -- zastępując SIGUSR1 i SIGUSR2 dowolnymi dwoma sygnałami czasu rzeczywistego wysłanymi za pomocą funkcji kill. 

