function result=iif(cond, iftrue, iffalse)
%IIF Summary of this function coming soon..
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

    if isscalar(cond) 
       if cond 
           result = iftrue;
       else
           result = iffalse;
       end
    else
      result = (cond).*iftrue + (~cond).*iffalse;
    end  
end