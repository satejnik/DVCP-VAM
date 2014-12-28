classdef CustomOutput
%CUSTOMOUTPUT represents one of the possible attack modes.
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
    
    properties
        String
        Value
    end
    
    methods
        function this = CustomOutput(string, value)
            this.String = string;
            this.Value = value;
        end
    end
    
    methods(Static)
        function outputs = Outputs()
            enums = enumeration('CustomOutput');
            outputs = {enums.String}';
        end
        
        function output = FromValue(value)
            switch value
                case 1
                    output = CustomOutput.HOLDINGVALUE;
                case 2
                    output = CustomOutput.SETTINGZERO;
                case 3
                    output = CustomOutput.EXTRAPOLATION;
                case 4
                    output = CustomOutput.CYCLICREPETITION;
                otherwise
                    error('CustomOutput:FromValue', 'Bad value (%f).', value);
            end
        end
    end
    
    enumeration
        HOLDINGVALUE('Holding final value', 1)
        SETTINGZERO('Setting to zero', 2)
        EXTRAPOLATION('Extrapolation', 3)
        CYCLICREPETITION('Cyclic repetition', 4)
    end
end



