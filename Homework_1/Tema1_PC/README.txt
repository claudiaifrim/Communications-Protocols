/*
 * Author: Flavius Tirnacop
 * Grupa 321CA
 * Date: 2 April 2012
 * File: README
 */

/**************************** TASK 0 ***************************/ 

Ca idee de lucru la TASK 0 se foloseste algoritmul descris in rezolvarea laboratorului 4.

Algoritm:
	- se trimite separat la inceput nume fisier si dimensiune
	- se trimite o fereastra 
	- pe rand se face recieve ACK si send la un nou mesaj (de file_size - window ori)
	- se primesc ultimele window ACK-uri


/**************************** TASK 1 ***************************/ 

La protocolul de tip Go-Back-N recieverul are o fereastra de dimensiune 1, adica face drop la toate celelalte pachete pana primeste ce are nevoie.

Algoritm:
	- se trimite separat la inceput nume fisier si dimensiune avand grija sa se trimita de mai multe ori in caz ca nu este receptionat
	In sender:

		- Se citeste la inceput un window de mesaje intr-un buffer si se trimite
		- Se face recieve cu timeout cat timp exptected_ack <= total_frames si lungimea messages_buffer!=0 iar apoi:

			daca primesc ACK
				shiftez elementele din messages_buffer cu o pozitie la stanga si citesc un nou frame din fisier pe care il adaug  la sfarsit si il trimit catre recv
			altfel
				se trimite un window din messages_buffer

	In reciever:

		- Se se intra intr-o bucla infinita in care se asteapta un "expected seq_number" iar dupa ce acesta e primit este scris in fisier si trimis ca ACK

/**************************** TASK 2 ***************************/ 

La protocolul de tip Selective-Repeat recieverul are o fereastra de dimensiune window, adica face memoreaza pachetele necesare.


Algoritm:
	- se trimite separat la inceput nume fisier , dimensiune fisier si dimensiune fereastra avand grija sa se trimita de mai multe ori in caz ca nu este receptionat

	In sender:
		- Se citeste pe rand cate un window de mesaje in buffer si se trimite catre recv
		- Se intra intr-un while de prelucrare in care
			daca se primeste ACK cu succes
				se elimina elementul respectiv din buffer
			altfel se retrimite acel window din buffer

	In reciever:	

	- se primesc pe rand mesaje se se pun pe pozitia corespunzatoare in bufferul de dimensiune window (pe pozitiile seq_number % window) si se trimite ACK ca s-a primit
	- se asteapta pana se umple bufferul cu toate pachetele necesare in ordinea lor corespunzatoare iar apoi se scriu in fisier window mesaje





/**************************** TASK 3 ***************************/ 

TODO

/**************************** TASK 4 ***************************/ 

TODO

P.S Sper ca nu este o problema daca unele comentarii sunt in romana, majoritatea am incercat sa le las in engleza.