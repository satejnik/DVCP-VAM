%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function controllers = initAttackControllers()

    XMVN = 26;
    XMSN = 43;
    
    for i=1:XMVN
        controllers.xmvs(i) = AttackController('xmv', i);
    end
    
    for i=1:XMSN
        controllers.xmeas(i) = AttackController('xmeas', i);
    end
end

