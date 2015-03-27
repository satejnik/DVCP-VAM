%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function child = setupChildWindow(parent)

    child = figure( ...
        'CloseRequestFcn', {@onCloseRequested, parent}, ...
        'CreateFcn', {@onCreateRequested, parent} ... 
    );

    function onCloseRequested(h, ~, parent)    
        delete(h);
        enableDisableFig(parent, true);
        figure(parent);
    end

    function onCreateRequested(h, ~, parent)
        enableDisableFig(parent, false);
        figure(h);
    end 
end

