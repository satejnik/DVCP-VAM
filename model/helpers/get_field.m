%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function value = get_field(struct, field, type, default)

    if nargin > 3
        assert(isa(default, type), ['Default value type mismatch for ', field, ', should be of type ', type, '!']);
    end
        
    if isstruct(struct) && isfield(struct, field)
        assert(isa(struct.(field), type), ['Type mismatch for parameter ' field ', should be of type ' type '.']);
        value = struct.(field);
    else
        value = default;
    end
end

