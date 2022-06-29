
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>

// DEFINICÇÃO DAS PORTAS ---------------------------------------
#define LDRradio         A0
#define LDRzello         A1
#define PTTradio         4
#define PTTzello         2
#define MP3ok            9
#define LED              13

// DEFINICÃO DOS ENDEREÇOS DA EEPROM ---------------------------
#define Home             11
#define lhoon            13
#define online           15
#define lontx            17
#define Vtx              19
#define ltxrx            21
#define Vrx              23
#define lrxoff           25
#define Voff             27

// Lista das MENSAGENS -----------------------------------------
#define rogerbeep        1
#define online           2
#define offline          3
#define telapagada       4
#define problema         5
#define mensagem1        6
#define rogerbeep2       7

#define INTERVALO        1800000

#define NC               50
#define VolumeVOZ        20
#define TelaOFF          10

SoftwareSerial ComunicacaoSerial(10, 11);  // Pinos 10 RX, 11 TX para comunicação serial com o DFPlayer
DFRobotDFPlayerMini ASSISTENTE;            // nomeando DFPlayer como "ASSISTENTE"
void printDetail(uint8_t type, int value);

Adafruit_SSD1306 display(128, 64, &Wire);

byte   MediaLDRzello[NC];                  // Array para cálculo da média móvel
long   mmLDRz();                           // Função média móvel para estabilização da leitura do LDR
long   mostrador();                        // Função para exibição no display
long   Leitura();

bool   LinkONLINE    = false;
bool   LinkOFFLINE   = false;
bool   ROGERBEEP     = false;
bool   pisca         = true;
String ESTADO        = "XX";
unsigned long Timer  = millis();
unsigned long HeartB = millis();

const byte   HOME     =(EEPROM.read (Home)) - 10;
const byte   LHOON    = EEPROM.read (lhoon);
const byte   ONLINE   = EEPROM.read (online);
const byte   LONTX    = EEPROM.read (lontx);
const byte   VTX      = EEPROM.read (Vtx);
const byte   LTXRX    = EEPROM.read (ltxrx);
const byte   VRX      = EEPROM.read (Vrx);
const byte   LRXOFF   = EEPROM.read (lrxoff);
const byte   VOFFLINE = EEPROM.read (Voff);

