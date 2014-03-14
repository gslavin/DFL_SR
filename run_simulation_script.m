% 2-D simulation for soft robotics

% model of 100 points
MODEL_SIZE = 10;
SPRING_CONSTANT = 1;
PRESSURE_VALUE = 1;
AREA = 1;
% proportionof the Net force that that will be used to calculate distance
% moved
FORCE_FACTOR = 0.05;

%globals:
%    point matrix
%    point matrix updated
% at the end of every state update point_matrix = point matrix updated
global point_matrix;
global point_matrix_updated;

% MODEL_SIZE number of groups
% each group:
%   index to point and three neighbor points
%   (with spring constant for each point)
%   [ index spring_constant ] (base point has spring constant of zero)
%        point = [ pos ]
%        pos = [ x_pos y_pos z_pos]

% model consists of array of points, array of forces
% and array of spring constants
% points are equally spaced on the unit circle
a = 2*pi/MODEL_SIZE:(2*pi/MODEL_SIZE):2*pi;
normal = (1/sqrt(2)).*ones(MODEL_SIZE,2);
pressure = PRESSURE_VALUE*normal;
net_force = pressure.*AREA;
points = [cos(a') sin(a') normal];


run_simulation(MODEL_SIZE,...
                PRESSURE_VALLUE, AREA, FORCE_FACTOR)