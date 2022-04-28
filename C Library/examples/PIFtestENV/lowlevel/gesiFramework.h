/**
 * @mainpage Bibliothek mit diversen, praktischen Funktionen.
 * @brief Die Bibliothek bring viele diverse Funktionen, von einfachen Ringbuffer �ber
 * millis-Funktionen bis hin zu kooperativen Multitasking.
 * 
 * @version 2.1\n
 * - MILLISTIMER f�r Xmega implementiert
 * - Kooperatives Multitasking optimiert
 *
 * @todo 
 * - ADC f�r Xmega hinzuf�gen
 * - PIC16F Familie hinzuf�gen
 *
 * @author Pascal Gesell
 * @date 18.01.2019
 */

/**
 * @file gesiFramework.h
 * @brief C-Datei des Frameworks.
 *
 * Eine Funktionssammlung vieler n�tzlicher Makros und Funktionen.
 */ 


#ifndef GESIFRAMEWORK_H_
#define GESIFRAMEWORK_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/**
 * @brief Ein einzelnes Bit setzten
 */
#define sbi(port, bit) \
		(port) |= (1 << (bit))

/**
 * @brief Ein einzelnes Bit l�schen
 */
#define cbi(port, bit) \
		(port) &=~ (1 << (bit))

/**
 * @brief Ein einzelnes Bit �berpr�fen
 */
#define tbi(port, bit) \
		(port) & (1<<(bit))

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#define DEFAULT 0
#define EXTERNAL 1
#define INTERNAL1V1 2
#define INTERNAL INTERNAL1V1
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define DEFAULT 0
#define EXTERNAL 4
#define INTERNAL1V1 8
#define INTERNAL INTERNAL1V1
#define INTERNAL2V56 9
#define INTERNAL2V56_EXTCAP 13
#else
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
#define EXTERNAL 0
#define INTERNALAVCC 1
#define INTERNAL1V1 2
#define INTERNAL2V56 3
#else
#define INTERNAL 3
#endif
#define DEFAULT 1
#define EXTERNAL 0
#endif

/**
 * @brief Berechnet den kleinsten Wert der beiden Zahlen
 * @param a Die erste Zahl, Datentyp egal
 * @param b Die zweite Zahl, Datentyp egal
 * @return Gibt die kleinere Zahl zur�ck
 */
#define min(a,b) ((a)<(b)?(a):(b))

/**
 * @brief Berechnet den gr�ssten Wert der beiden Zahlen
 * @param a Die erste Zahl, Datentyp egal
 * @param b Die zweite Zahl, Datentyp egal
 * @return Gibt die gr�ssere Zahl zur�ck
 */
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * @brief Begrenzt eine Zahl in einem gewissen Bereich
 * @param amt Die zu begrenzende Zahl, Datentyp egal
 * @param low Der Minimumwert des Bereiches
 * @param high Der Maximumwert des Bereiches
 * @return Gibt 'amt' zur�ck, wenn die Zahl zwischen 'low' und 'high' ist, sonst mind. 'low' und max. 'high'
 */
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

/**
 * @brief Extrahiert die 8 tieferen Bits einer 16-Bit Variable
 * @param w 16bit Variable (u)int16_t
 * @return Die 8 tieferen Bits als uint8_t
 */
#define lowByte(w) ((uint8_t) ((w) & 0xff))

/**
 * @brief Extrahiert die 8 h�heren Bits einer 16-Bit Variable
 * @param w 16bit Variable (u)int16_t
 * @return Die 8 h�heren Bits als uint8_t
 */
#define highByte(w) ((uint8_t) ((w) >> 8))

/**
 * @brief Taktzyklen pro Mikrosekunde
 */
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
/**
 * @brief Taktzyklen in Mikrosekunden umrechnen
 * @param a Anzahl Taktzyklen
 * @return Mikrosekunden
 */
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
/**
 * @brief Mikrosekunden in Taktzyklen umrechnen
 * @param a Mikrosekunden
 * @param Anzahl Taktzyklen
 */
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

/**
 * @brief Bit-Reihenfolge f�r die Software-SPI Funktionen.
 */
enum bitOrder {LSBFIRST = 0,MSBFIRST = 1};

/**
 * @brief Initialisierungsparameter f�r \a gf_init()
 */
enum gfInit {ADCON = 0x4, MILLISTIMER = 0x1, ADCON_MILLISTIMER = 0x5};

/**
 * @brief FIFO-Buffer Struktur
 * 
 * Enth�lt den Buffer, die Gr�sse und das Ende / der Start des Ringbuffers.
 */
