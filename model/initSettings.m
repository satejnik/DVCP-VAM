%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function settings = initSettings()

    settings.simulation.duration = 24; % hours
    settings.simulation.disturbances = zeros(1, 7);
    settings.simulation.constraints = ones(1, 8);
    settings.simulation.callbacks.afterend = 'overview();';
end

