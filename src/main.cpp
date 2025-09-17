// --- Bibliotecas ---
#include <ESP32Servo.h>

// --- Configurações de Hardware ---
const int PINO_POTENCIOMETRO = 34; // Pino ADC para ler o ângulo
const int PINO_ESC_SINAL = 23;   // Pino para o sinal de controle do ESC

// --- Parâmetros de Controle PID ---
const float Kp = 0.6;
const float Ki = 0.009;
const float Kd = 1.1;

// --- Ponto de Operação (Setpoint) ---
const float setpoint_angulo = 90.0;
const int pulse_equilibrio = 1100;

// --- Calibração do Sensor (Potenciômetro) ---
const int POT_MIN = 0;
const int POT_MAX = 4095;
const float ANGULO_MIN = 0.0;
const float ANGULO_MAX = 364.0;

// --- Variáveis Globais ---
Servo motor_esc;
float erro_anterior = 0;
float acumulador_erro = 0;
float derivada_filtrada_anterior = 0;

int index_teste = 0;

const float ALFA_FILTRO_D = 0.2;

unsigned long tempo_anterior = 0;

int lerAngulo(int index);

void setup() {
  Serial.begin(115200);
  Serial.println("Controlador PID para Motor Brushless Iniciado.");

  pinMode(PINO_POTENCIOMETRO, INPUT);

  // IMPORTANTE: CUIDADO AO SETAR A VELOCIDADE MAXIMA DO MOTOR
  // Setado em 1450 por seguranca!
  motor_esc.attach(PINO_ESC_SINAL, 1000, 1450);

  // --- SEQUÊNCIA DE ARME DO ESC ---
  Serial.println("Armando o ESC... Enviando sinal mínimo (1000us).");
  Serial.println("ATENÇÃO: Mantenha a hélice livre de obstruções!");
  motor_esc.writeMicroseconds(1000);
  delay(7000);
  
  Serial.println("ESC armado. O controle está ativo.");

  tempo_anterior = millis();
}

void loop() {
  // --- CÁLCULO DO TEMPO (dt) ---
  unsigned long tempo_atual = millis();
  float dt = (tempo_atual - tempo_anterior) / 1000.0;
  if (dt <= 0) dt = 0.001;
  tempo_anterior = tempo_atual;

  // --- LEITURA DO SENSOR E CÁLCULO DO ERRO ---
  if(index_teste == 10) {
    index_teste = 0;
  }
  float angulo_atual = lerAngulo(index_teste);
  index_teste++;

  float erro = setpoint_angulo - angulo_atual;

  // --- CÁLCULO DAS CONSTANTES PID ---
  float termo_P = Kp * erro;

  acumulador_erro += erro * dt;
  acumulador_erro = constrain(acumulador_erro, -300, 300); //REVISAR ESSE VALOR DE 300 COMO LIMITE

  float termo_I = Ki * acumulador_erro;
  float derivada_erro = (erro - erro_anterior) / dt;
  float derivada_filtrada = (ALFA_FILTRO_D * derivada_erro) + (1.0 - ALFA_FILTRO_D) * derivada_filtrada_anterior;
  float termo_D = Kd * derivada_filtrada;
  
  // --- CÁLCULO DA AÇÃO DE CONTROLE ---
  float delta_pulse = termo_P + termo_I + termo_D;

  int pulse_final = pulse_equilibrio + delta_pulse;

  // --- 5. SATURAÇÃO DO ATUADOR (Anti-Windup) ---
  // Limita o pulso final para a faixa válida do ESC.
  pulse_final = constrain(pulse_final, 1000, 1450); //IMPORTANTE: Valor maximo de pulse_final deve ser igual o maximo do motor!

  // --- 6. ATUAÇÃO ---
  motor_esc.writeMicroseconds(pulse_final);

  // --- 7. ATUALIZAÇÃO E DEBUG ---
  erro_anterior = erro;
  derivada_filtrada_anterior = derivada_filtrada;

  // Imprime os dados no Serial Plotter
  Serial.print("Angulo: "); Serial.print(angulo_atual);
  Serial.print(" Termo p: "); Serial.print(termo_P);
  Serial.print(" Termo i: "); Serial.print(termo_I);
  Serial.print(" Termo d: "); Serial.print(termo_D);
  Serial.print(" Acumulador: "); Serial.print(acumulador_erro);

  Serial.print("  Delta: "); Serial.print(delta_pulse);
  Serial.print("  Erro: "); Serial.print(erro);
  Serial.print("  Pulso: "); Serial.println(pulse_final);

  delay(10); 
}

int lerAngulo(int index) {
  // int valor_adc = analogRead(PINO_POTENCIOMETRO);
  // float angulo = map(valor_adc, POT_MIN, POT_MAX, ANGULO_MAX, ANGULO_MIN);
  // return constrain(angulo, ANGULO_MIN, ANGULO_MAX);

  int test_degrees[10] = {50,60,70,80,90,95,93,100,130,15};

  return test_degrees[index];
}