typedef struct {
	uint8_t *buf; /**< Pointer zum Buffer */
	uint16_t head; /**< Endposition des Ringbuffers */
	uint16_t tail; /**< Anfangsposition des Ringbuffers */
	uint16_t size; /**< Gr�sse des Buffers */
} fifo_t;

/**
 * @brief Struktur f�r die Entprell-Funktionen
 *
 * Enth�lt die notwendigen Register und Pins, sowie auch Pointers zu Variablen. Pro Register ist eine andere Variable notwendig.
 */
typedef struct {
	volatile uint8_t *btn_port; /**< Eingabeport */
	uint8_t btn_pin; /**< Zu �berpr�fenden Pin */
	uint8_t *btn_pressedVar; /**< Variable zum speichern, ob der Taster schon gedr�ckt wurde - Muss pro register/port anders sein! */
	uint8_t *btn_releasedVar; /**< Variable zum speichern, ob der Taster schon losgelassen wurde - Muss pro register/port anders sein! */
	volatile uint8_t *beep_port; /**< Ausgabeport f�r optionales LED oder Buzzer Feedback. 0 Falls nicht benutzt werden soll.*/
	uint8_t beep_pin; /**< Pin des Feedback-Ausgabeports */
	uint8_t beep_timeMS; /**< in MS wie lange der Ausgabeport angesteuert werden soll. Wird auch benutzt, wenn \a *beep_port 0 ist zur Tasterentprellung.*/
	uint8_t beep_activeState; /**< Ob der Pin active-low oder active-high, f�r das Feedback. */
} btn_check_t;

/**
 * @brief Definiert die Speichergr�sse f�r gf_tasks
 * 
 * Anzahl m�gliche Tasks werden hier definiert.
 */
#define GF_TASK_SIZE		0

/**
 * @brief R�ckgabewert von gf_tasks Funktionen, wenn Operation erfolgreich.
 */
#define GF_TASK_DONE		0x00

/**
 * @brief R�ckgabewert von gf_tasks Funktionen, wenn Task-Buffer voll ist.
 */
#define GF_TASK_MAXREACHED	0x01

/**
 * @brief R�ckgabewert von gf_tasks Funktionen, wenn Task nicht gefunden wurde.
 */
#define GF_TASK_NOTFOUND	0x02

/**
 * @brief R�ckgabewert von gf_tasks Funktionen, wenn Task pausiert ist.
 */
#define GF_TASK_PAUSED		0x04

/**
 * @brief R�ckgabewert von gf_tasks Funktionen, wenn Task l�uft.
 */
#define GF_TASK_RUNNING		0x08

/**
 * @brief Struktur f�r die das kooperative Multitasking
 *
 * Enth�lt der gew�nschte und letzte Funktionsaufruf, einige Informationen und ein Pointer zur Funktion selber.
 */
typedef struct {
	void (*userFunc)(void); /**< Pointer zur Benutzerfunktion */
	uint32_t oldCallTime; /**< Letzter Aufruf der Funktion */
	uint32_t callEveryTime; /**< Gew�nschte Zeit wann die Funktion ausgef�hrt werden soll. */
	uint8_t taskInfo; /**< Enth�lt die Info, ob der Task pausiert oder ausgef�hrt wird. */
}task_t;

/**
 * @brief Struktur f�r das Blockierfreie Delay \a gf_TimeCheck
 *
 * Enh�lt die letzte Aufrufzeit sowie wie oft es aufgerufen werden soll.
 */
typedef struct {
	uint32_t oldCallTime;	/**< Letzte Aufrufzeit */
	uint32_t callEveryTime;	/**< Alle ms/us Aufrufen */
}gf_delay_t;

/**
 * @brief GesiFramework ADC/Millistimer initialisieren
 *
 * Initialisiert den Microcontroller f�r den ADC, Millistimer.
 * Timer0 wird f�r den Millistimer verwendet.
 * \n Beispiel: \n \code{.c}
 * gf_init(MILLISTIMER | ADCON);
 * \endcode Initialisiert den Millistimer und den AD-Wandler
 * @param mode Zu initialisierende Funktionen
 */
extern void gf_init();

/**
 * @brief Initialisiert den Ringbuffer
 *
 * Funktion zum Erstellen eines Ringbuffers f�r 8-bit Variablen mit
 * einer maximalen Gr�sse von 64k Bytes.
 * \n Beispiel: \n \code{.c}
 * #define BUFFER_LEN 10
 * fifo_t myFifoStruct;
 * uint8_t myFifoBuffer[BUFFER_LEN];
 * gf_fifo_init(&myFifoStruct, &myFifoBuffer, BUFFER_LEN);
 * \endcode Initialisiert einen FIFO Buffer mit der Gr�sse von 10 Bytes.
 * @param f Pointer zum Struct des zu initialisierenden FIFO Buffers
 * @param buf Pointer zum Buffer
 * @param size G�sse des FIFO Buffers
 */
