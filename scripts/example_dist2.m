%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
% model loading
if ~bdIsLoaded(model)
    load_system(model);
end

% remove all attacks
rmatks;

reset_disturb;
% set pause to 1 hour
set_pause(1);

% run simulation
set_param(bdroot,'SimulationCommand','start');

while ~strcmp(get_param(bdroot,'SimulationStatus'),'paused')
    pause(0.5);
end

set_disturb(2);
% set pause to 65 minutes
set_pause(65/60);
set_param(bdroot,'SimulationCommand','continue');

while ~strcmp(get_param(bdroot,'SimulationStatus'),'paused')
    pause(0.1);
end

reset_disturb;
reset_pause;
set_param(bdroot,'SimulationCommand','continue');
