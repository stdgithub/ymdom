#include <EEPROM.h>
#define MAXMILLIS 4294967295
#include "DHT.h"
#define DHTPIN 8 // пин для подключения датчика
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

int hum = 110; // влажность
int thum = 110; // температура

byte d2 = EEPROM.read(2);     // флаги (состояние пинов) хранится в EEPROM, считываем их
byte d3 = EEPROM.read(3);
byte d4 = EEPROM.read(4);
int shim1 = EEPROM.read(5); // значение ШИМ хранится в EEPROM, считываем их
int shim2 = EEPROM.read(6);
byte d7 = 0; 
byte d8 = 0;
byte d11 = EEPROM.read(11);
byte d12 = EEPROM.read(12);
byte d13 = EEPROM.read(13);

int oxlagdenie = EEPROM.read(14); // заданая температура для включения кондиционера
int toxl = EEPROM.read(15); // флаг включения

int podogrev = EEPROM.read(16); // заданая температура для включения печки
int tpod = EEPROM.read(17); // флаг включения

byte descript[5]; // массив

unsigned long   time; // время

long timelapsed = 0;

byte count=0; // счетчик минут

void setup() 
{
  time = millis();  // время
  Serial.begin(57600);
  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT); // ШИМ
  pinMode(6, OUTPUT); // ШИМ
  pinMode(7, OUTPUT); // охлаждение
  pinMode(8, OUTPUT); // подогрев 
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  thum = dht.readTemperature(); // считываем температуру, сначала делаем это в сетапе, так как следующее чтение будет только через минуту
  hum = dht.readHumidity(); // считываем влажность
  
  if(d2) digitalWrite(2, HIGH); else digitalWrite(2, LOW); // если до перезагрузки d2 была включена, то включаем, если нет, то нет 
  delay(500); // чтобы не включалось всё сразу, делаем паузы
  if(d3) digitalWrite(3, HIGH); else digitalWrite(3, LOW);
  delay(500);
  if(d4) digitalWrite(4, HIGH); else digitalWrite(4, LOW);
  delay(500);
  analogWrite(5, shim1 * 2.55); // включаем ШИМ d5
  delay(500);
  analogWrite(6, shim2 * 2.55); // включаем ШИМ d6
  delay(500);
  if(d11) digitalWrite(11, HIGH); else digitalWrite(11, LOW);
  delay(500);
  if(d12) digitalWrite(12, HIGH); else digitalWrite(12, LOW);
  delay(500);
  if(d13) digitalWrite(13, HIGH); else digitalWrite(13, LOW);
}
  
