#ifndef __ZAD1_H__
#define __ZAD1_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LINE_WIDTH 2048

struct block 
{
    char ** fline;
    size_t size;
}; typedef struct block block;


struct blockchain 
{
    block * blkarr;
    size_t size;
}; typedef struct blockchain blockch; 


/**
 * Tworzy i inicjuje blok o zadanym rozmiarze.
 * 
 * Inicjacja odbywa się poprzez procedurę block_init(…)
 * 
 * @param   size 
 * @return  wskaźnik do nowoutworzonego obiektu lub NULL w przypadku 
 *          niepowodzenia rezerwacji pamięci
 */
block * block_create(size_t size);


/**
 * Rezerwuje tablicę wskaźników (zeruje pamięć) wskazujących na poszczególne linie tekstu.
 * 
 * W celu zweryfikowania sukcesu incjalizacji, należy sprawdzić czy blk->fline != NULL
 * 
 * @param blk - wskaźnik do bloku
 * @param size - liczba linii w bloku
 */
void block_init(block * blk, size_t size);


/**
 * 1. Zwalnia obszary pamięci przechowujące poszczególnie linie tekstu 
 * 2. Zwalnia tablicę wskaźników wskazujących na poszczególne linie
 * 
 * Nie zwalnia wskaźnika na blok! (uzupełniany jest wartością NULL)
 * 
 * @param blk - wskaźnik do bloku przeznaczonego do wyczyszczenia
 */
void block_clear(block * blk);


/**
 * Usunięcie (zwolnienie pamięci) obiektu utworzonego metodą block_create(…)
 * 
 * @param blk - wskaźnik do bloku przeznaczonego do usunięcia
 */
void block_delete(block * blk);


/**
 * Wczytuje zawartość podanego pliku do podanego bloku.
 * Zakłada że każda linia pliku jest zakończona znakiem nowej linii.
 * 
 * @param blk       wskaźnik na blok do którego ma zostać wczyatana zawartość pliku;
 *                  obiekt powinien być niezainicjalizowany!
 * @param pathname  ścieżka do pliku, którego zawartość ma zostać wczytana
 */
void block_read(block * blk, const char * pathname);


/** 
 * Rezerwuje (zeruje pamięć) tablicę obiektów typu struct block.
 * Wskaźniki na linie w poszczególnych obiektach block są ustawione na NULL
 * 
 * @param size liczba bloków
 * @return wskaźnik do nowo utworzonego bloku lub NULL, jeżeli wystąpiły błędy. 
 */
blockch * blockch_create(size_t size);


/**
 * Wczytuje zawartość podanego pliku do pierwszego wolnego bloku 
 * w podanym łańcuchu (tablicy bloków).
 * Zakłada, że każda linia pliku jest zakończona znakiem nowej linii.
 * 
 * @param blkc      wskaźnik na tablicę bloków do której ma zostać 
 *                  wczytana zawartość podanego pliku
 * @param pathname  ścieżka do pliku, którego zawartość ma zostać wczytana
 * @return size_t   indeks bloku w tablicy, do którego została wczytana 
 *                  zawartość podanego pliku; -1 w razie niepowodzenia
 */
size_t blockch_read_block(blockch * blkc, const char * pathname);


/**
 * Zwalnia pamięć wykorzystywaną przez obiekty typu blockch.
 * 
 * 1. Zwalnia tablicę obiektów typu block. 
 * 2. Dla każdego bloku wywołuję metodę block_clear
 * 
 * @param blkc wskaźnik do obiektu typu blockch przeznaczonego do usunięcia
 */
void blockch_delete(blockch * blkc);


/**
 * Zwalnia całą pamięć wykorzystywaną przez obietky typu blockch, razem 
 * z pamięcią wykorzystywaną przez skojarzone obiekty typu block.
 * 
 * @param blkc - wskaźnik do obiektu typu blockch przeznaczonego do usunięcia
 */
void blockch_delete_all(blockch * blkc);


/**
 * Wstawia podaną linię do podanego bloku na podaną pozycję. 
 * 
 * Zawartość lini jest kopiowana! (do sturktury wstawiana jest kopia)
 * 
 * @param blk - blok wierszy do którego ma zostać wstawiona linia
 * @param idx - indeks na który ma zostać wstawiona linia
 * @param line - linia (wiersz tekstu) do wstawienia
 */
void block_insert_at(block * blk, size_t idx, char * line);


/**
 * Usuwa wiersz pod podanym indeksem z podanego bloku. 
 * Zwalnia pamięć zarezerwowaną na wiersz.
 * 
 * @param blk 
 * @param idx 
 */
void block_remove_from(block * blk, size_t idx);


/**
 * Zwraca liczbę wierszy dla których wskaźnik na nie wskazujący nie jest nullem 
 * ==> Linia na którą wskazuje może być pusta, jeżeli taka została tam wstawiona.
 * 
 * @param blk -- blok 
 * @return liczba "niepustych" wierszy; jeżeli przesłany wskaźnik jest nullem -- zwraca 0
 */
size_t block_linecount(block * blk);


/**
 * Wypisuje zawartość pojedynczego bloku
 *
 * @param blk - blok do wypisania 
 */
void block_print(block * blk, size_t idx); 


/**
 * Wypisuje zawartość wszysktich bloków przechowywanych w podanym łańcuchu bloków.
 * 
 * @param blkc blockchain do wypisania
 */
void blockch_print(blockch * blkc);


/**
 * 
 * 
 */
void merge_files(block * fseq, blockch * blkc, int );


size_t file_line_count(const char * pathname);


void block_save(block * blk, const char * pathname);


void rm_tmp_files(block * fseq);


/**
 * Zwraca nazwę pliku powstałą z połączenia przesłanych nazw plików. 
 * 
 * @param pn1 
 * @param pn2 
 * @return  wskaźnik do tablicy znaków z stworzoną nazwą pliku 
 */
char * get_tmp_pathname(const char * pn1, const char * pn2);

#endif