%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function set_constraints(input)

    if ischar(input)
        set_param([model '/Constraints'], 'Value', input);
        return;
    end

    if isnumeric(input) && length(input) < 2
        disturbances = zeros(1, 8);
        disturbances(input) = 1;
        set_param([model '/Constraints'], 'Value', mat2str(disturbances));
        return;
    end
    
    if ~isvector(input)
        error('Bad input parameter.');
    end
    
    if size(input, 2) == 1
        input = input';
    end
    
    set_param([model '/Constraints'], 'Value', mat2str(input));
end

