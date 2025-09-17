K = 0.000166;

J = 0.020218; % inercia determinado experimentalmente

b = 0.010681; % coeficiente de atrito determinado experimentalmente

L1 = 0.3; % braco - centro de massa

L2 = 0.35; % braco - motor

m = 0.267; % massa

g = 9.81; % aceleracao da gravidade

theta = 90 * (pi/180);

pwm = 89;

%Funcao transferencia
num = [K * L2 * 2 * pwm / J];
den = [1, b/J, (m*g/J) * L1 * cos(theta)];
sys = tf(num, den);