void setup()
 {
   ComunicacaoSerial.begin(9600);
   
   pinMode(LDRzello, INPUT);
   pinMode(LDRradio, INPUT);
   pinMode(MP3ok   , INPUT);
   pinMode(PTTzello, OUTPUT);
   pinMode(PTTradio, OUTPUT);
   pinMode(LED,      OUTPUT);
   
   digitalWrite (PTTradio, LOW);
   digitalWrite (PTTzello, LOW);
   digitalWrite (LED,      LOW);
   
   while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
      delay (50);
      digitalWrite (LED, 0);
      delay (50);
      digitalWrite (LED, 1);
    }
   display.display();
   delay(1000);
   display.clearDisplay(); 

   while(!ASSISTENTE.begin(ComunicacaoSerial))
    {
      delay (500);
      digitalWrite (LED, 0);
      delay (500);
      digitalWrite (LED, 1);
    }

   ASSISTENTE.setTimeOut(500);                    // ajustando o timeout da comunicação serial 500ms
   ASSISTENTE.volume(VolumeVOZ);                  // Ajustando o volume (0~30).
   ASSISTENTE.EQ(DFPLAYER_EQ_BASS);               // configurando equalizador = BASS
   ASSISTENTE.outputDevice(DFPLAYER_DEVICE_SD);   // configurando a media usada = SDcard

   // MODO CONFIGURAÇÃO ------------------------------------------------------
   
   if (digitalRead (LDRradio))
    {
      byte LDRon   = mmLDRz();
      byte LDRtx   = mmLDRz();
      byte LDRrx   = mmLDRz();
      byte LDRoff  = mmLDRz();
      byte LDRhome = mmLDRz();

      display.setTextColor(SSD1306_WHITE);
      
      // Procedimento 1/5 - Coletando valor ONLINE ---------------------------
      display.setTextSize(2);
      display.setCursor(5, 0); 
      display.print(F("CONFIG 1/5"));
      display.setTextSize(1);
      display.setCursor(6, 22); 
      display.print(F("Mantenha o ZELLO em"));
      display.setCursor(14, 35);
      display.print(F("ONLINE - LARANJA "));
      display.setCursor(12, 48);
      display.print(F("e pressione o PTT"));
      display.display();
      display.clearDisplay(); 
      delay (5000);
      while (!digitalRead(LDRradio)) mmLDRz();
      Leitura();
      LDRon = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRon);
      display.display();
      display.clearDisplay(); 
      delay (2000);

      // Procedimento 2/5 - Coletando valor TX ----------------------------------------
      display.setTextSize(2);
      display.setCursor(5, 0); 
      display.print(F("CONFIG 2/5"));
      display.setTextSize(1);
      display.setCursor(6, 22); 
      display.print(F("Mantenha o ZELLO em"));
      display.setCursor(29, 35);   
      display.print(F("TX VERMELHO"));
      display.setCursor(12, 48);
      display.print(F("e pressione o PTT"));
      display.display();
      display.clearDisplay(); 
      delay (2000);
      while (!digitalRead(LDRradio)) mmLDRz();
      Leitura();
      LDRtx  = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRtx);
      display.display();
      display.clearDisplay(); 
      delay (2000);

      // Procedimento 3/5 - Coletando valor RX --------------------------------------
      display.setTextSize(2);
      display.setCursor(5, 0); 
      display.print(F("CONFIG 3/5"));
      display.setTextSize(1);
      display.setCursor(6, 22); 
      display.print(F("Mantenha o ZELLO em"));
      display.setCursor(33, 35);
      display.print(F("RX - VERDE"));
      display.setCursor(12, 48);
      display.print(F("e pressione o PTT"));
      display.display();
      display.clearDisplay(); 
      delay (2000);
      while (!digitalRead(LDRradio)) mmLDRz();
      Leitura();
      LDRrx  = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRrx);
      display.display();
      display.clearDisplay(); 
      delay (2000);

      // Procedimento 4/5 - Coletando valor OFFLINE ---------------------------
      display.setTextSize(2);
      display.setCursor(5, 0); 
      display.print(F("CONFIG 4/5"));
      display.setTextSize(1);
      display.setCursor(6, 22); 
      display.print(F("Mantenha o ZELLO em")); 
      display.setCursor(17, 35);     
      display.print(F("OFFLINE - CINZA"));
      display.setCursor(12, 48);
      display.print(F("e pressione o PTT"));
      display.display();
      display.clearDisplay(); 
      delay (2000);
      while (!digitalRead(LDRradio)) mmLDRz();
      Leitura();
      LDRoff = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRoff);
      display.display();
      display.clearDisplay(); 
      delay (2000);
 
      // Procedimento 5/5 - Coletando valor TELA INICIAL ---------------------------
      display.setTextSize(2);
      display.setCursor(5, 0); 
      display.print(F("CONFIG 5/5"));
      display.setTextSize(1);
      display.setCursor(23, 22);
      display.print(F("FECHE o ZELLO")); 
      display.setCursor(8, 35);   
      display.print(F("para a tela inicial"));
      display.setCursor(12, 48);
      display.print(F("e pressione o PTT"));
      display.display();
      display.clearDisplay(); 
      delay (2000);
      while (!digitalRead(LDRradio)) mmLDRz();
      Leitura();
      LDRhome = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRhome);
      display.display();
      display.clearDisplay(); 
      delay (2000);
     
      
      // Calculand os limites com valores coletados
      byte LimiteHO  = (((LDRhome - LDRon )/2) + LDRon );
      byte LimiteTX  = (((LDRon   - LDRtx )/2) + LDRtx );
      byte LimiteRX  = (((LDRtx   - LDRrx )/2) + LDRrx );
      byte LimiteOFF = (((LDRrx   - LDRoff)/2) + LDRoff);
      
      // Salvando na EEPROM os todos os dados coletados/calculados
      EEPROM.write (Home,   LDRhome  );
      EEPROM.write (lhoon,  LimiteHO );
      EEPROM.write (online, LDRon    );
      EEPROM.write (lontx,  LimiteTX );
      EEPROM.write (Vtx,    LDRtx    );
      EEPROM.write (ltxrx,  LimiteRX );
      EEPROM.write (Vrx,    LDRrx    );
      EEPROM.write (lrxoff, LimiteOFF);
      EEPROM.write (Voff,   LDRoff   ); 

      // mostrando valores armazenados na EEPROM
      display.setTextSize(1);
      display.setCursor(0, 0); 
      display.print(F("Home---: "));
      display.println(EEPROM.read (Home));
      display.print(F("Von----: "));
      display.println(EEPROM.read (online));
      display.print(F("Lontx--: "));
      display.println(EEPROM.read (lontx));
      display.print(F("Vtx----: "));
      display.println(EEPROM.read (Vtx));
      display.print(F("Ltxrx--: "));
      display.println(EEPROM.read (ltxrx));
      display.print(F("Vrx----: "));
      display.println(EEPROM.read (Vrx));
      display.print(F("Lrxoff-: "));
      display.println(EEPROM.read (lrxoff));
      display.print(F("Voff---: "));
      display.println(EEPROM.read (Voff));
      display.display();
      display.clearDisplay(); 
      delay (3000);

      display.setTextSize(2);
      display.setCursor(47, 2); 
      display.print(F("FIM"));
      display.setCursor(18, 26);
      display.print(F("REINICIE"));
      display.setCursor(32, 50);
      display.print(F("O LINK"));
      display.display();
      display.clearDisplay();

      while (true) {}
    }
 }

