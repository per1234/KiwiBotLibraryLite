#include <KiwiBotLite.h>
float Tmedia;
Canvas temp; //canvass temporal global para que no pierda la existencia
byte n[10][5] = {{7,5,5,5,7},{1,3,5,1,1},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,3,1,1},{7,5,7,5,7},{7,5,7,1,1}};
byte k;
byte* arrayNumbers(byte num)
{
  temp.clear(); //borro el canvass temporal
  //decenas
  temp.getArray(n[num/10]); //tomo el número de las decenas
  temp.scrollH(-4); //lo desplazo 4 pixeles a la izquierda
  temp.merge(n[num%10],OR); //superpongo la imagen de las unidades
  return temp.canvas; //devuelvo el puntero del canvas temporal
}

void setup() {
  Serial.begin(9600);
  kwbl.begin(10000);
  Screen.display(numero); // muestro el canvas 
}

void loop() {
  // put your main code here, to run repeatedly:
Tmedia = 0.9*Tmedia+0.1*Temperature;
numero.getArray(arrayNumbers((int)Tmedia);  
//Serial.println(Tmedia);
}
