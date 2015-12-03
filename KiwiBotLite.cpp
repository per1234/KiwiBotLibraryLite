/*
**********LIBRERIA KIWIBOT LITE****************
**SEGUNDO ÁLVAREZ GUIRADO NOVIEMBRE DE 2015****
Librería diseñada para el control del escudo básico kiwibot
centrada fundamentalmente en el control de la matriz de leds
VERSIÓN ALFA
*/
#ifndef KIWIBOTLITE_cpp
#define KIWIBOTLITE_cpp

#include "KiwiBotLite.h"
#include <stdio.h>
#include "Arduino.h"

//preinstanciamos y declaramos de forma global
//Clases
	KiwiBotLite kwbl;
	ScreenClass Screen;
	VarClass Var;
	LedClass Led;
	//Canvas canvas;

//variables globales;
	boolean Button1;
	boolean Button2;
	boolean Matrix;
	int JoyX;
	int JoyY;
	int ClimbX;
	int ClimbY;
	float Temperature;
	int Light;
	
ISR(TIMER1_OVF_vect)// llamamos a la interrupción de desborde del Timer1 (multitarea)
{
  if(Matrix) Screen.refresh(); //refrescamos la pantalla
  Var.refresh(); //actualizamos las variables

  TCNT1 = kwbl.ctime; //recargamos el contador para el tiempo de refresco
}

//Clase KiwiBotLite
void KiwiBotLite::begin(long tcall=10000) //inicializa la multitarea y el sistema
{
  //iniciamos los pines (este proceso se puede resumir para que ocupe memos memoria)
  //dado que estamos en la versión beta, pasaremos en la posterior con un for y un array
  //INPUTS
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(JOYX, INPUT);
  pinMode(JOYY, INPUT);
  pinMode(CLIMBX, INPUT);
  pinMode(CLIMBY, INPUT);
  pinMode(TEMP, INPUT);
  pinMode(LDR, INPUT);
  //OUTPUTS
  pinMode(LEDPINRED, OUTPUT);
  pinMode(LEDPINGREEN, OUTPUT);
  pinMode(LEDPINBLUE, OUTPUT);
  //No hace falta iniciar la matriz puesto que comparte pines con los LEDs
  
  
  //iniciamos la multitarea
  long cycles;
  TCCR1A = 0;// Limpiamos el registro de control del puerto A 
  TCCR1B = 0;// Limpiamos el registro de control del puerto B
  unsigned char preescaler; //variable del preescaler
  cycles = tcall*(F_CPU / 2000000); // Calculamos el número de ciclos de cpu que hay
	//en el tiempo que hemo establecido de CPU. Si el tiempo supera los 16 bits hay que 
	//preescalar. Ver AVR especificación.
  if(cycles < MAX_COUNT)  ctime = cycles, preescaler = _BV(CS10);// sin preescalar
  else if((cycles/8) < MAX_COUNT) ctime = cycles/8, preescaler = _BV(CS11);              // x8
  else if((cycles/64) < MAX_COUNT) ctime = cycles/64, preescaler = _BV(CS11) | _BV(CS10);  // x64
  else if((cycles/256) < MAX_COUNT) ctime = cycles/256, preescaler = _BV(CS12);              // x256
  else  ctime=65535, preescaler = _BV(CS12) | _BV(CS10);  // cualquier otra cantidad lo ponemos en el preescaler máximo x1024
  ctime = 65536-ctime; //ciclos hasta llegar al desborde
  TCCR1B = preescaler; // preescaler calculado
  TIMSK1 = (1 << TOIE1); // Activamos interrupción por desborde
  sei(); //interrupciones generales activadas
  
}
//Clase Screen
void ScreenClass::refresh()
{
  for(byte k=0;k<5;k++)
  {
	digitalWrite(MATRIXLATCH, LOW);//libero el latch
	//primero las filas
	shiftOut(MATRIXDATA, MATRIXCLOCK, MSBFIRST,~(16>>k)); //desplazo bits de 16 a la derecha y niego
	//ahora las columnas en forma de bytes
	shiftOut(MATRIXDATA, MATRIXCLOCK, MSBFIRST, canv[k]);
	digitalWrite(MATRIXLATCH, HIGH); //bloqueo el latch
  }
/* 
Lo que viene se hace para que la última fila no se vea más brillante ya que al quedarse la fila iluminada
la última fila tiene una permanencia. Si se quitan las 4 últimas líneas todo funciona bien pero
la última fila de led se ve muy brillante
*/
	digitalWrite(MATRIXLATCH, LOW);
	shiftOut(MATRIXDATA, MATRIXCLOCK, MSBFIRST,254); //última fila
	shiftOut(MATRIXDATA, MATRIXCLOCK, MSBFIRST, 0); //apagamos todos los leds
	digitalWrite(MATRIXLATCH, HIGH);


}
void ScreenClass::display(Canvas &cv)
{
	Matrix = ON;
	//memcpy(canvas,cv.canvas,sizeof(canvas)); //copia el array al interno
	canv = cv.canvas; //asignamos la memoria del canvas
}
void ScreenClass::clear()
{
	canv = cblanck; //asignamos el canvas vacío
}
//Clase VarClass
void VarClass::refresh()
{
	
	Button1 = digitalRead(BUTTON1);
	Button2 = digitalRead(BUTTON2);
	JoyX = analogRead(JOYX);
	JoyY = analogRead(JOYY);
	ClimbX = analogRead(CLIMBX);
	ClimbY = analogRead(CLIMBY);
	Temperature= (analogRead(TEMP)*500)>>10; // ver https://github.com/riyadhalnur/LM35_library_for_Arduino/blob/master/lm35.cpp
	Light= analogRead(LDR);
	
}
void LedClass::rgb(byte red, byte green, byte blue)
{
	Matrix = OFF;
	analogWrite(LEDPINRED, red);
	analogWrite(LEDPINGREEN, green);
	analogWrite(LEDPINBLUE, blue);
}
void LedClass::off()
{
	rgb(0,0,0); //pone los pines a cero
}
//Clase Canvas
void Canvas::plot(byte x, byte y)
{
	if(x>=0&&x<=6&&y>=0&&y<=4)
	{
		canvas[y]|=(64>>x); //hago un OR en el byte
	}
}
void Canvas::unplot(byte x, byte y)
{
	if(x>=0&&x<=6&&y>=0&&y<=4)
	{
		canvas[y]&=~(64>>x); //hago un AND con el negado en el byte
	}
}
void Canvas::getArray(byte* array)
{
	clear(); //limpiamos el canvas
	memcpy(canvas,array,sizeof(canvas)); //copia el array al interno
	//si el tamaño del origen es menor puede resultar puntos extraños
	//si el tamaño del origen es mayor sólo copia la primera parte
	for(byte k=0;k<6;k++) temp[k]=0; //limpiamos el temporal
	scrolled=false; //nunca se ha realizado scroll
}
boolean Canvas::getBit(byte dato, byte pos)
{                            
       if(pos>5) return false; //protección
	   byte b = dato >> pos ;  // desplazo la cantidad de bits
       b = b & 1 ;             // hago and con 1
       return b ; //usamos el auto-cast
}


