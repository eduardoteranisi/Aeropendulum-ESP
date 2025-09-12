pwm = 89;

K = 0.000166;

V = 11.1;

J = 0.020218; % inercia determinado experimentalmente

b = 0.010681; % coeficiente de atrito determinado experimentalmente

L1 = 0.177; % braco - centro de massa

L2 = 0.45; % braco - motor

m = 0.218; % massa

g = 9.81; % aceleracao da gravidade

%theta = 90 * (pi/180);

theta = 90;

%Funcao transferencia
num = [K * L2 * 2 * pwm / J];
den = [1, b/J, (m*g/J) * L1 * cos(theta)];
sys = tf(num, den);