Kompilacja `make`

Mozliwe sa trzy opcje do przetestowania:

`./main siginfo` -- pokazuje ze z flaga SA_SIGINFO wykonywana jest funkcja przypisana do sa_sigaction a nie sa_handler

main: 21: pid: 89267: <-- id of main process
main: 46: pid: 89267: parent process: sending signal no 10 to process 89268
main: 51: pid: 89267: parent process: sending signal no 10 to process 89268
custom_sigaction: 136: pid: 89267: Custom handler executed! SIGNO: 10 PPID: 89267
custom_sigaction: 136: pid: 89268: Custom handler executed! SIGNO: 10 PPID: 89267

`./main nocldstop` -- pokazuje ze z flaga SA_NOCLDSTOP -- potomek nie generuje sygnalu SIGCHLD gdy jest zatrzymany

main: 21: pid: 89459: <-- id of main process
sending sigstop
==========================================================
custom_sigaction: 136: pid: 89459: Custom handler executed! SIGNO: 17 PPID: 89460
sending sigstop
custom_sigaction: 136: pid: 89459: Custom handler executed! SIGNO: 17 PPID: 89461


`./main resethand` -- pokazuje ze ustawiony handler wykonuje sie tylko raz, a potem ustawiany jest automatycznie handler domyslny

main: 21: pid: 89848: <-- id of main process
custom_sigaction: 132: pid: 89848: Custom handler for SIGUSR2 executed! SIGNO: 12 PPID: 89848
User defined signal 2