void loop()
  {

    mostrador(); 
    

    // Prevenção TELA APAGADA ou ZELLO FECHADO ---------------------------------------

    if (((ESTADO == "XX") & (LinkOFFLINE)) | ((ESTADO == "XX") & (LinkONLINE)) | ((ESTADO == "XX") & (digitalRead (LDRradio))))
      {
        while (digitalRead (LDRradio))
         {
           mostrador();
         }
        delay (100);
        digitalWrite (PTTradio,   HIGH);
        digitalWrite (LED,        HIGH);
        delay (200);
        if (mmLDRz() < TelaOFF) ASSISTENTE.play(telapagada);
        if (mmLDRz() > HOME)    ASSISTENTE.play(problema);
        delay(500);
        while (!digitalRead (MP3ok))
         {
           mostrador();
         }
        delay (200);
        digitalWrite (PTTradio,   LOW);
        digitalWrite (LED,        LOW);
        LinkOFFLINE = false;
        LinkONLINE  = false;
        Timer       = millis();
      }

   // LINK OFFLINE ----------------------------------------------

    if (((ESTADO == "OF") & (LinkONLINE)) | ((ESTADO == "OF") & (!LinkOFFLINE)) | ((ESTADO == "OF") & (digitalRead (LDRradio))))
      {
        while (digitalRead (LDRradio))
         {
           mostrador();
         }
        delay (100);
        digitalWrite (PTTradio,   HIGH);
        digitalWrite (LED,        HIGH);
        delay (200);
        ASSISTENTE.play(offline);
        delay(500);
        while (!digitalRead (MP3ok))
         {
           mostrador();
         }
        delay (200);
        digitalWrite (PTTradio,   LOW);
        digitalWrite (LED,        LOW);
        LinkOFFLINE = true;
        LinkONLINE  = false;
        Timer       = millis();
      }
      
         
   // LINK ONLINE ----------------------------------------------

   if (((ESTADO == "ON") & (LinkOFFLINE)) | ((ESTADO == "ON") & (!LinkONLINE)))
    {
      digitalWrite (PTTradio,   HIGH);
      digitalWrite (LED,        HIGH);
      delay (200);
      ASSISTENTE.play(online);
      delay(500);
      while (!digitalRead (MP3ok))
       {
         mostrador();
       }
      delay (200);
      digitalWrite (PTTradio,   LOW);
      digitalWrite (LED,        LOW);
      LinkOFFLINE = false;
      LinkONLINE  = true;
      Timer       = millis();
    }
   

   // Link em RX - RECEBENDO PELO RÁDIO ----------------------------------------------
  
   if ((LinkONLINE) & (digitalRead (LDRradio)))
    {
      digitalWrite (PTTzello, HIGH);
      digitalWrite (LED,      HIGH);
      while (digitalRead (LDRradio))
       {
         mostrador();
       }
      delay (200);
      digitalWrite (PTTzello, LOW);
      digitalWrite (LED,      LOW);
      ROGERBEEP  = true;
      Timer      = millis();
    }


   // Link em TX - RECEBENDO PELO ZELLO ----------------------------------------------
  
   if ( ((LinkONLINE) & (mmLDRz() < LTXRX)))
    {
      digitalWrite (PTTradio, HIGH);
      digitalWrite (LED,      HIGH);
      while ((mmLDRz() < LTXRX) & (mmLDRz() > LRXOFF))
       {
         mostrador();
       }
      digitalWrite (PTTradio, LOW);
      digitalWrite (LED,      LOW);
      Timer  = millis();
    }

   // ROGER BEEP ----------------------------------------------

   if (ROGERBEEP)
    {
      digitalWrite (PTTradio,   HIGH);
      digitalWrite (LED,        HIGH);
      delay (250);
      ASSISTENTE.play(rogerbeep);
      delay(250);
      while (!digitalRead (MP3ok))
       {
         mostrador();
       }
      digitalWrite (PTTradio,   LOW);
      digitalWrite (LED,        LOW);
      ROGERBEEP = false;
      Timer     = millis();
    }

   // Mensagens periódica ---------------------------------------
   if (ESTADO != "ON") Timer  = millis();

   if ((millis() - Timer) > INTERVALO)
    {
      digitalWrite (PTTradio,   HIGH);
      digitalWrite (LED,        HIGH);
      delay (100);
      ASSISTENTE.play(mensagem1);
      delay(250);
      while (!digitalRead (MP3ok))
       {
         mostrador();
       }
      digitalWrite (PTTradio,   LOW);
      digitalWrite (LED,        LOW);
      Timer  = millis();
    }
 }

