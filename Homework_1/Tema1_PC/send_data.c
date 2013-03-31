static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
/* construieşte şi trimite un cadru de date */
frame s;
/* variabilă temporară */
s.info = buffer[frame_nr];
/* inserează pachetul în cadru
s.seq = frame_nr;
/* inserează numărul de secvenţă în cadru
s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
/* ataşează confirmarea
to_physical_layer(&s);
/* transmite cadrul
start_timer(frame_nr);
/* porneşte ceasul
}