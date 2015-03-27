%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------
function result = runOptimalAttackValue(type, block, values, starts, durations, opts)

    if nargin < 6
        opts = struct('startTimeUnits',   TimeUnits.MINUTES, ...    % default
                      'durationTimeUnits', TimeUnits.SECONDS, ...   % default
                      'simTime', 24);                               % default
    end
    
    startTimeUnits = get_field(opts, 'startTimeUnits', 'TimeUnits', TimeUnits.MINUTES);
    durationTimeUnits = get_field(opts, 'durationTimeUnits', 'TimeUnits', TimeUnits.SECONDS);
    simtime = get_field(opts, 'simTime', 'double', 24);
    
    starts = convertToHours(starts, startTimeUnits);
    durations = convertToHours(durations, durationTimeUnits);

    progressbar('Overall progress/Value scope..', 0, 'Color', 'r');
    cHandler = onCleanup(@onClosing);
    
    % model loading
    if ~bdIsLoaded(model)
        load_system(model);
    end
    
    warning('off','all');
    
    % remove callback temporarily
    callb = get_param(bdroot, 'StopFcn');
    set_param(bdroot, 'StopFcn', '');
    
    % set sim time
    set_param(bdroot, 'StopTime', mat2str(simtime));
    
    % attack controller
    controller = AttackController(type, block);
    controller.Type = AttackType.INTEGRITY;
    controller.Mode = AttackMode.PERIODIC;
    
    % compute normal costs
    rmatks;
    simout = sim(bdroot, 'SaveOutput', 'On');
    tout = simout.get('tout');
    xmeas = simout.get('simout');
    states = simout.get('states');
    clear simout;
    normal_costs = get_costs(states, xmeas);
    
    index = 0;
    for value = values
        rmatks();
        controller.Value = value;
        
        progressbar('Start time scope..', 0, 'Color', 'b');
        for start = starts
            rmatks;
            controller.Start = start;
            
            progressbar('Duration time scope..', 0, 'Color', 'g');
            for duration = durations
                rmatks;  
                controller.Duration = duration;

                controller.Set('caller');
                % run simulation without attacking
                simout = sim(bdroot, 'SaveOutput', 'On');
                tout = simout.get('tout');
                xmeas = simout.get('simout');
                states = simout.get('states');
                clear simout;
                
                index = index + 1;
                result(index).value = value; %#ok<*AGROW>
                result(index).start = start;
                result(index).duration = duration;
                result(index).errors = false;
                result(index).warnings = false;
                result(index).costs = 0;
                result(index).loss = 0;
                if simtime - tout(end) > .1e-15 % error occured % constraints 1,2 or 3 violated
                                           % floating points number comparison problem
                    result(index).errors = true;
                else 
                    result(index).costs = get_costs(states, xmeas);
                    result(index).loss = normal_costs - result(index).costs;
                    % any warnings occured / constraint 4 - 8 violated
                    warnings = false;
                    % Liquid levels in the vaporizer, separator, absorber base, column base, and decanter must operate
                    % within the limits of 10-90%.
                    if ~warnings && ...
                       (max(xmeas(:,2)) > 0.9 || min(xmeas(:,2)) < 0.1 || ...
                        max(xmeas(:,9)) > 0.9 || min(xmeas(:,9)) < 0.1 || ...
                        max(xmeas(:,13)) > 0.9 || min(xmeas(:,13)) < 0.1 || ...
                        max(xmeas(:,21)) > 0.9 || min(xmeas(:,21)) < 0.1 || ...
                        max(xmeas(:,18)) > 0.9 || min(xmeas(:,18)) < 0.1 || ...
                        max(xmeas(:,19)) > 0.9 || min(xmeas(:,19)) < 0.1)
                        warnings = true;
                    end
                    % Reactor feed temperature must be greater than 130C.
                    if ~warnings && min(states(:,69)) < 130
                        warnings = true;
                    end
                    % FEHE hot effluent temperature must be greater than 130 C.
                    if ~warnings && min(xmeas(:,8)) < 130
                        warnings = true;
                    end
                    % The HAc composition must not exceed 0.06 mol% in the decanter organic phase.
                    if ~warnings && max(xmeas(:,26)) > 0.0006
                        warnings = true;
                    end
                    % The VAc composition must not exceed 0.01 mol% in the column bottom.
                    if ~warnings && max(xmeas(:,27)) > 0.0001
                        warnings = true;
                    end
                    result(index).warnings = warnings;
                end
                
                % update progress bar
                progressbar('Duration time scope..', indexof(durations, duration)/length(durations));
            end      
            % update progress bar
            progressbar('Start time scope..', indexof(starts, start)/length(starts));
        end 
        
        % update progress bar
        progressbar('Overall progress/Value scope..', indexof(values, value)/length(values));
    end
    
    % clean up function handler
    function onClosing()
        %% restore previous settings
        if ~isempty(callb) %#ok<MOCUP>
            set_param(bdroot, 'StopFcn', callb);
        end
        bdclose;
        warning('on','all');
        progressbar('Overall progress/Value scope..', 'Close');
        progressbar('Start time scope..', 'Close');
        progressbar('Duration time scope..', 'Close');
    end
end

function hours = convertToHours(values, fromUnits)
    switch fromUnits
        case TimeUnits.MINUTES
            hours = values./60;
        case TimeUnits.SECONDS
            hours = values./3600;
        otherwise
            hours = values;
    end
end