void Canvas::reflect()
{
    byte temp;
    for(byte row=0;row<5;row++)
	{
		temp = canvas[row];//cargamos byte temporal
		canvas[row]=0;//limpiamos el byte
		for (byte col= 0; col<7; col++)
		{
			if(getBit(temp,col)) canvas[row]|=(1<<6-col); //comprobamos cada bit y lo asignamos a su posición
		}	
	}
}

void Canvas::invert()
{
	memcpy(temp, canvas,sizeof(canvas)); //copiamos el contenido al canvas temporal
	for(byte row=0;row<5;row++)
	{
		canvas[row] = temp[4-row]; //cambiamos los bytes de arriba a abajo
	}
}
void Canvas::clear()
{
	for(byte row=0;row<6;row++) canvas[row] = 0; //rellena la matriz con ceros
}

void Canvas::scrollH(int d, boolean bucle) //desplazamiento de uno a la izquierda por defecto
{
	boolean signo = false; //scroll negativo o positivo
	boolean bito; //bit fuera para ponerlo en el otro lado
	if(d>=-7 || d <= 7) //protección
	{
		for(byte k=0; k<abs(d);k++) //repite tabtas veces
		{
			if(d<0) signo = true;
			for(byte row=0;row<5;row++)
			{
				if(signo)
				{
					bito = getBit(canvas[row],7);
					canvas[row] <<=1; //desplazo los bits a la izquierda
					if(bito & bucle) canvas[row]|=1; //añado el 1 
				}
				else
				{
					bito = getBit(canvas[row],0);
					canvas[row] >>=1; //desplazo los bits a la derecha
					if(bito & bucle) canvas[row]|=128; //activo el bit 7  				
				}
			}
		}
	}
	
}
void Canvas::scrollH(int d)
{
	scrollH(d, LOOP);
}
void Canvas::scrollV(int d, boolean bucle, boolean space)
{
	//boolean signo = false;
	if((d>=-5 || d <= 5) && d!=0)//protección del desplazamiento
	{
		if(scrolled) scrolled =true;
		else memcpy(temp, canvas,sizeof(canvas)); //copiamos el contenido al canvas temporal
		//temp[5]=0; //limpiamos el último byte
		//if(d<0) signo = true;
		//d=abs(d);
		/*for(int r=0; r<5;r++)
		{
				if(r+d<5 && r+d>=0) canvas[r] = temp[r+d];
				else
				{
					if(bucle) canvas[r] = temp[r-d];
					else canvas[r]=0;
				}
		}*/
		if(d>=0) 
		{
			memcpy(canvas,temp+d,5-d+(int)space);
			if(bucle) memcpy(canvas+5-d+(int)space,temp,d);
			else for(int k=5-d;k<5;k++) canvas[k]=0;
		}
		else
		{
			d=abs(d);
			memcpy(canvas+d,temp,5-d);			
			if(bucle) memcpy(canvas,temp+5-d+(int)space,d);
			else for(int k=0;k<d;k++) canvas[k]=0;
		}
	}
}
void Canvas::scrollV(int d)
{
	scrollV(d, LOOP, SPACE);
}
void Canvas::notArray()
{
	 for(byte row=0;row<5;row++) canvas[row]=~canvas[row];
}
void Canvas::merge(byte* array, byte op)
{
	for(byte row=0;row<5;row++)
	{
		switch(op)
		{
			case OR:
				canvas[row] |= array[row];
			break;
			case XOR:
				canvas[row] ^= array[row];
			break;
			case AND:
				canvas[row] &= array[row];
			break;
		}
	}
}
//clase sequence

