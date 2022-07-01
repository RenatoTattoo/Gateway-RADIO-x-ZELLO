
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>

// DEFINICÇÃO DAS PORTAS ----------------------------------------------------------------------------------------------
#define LDRradio         A0
#define LDRzello         A1
#define PTTradio         4
#define PTTzello         5
#define MP3ok            9
#define LED              13

// DEFINICÃO DOS ENDEREÇOS DA EEPROM ----------------------------------------------------------------------------------
#define Home             11
#define lhoon            13
#define online           15
#define lonrx            17
#define Vrx              19
#define lrxtx            21
#define Vtx              23
#define ltxoff           25
#define Voff             27

// Lista das MENSAGENS ---------------------------------------------------------------------------------------------------
#define rogerbeeplocal   1
#define online           2
#define offline          3
#define telapagada       4
#define problema         5
#define mensagem1        6
#define rogerbeepweb     7

#define INTERVALO        1800000

#define NC               35
#define VolumeVOZ        25
#define TelaOFF          10

SoftwareSerial ComunicacaoSerial(10, 11);  // Pinos 10 RX, 11 TX para comunicação serial com o DFPlayer
DFRobotDFPlayerMini ASSISTENTE;            // nomeando DFPlayer como "ASSISTENTE"
void printDetail(uint8_t type, int value);

Adafruit_SSD1306 display(128, 64, &Wire);

byte   MediaLDRzello[NC];                  // Array para cálculo da média móvel
long   mmLDRz();                           // Função média móvel para estabilização da leitura do LDR
long   mostrador();                        // Função para exibição no display
long   Leitura();

bool   LinkONLINE     = false;
bool   LinkOFFLINE    = false;
bool   ROGERBEEPlocal = false;
bool   ROGERBEEPweb   = false;
bool   pisca          = true;
String ESTADO         = "XX";
unsigned long Timer   = millis();
unsigned long HeartB  = millis();

const byte   HOME     = EEPROM.read (Home);
const byte   LHOON    = EEPROM.read (lhoon);
const byte   ONLINE   = EEPROM.read (online);
const byte   LONRX    = EEPROM.read (lonrx);
const byte   VRX      = EEPROM.read (Vrx);
const byte   LRXTX    = EEPROM.read (lrxtx);
const byte   VTX      = EEPROM.read (Vtx);
const byte   LTXOFF   = EEPROM.read (ltxoff);
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

   // MODO CONFIGURAÇÃO --------------------------------------------------------------------------------------------------
   
   if (digitalRead (LDRradio))
    {
      byte LDRhome = mmLDRz();
      byte LDRon   = mmLDRz();
      byte LDRrx   = mmLDRz();
      byte LDRtx   = mmLDRz();
      byte LDRoff  = mmLDRz();

      display.setTextColor(SSD1306_WHITE);
      
      // Procedimento 1/5 - Coletando valor ONLINE -----------------------------------------------------------------------
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

      // Procedimento 2/5 - Coletando valor RX ---------------------------------------------------------------------------
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
      LDRrx  = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRtx);
      display.display();
      display.clearDisplay(); 
      delay (2000);

      // Procedimento 3/5 - Coletando valor TX ---------------------------------------------------------------------------
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
      LDRtx  = mmLDRz();
      display.setTextSize(2);
      display.setCursor(0, 35); 
      display.print(F("LIDO: "));
      display.print(LDRrx);
      display.display();
      display.clearDisplay(); 
      delay (2000);

      // Procedimento 4/5 - Coletando valor OFFLINE ----------------------------------------------------------------------
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
 
      // Procedimento 5/5 - Coletando valor TELA INICIAL -----------------------------------------------------------------
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
      byte LimiteHOON  = (((LDRhome - LDRon )/2) + LDRon );
      byte LimiteONRX  = (((LDRon   - LDRrx )/2) + LDRrx );
      byte LimiteRXTX  = (((LDRrx   - LDRtx )/2) + LDRtx );
      byte LimiteTXOFF = (((LDRtx   - LDRoff)/2) + LDRoff);
      
      // Salvando na EEPROM os todos os dados coletados/calculados
      EEPROM.write (Home,   LDRhome    );
      EEPROM.write (lhoon,  LimiteHOON );
      EEPROM.write (online, LDRon      );
      EEPROM.write (lonrx,  LimiteONRX );
      EEPROM.write (Vrx,    LDRrx      );
      EEPROM.write (lrxtx,  LimiteRXTX );
      EEPROM.write (Vtx,    LDRtx      );
      EEPROM.write (ltxoff, LimiteTXOFF);
      EEPROM.write (Voff,   LDRoff     ); 

      // mostrando valores armazenados na EEPROM
      display.setTextSize(1);
      display.setCursor(0, 0); 
      display.print(F("Home---: "));
      display.println(EEPROM.read (Home));
      display.print(F("Von----: "));
      display.println(EEPROM.read (online));
      display.print(F("Lontx--: "));
      display.println(EEPROM.read (lonrx));
      display.print(F("Vtx----: "));
      display.println(EEPROM.read (Vrx));
      display.print(F("Ltxrx--: "));
      display.println(EEPROM.read (lrxtx));
      display.print(F("Vrx----: "));
      display.println(EEPROM.read (Vtx));
      display.print(F("Lrxoff-: "));
      display.println(EEPROM.read (ltxoff));
      display.print(F("Voff---: "));
      display.println(EEPROM.read (Voff));
      display.display();
      display.clearDisplay(); 
      delay (200);
      while (!digitalRead(LDRradio)) mmLDRz();

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
    mmLDRz();

    // Prevenção TELA APAGADA ou ZELLO FECHADO ---------------------------------------------------------------------------

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

   // LINK OFFLINE ------------------------------------------------------------------------------------------------------

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
      
         
   // LINK ONLINE --------------------------------------------------------------------------------------------------------

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
   

   // Link em RX - RECEBENDO PELO RÁDIO -----------------------------------------------------------------------------------
  
   if (digitalRead (LDRradio))
    {
      digitalWrite (PTTzello, HIGH);
      digitalWrite (LED,      HIGH);
      while (digitalRead (LDRradio))
       {
         mostrador();
       }
      digitalWrite (PTTzello, LOW);
      digitalWrite (LED,      LOW);
      ROGERBEEPlocal = true;
      Timer          = millis();
    }


   // Link em TX - RECEBENDO PELO ZELLO -----------------------------------------------------------------------------------
  
   if ( ((LinkONLINE) & (mmLDRz() < LRXTX)))
    {
      digitalWrite (PTTradio, HIGH);
      digitalWrite (LED,      HIGH);
      while ((mmLDRz() < LRXTX) & (mmLDRz() > LTXOFF))
       {
         mostrador();
       }
      ROGERBEEPweb = true;
      Timer        = millis();
    }

   // ROGERBEEP LOCAL ------------------------------------------------------------------------------------------------------

   if (ROGERBEEPlocal)
    {
      digitalWrite (PTTradio, HIGH);
      digitalWrite (LED,      HIGH);
      ASSISTENTE.play(rogerbeeplocal);
      delay(250);
      while (!digitalRead (MP3ok))
       {
         mostrador();
       }
      digitalWrite (PTTradio,   LOW);
      digitalWrite (LED,        LOW);
      ROGERBEEPlocal = false;
      Timer     = millis();
    }

   // ROGERBEEP WEB -------------------------------------------------------------------------------------------------------

   if (ROGERBEEPweb)
    {
      ASSISTENTE.play(rogerbeepweb);
      delay(250);
      while (!digitalRead (MP3ok))
       {
         mostrador();
       }
      digitalWrite (PTTradio,   LOW);
      digitalWrite (LED,        LOW);
      ROGERBEEPweb = false;
      Timer        = millis();
    }

   // Qualquer atividade zera TIMER das Mensagens periódica ---------------------------------------------------------------
   if (ESTADO != "ON") Timer  = millis();

   // Mensagens periódica -------------------------------------------------------------------------------------------------

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

