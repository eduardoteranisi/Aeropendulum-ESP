// --- Bibliotecas ---
#include <ESP32Servo.h>

// --- Configurações de Hardware ---
const int PINO_POTENCIOMETRO = 34; // Pino ADC para ler o ângulo
const int PINO_ESC_SINAL = 23;   // Pino para o sinal de controle do ESC

// --- Parâmetros de Controle PID ---
const float Kp = 0.9762;
const float Ki = 0.1463;
const float Kd = 1.6282;

const float ALFA_FILTRO = 0.2;
float derivada_filtrada_anterior = 0;

// --- Ponto de Operação (Setpoint) ---
const float setpoint_angulo = 60.0;
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
unsigned long tempo_anterior = 0;

float lerAngulo();

void setup() {
  Serial.begin(115200);
  Serial.println("Controlador PID para Motor Brushless Iniciado.");

  pinMode(PINO_POTENCIOMETRO, INPUT);

  motor_esc.attach(PINO_ESC_SINAL, 1000, 1300);

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
  float angulo_atual = lerAngulo();
  float erro = setpoint_angulo - angulo_atual;

  // --- CÁLCULO DAS CONSTANTES PID ---
  float termo_P = Kp * erro;

  float derivada_erro = (erro - erro_anterior) / dt;
  float derivada_filtrada = (ALFA_FILTRO * derivada_erro) + (1.0 - ALFA_FILTRO) * derivada_filtrada_anterior; 

  float termo_D = Kd * derivada_filtrada;
  
  float tentativa_delta = termo_P + termo_D + Ki * acumulador_erro;
  int tentativa_pulse = pulse_equilibrio + tentativa_delta;

  // --- checa saturação antes de integrar ---
  if ((tentativa_pulse > 1000) && (tentativa_pulse < 1300)) {
      acumulador_erro += erro * dt;
  }

  float termo_I = Ki * acumulador_erro;
  
  // --- CÁLCULO DA AÇÃO DE CONTROLE ---
  float delta_pulse = termo_P + termo_I + termo_D;

  int pulse_final = pulse_equilibrio + delta_pulse;

  // --- 5. SATURAÇÃO DO ATUADOR (Anti-Windup) ---
  // Limita o pulso final para a faixa válida do ESC.
  pulse_final = constrain(pulse_final, 1000, 1300);

  // --- 6. ATUAÇÃO ---
  motor_esc.writeMicroseconds(pulse_final);

  // --- 7. ATUALIZAÇÃO E DEBUG ---
  erro_anterior = erro;

  // Imprime os dados no Serial Plotter
  Serial.print("Angulo: "); Serial.print(angulo_atual);
  Serial.print("  Erro: "); Serial.print(erro);
  Serial.print("  Delta: "); Serial.print(delta_pulse);
  Serial.print("  Pulso: "); Serial.println(pulse_final);

  delay(10); 
}

float lerAngulo() {
  int valor_adc = analogRead(PINO_POTENCIOMETRO);
  float angulo = map(valor_adc, POT_MIN, POT_MAX, ANGULO_MAX, ANGULO_MIN);
  return constrain(angulo, ANGULO_MIN, ANGULO_MAX);
}