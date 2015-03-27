%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
classdef LinkStatus
    
    properties
       String 
    end
    
    methods
       function self = LinkStatus(string)
            self.String = string;
        end 
    end
    
    enumeration
       NONE('none')
       INACTIVE('inactive')
       RESTORE('restore')
       PROPAGATE('propagate')
    end
    
end

