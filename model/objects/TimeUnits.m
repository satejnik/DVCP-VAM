%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
classdef TimeUnits
    
    properties
        String
        Value
    end
    
    methods
        function this = TimeUnits(string, value)
            this.String = string;
            this.Value = value;
        end
    end
    
    methods(Static)
        function units = Units()
            enums = enumeration('TimeUnits');
            units = {enums.String}';
        end
        
        function mode = FromValue(value)
            switch value
                case 1
                    mode = TimeUnits.HOURS;
                case 2
                    mode = TimeUnits.MINUTES;
                case 3
                    mode = TimeUnits.SECONDS;
                otherwise
                    error('TimeUnits:FromValue', 'Bad value (%f).', value);
            end
        end
    end
    
    enumeration
        HOURS('Hours', 1)
        MINUTES('Minutes', 2)
        SECONDS('Seconds', 3)
    end
end