void loop() 
{  
  if (Serial.available()>4) // ждём дескриптор и нужный символ
   {
    if (Serial.read()=='Y') // проверяем первый символ, если это 'Y', то продолжаем принимать, если нет, то выходим из цикла чтения
     {
      for (byte i=0; i < 5; i++)
        {
           descript[i] = Serial.read(); // добавляем символы в массив   
        } 
        
    if((descript[0] =='+') && (descript[1] =='=') && (descript[2] =='Z')) // проверяем дескриптор
     {
      switch (descript[3])
       {
         case 'o': // обновление
         glavnaia(); // отправка ответа
         break;
         
         ////////////// Кнопки ///////////////////
         case 'A': // d2 вкл
         digitalWrite(2, HIGH); // вкл d2
         d2 = 1; // ставим флаг в единицу (вкл)
         EEPROM.write(2, d2); // записываем состояние d2 в ячейку №2 EEPROM 
         glavnaia(); // отправка ответа
         break;
         
         case 'a': // d2 откл
         digitalWrite(2, LOW); // откл d2
         d2 = 0; // ставим флаг в ноль (откл)
         EEPROM.write(2, d2); // записываем состояние d2 в ячейку №2 EEPROM 
         glavnaia(); // отправка ответа
         break; 
 
         case 'B': // d3
         digitalWrite(3, HIGH);
         d3 = 1;
         EEPROM.write(3, d3);
         glavnaia();
         break;
         
         case 'b': // d3
         digitalWrite(3, LOW);
         d3 = 0;
         EEPROM.write(3, d3);
         glavnaia();
         break;          
  
         case 'C': // d4
         digitalWrite(4, HIGH);
         d4 = 1;
         EEPROM.write(4, d4);
         glavnaia();
         break;
         
         case 'c': // d4
         digitalWrite(4, LOW);
         d4 = 0;
         EEPROM.write(4, d4);
         glavnaia();
         break;   
         
         /////////////// ШИМ ///////////////
         case 'D': // d5 прибавляем shim1
         shim1++;
         if(shim1 > 100) shim1 = 100;
         EEPROM.write(5, shim1);
         analogWrite(5, shim1 * 2.55);
         glavnaia();
         break;
         
         case 'd': // d5 убавляем shim1
         shim1--;
         if(shim1 < 1) shim1 = 0;
         EEPROM.write(5, shim1);
         analogWrite(5, shim1 * 2.55);
         glavnaia();
         break; 
  
         case 'E': // d6 прибавляем shim2
         shim2++;
         if(shim2 > 100) shim2 = 100;
         EEPROM.write(6, shim2);
         analogWrite(6, shim2 * 2.55);
         glavnaia();
         break;
         
         case 'e': // d6 убавляем shim2
         shim2--;
         if(shim2 < 1) shim2 = 0;
         EEPROM.write(6, shim2);
         analogWrite(6, shim2 * 2.55);
         glavnaia();
         break;   
  
         case 'F': // мгновенное включение ШИМ на D5
         shim1 = EEPROM.read(5); // считываем значение ШИМ из EEPROM
         analogWrite(5, shim1 * 2.55); // включаем ШИМ D5
         glavnaia();
         break;
         
         case 'f': // мгновенное отключение ШИМ на D5
         shim1 = 0;
         analogWrite(5, shim1); // отключаем ШИМ D5, но НЕ записываем в EEPROM
         glavnaia();
         break;  
 
         case 'G': // мгновенное включение ШИМ на D6
         shim2 = EEPROM.read(6); // считываем значение ШИМ из EEPROM
         analogWrite(6, shim2 * 2.55); // включаем ШИМ D6
         glavnaia();
         break;
         
         case 'g': // мгновенное отключение ШИМ на D6
         shim2 = 0;
         analogWrite(6, shim2); // отключаем ШИМ D6, но НЕ записываем в EEPROM
         glavnaia();
         break;  
         
         //////////////// Кнопки//////////////////
         case 'J': // d11
         digitalWrite(11, HIGH);
         d11 = 1;
         EEPROM.write(11, d11);
         glavnaia();
         break;
         
         case 'j': // d11
         digitalWrite(11, LOW);
         d11 = 0;
         EEPROM.write(11, d11);
         glavnaia();
         break;  
        
         case 'K': // d12
         digitalWrite(12, HIGH);
         d12 = 1;
         EEPROM.write(12, d12);
         glavnaia();
         break;
         
         case 'k': // d12
         digitalWrite(12, LOW);
         d12 = 0;
         EEPROM.write(12, d12);
         glavnaia();
         break;         
      
         case 'M': // d13
         digitalWrite(13, HIGH);
         d13 = 1;
         EEPROM.write(13, d13);
         glavnaia();
         break;
         
         case 'm': // d13
         digitalWrite(13, LOW);
         d13 = 0;
         EEPROM.write(13, d13);
         glavnaia();
         break;
         
         ////////////// ОХЛАЖДЕНИЕ /////////////////
         case 'N': // увеличение "температуры включения охлаждения"
         oxlagdenie++; // если "температура включения охлаждения" больше нуля, то охлаждение будет работать в автоматическом режиме
         EEPROM.write(14, oxlagdenie); // запишем значение в память
         toxl = 1; // включаем флаг
         EEPROM.write(15, toxl); // запишем флаг в память
         glavnaia();
         break;
         
         case 'n': // уменьшение "температуры включения охлаждения"
         oxlagdenie--;
         if(oxlagdenie < 1) // если "температура включения охлаждения" = 0, то охлаждение отключится
            {
               oxlagdenie = 0; // меньше нуля, у нас не будет
               toxl = 0; // отключаем флаг
               EEPROM.write(14, oxlagdenie); // запишем значение в память
               EEPROM.write(15, toxl); // запишем флаг в память
               digitalWrite(7, LOW); // отключаем
               d7 = 0; 
            }
         glavnaia();
         break;  
  
         ////////////// ПОДОГРЕВ /////////////////
         case 'P': // увеличение "температуры включения печки"
         podogrev++; // если "температура включения печки" больше нуля, то печка будет работать в автоматическом режиме
         EEPROM.write(16, podogrev); // запишем значение в память
         tpod = 1; // включаем флаг
         EEPROM.write(17, tpod); // запишем флаг в память
         glavnaia();
         break;
         
         case 'p': // уменьшение "температуры включения печки"
         podogrev--;
         if(podogrev < 1) // если "температура включения печки" = 0, то печка отключится
            {
               podogrev = 0; // меньше нуля, у нас не будет
               tpod = 0; // отключаем флаг
               EEPROM.write(16, podogrev); // запишем значение в память
               EEPROM.write(17, tpod); // запишем флаг в память
               digitalWrite(8, LOW); // отключаем
               d8 = 0; 
            }
         glavnaia();
         break;  
  
         default:
         glavnaia();
       }
     }
   
    else // если символ был не 'Y', то очищаем буфер
      {
        for(byte i=0; i < 255; i++) 
         {
           Serial.read();    
         } 
      } 
     }    // конец if (Serial.read()=='Y')
   }    // конец чтение порта
 
 
 unsigned long currtime = millis(); 
 
 if(currtime > time) timelapsed = (currtime - time); 
 
 else timelapsed = (MAXMILLIS - time + currtime);

 if(timelapsed >= 120000)  // функция будет выполняться раз в 2 минуты
   { 
     time = currtime;
     count++;
     // поскольку время чтения каждого датчика ~ 250 мс (программа вешается на это время), то будем читать их по отдельности, в разное время  
     if(count == 1)  thum = dht.readTemperature(); // считываем температуру. Время чтения = 250 мс.
     if(count == 2)  hum = dht.readHumidity(); // считываем влажность, если не нужна, то лучше отключить. Время чтения = 250 мс
     if(count > 1) count=0;
 
   /////////////////////// Охлаждение ///////////////////////   
       if((thum > oxlagdenie) && (toxl == 1)) // если тепература больше заданого значения и флаг = 1, тогда включаем кондиционер (если флаг = 0, тогда функция не работает)
         {                                  
           digitalWrite(7, HIGH); // включили охлаждение                        
           d7 = 1;                          
         }
   
       else if(toxl == 1) // иначе если меньше и флаг = 1, тогда отключаем охлаждение                                   
         {
           digitalWrite(7, LOW); // отключили охлаждение  
           d7 = 0;
         }  
        
   /////////////////////// Подогрев ////////////////////////
       if((thum < podogrev) && (tpod == 1)) // если тепература меньше заданого значения и флаг = 1, тогда включаем печку (если флаг = 0, тогда функция не работает)  
         {                                
           digitalWrite(8, HIGH); // включили печку                      
           d8 = 1;                       
         }
   
       else if(tpod == 1) // иначе если больше и флаг = 1, тогда отключаем печку
         {
           digitalWrite(8, LOW); // отключили печку 
           d8 = 0;
         }
         
   } // конец включения по температуре
   
 } // конец loop