// --------------------------------- FUNÇÃO MEDIA MOVEL ------------------------------------------------------------------

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

// --------------------------------- FUNÇÃO MOSTRADOR --------------------------------------------------------------------

long mostrador()
 {
   byte ValorAtual = mmLDRz();
   if ((MediaLDRzello[0] == MediaLDRzello[15]) & (MediaLDRzello[15] == MediaLDRzello[30]))
    {
     if ((ValorAtual < TelaOFF) | (ValorAtual > LHOON )) ESTADO = "XX";
     if ((ValorAtual > TelaOFF) & (ValorAtual < LTXOFF)) ESTADO = "OF";
     if ((ValorAtual > LONRX)   & (ValorAtual < LHOON )) ESTADO = "ON";
     if ((ValorAtual > LTXOFF)  & (ValorAtual < LRXTX )) ESTADO = "TX";
     if (digitalRead (LDRradio))                         ESTADO = "RX";
    }
   
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
   const byte CLLONRX    = map (LONRX,      0, HOME, 0, 128);
   const byte CLLRXTX    = map (LRXTX,      0, HOME, 0, 128);
   const byte CLLTXOFF   = map (LTXOFF,     0, HOME, 0, 128);
         byte CLLEITURA  = map (ValorAtual, 0, HOME, 0, 128);
   
   const byte CLVRX      = (((CLLONRX - CLLRXTX)  / 2) + CLLRXTX ) - 4;
   const byte CLVTX      = (((CLLRXTX - CLLTXOFF) / 2) + CLLTXOFF) - 5;
   const byte CLVOFFLINE = (((CLLTXOFF -      10) / 2) +       11);

   display.drawLine (118, 0, 118, 16, SSD1306_WHITE);
   display.setCursor (CLONLINE +5,  17); 
   display.print     (F("ON"));
   display.drawLine (CLLONRX, 0, CLLONRX, 16, SSD1306_WHITE);
   display.setCursor (CLVTX, 17); 
   display.print     (F("TX"));
   display.drawLine (CLLRXTX, 0, CLLRXTX, 16, SSD1306_WHITE); 
   display.setCursor (CLVRX, 17); 
   display.print     (F("RX"));
   display.drawLine (CLLTXOFF, 0, CLLTXOFF, 16, SSD1306_WHITE);
   display.setCursor (CLVOFFLINE-8, 17); 
   display.print     (F("OFF"));
   display.drawLine (10, 0, 10, 16, SSD1306_WHITE);
   
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