extern void gf_fifo_init(fifo_t *f, uint8_t *buf, uint16_t size);

/**
 * @brief Ein Byte aus dem FIFOBuffer lesen
 *
 * Liest ein Byte aus dem FIFO-Buffer und gibt diesen zur�ck.
 * Falls der Buffer leer ist, sind die 8-h�herwertigen Bits, die
 * die Funktion zur�ckgibt, auf 0xFF.
 * \n Beispiel: \n \code{.c}
 * uint16_t buf_data = gf_fifo_read(&myFifoStruct);
 * if ((buf_data & 0xFF00))
 * {
 *		// Fehler, Buffer schon leer.
 * }
 * \endcode Liest ein Byte aus den Fifo-Buffer und �berpr�ft den R�ckmeldecode.
 * @param f Pointer zum Struct des zu initialisierenden FIFO Buffers
 * @return H�here 8Bit geben den Fehler zur�ck, tiefere 8Bit den gelesenen Wert.
 */
extern uint16_t gf_fifo_read(fifo_t *f);

/**
 * @brief Mehrere Bytes aus dem FIFOBuffer lesen
 *
 * Liest mehrere Bytes aus dem FIFO-Buffer in den Lesebuffer.
 * Gibt die Anzahl gelesene Bytes zur�ck, welche weniger als
 * nbytes ist, falls weniger Daten im Buffer sind als angefordert.
 * \n Beispiel: \n \code{.c}
 * #define READ_LEN 3
 * uint8_t tempBuffer[TEMPBUF_LEN];
 * if (gf_fifo_reads(&myFifoStruct, &tempBuffer, READ_LEN) != READ_LEN)
 * {
 *		// Fehler, Buffer schon leer.
 * }
 * \endcode Liest n-Bytes aus den Fifo-Buffer in den Lesebuffer.
 * @param f Pointer zum Struct des FIFO Buffers
 * @param buf Pointer zum Buffer
 * @param nbytes Anzahl zu lesende Bytes
 * @return Anzahl gelesene Bytes
 */
extern uint16_t gf_fifo_reads(fifo_t *f, void *buf, uint16_t nbytes);

/**
 * @brief Ein Byte in den FIFOBuffer schreiben
 *
 * Schreibt ein Byte aus 'buf' in den FIFO-Buffer.
 * Gibt den Wert "1" zur�ck, falls der Buffer voll ist.
 * \n Beispiel: \n \code{.c}
 * uint8_t writeData = 0x2B;
 * if (gf_fifo_write(&myFifoStruct, writeData))
 * {
 *	 // Fehler, Buffer schon voll.
 * }
 * \endcode Schreibt ein Byte in den Fifo-Buffer.
 * @param f Pointer zum Struct des FIFO Buffers
 * @param data Das zu schreibende Byte
 * @return "1" falls der Buffer voll ist, sonst "0"
 */
extern uint8_t gf_fifo_write(fifo_t *f, uint8_t data);

/**
 * @brief Mehrere Bytes in den FIFOBuffer schreiben
 *
 * Schreibt mehrere Bytes aus 'buf' in den FIFO-Buffer.
 * Gibt die Anzahl geschriebene Bytes zur�ck, welche weniger als
 * nbytes sein kann, falls der Buffer voll ist.
 * \n Beispiel: \n \code{.c}
 * #define WRITE_LEN 3
 * uint8_t writeBuf[WRITE_LEN] = {'H','i','\0'};
 * if (gf_fifo_writes(&myFifoStruct, &writeBuf, WRITE_LEN) != WRITE_LEN)
 * {
 *	 // Fehler, Buffer schon voll.
 * }
 * \endcode Schreibt n-Bytes in den Fifo-Buffer aus den Schreibbuffer.
 * @param f Pointer zum Struct des FIFO Buffers
 * @param buf Pointer zum Buffer
 * @param nbytes Anzahl zu schreibende Bytes
 * @return Anzahl geschriebene Bytes
 */
extern uint16_t gf_fifo_writes(fifo_t *f, const void *buf, uint16_t nbytes);

/**
 * @brief Gibt die Betriebszeit zur�ck
 *
 * Gibt zur�ck in Millisekunden zur�ck, wie lange der 
 * Microcontroller seit der Initialisierung von gf_init l�uft.
 * Erfordert das gf_init mit Millistimer initialisiert wird.
 */
