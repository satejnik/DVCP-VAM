%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function set_link(block, status)

    if ~isa(status, 'LinkStatus')
        error('VAC:set_link', 'New link status parameter must be of type ''LinkStatus''.');
    end

    current = get_param(block, 'LinkStatus');
    
    if ~strcmpi(current, status.String)
        set_param(block, 'LinkStatus', status.String);
    end
end