 // Pinos dos segmentos para o primeiro par de displays
const int a1 = 22, b1 = 24, c1 = 26, d1 = 28, e1 = 30, f1 = 32,  g1 = 34;

// Pinos de controle para o primeiro par de displays
const int d1_1 = 36;  // Unidade do primeiro número
const int d1_2 = 38;  // Dezena do primeiro número

// Pinos dos segmentos para o segundo par de displays
const int a2 = 31, b2 = 33, c2 = 35, d2 = 37, e2 = 39, f2 = 40, g2 = 41;

// Pinos de controle para o segundo par de displays
const int d2_1 = 42;  // Unidade do segundo número
const int d2_2 = 43;  // Dezena do segundo número

// Tabela de segmentos para os números de 0 a 9
byte segmentos[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1},  // 2
  {1, 1, 1, 1, 0, 0, 1},  // 3
  {0, 1, 1, 0, 0, 1, 1},  // 4
  {1, 0, 1, 1, 0, 1, 1},  // 5
  {1, 0, 1, 1, 1, 1, 1},  // 6
  {1, 1, 1, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1},  // 8
  {1, 1, 1, 1, 0, 1, 1}   // 9
};

const int botao = 8;  // Pino do botão

const int pot1 = A0;  // Primeiro número
const int pot2 = A1;  // Segundo número

int count1 = 0, count2 = 0;
int flag = 0;


bool travado = false;  // Variável para controlar o estado travado/destravado
bool botaoPressionado = false;
unsigned long ultimaLeituraBotao = 0;
const unsigned long debounceDelay = 50;  // Tempo de debounce para o botão

int numero1 = 0, numero2 = 0;  // Leituras dos potenciômetros

const int buzzerPin = 53;

const int sensorAgua = A2;  // Pino analógico ou digital conectado ao sensor de água

#define RELE_PIN 7  // Pino digital conectado ao módulo relé

const int sensorTerra = A3;
const int ldrPin=A4;

const int ledVerde = 51;
const int ledVermelho = 50;

