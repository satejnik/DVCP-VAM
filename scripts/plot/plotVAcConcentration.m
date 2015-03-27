%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

figure;
plot(tout, states(:,39:48));
title('VAc concentration');
xlabel('hours');
ylabel('Concentration');
legend('VAc Tube 1','VAc Tube 2', 'VAc Tube 3', 'VAc Tube 4', 'VAc Tube 5','VAc Tube 6','VAc Tube 7', 'VAc Tube 8','VAc Tube 9','VAc Tube 10');