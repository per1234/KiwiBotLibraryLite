/*
**********LIBRERIA KIWIBOT LITE****************
**SEGUNDO ÁLVAREZ GUIRADO NOVIEMBRE DE 2015****
**ALBERTO PUMAR JIMÉNEZ DICIEMBRE DE 2015******
Librería diseñada para el control del escudo básico kiwibot
centrada fundamentalmente en el control de la matriz de leds
VERSIÓN ALFA
*/
#ifndef KIWIBOTLITE_h
#define KIWIBOTLITE_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"
//defines (no ocupan memoria -precompilador)
#define LEDPINRED		6
#define LEDPINGREEN		9
#define LEDPINBLUE		5
#define BUTTON1			2
#define BUTTON2			3
#define MATRIXCLOCK		9
#define MATRIXDATA		5
#define MATRIXLATCH		6
#define CLIMBX			A2
#define CLIMBY			A3
#define JOYX			A4
#define JOYY			A5
#define LDR				A0
#define TEMP			A1
#define ON				true
#define OFF				false

#define OR				0
#define XOR				1
#define AND				2

#define LOOP			1
#define NOLOOP			0

#define SPACE			1
#define NOSPACE			0


#define MAX_COUNT 65535

//variables globales;
	extern boolean Button1;
	extern boolean Button2;
	extern int JoyX;
	extern int JoyY;
	extern int ClimbX;
	extern int ClimbY;
	extern float Temperature;
	extern int Light;
	extern boolean Matrix;

class Canvas //la clase Canvas puede ser instanciada muchas veces
{
	//ha realizar en la proxima versión
	//las funciones estáticas ahorran memoria ya que sólo hay una 
	//copia para todas las funciones de la misma clase
	public:
	byte canvas[5]; //buffer donde se guardan los datos
	void plot(byte, byte); //dibuja un punto en el array
	void unplot(byte, byte); //borra un punto en el array
	void getArray(byte*); //toma un array de datos (5 bytes)
	void invert(); //coloca boca abajo el array
	void reflect();//refleja el array
	void scrollH(int); //desplaza el array horizontal (-) izquierda (+) derecha con bucle
	void scrollV(int); //desplaza el array vertical (-) abajo (+) arriba con bucle
	void scrollH(int,boolean); //desplaza el array horizontal (-) izquierda (+) derecha con bucle
	void scrollV(int,boolean, boolean); //desplaza el array vertical (-) abajo (+) arriba con bucle	
	void notArray(); //niega la matriz
	void clear(); //limpia la matriz
	void merge(byte*, byte);
	boolean getBit(byte, byte); //extrae un bit del byte
	private:
	byte temp[6];
	boolean scrolled =false;
};

class ScreenClass //la clase Screen solo tiene una instancia
{
	public:
		void refresh(); //funcion para la multitarea. Refresca los LEDs
		void clear(); //limpia los leds
		void display(Canvas&); //muestra el canvas seleccionado
		byte *canv; //puntero a la zona de memoria
	private:
		byte cblanck[5]={0,0,0,0,0}; //almacena un canvas limpio para la pantalla
};
extern ScreenClass Screen;

class VarClass //se instancia sólo una vez
{	
	//la clase VarClass refresca las variables que están declaradas como globales en el .cpp
	//se hace así por simplicidad y no tener que acceder a la clase para consultarlas
	public:
	void refresh(); //refresca las variables
};
extern VarClass Var;
class LedClass //se instancia una sola vez
{
	public:
	void off();
	void rgb(byte, byte, byte); //enciende el Led con los componentes RGB
};
extern LedClass Led;
class KiwiBotLite //solo tiene una instancia
{
	public:
	unsigned int ctime; //tiempo de llamada a la multitarea
	void begin(long); //inicia el sistema
};
extern KiwiBotLite kwbl;

//Clase sequence
class dSeq {
  public:
	unsigned int col; //estado del scroll
	unsigned int row; //estado del scroll
	int frame;//fotograma a mostrar
	Canvas *buffer; // buffer de Canvas
	dSeq(byte); //constructor
	~dSeq(); //destructor
	Canvas show;
	Canvas temp;
	void add(Canvas&); //añade array al vector
	void showH(int);
	void showV(int);
	void scrollL(void);
	void scrollR(void);
	void scrollD(void);
	void scrollU(void);
	void next(); //proxima secuencia
	void back(); //anterior secuencia
	void reset(void);
	
 
  private:
	byte size; // tamaño actual del vector
	byte nmax; //tamaño máximo del vector
};
#endif