extern uint32_t gf_millis();

/**
 * @brief Gibt die Betriebszeit zur�ck
 *
 * Gibt zur�ck in Microsekunden zur�ck, wie lange der 
 * Microcontroller seit der Initialisierung von gf_init l�uft.
 * Erfordert das gf_init mit Millistimer initialisiert wird.
 */
extern uint32_t gf_micros();

extern void gf_delay_us(uint32_t us);

extern void gf_delay_ms(uint32_t ms);

/**
 * @brief Liest 8 Bits manuell ein
 *
 * Liest per Software-SPI 8 Bits aus dem Port ein
 * \n Beispiel: \n \code{.c}
 * uint8_t readIn = gf_shiftIn(PB0, PB1, MSBFIRST, &PORTB, &PINB);
 * \endcode Liest ein Byte per Software-SPI ein.
 * @param dataPin Pin am Port wo die Daten eingelesen werden
 * @param clockPin Pin am Port wo das Clocksignal ist
 * @param _bitorder MSB oder LSB
 * @param portout Das Ausgaberegister des Ports
 * @param portin Das Eingaberegister des Ports
 * @return 8 eingelesene Bits
 */
extern uint8_t gf_shiftIn(uint8_t dataPin, uint8_t clockPin, enum bitOrder _bitorder, volatile uint8_t *portout, volatile uint8_t *portin);

/**
 * @brief Schreibt 8 Bits manuell aus
 *
 * Schreibt per Software-SPI 8 Bits aus dem Port aus
 * \n Beispiel: \n \code{.c}
 * gf_shiftOut(PB2, PB1, MSBFIRST, 0xAA, &PORTB);
 * \endcode Schreibt ein Byte per Software-SPI aus.
 * @param dataPin Pin am Port wo die Daten ausgegeben werden
 * @param clockPin Pin am Port wo das Clocksignal ist
 * @param _bitorder MSB oder LSB
 * @param val Die zu sendenden Daten
 * @param port Das Ausgaberegister des Ports
 */
extern void gf_shiftOut(uint8_t dataPin, uint8_t clockPin, enum bitOrder _bitorder, uint8_t val, volatile uint8_t *port);

/**
 * @brief Taster mit Software-Entprellung einlesen.
 *
 * Wird nur ein Mal eingelesen bis der Taster losgelassen wird. Erfordert btn_check als Argument.
 * \n Beispiel: \n \code{.c}
 * char key_pressed, key_release;
 * btn_check myButton = {&PINA, 1, &key_pressed, &key_release, &PORTA, PIEZOORLEDPIN, 10, 1};
 * if (gf_isButtonPressed(&myButton))
 * { // Do something when button is pressed }
 * \endcode Taster an PortA, Pin 1 einlesen und bei einem Tasterdruck einen Summer f�r 10ms piepsen lassen 
 * @param *b Konfigurierte btn_check Variable
 * @return True beim ersten Erkennen eines gedr�ckten Tasters
 */
extern uint8_t gf_isButtonPressed(btn_check_t *b);

/**
 * @brief Taster mit Software-Entprellung einlesen.
 *
 * Wird nur ein Mal eingelesen bis der Taster gedr�ckt wird. Erfordert btn_check als Argument.
 * \n Beispiel: \n \code{.c}
 * char key_pressed, key_release;
 * btn_check myButton = {&PINA, 2, &key_pressed, &key_release, &PORTA, PIEZOORLEDPIN, 10, 1};
 * if (gf_isButtonReleased(&myButton))
 * { // Do something when button is pressed }
 * \endcode Taster an PortA, Pin 2 einlesen und bei einem Tasterrelease einen Summer f�r 10ms piepsen lassen 
 * @param *b Konfigurierte btn_check Variable
 * @return True beim ersten Erkennen eines losgelassenen Tasters
 */
extern uint8_t gf_isButtonReleased(btn_check_t *b);

/**
 * @brief K�nstliches, unblockierendes Delay
 *
 * Eine simple Funktion, die in Kombination mit gf_millis ein delay erzeugt, ohne das Programm
 * aufzuh�ngen. Erfordert gf_millis und die aktuelle Zeit, in Mikro- oder Millisekunden.
 * \n Beispiel: \n \code{.c}
 * gf_millis button_check = {0, 200};
 * if (gf_TimeCheck(&button_check, gf_millis())
 * { // Wird alle 200 MS ausgef�hrt... }
 * \endcode Alle 200ms wird die if-schleife ausgef�hrt.
 * @param *m Pointer zur gf_millis_t struktur
 * @param _time Die Zeit in ms oder us.
 * @return 1 wenn genug Zeit seit dem letzten Aufruf verging, sonst 0
 */