void setup() {
  Serial.begin(9600);

  // Configura os pinos dos segmentos para o primeiro par como saída
  pinMode(a1, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(c1, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(e1, OUTPUT);
  pinMode(f1, OUTPUT);
  pinMode(g1, OUTPUT);
  pinMode(d1_1, OUTPUT);
  pinMode(d1_2, OUTPUT);

  // Configura os pinos dos segmentos para o segundo par como saída
  pinMode(a2, OUTPUT);
  pinMode(b2, OUTPUT);
  pinMode(c2, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(e2, OUTPUT);
  pinMode(f2, OUTPUT);
  pinMode(g2, OUTPUT);
  pinMode(d2_1, OUTPUT);
  pinMode(d2_2, OUTPUT);

  pinMode(botao, INPUT_PULLUP);  // Configura o botão como entrada com pull-up

  // Configura os pinos analógicos para leitura (opcional)
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(RELE_PIN, OUTPUT);  // Define o pino do relé como saída

  pinMode(sensorAgua, INPUT);  // Configura o pino do sensor como entrada

  pinMode(sensorTerra, INPUT);

  pinMode(ldrPin,INPUT);

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // Configuração do Timer1
  cli();  // Desabilita interrupções globais
  TCCR1A = 0;  // Operação normal
  TCCR1B = (1 << CS10);  // Prescaler = 1
  TCNT1 = 0xC180;  // Inicializa o Timer1 com o valor para estourar em 1 segundo
  TIMSK1 |= (1 << TOIE1);  // Habilita a interrupção de overflow do Timer1
  sei();  // Habilita interrupções globais
}

int umidadeMinima;
int luminosidadeMinima;
int umidadeLida;
int luminosidadeLida;

bool ativarAgua = false;
bool nivelAgua = false;
bool agua = false;

void loop() {

  //nivel d'agua no reservatorio
  int nivel = analogRead(sensorAgua);
  if(nivel<=300){
    buzzer(1);
    nivelAgua = false;
  }else{
    buzzer(0);
    nivelAgua = true;
  }

  
  // leitura do sensor de umidade (valores mapeados para 0-99)
  umidadeLida = map(analogRead(sensorTerra), 1023, 0, 0, 99);

  luminosidadeLida = map(analogRead(ldrPin), 0, 1023, 0, 99);


  if (!travado) {
    //receber valores
    // Leitura dos dois potenciômetros (valores mapeados para 0-99)
    numero1 = map(analogRead(pot1), 0, 1023, 0, 99);
    numero2 = map(analogRead(pot2), 0, 1023, 0, 99);

    umidadeMinima=numero1;
    luminosidadeMinima=numero2;

    ligar(numero1, numero2);  // Exibe os números apenas se não estiver travado
    ativarAgua= false;
  }else{
    //exibir leituras
    ligar(umidadeLida, luminosidadeLida); //Exibe as informações lidas pelos sensores
    ativarAgua= true;
  }
  
  if(umidadeLida<umidadeMinima){
    agua=true;
  }else{
    agua = false;
  }


  controleLed(luminosidadeMinima, luminosidadeLida);
  aguarPlanta(ativarAgua, nivelAgua, agua);
}

void controleLed(int lumin, int lulida){
    if(lulida<lumin){
        digitalWrite(ledVermelho, HIGH);
        digitalWrite(ledVerde, LOW);
      }else{
        digitalWrite(ledVerde, HIGH);
        digitalWrite(ledVermelho, LOW);
    }
}

//controle do rele
void aguarPlanta(bool ativar, bool nivel, bool agua){
  if(ativar && nivel && agua){
    digitalWrite(RELE_PIN, HIGH); // Liga o relé
  }else{
    digitalWrite(RELE_PIN, LOW); // Desliga o relé
  }
}


//controle dos leds


// controle do buzzer
void buzzer(bool alarme) {
  if (alarme) {  
    digitalWrite(buzzerPin, HIGH);  // Liga o buzzer
  } else {
    digitalWrite(buzzerPin, LOW);  // Desliga o buzzer
  }
}

void ligar(int numero1, int numero2) {
  int dezena1 = numero1 / 10;
  int unidade1 = numero1 % 10;
  int dezena2 = numero2 / 10;
  int unidade2 = numero2 % 10;

  if (count1 >= 5) {
    count1 = 0;

    if (flag == 0) {  // Exibe unidade do primeiro número
      digitalWrite(d1_1, LOW);
      digitalWrite(d1_2, HIGH);
      digitalWrite(d2_1, HIGH);
      digitalWrite(d2_2, HIGH);
      mostrarDigito(unidade1, a1, b1, c1, d1, e1, f1, g1);
      flag = 1;
    } else if (flag == 1) {  // Exibe dezena do primeiro número
      digitalWrite(d1_1, HIGH);
      digitalWrite(d1_2, LOW);
      digitalWrite(d2_1, HIGH);
      digitalWrite(d2_2, HIGH);
      mostrarDigito(dezena1, a1, b1, c1, d1, e1, f1, g1);
      flag = 2;
    } else if (flag == 2) {  // Exibe unidade do segundo número
      digitalWrite(d1_1, HIGH);
      digitalWrite(d1_2, HIGH);
      digitalWrite(d2_1, LOW);
      digitalWrite(d2_2, HIGH);
      mostrarDigito(unidade2, a2, b2, c2, d2, e2, f2, g2);
      flag = 3;
    } else {  // Exibe dezena do segundo número
      digitalWrite(d1_1, HIGH);
      digitalWrite(d1_2, HIGH);
      digitalWrite(d2_1, HIGH);
      digitalWrite(d2_2, LOW);
      mostrarDigito(dezena2, a2, b2, c2, d2, e2, f2, g2);
      flag = 0;
    }
  }
}

void mostrarDigito(int digito, int a, int b, int c, int d, int e, int f, int g) {
  // Ativa os segmentos para o dígito especificado
  digitalWrite(a, segmentos[digito][0]);
  digitalWrite(b, segmentos[digito][1]);
  digitalWrite(c, segmentos[digito][2]);
  digitalWrite(d, segmentos[digito][3]);
  digitalWrite(e, segmentos[digito][4]);
  digitalWrite(f, segmentos[digito][5]);
  digitalWrite(g, segmentos[digito][6]);
}

ISR(TIMER1_OVF_vect) {  // Interrupção do Timer1
  TCNT1 = 0xC180;  // Reinicia o Timer1
  count1++;
  count2++;

  // Lógica de debounce e travamento
  if (digitalRead(botao) == LOW && !botaoPressionado) {  // Botão foi pressionado
    travado = !travado;  // Alterna entre travado e destravado
    botaoPressionado = true;  // Marca que o botão foi pressionado
    Serial.println(travado ? "Travado" : "Destravado");
  } else if (digitalRead(botao) == HIGH) {  // Botão foi solto
    botaoPressionado = false;  // Libera o botão para detectar nova pressão
  }
}