void glavnaia() // отправка данных
 {
      Serial.print(d2);//0
      Serial.print(",");
      Serial.print(d3);//1
      Serial.print(",");
      Serial.print(d4);//2
      Serial.print(",");
      Serial.print(0);//3  //  пока отключаем, потом пригодится 
      Serial.print(",");
      Serial.print(0);//4  //  пока отключаем, потом пригодится 
      Serial.print(",");
      Serial.print(d7);//5 охлаждение
      Serial.print(",");
      Serial.print(d8);//6 подогрев
      Serial.print(",");
      Serial.print(0);//7  //  пока отключаем, потом пригодится 
      Serial.print(",");
      Serial.print(0);//8 //  пока отключаем, потом пригодится 
      Serial.print(",");
      Serial.print(d11);//9
      Serial.print(",");
      Serial.print(d12);//10
      Serial.print(",");
      Serial.print(d13);//11 
      Serial.print(",");
      Serial.print(shim1); // 12 
      Serial.print(",");
      Serial.print(shim2); // 13 
      Serial.print(",");
      Serial.print(thum); // 14 //температура
      Serial.print(",");
      Serial.print(hum); // 15 // влажность
      Serial.print(",");
      Serial.print(oxlagdenie);//16 
      Serial.print(",");
      Serial.println(podogrev);//17 , отсылается 18 значений разделённых запятой
 }