extern uint8_t gf_TimeCheck(gf_delay_t *m, uint32_t _time);

/**
 * @brief Scheduler mit Microsekunden ausf�hren
 *
 * F�hrt den Scheduler aus, welche die Funktionen �berpr�ft und gegebenenfalls ausf�hrt.
 * \n Beispiel: \n \code{.c}
 * gf_runScheduler(gf_micros());
 * \endcode F�hrt den Scheduler mit den aktuellen Mikrosekunden aus.
 * @param _time Mikrosekunden seitdem der Prozessor l�uft.
 */
extern void gf_runScheduler(uint32_t _time);

/**
 * @brief Funktion zur Taskliste hinzuf�gen
 * 
 * F�gt eine Funktion zur Taskliste hinzu, zusammen mit der Zeit in Mikrosekunden, welche festlegt nach wieviel
 * uS die Funktion ausgef�hrt werden soll.
 * \n Beispiel: \n \code{.c}
 * uint16_t taskReturn = gf_addTask(&LEDBlink(), 1000);
 * if (taskReturn & 0xFF00)
 * { //Task nicht erfolgreich hinzugef�gt - Liste schon voll! }
 * \endcode F�gt die Funktion "LEDBlink" hinzu, welche alle 1000 Mikrosekunden ausgef�hrt werden soll und �berpr�ft danach,
 * ob die Funktion einen Fehler zur�ckgibt.
 * @param userFunction Pointer zur Funktion
 * @param timeRun Anzahl Mikrosekunden, welche die Task immer ausgef�hrt werden soll.
 * @return Fehlercodes (GF_TASK_DONE / GF_TASK_MAXREACHED) in der oberen 8bits, Anzahl hinzugef�gte Tasks in den unteren 8bits.
 */
extern uint16_t gf_addTask(void (*userFunction)(void), uint32_t timeRun);

/**
 * @brief Pausiert den Task
 *
 * Verhindert, das der Task weiterhin ausgef�hrt wird bei der n�chsten Ausf�hrung. Der Task kann sich
 * selber stoppen, aber logischerweise sich nicht selber wieder starten.
 * \n Beispiel: \n \code{.c}
 * if (gf_pauseTask(&LEDBlink() != GF_TASK_PAUSED)
 * {  // Task nicht gefunden  }
 * \endcode Pausiert die Funktion LEDBlink() und �berpr�ft, ob die Operation erfolgreich war.
 * @param userFunction Die zu pausierende Funktion
 * @return GF_TASK_NOTFOUND wenn der Prozess nicht gefunden wurde, sonst GF_TASK_PAUSED wenn erfolgreich pausiert wurde.
 */
extern uint8_t gf_pauseTask(void (*userFunction)(void));

/**
 * @brief Startet den Task wieder
 *
 * F�hrt den Task wieder beim n�chsten gf_runScheduler aus, nachdem er vorher pausiert war.
 * \n Beispiel: \n \code{.c}
 * if (gf_runTask(&LEDBlink() != GF_TASK_RUNNING)
 * {  // Task nicht gefunden  }
 * \endcode Startet die Funktion LEDBlink() und �berpr�ft, ob die Operation erfolgreich war.
 * @param userFunction Die zu pausierende Funktion
 * @return GF_TASK_NOTFOUND wenn der Prozess nicht gefunden wurde, sonst GF_TASK_RUNNING wenn erfolgreich gestartet wurde.
 */
extern uint8_t gf_runTask(void (*userFunction)(void));

/**
 * @brief �berpr�ft den Task Status
 *
 * Gibt zur�ck, ob der Task pausiert wurde oder ausgef�hrt wird.
 * \n Beispiel: \n \code{.c}
 * uint8_t taskStatus = gf_getTaskStatus(&LEDBlink());
 * if (taskStatus == GF_TASK_NOTFOUND)
 * {  // Task nicht gefunden  }
 * \endcode Liest den aktuellen Status der Funktion LEDBlink.
 * @param userFunction Die zu auszulesende Funktion
 * @return GF_TASK_NOTFOUND wenn der Prozess nicht gefunden wurde, sonst den Status ob der Prozess l�uft oder pausiert ist.
 */
extern uint8_t gf_getTaskStatus(void (*userFunction)(void));

/**
 * @brief L�ngeres ms-delay
 * L�ngerer Delay, einfach eine 16bit for-schleife mit einem 1ms-delay
 * @param ms Delay in Millisekunden
 */
extern void gf_delaymsLong(uint16_t ms);


#endif /* GESIFRAMEWORK_H_ */