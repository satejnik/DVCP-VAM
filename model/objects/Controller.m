classdef Controller < handle
%CONTROLLER Summary of this class coming soon..
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
    
    properties
        Name
        XmvBlock
        XmeasBlock
        SetPoint
        Type
        Gain
        ResetTime
    end
    
    methods
        function this = Controller(name, xmv, xmeas)
            if ~ischar(name)
               error('VAC:Controller:contructor', 'First input parameter must be a string.'); 
            end
            if ~isnumeric(xmv)
               error('VAC:Controller:contructor', 'Xmv number parameter must be a numeric value.'); 
            end
            if ~isnumeric(xmeas)
               error('VAC:Controller:contructor', 'Xmeas number parameter must be a numeric value.'); 
            end
            this.Reset();
            this.Name = name;
            this.XmvBlock = xmv;
            this.XmeasBlock = xmeas;
        end
        
        function set.Type(object, value)
            if ~isa(value, 'ControllerType')
                error('VAC:Controller:Type:setter', 'Value must be of type ControllerType');
            end
            object.Type = value;
        end
        
        function Reset(object)
            object.Type = ControllerType.PI;
            object.SetPoint = 0;
            object.Gain = 1;
            object.ResetTime = 0;
        end
        
        function Set(object, workspace)
            if nargin < 2
                workspace = 'base';
            end
            evalin(workspace, ['set_param([model ''/xmv' num2str(object.XmvBlock) '''],''' ControllerProperties.SETPOINT.String ''',''' num2str(object.SetPoint) ''');' ...
                               'set_param([model ''/VA Plant/' object.Name '''],''' ControllerProperties.TYPE.String ''',' num2str(object.Type.Value-1) ');' ...
                               'set_param([model ''/VA Plant/' object.Name '''],''' ControllerProperties.RESETTIME.String ''',''' num2str(object.ResetTime) ''');' ...
                               'set_param([model ''/VA Plant/' object.Name '''],''' ControllerProperties.GAIN.String ''',''' num2str(object.Gain) ''');']);
        end
    end
end

