%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function controllers = initControllers()

    xmvs = createData();
    spoints = xmvs.xmeas.steadys;
    xmvs = xmvs.xmvs.titles;
    
    %CL1: XMEAS37 -> XMV1
    controllers(1) = Controller(xmvs{1}, 1, 37);
    controllers(1).Type = ControllerType.PI;
    controllers(1).Gain = 5;
    controllers(1).ResetTime = 10;
    controllers(1).SetPoint = spoints{37};
    
    %CL2: XMEAS12 -> XMV2
    controllers(2) = Controller(xmvs{2}, 2, 12);
    controllers(2).Type = ControllerType.PI;
    controllers(2).Gain = 0.3;
    controllers(2).ResetTime = 20;
    controllers(2).SetPoint = spoints{12};
    
    %CL3: XMEAS23 -> XMV3
    controllers(3) = Controller(xmvs{3}, 3, 23);
    controllers(3).Type = ControllerType.P;
    controllers(3).Gain = 2;
    controllers(3).ResetTime = 100;
    controllers(3).SetPoint = spoints{23};
    
    %CL4: XMEAS2 -> XMV4
    controllers(4) = Controller(xmvs{4}, 4, 2);
    controllers(4).Type = ControllerType.PI;
    controllers(4).Gain = -0.1;
    controllers(4).ResetTime = 30;
    controllers(4).SetPoint = spoints{2};
    
    %CL5: XMEAS1 -> XMV5
    controllers(5) = Controller(xmvs{5}, 5, 1);
    controllers(5).Type = ControllerType.PI;
    controllers(5).Gain = -5;
    controllers(5).ResetTime = 10;
    controllers(5).SetPoint = spoints{1};
    
    %CL6: XMEAS4 -> XMV6
    controllers(6) = Controller(xmvs{6}, 6, 4);
    controllers(6).Type = ControllerType.PI;
    controllers(6).Gain = 1;
    controllers(6).ResetTime = 5;
    controllers(6).SetPoint = spoints{4};
    
    %CL7: XMEAS5 -> XMV7
    controllers(7) = Controller(xmvs{7}, 7, 5);
    controllers(7).Type = ControllerType.PI;
    controllers(7).Gain = 3;
    controllers(7).ResetTime = 10;
    controllers(7).SetPoint = spoints{5};
    
    %CL8: XMEAS9 -> XMV8
    controllers(8) = Controller(xmvs{8}, 8, 9);
    controllers(8).Type = ControllerType.P;
    controllers(8).Gain = -5;
    controllers(8).ResetTime = 100;
    controllers(8).SetPoint = spoints{9};
    
    %CL9: XMEAS10 -> XMV9
    controllers(9) = Controller(xmvs{9}, 9, 10);
    controllers(9).Type = ControllerType.PI;
    controllers(9).Gain = -5;
    controllers(9).ResetTime = 20;
    controllers(9).SetPoint = spoints{10};
    
    %CL10: XMV10
    controllers(10) = Controller(xmvs{10}, 10, -1);
    controllers(10).Type = ControllerType.FIXED;
    controllers(10).Gain = 0;
    controllers(10).ResetTime = 0;
    controllers(10).SetPoint = 16.1026;
    
    %CL11: XMEAS11 -> XMV11
    controllers(11) = Controller(xmvs{11}, 11, 11);
    controllers(11).Type = ControllerType.PI;
    controllers(11).Gain = -1;
    controllers(11).ResetTime = 5;
    controllers(11).SetPoint = spoints{11};
    
    %CL12: XMEAS13 -> XMV12
    controllers(12) = Controller(xmvs{12}, 12, 13);
    controllers(12).Type = ControllerType.P;
    controllers(12).Gain = -5;
    controllers(12).ResetTime = 100;
    controllers(12).SetPoint = spoints{13};
    
    %CL13: XMV13
    controllers(13) = Controller(xmvs{13}, 13, -1);
    controllers(13).Type = ControllerType.FIXED;
    controllers(13).Gain = 0;
    controllers(13).ResetTime = 0;
    controllers(13).SetPoint = 15.1198;
    
    %CL14: XMEAS14 -> XMV14
    controllers(14) = Controller(xmvs{14}, 14, 14);
    controllers(14).Type = ControllerType.PI;
    controllers(14).Gain = -1;
    controllers(14).ResetTime = 5;
    controllers(14).SetPoint = spoints{14};
    
    %CL15: XMV15
    controllers(15) = Controller(xmvs{15}, 15, -1);
    controllers(15).Type = ControllerType.FIXED;
    controllers(15).Gain = 0;
    controllers(15).ResetTime = 0;
    controllers(15).SetPoint = 0.756;
    
    %CL16: XMEAS15 -> XMV16
    controllers(16) = Controller(xmvs{16}, 16, 15);
    controllers(16).Type = ControllerType.PI;
    controllers(16).Gain = -1;
    controllers(16).ResetTime = 5;
    controllers(16).SetPoint = spoints{15};
    
    %CL17: XMEAS31 -> XMV17
    controllers(17) = Controller(xmvs{17}, 17, 31);
    controllers(17).Type = ControllerType.P;
    controllers(17).Gain = -1;
    controllers(17).ResetTime = 100;
    controllers(17).SetPoint = spoints{31};
    
    %CL18: XMEAS33 -> XMV18
    controllers(18) = Controller(xmvs{18}, 18, 33);
    controllers(18).Type = ControllerType.P;
    controllers(18).Gain = -1;
    controllers(18).ResetTime = 100;
    controllers(18).SetPoint = spoints{33};
    
    %CL19: XMEAS8 -> XMV19
    controllers(19) = Controller(xmvs{19}, 19, 8);
    controllers(19).Type = ControllerType.PI;
    controllers(19).Gain = 5;
    controllers(19).ResetTime = 10;
    controllers(19).SetPoint = spoints{8};
    
    %CL20: XMEAS28 -> XMV20
    controllers(20) = Controller(xmvs{20}, 20, 28);
    controllers(20).Type = ControllerType.PI;
    controllers(20).Gain = -0.5;
    controllers(20).ResetTime = 60;
    controllers(20).SetPoint = spoints{28};
    
    %CL21: XMEAS22 -> XMV21
    controllers(21) = Controller(xmvs{21}, 21, 22);
    controllers(21).Type = ControllerType.PI;
    controllers(21).Gain = 20;
    controllers(21).ResetTime = 30;
    controllers(21).SetPoint = spoints{22};
    
    %CL22: XMEAS20 -> XMV22
    controllers(22) = Controller(xmvs{22}, 22, 20);
    controllers(22).Type = ControllerType.PI;
    controllers(22).Gain = -1;
    controllers(22).ResetTime = 5;
    controllers(22).SetPoint = spoints{20};
    
    %CL23: XMEAS18 -> XMV23
    controllers(23) = Controller(xmvs{23}, 23, 18);
    controllers(23).Type = ControllerType.P;
    controllers(23).Gain = -1;
    controllers(23).ResetTime = 100;
    controllers(23).SetPoint = spoints{18};
    
    %CL24: XMEAS19 -> XMV24
    controllers(24) = Controller(xmvs{24}, 24, 19);
    controllers(24).Type = ControllerType.P;
    controllers(24).Gain = -1;
    controllers(24).ResetTime = 100;
    controllers(24).SetPoint = spoints{19};
    
    %CL25: XMEAS21 -> XMV25
    controllers(25) = Controller(xmvs{25}, 25, 21);
    controllers(25).Type = ControllerType.P;
    controllers(25).Gain = -1;
    controllers(25).ResetTime = 100;
    controllers(25).SetPoint = spoints{21};
    
    %CL26: XMV26
    controllers(26) = Controller(xmvs{26}, 26, -1);
    controllers(26).Type = ControllerType.FIXED;
    controllers(26).Gain = 0;
    controllers(26).ResetTime = 0;
    controllers(26).SetPoint = 2.1924;
end
