%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function data = createData()

    %% Labels
    XMN = 43;
    MVN = 26;

    data.xmvs.titles    = cell(1,MVN);
    data.xmvs.xlabels   = cell(1,MVN);
    data.xmvs.ylabels   = cell(1,MVN);
    data.xmvs.steadys   = cell(1,MVN);
    data.xmvs.mins      = cell(1,MVN);
    data.xmvs.maxs      = cell(1,MVN);
    
    data.xmeas.titles   = cell(1,XMN);
    data.xmeas.xlabels  = cell(1,XMN);
    data.xmeas.ylabels  = cell(1,XMN);
    data.xmeas.steadys  = cell(1,XMN);
    data.xmeas.mins     = cell(1,XMN);
    data.xmeas.maxs     = cell(1,XMN);
    
    data.xmvs.titles{1}  = 'Fresh O2 Feed';
    data.xmvs.xlabels{1} = 'Hours';
    data.xmvs.ylabels{1} = 'Kmol/min';
    data.xmvs.steadys{1} = 0.52343;
    data.xmvs.mins{1}    = 0;
    data.xmvs.maxs{1}    = 2.268;
    
    data.xmvs.titles{2}  = 'Fresh C2H4 Feed';
    data.xmvs.xlabels{2} = 'Hours';
    data.xmvs.ylabels{2} = 'Kmol/min';
    data.xmvs.steadys{2} = 0.83522;
    data.xmvs.mins{2}    = 0;
    data.xmvs.maxs{2}    = 7.56;
    
    data.xmvs.titles{3}  = 'Fresh HAc Feed';
    data.xmvs.xlabels{3} = 'Hours';
    data.xmvs.ylabels{3} = 'Kmol/min';
    data.xmvs.steadys{3} = 0.79003;
    data.xmvs.mins{3}    = 0;
    data.xmvs.maxs{3}    = 4.536;
    
    data.xmvs.titles{4}  = 'Vaporizer Steam Duty';
    data.xmvs.xlabels{4} = 'Hours';
    data.xmvs.ylabels{4} = 'Kcal/min';
    data.xmvs.steadys{4} = 21877;
    data.xmvs.mins{4}    = 0;
    data.xmvs.maxs{4}    = 1433400;
    
    data.xmvs.titles{5}  = 'Vaporizer Vapor Exit';
    data.xmvs.xlabels{5} = 'Hours';
    data.xmvs.ylabels{5} = 'Kmol/min';
    data.xmvs.steadys{5} = 18.728;
    data.xmvs.mins{5}    = 0;
    data.xmvs.maxs{5}    = 50;
    
    data.xmvs.titles{6}  = 'Vaporizer Heater Duty';
    data.xmvs.xlabels{6} = 'Hours';
    data.xmvs.ylabels{6} = 'Kcal/min';
    data.xmvs.steadys{6} = 9008.54;
    data.xmvs.mins{6}    = 0;
    data.xmvs.maxs{6}    = 15000;
    
    data.xmvs.titles{7}  = 'Reactor Shell Temp.';
    data.xmvs.xlabels{7} = 'Hours';
    data.xmvs.ylabels{7} = 'Deg C';
    data.xmvs.steadys{7} = 135.02;
    data.xmvs.mins{7}    = 110;
    data.xmvs.maxs{7}    = 150;
    
    data.xmvs.titles{8}  = 'Separator Liquid Exit';
    data.xmvs.xlabels{8} = 'Hours';
    data.xmvs.ylabels{8} = 'Kmol/min';
    data.xmvs.steadys{8} = 2.7544;
    data.xmvs.mins{8}    = 0;
    data.xmvs.maxs{8}    = 4.536;
    
    data.xmvs.titles{9}  = 'Separator Jacket Temp.';
    data.xmvs.xlabels{9} = 'Hours';
    data.xmvs.ylabels{9} = 'Deg C';
    data.xmvs.steadys{9} = 36.001;
    data.xmvs.mins{9}    = 0;
    data.xmvs.maxs{9}    = 80;
    
    data.xmvs.titles{10}  = 'Separator Vapor Exit';
    data.xmvs.xlabels{10} = 'Hours';
    data.xmvs.ylabels{10} = 'Kmol/min';
    data.xmvs.steadys{10} = 16.1026;
    data.xmvs.mins{10}    = 0;
    data.xmvs.maxs{10}    = 30;
    
    data.xmvs.titles{11}  = 'Compressor Heater Duty';
    data.xmvs.xlabels{11} = 'Hours';
    data.xmvs.ylabels{11} = 'Kcal/min';
    data.xmvs.steadys{11} = 27192;
    data.xmvs.mins{11}    = 0;
    data.xmvs.maxs{11}    = 50000;
    
    data.xmvs.titles{12}  = 'Absorber Liquid Exit';
    data.xmvs.xlabels{12} = 'Hours';
    data.xmvs.ylabels{12} = 'Kmol/min';
    data.xmvs.steadys{12} = 1.2137;
    data.xmvs.mins{12}    = 0;
    data.xmvs.maxs{12}    = 4.536;
    
    data.xmvs.titles{13}  = 'Absorber Circulation Flow';
    data.xmvs.xlabels{13} = 'Hours';
    data.xmvs.ylabels{13} = 'Kmol/min';
    data.xmvs.steadys{13} = 15.1198;
    data.xmvs.mins{13}    = 0;
    data.xmvs.maxs{13}    = 50;
    
    data.xmvs.titles{14}  = 'Circulation Cooler Duty';
    data.xmvs.xlabels{14} = 'Hours';
    data.xmvs.ylabels{14} = 'Kcal/min';
    data.xmvs.steadys{14} = 10730;
    data.xmvs.mins{14}    = 0;
    data.xmvs.maxs{14}    = 30000;
    
    data.xmvs.titles{15}  = 'Absorber Scrub Flow';
    data.xmvs.xlabels{15} = 'Hours';
    data.xmvs.ylabels{15} = 'Kmol/min';
    data.xmvs.steadys{15} = 0.756;
    data.xmvs.mins{15}    = 0;
    data.xmvs.maxs{15}    = 7.560;
    
    data.xmvs.titles{16}  = 'Scrub Cooler Duty';
    data.xmvs.xlabels{16} = 'Hours';
    data.xmvs.ylabels{16} = 'Kcal/min';
    data.xmvs.steadys{16} = 2018.43;
    data.xmvs.mins{16}    = 0;
    data.xmvs.maxs{16}    = 5000;
    
    data.xmvs.titles{17}  = 'CO2 Removal Inlet';
    data.xmvs.xlabels{17} = 'Hours';
    data.xmvs.ylabels{17} = 'Kmol/min';
    data.xmvs.steadys{17} = 6.5531;
    data.xmvs.mins{17}    = 0;
    data.xmvs.maxs{17}    = 22.68;
    
    data.xmvs.titles{18}  = 'Purge';
    data.xmvs.xlabels{18} = 'Hours';
    data.xmvs.ylabels{18} = 'Kmol/min';
    data.xmvs.steadys{18} = 0.003157;
    data.xmvs.mins{18}    = 0;
    data.xmvs.maxs{18}    = 0.02268;
    
    data.xmvs.titles{19}  = 'FEHE Bypass Ratio';
    data.xmvs.xlabels{19} = 'Hours';
    data.xmvs.ylabels{19} = '-';
    data.xmvs.steadys{19} = 0.31303;
    data.xmvs.mins{19}    = 0;
    data.xmvs.maxs{19}    = 1;
    
    data.xmvs.titles{20}  = 'Column Reflux';
    data.xmvs.xlabels{20} = 'Hours';
    data.xmvs.ylabels{20} = 'Kmol/min';
    data.xmvs.steadys{20} = 4.9849;
    data.xmvs.mins{20}    = 0;
    data.xmvs.maxs{20}    = 7.56;
    
    data.xmvs.titles{21}  = 'Column Reboiler Duty';
    data.xmvs.xlabels{21} = 'Hours';
    data.xmvs.ylabels{21} = 'Kcal/min';
    data.xmvs.steadys{21} = 67179;
    data.xmvs.mins{21}    = 0;
    data.xmvs.maxs{21}    = 100000;
    
    data.xmvs.titles{22}  = 'Column Condenser Duty';
    data.xmvs.xlabels{22} = 'Hours';
    data.xmvs.ylabels{22} = 'Kcal/min';
    data.xmvs.steadys{22} = 60367;
    data.xmvs.mins{22}    = 0;
    data.xmvs.maxs{22}    = 150000;
    
    data.xmvs.titles{23}  = 'Column Organic Exit';
    data.xmvs.xlabels{23} = 'Hours';
    data.xmvs.ylabels{23} = 'Kmol/min';
    data.xmvs.steadys{23} = 0.8290;
    data.xmvs.mins{23}    = 0;
    data.xmvs.maxs{23}    = 2.4;
    
    data.xmvs.titles{24}  = 'Column Aqueous Exit';
    data.xmvs.xlabels{24} = 'Hours';
    data.xmvs.ylabels{24} = 'Kmol/min';
    data.xmvs.steadys{24} = 0.8361;
    data.xmvs.mins{24}    = 0;
    data.xmvs.maxs{24}    = 2.4;
    
    data.xmvs.titles{25}  = 'Column Bottom Exit';
    data.xmvs.xlabels{25} = 'Hours';
    data.xmvs.ylabels{25} = 'Kmol/min';
    data.xmvs.steadys{25} = 2.1584;
    data.xmvs.mins{25}    = 0;
    data.xmvs.maxs{25}    = 4.536;
    
    data.xmvs.titles{26}  = 'Vaporizer Liquid Inlet';
    data.xmvs.xlabels{26} = 'Hours';
    data.xmvs.ylabels{26} = 'Kmol/min';
    data.xmvs.steadys{26} = 2.1924;
    data.xmvs.mins{26}    = 0;
    data.xmvs.maxs{26}    = 4.536;
    
    data.xmeas.titles{1}  = 'Vaporizer Pressure';
    data.xmeas.xlabels{1} = 'Hours';
    data.xmeas.ylabels{1} = 'Psia';
    data.xmeas.steadys{1} = 128;
    
    data.xmeas.titles{2}  = 'Vaporizer Level';
    data.xmeas.xlabels{2} = 'Hours';
    data.xmeas.ylabels{2} = '-';
    data.xmeas.steadys{2} = 0.7;
    
    data.xmeas.titles{3}  = 'Vaporizer Temperature';
    data.xmeas.xlabels{3} = 'Hours';
    data.xmeas.ylabels{3} = 'Deg C';
    data.xmeas.steadys{3} = 119.145;
    
    data.xmeas.titles{4}  = 'Heater Exit Temperature';
    data.xmeas.xlabels{4} = 'Hours';
    data.xmeas.ylabels{4} = 'Deg C';
    data.xmeas.steadys{4} = 150;
    
    data.xmeas.titles{5}  = 'Reactor Exit Temperature';
    data.xmeas.xlabels{5} = 'Hours';
    data.xmeas.ylabels{5} = 'Deg C';
    data.xmeas.steadys{5} = 159.17;
    
    data.xmeas.titles{6}  = 'Reactor Exit Flowrate';
    data.xmeas.xlabels{6} = 'Hours';
    data.xmeas.ylabels{6} = 'Kmol/min';
    data.xmeas.steadys{6} = 18.857;
    
    data.xmeas.titles{7}  = 'FEHE Cold Exit Temperature';
    data.xmeas.xlabels{7} = 'Hours';
    data.xmeas.ylabels{7} = 'Deg C';
    data.xmeas.steadys{7} = 97.1;
    
    data.xmeas.titles{8}  = 'FEHE Hot Exit Temperature';
    data.xmeas.xlabels{8} = 'Hours';
    data.xmeas.ylabels{8} = 'Deg C';
    data.xmeas.steadys{8} = 134;
    
    data.xmeas.titles{9}  = 'Separator Level';
    data.xmeas.xlabels{9} = 'Hours';
    data.xmeas.ylabels{9} = '-';
    data.xmeas.steadys{9} = 0.5;
    
    data.xmeas.titles{10}  = 'Separator Temperature';
    data.xmeas.xlabels{10} = 'Hours';
    data.xmeas.ylabels{10} = 'Deg C';
    data.xmeas.steadys{10} = 40;
    
    data.xmeas.titles{11}  = 'Compressor Exit Temperature';
    data.xmeas.xlabels{11} = 'Hours';
    data.xmeas.ylabels{11} = 'Deg C';
    data.xmeas.steadys{11} = 80;
    
    data.xmeas.titles{12}  = 'Absorber Pressure';
    data.xmeas.xlabels{12} = 'Hours';
    data.xmeas.ylabels{12} = 'Psia';
    data.xmeas.steadys{12} = 128;
    
    data.xmeas.titles{13}  = 'Absorber Level';
    data.xmeas.xlabels{13} = 'Hours';
    data.xmeas.ylabels{13} = '-';
    data.xmeas.steadys{13} = 0.5;
    
    data.xmeas.titles{14}  = 'Circulation Cooler Exit Temperature';
    data.xmeas.xlabels{14} = 'Hours';
    data.xmeas.ylabels{14} = 'Deg C';
    data.xmeas.steadys{14} = 25;
    
    data.xmeas.titles{15}  = 'Scrub Cooler Exit Temperature';
    data.xmeas.xlabels{15} = 'Hours';
    data.xmeas.ylabels{15} = 'Deg C';
    data.xmeas.steadys{15} = 25;
    
    data.xmeas.titles{16}  = 'Gas Recycle Flowrate';
    data.xmeas.xlabels{16} = 'Hours';
    data.xmeas.ylabels{16} = 'Kmol/min';
    data.xmeas.steadys{16} = 16.5359;
    
    data.xmeas.titles{17}  = 'Organic Product Flowrate';
    data.xmeas.xlabels{17} = 'Hours';
    data.xmeas.ylabels{17} = 'Kmol/min';
    data.xmeas.steadys{17} = 0.829;
    
    data.xmeas.titles{18}  = 'Decanter Level (Organic)';
    data.xmeas.xlabels{18} = 'Hours';
    data.xmeas.ylabels{18} = '-';
    data.xmeas.steadys{18} = 0.5;
    
    data.xmeas.titles{19}  = 'Decanter Level (Aqueous)';
    data.xmeas.xlabels{19} = 'Hours';
    data.xmeas.ylabels{19} = '-';
    data.xmeas.steadys{19} = 0.5;
    
    data.xmeas.titles{20}  = 'Decanter Temperature';
    data.xmeas.xlabels{20} = 'Hours';
    data.xmeas.ylabels{20} = 'Deg C';
    data.xmeas.steadys{20} = 45.845;
    
    data.xmeas.titles{21}  = 'Column Bottom Level';
    data.xmeas.xlabels{21} = 'Hours';
    data.xmeas.ylabels{21} = '-';
    data.xmeas.steadys{21} = 0.5;
    
    data.xmeas.titles{22}  = '5th Tray Temperature';
    data.xmeas.xlabels{22} = 'Hours';
    data.xmeas.ylabels{22} = 'Deg C';
    data.xmeas.steadys{22} = 110;
    
    data.xmeas.titles{23}  = 'HAc Tank Level';
    data.xmeas.xlabels{23} = 'Hours';
    data.xmeas.ylabels{23} = '-';
    data.xmeas.steadys{23} = 0.5;
    
    data.xmeas.titles{24}  = 'Organic Product Composition (VAc)';
    data.xmeas.xlabels{24} = 'Hours';
    data.xmeas.ylabels{24} = '%mol';
    data.xmeas.steadys{24} = 0.949786;
    
    data.xmeas.titles{25}  = 'Organic Product Composition (H2O)';
    data.xmeas.xlabels{25} = 'Hours';
    data.xmeas.ylabels{25} = '%mol';
    data.xmeas.steadys{25} = 0.049862;
    
    data.xmeas.titles{26}  = 'Organic Product Composition (HAc)';
    data.xmeas.xlabels{26} = 'Hours';
    data.xmeas.ylabels{26} = '%mol';
    data.xmeas.steadys{26} = 0.000352;
    
    data.xmeas.titles{27}  = 'Column Bottom Composition(VAc)';
    data.xmeas.xlabels{27} = 'Hours';
    data.xmeas.ylabels{27} = '%mol';
    data.xmeas.steadys{27} = 0.000010;
    
    data.xmeas.titles{28}  = 'Column Bottom Composition(H2O)';
    data.xmeas.xlabels{28} = 'Hours';
    data.xmeas.ylabels{28} = '%mol';
    data.xmeas.steadys{28} = 0.093440;
    
    data.xmeas.titles{29}  = 'Column Bottom Composition(HAc)';
    data.xmeas.xlabels{29} = 'Hours';
    data.xmeas.ylabels{29} = '%mol';
    data.xmeas.steadys{29} = 0.906550;
    
    data.xmeas.titles{30}  = 'Gas Recycle Composition (O2)';
    data.xmeas.xlabels{30} = 'Hours';
    data.xmeas.ylabels{30} = '%mol';
    data.xmeas.steadys{30} = 0.055664;
    
    data.xmeas.titles{31}  = 'Gas Recycle Composition (CO2)';
    data.xmeas.xlabels{31} = 'Hours';
    data.xmeas.ylabels{31} = '%mol';
    data.xmeas.steadys{31} = 0.007304;
    
    data.xmeas.titles{32}  = 'Gas Recycle Composition (C2H4)';
    data.xmeas.xlabels{32} = 'Hours';
    data.xmeas.ylabels{32} = '%mol';
    data.xmeas.steadys{32} = 0.681208;
    
    data.xmeas.titles{33}  = 'Gas Recycle Composition (C2H6)';
    data.xmeas.xlabels{33} = 'Hours';
    data.xmeas.ylabels{33} = '%mol';
    data.xmeas.steadys{33} = 0.249191;
    
    data.xmeas.titles{34}  = 'Gas Recycle Composition (VAc)';
    data.xmeas.xlabels{34} = 'Hours';
    data.xmeas.ylabels{34} = '%mol';
    data.xmeas.steadys{34} = 0.001597;
    
    data.xmeas.titles{35}  = 'Gas Recycle Composition (H2O)';
    data.xmeas.xlabels{35} = 'Hours';
    data.xmeas.ylabels{35} = '%mol';
    data.xmeas.steadys{35} = 0.000894;
    
    data.xmeas.titles{36}  = 'Gas Recycle Composition (HAc)';
    data.xmeas.xlabels{36} = 'Hours';
    data.xmeas.ylabels{36} = '%mol';
    data.xmeas.steadys{36} = 0.004142;
    
    data.xmeas.titles{37}  = 'Reactor Feed Composition (O2)';
    data.xmeas.xlabels{37} = 'Hours';
    data.xmeas.ylabels{37} = '%mol';
    data.xmeas.steadys{37} = 0.075;
    
    data.xmeas.titles{38}  = 'Reactor Feed Composition (CO2)';
    data.xmeas.xlabels{38} = 'Hours';
    data.xmeas.ylabels{38} = '%mol';
    data.xmeas.steadys{38} = 0.006273;
    
    data.xmeas.titles{39}  = 'Reactor Feed Composition (C2H4)';
    data.xmeas.xlabels{39} = 'Hours';
    data.xmeas.ylabels{39} = '%mol';
    data.xmeas.steadys{39} = 0.585110;
    
    data.xmeas.titles{40}  = 'Reactor Feed Composition (C2H6)';
    data.xmeas.xlabels{40} = 'Hours';
    data.xmeas.ylabels{40} = '%mol';
    data.xmeas.steadys{40} = 0.214038;
    
    data.xmeas.titles{41}  = 'Reactor Feed Composition (VAc)';
    data.xmeas.xlabels{41} = 'Hours';
    data.xmeas.ylabels{41} = '%mol';
    data.xmeas.steadys{41} = 0.001373;
    
    data.xmeas.titles{42}  = 'Reactor Feed Composition (H2O)';
    data.xmeas.xlabels{42} = 'Hours';
    data.xmeas.ylabels{42} = '%mol';
    data.xmeas.steadys{42} = 0.008558;
    
    data.xmeas.titles{43}  = 'Reactor Feed Composition (HAc)';
    data.xmeas.xlabels{43} = 'Hours';
    data.xmeas.ylabels{43} = '%mol';
    data.xmeas.steadys{43} = 0.109648;
    
    %% Values
    if evalin('base', 'exist(''tout'')')
        data.tout = evalin('base', 'tout');
    end
    if evalin('base', 'exist(''simout'')')
        data.xmeas.values = evalin('base', 'simout');
    end
    if evalin('base', 'exist(''tout'')')
        data.xmvs.values = evalin('base', 'xmv');
    end
end

