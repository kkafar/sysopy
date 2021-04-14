1. Jeżeli proces wywołujący exec'a ingoruje jakiś sygnał albo go obsługuje, to nowo załadowany proces i tak wykonuje akcję defaultową?
Dzieje się tak dlatego, że funkcja obsługująca jakiś sygnał jest tworzona na nowo i przesłany wcześniej do systemu adres nie miałby w nowym procesie żadnego sensu.

Wpis z mana `execve`

       All process attributes are preserved during an execve(), except the following:

       *  The dispositions of any signals that are being caught are reset to the default (signal(7)).

       *  Any alternate signal stack is not preserved (sigaltstack(2)).

2. W przypadku forka, proces potomny dostaję kopię pamięci procesu macierzystego ==> ma takie same zachowania w obsłudze sygnałów jak miał proces macierzysty przed wywołaniem forka.