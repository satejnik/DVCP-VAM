function controllers = initAttackControllers()
%INITATTACKCONTROLERS Summary of this function coming soon..
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

    XMVN = 26;
    XMSN = 43;
    
    for i=1:XMVN
        controllers.xmvs(i) = AttackController('xmv', i);
    end
    
    for i=1:XMSN
        controllers.xmeas(i) = AttackController('xmeas', i);
    end
end

