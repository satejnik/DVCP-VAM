%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function costs = get_costs(states, simout)
% @param states 			Reference simulation output: states
% @param simout 			Reference simulation output: simout
% @param time					Simulation time in hours.

    components = zeros(1, 7);
	j = 1;
	for i = 1:180:size(states, 1)
		% read out component amounts and sum up
		components(1) = states(i,18);
		components(2) = states(i,28);
		components(3) = states(i,38);
		components(4) = states(i,256);
		components(5) = states(i,48);
		components(6) = states(i,58);
		components(7) = states(i,68);
		total = sum(components);
		
		% compute molar fractions
		components(1) = (components(1) / total);
		components(2) = (components(2) / total);
		components(3) = (components(3) / total);
		components(4) = (components(4) / total);
		components(5) = (components(5) / total);
		components(6) = (components(6) / total);
		components(7) = (components(7) / total);
		
		% read out reactor outflow rate and compute absolute outflow
		flowrate = simout(i,6);
		comp_VAc_out(j) = components(5) * flowrate; %#ok<AGROW>
		
		j = j + 1;
	end
	
	% compute product loss
	total = sum(comp_VAc_out);
	costs = total * 86.09 * 0.971;	
end
