%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
type = AttackController.XMV; % xmeas
%type = AttackController.XMV; % xmv

%{
    block number of attacked controlling block.
    xmeas   -> 1-43
    xmv     -> 1-26
%}
block = 3;

% attack value limits with step
value_from = 10;
value_to = 11;
value_step = 1;

% attack start time limits with step
start_from = 30;
start_to = 31;
start_step = 1;

% attack duration time limits with step
duration_from = 30;
duration_to = 30;
duration_step = 1;

% optional
opts = struct('startTimeUnits',   TimeUnits.MINUTES, ...    % default
              'durationTimeUnits', TimeUnits.MINUTES, ...   % default
              'simTime', 24);                               % default

result = runOptimalAttackValue(type, ...
                               block, ...
                               value_from:value_step:value_to, ...
                               start_from:start_step:start_to, ...
                               duration_from:duration_step:duration_to, ...
                               opts);
                           
max_index = indexof([result.loss], max([result.loss]));
sprintf('Optimal values for max loss (index: %d):\nValue: %0.2f\nStart: %0.2fh\nDuration: %0.2fh\nLoss: %0.2f$', ...
                                      max_index, result(max_index).value, result(max_index).start, ...
                                      result(max_index).duration, result(max_index).loss)