// --------------------------------- FUNÇÃO MEDIA MOVEL ---------------------------------------------

long mmLDRz()
 {
   for(byte x= NC-1; x>0; x--) 
     MediaLDRzello[x] = MediaLDRzello[x-1];
   MediaLDRzello[0] = analogRead (LDRzello);
   word contador  = 0;
   for (byte x=0; x<NC; x++)
     contador += MediaLDRzello[x];
   return (contador/NC);
 }

// --------------------------------- FUNÇÃO MOSTRADOR ---------------------------------------------

long mostrador()
 {
   byte ValorAtual = mmLDRz();
   if ((MediaLDRzello[0] == MediaLDRzello[10]) & (MediaLDRzello[20] == MediaLDRzello[30]))
    {
     if ((ValorAtual < TelaOFF) | (ValorAtual > LHOON )) ESTADO = "XX";
     if ((ValorAtual > TelaOFF) & (ValorAtual < LRXOFF)) ESTADO = "OF";
     if ((ValorAtual > LONTX)   & (ValorAtual < LHOON )) ESTADO = "ON";
     if ((ValorAtual > LRXOFF)  & (ValorAtual < LTXRX )) ESTADO = "TX";
    }
   if (digitalRead (LDRradio))                       ESTADO = "RX";
   display.setTextColor(SSD1306_WHITE); // preparando tela com informações
   display.setTextSize(4);
   display.setCursor(74, 33); 
   display.print(ESTADO);              
   display.setTextSize(1);
   display.setCursor(0, 30); 
   display.print(F("ZELLO: "));              
   display.print(ValorAtual);
   display.setCursor(0, 39); 
   display.print(F("RADIO: "));              
   display.print(digitalRead (LDRradio));
   display.setCursor(0, 48); 
   display.print(F("AUDIO: "));              
   display.print(!digitalRead (MP3ok));
   display.setCursor(0, 57);
   display.print(F("ATIVO: ")); 
   if ((millis() - HeartB) > 500)
     {
       HeartB = millis();
       pisca = !pisca;
     }              
   if (pisca)
     {
       display.fillRect(42, 57, 11, 7, SSD1306_WHITE);
     }
     else
     {
       display.drawRect(42, 57, 11, 7, SSD1306_WHITE);
     }
   
   const byte CLONLINE   = map (ONLINE,     0, HOME, 0, 128);
   const byte CLLONTX    = map (LONTX,      0, HOME, 0, 128);
   const byte CLVTX      = map (VTX,        0, HOME, 0, 128);
   const byte CLLTXRX    = map (LTXRX,      0, HOME, 0, 128);
   const byte CLVRX      = map (VRX,        0, HOME, 0, 128);
   const byte CLLRXOFF   = map (LRXOFF,     0, HOME, 0, 128);
   const byte CLVOFFLINE = map (VOFFLINE,   0, HOME, 0, 128);
         byte CLLEITURA  = map (ValorAtual, 0, HOME, 0, 128);

   display.setCursor (CLONLINE +5,  17); 
   display.print     (F("ON"));
   display.drawLine (CLLONTX+2, 0, CLLONTX+2, 16, SSD1306_WHITE);
   display.setCursor (CLVTX-6, 17); 
   display.print     (F("RX"));
   display.drawLine (CLLTXRX-2, 0, CLLTXRX-2, 16, SSD1306_WHITE); 
   display.setCursor (CLVRX-8, 17); 
   display.print     (F("TX"));
   display.drawLine (CLLRXOFF, 0, CLLRXOFF, 16, SSD1306_WHITE);
   display.setCursor (CLVOFFLINE-10, 17); 
   display.print     (F("OF"));
   
   display.drawRect(0, 0, 128, 10, SSD1306_WHITE);
   display.fillTriangle(CLLEITURA    ,10,
                        CLLEITURA - 4, 0,
                        CLLEITURA + 4, 0,
                            SSD1306_WHITE);
    
   display.display();
   display.clearDisplay();
 }

// --------------------------------- FUNÇÃO TELA LEITURAS DE CALIBRAÇÃO --------------------------------------------------
long Leitura()
 {
  for(int x=0; x<127; x+=1)
   {
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 35); 
    display.print(F("SENSOR: "));
    display.print(mmLDRz());
    display.drawRect(0, 0, 128, 16, SSD1306_WHITE);
    display.fillRect(0, 0,   x, 16, SSD1306_WHITE);
    display.display();
    display.clearDisplay();
   }
 }