dSeq::dSeq(byte nm)
{
	size=0; //sin elementos
	buffer =(Canvas*)malloc(1+sizeof(Canvas)*nm); //reservamos el numero de canvas en memoria
	col=row=0; //reiniciamos las columnas
}
dSeq::~dSeq()
{
	free(buffer);
}

void dSeq::add(Canvas &cv) //usando los canvas la memoria ya está reservada
{ 
	buffer[size] = cv; //asignamos el puntero del canvas
	buffer[size+1].clear(); //borramos el último canvas
	size++;
}
void dSeq::showH(int c)
{
	if(c<size*8) //máxima columna
	{
		show.getArray(buffer[c/8].canvas); // tomo el indice del principal y lo copio
		show.scrollH(-c%8, NOLOOP); //lo desplazo el resto
		temp.getArray(buffer[(c/8) +1].canvas); //tomo el siguiente caracter y lo copio
		temp.scrollH(8-c%8,NOLOOP); //lo desplazo a la derecha lo que le falta al principal
		show.merge(temp.canvas, OR);
	}
}
void dSeq::showV(int r)
{
	if(r<size*6) //máxima fila
	{
		show.getArray(buffer[r/5].canvas); // tomo el indice del principal y lo copio
		show.scrollV(-r%5, NOLOOP, SPACE); //lo desplazo el resto
		temp.getArray(buffer[(r/5) +1].canvas); //tomo el siguiente caracter y lo copio
		temp.scrollV(6-r%5, NOLOOP, SPACE); //lo desplazo a la derecha lo que le falta al principal
		show.merge(temp.canvas,OR);
	}

}
void dSeq::scrollL()
{
	showH(col);
	col++;
	if(col>(size-1)*8) col=0;
}

void dSeq::scrollR()
{
	showH(col);
	if(col<1) col=(size-1)*8;
	else col--;
	
}

void dSeq::scrollD()
{
	if(row>0) showV(row), row--;
	else row=size*5;
}
void dSeq::scrollU()
{
	
	if(row>(size)*5) row=0;
	else showV(row),row++; 
}
void dSeq::reset()
{
	col=0;
	row=0;
}
void dSeq::next()
{
	if(frame<size) show.getArray(buffer[frame].canvas), frame++;
	else frame=0;
}
void dSeq::back()
{
	if(frame>=0) show.getArray(buffer[frame].canvas), frame--;
	else frame=size-1;
}

#endif
