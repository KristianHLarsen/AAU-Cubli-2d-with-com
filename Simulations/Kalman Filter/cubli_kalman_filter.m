clc
close all
clear all

%% Load file
% 1. Timestamp - 2. Potentiometer angle - 3. Gyro speed of frame - 4. Acceleration in X - 5. Acceleration in Y - 6. Speed of wheel
data = dlmread('balance_data_3.csv', ';');
% data = csvread('balance_data_3.csv');

%% measurements
% 1. Angle of frame - 3. Gyro speed of frame - 4. Speed of wheel
z = transpose([(atan2(data(:,4), data(:,5)) - pi/4) , data(:,3), data(:,6)]);
pot = data(:,2)*pi/180;
filtered_ang_pos = data(:,7);
% plot(z(:,1))
% hold on
% plot(data(:,2)*pi/180)

%%-(((atan2(-AcY, -AcX) + PI) * RAD_TO_DEG) - 45);
%% Model parameter definitions

g = 9.816; % Gravitational acceleration [m/s^2]
m_w = 0.220; % Mass of the wheel [kg]
l_w = 0.092; % Distance between flywheel's COM and origin [m]
J_w = 0.699*10^(-3); % Inertia of the flyweheel [kg*m^2]
b_w = 1.93*10^(-5); % Damping coefficient for the flywheel [N*m*s/rad]
m_f = 0.766-m_w; % Mass of the frame [kg]
l_f = 0.0958; % Distance between frame COM and the origin [m]
J_f = 0.0067; % Inertia of the frame [kg*m^2]
b_f = (0.0036+0.0033+0.0038)/3; % Damping coefficient for the frame [N*m*s/rad]

%% State space model

A = zeros(3,3);
A(1,1) = 0;
A(1,2) = 1;
A(1,3) = 0;
A(2,1) = g*(m_f*l_f+m_w*l_w)/(J_f + m_w*(l_w)^2);
A(2,2) = -b_f/(J_f+m_w*(l_w)^2);
A(2,3) = b_w/(J_f+m_w*(l_w)^2);
A(3,1) = - g*(m_f*l_f+m_w*l_w)/(J_f+m_w*(l_w)^2);
A(3,2) = b_f/(J_f+m_w*(l_w)^2);
A(3,3) = -b_w*(J_w+J_f+m_w*(l_w)^2)/(J_w*(J_f+m_w*(l_w)^2));

B = zeros(3,1);
B(1) = 0;
B(2) = -1/(J_f+m_w*(l_w)^2);
B(3) = (J_w+J_f+m_w*(l_w)^2)/(J_w*(J_f+m_w*(l_w)^2));

u = data(:,7);

%%%%%%%%%%%%%%%%% KALMAN FILTER %%%%%%%%%%%%%%%%%%%%%

%%% initial guess of the state
x_prior(:,1) = [0 0 0];
x_post(:,1) = [0 0 0];

P_prior = eye(3);
P_post = eye(3);
H = eye(3);

%%% Covariance of the noise in the measurement
R = [0.0001 0 0;
        0 1 0;
        0 0 1];

%%% Covariance of the noise in the process
Q = [1 0 0;
        0 1 0;
        0 0 1];
    
Q = 0.0001*Q;


for i = 2:length(u)
    x_prior(:,i) = A*x_post(:,i-1) + B*u(i);
    P_prior = A*P_post*transpose(A) + Q;
    K = P_prior * transpose(H) * ...
           inv((H * P_prior * transpose(H) + R));
    
    x_post(:,i) = x_prior(:,i) + K*(z(:,i) - H*x_prior(:,i));
    P_post = P_prior - K*H*P_prior;
end

plot(x_post(1,:))
hold on
plot(filtered_ang_pos)
 