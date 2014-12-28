%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
% get the base operation
[~, xstates, MVs, y_ss]=VAModelb(zeros(246,1), zeros(26,1), 0, 1, zeros(1, 7));

%-----------------------------------------------------------------------------
% Implement Luyben and Tyreus's control structure for the VA plant
% Note that: in this example,  
%   1. for each transmitter, a 3 second time lag is used
%   2. for two analyzer transmitters (on the gas recycle and column bottom), a 10 minute deadtime is used
%   3. for other transmitters, no deadtime is used
%   4. for three controllers using analyzers on the gas recycle and column bottom, a 10 minute sampling time is used
%   5. for other controllers, a 1 second sampling time is used
%   6. for each transmitter, a 1 second sampling time is used
%-----------------------------------------------------------------------------
transmitter_lag=0.05; %in minute
transmitter_deadtime=[0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 10 10 0 10 0 0 0 0 0 0]; %in minute
transmitter_sampling_frequency=[60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60];
controller_sampling_frequency=[60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 0.1 0.1 60 0.1 60 60 60 60 60 60];

% TS_base is the integration step size in hours.  The simulink model
% is using a fixed step size (Euler) method.
%Ts_base=9.2593e-05;
Ts_base=1/(180*60);% 1/3 sec step
% TS_save is the sampling period for saving results.  The following
% variables are saved at the end of a run:
% tout    -  the elapsed time (hrs), length N.
% simout  -  the TE plant outputs, N by 41 matrix
% xmv     -  the TE plant manipulated variables, N by 12 matrix
% idv     -  the TE plant disturbances, N by 20 matrix
...
Ts_save=Ts_base;
