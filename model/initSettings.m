function settings = initSettings()
%INITSETTINGS Summary of this function coming soon..
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

    settings.simulation.duration = 24; % hours
    settings.simulation.disturbances = zeros(1, 7);
    settings.simulation.callbacks.afterend = 'overview();';
end

