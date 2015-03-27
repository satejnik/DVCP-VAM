%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
classdef ControllerType
    
    properties
        String
        Value
    end
    
    methods
        function this = ControllerType(string, value)
            this.String = string;
            this.Value = value;
        end
    end
    
    methods(Static)
        function types = Types()
            enums = enumeration('ControllerType');
            types = {enums.String}';
        end
        
        function type = FromValue(value)
            switch value
                case 1
                    type = ControllerType.PI;
                case 2
                    type = ControllerType.P;
                case 3
                    type = ControllerType.FIXED;
                otherwise
                    error('ControllerType:FromValue', 'Bad value (%f).', value);
            end
        end
        
        function type = FromString(string)
            switch string
                case 'PI'
                    type = ControllerType.PI;
                case 'P'
                    type = ControllerType.P;
                case 'Fixed'
                    type = ControllerType.FIXED;
                otherwise
                    error('ControllerType:FromString', 'Bad value (%f).', string);
            end
        end
    end
    
    enumeration
        PI('PI', 1)
        P('P', 2)
        FIXED('Fixed', 3)
    end
end

