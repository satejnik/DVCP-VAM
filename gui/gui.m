function gui(varargin)
%GUI Summary of this function coming soon..
%   Detailed explanation coming soon..
%
%	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

    %args = varargin;
    data = createData();
    data.attackcontrollers = initAttackControllers();
    data.controllers = initControllers();
    data.settings = initSettings();
    interface = createInterface();
    updateInterface();
    
    function gui = createInterface()
        % Open a window and add some menus
        gui.window = figure( ...
            'Name', 'Simulation', ...
            'NumberTitle', 'off', ...
            'MenuBar', 'none', ...
            'Toolbar', 'none', ...
            'OuterPosition', [50,50,1000,700]);
        fullscreen(gui.window);
                
        %menu
        gui.menu.file = uimenu(gui.window, 'Label', 'File');
        uimenu(gui.menu.file, 'Label', 'Exit', 'Callback', @onExit);
        function onExit( ~, ~ )
            close(interface.window);
        end % onExit
        
        gui.menu.edit = uimenu(gui.window, 'Label', 'Edit');
        uimenu(gui.menu.edit, 'Label', 'Settings', 'Callback', @onSettings);
        function onSettings( ~, ~ )
            settingsWindow = setupChildWindow(gui.window);
            settings(settingsWindow, data.settings, @callback);
        end % onSettings
        
        function callback(settings)
            data.settings = settings;
        end
        
        layout = uiextras.Grid('Parent', gui.window, ...
                             'Spacing', 8, ...
                             'Padding', 6);

        %tabs        
        gui.tabs = uiextras.TabPanel('Parent', layout);
        gui = createCVs(gui, gui.tabs);
        gui = createXMs(gui, gui.tabs);
        gui = createCLoops(gui, gui.tabs);
        gui.tabs.TabNames = {'XMV', 'XMEAS', 'CLOOPS'};
        gui.tabs.SelectedChild = 1;
        gui = createControls(gui, layout);
        set(layout, 'ColumnSizes', [-3 -1], 'RowSizes', [-1 50]);
        
        
        function gui = createCVs(gui, parent)
            gui.xmvs.table = uitable('Parent', parent, ...
                'Data', [data.xmvs.titles', data.xmvs.steadys', data.xmvs.mins', data.xmvs.maxs', data.xmvs.ylabels'], ...
                'ColumnName', {'Description', 'Steady State', 'Min', 'Max', 'Unit'}, ...
                'ColumnFormat', {'char', 'char', 'char', 'char', 'char'}, ...
                'CellSelectionCallback', @onCellSelect);
            gui.xmvs.selected = 1;

            function onCellSelect(~, args)
                if any(args.Indices)
                    if isfield(interface.xmeas, 'selected')
                        interface.xmeas = rmfield(interface.xmeas, 'selected');
                    end
                    interface.xmvs.selected = args.Indices(1, 1);
                    updateInterface();
                end
            end
        end % createCVs
        
        function gui = createXMs(gui, parent)
            gui.xmeas.table = uitable('Parent', parent, ...
                'Data', [data.xmeas.titles', data.xmeas.steadys', data.xmeas.ylabels'], ...
                'ColumnName', {'Description', 'Steady State', 'Unit'}, ...
                'ColumnFormat', {'char', 'char', 'char'}, ...
                'CellSelectionCallback', @onCellSelect);
            
            function onCellSelect(~, args)
                if any(args.Indices)
                    if isfield(interface.xmvs, 'selected')
                        interface.xmvs = rmfield(interface.xmvs, 'selected');
                    end
                    interface.xmeas.selected = args.Indices(1, 1);
                    updateInterface();
                end
            end
        end % createXMs
        
        function gui = createCLoops(gui, parent)
            gui.cloops.table = uitable('Parent', parent, ...
                'CellEditCallback',@onCellEdit);
            function onCellEdit(h, event)
                % --------------------------------------------------------------- %
                % CellEditCallback event structure fields:
                % Indices, 
                %  1-by-2 matrix, 
                %  Row index and column index of the cell the user edited.
                % PreviousData, 
                %  1-by-1 matrix or cell array, 
                %  Previous data for the changed cell. The default is an empty matrix, [].
                % EditData, 
                %  String, 
                %  User-entered string.
                % NewData, 
                %  1-by-1 matrix or cell array,
                %  Value that uitable wrote to Data.
                % Error,
                %  String 
                %  Error that occurred when uitable tried to convert the EditData string
                % --------------------------------------------------------------- %        
                if (isempty(event.Error))
                    set(h,'Enable','off');
                    row = event.Indices(1);
                    column = event.Indices(2);
                    switch (column)
                        case 4 % change controller type
                            data.controllers(row).Type = ControllerType.FromString(event.NewData);
                        case 5 % change set point
                            data.controllers(row).SetPoint = event.NewData;
                        case 6 % change Kc
                            data.controllers(row).Gain = event.NewData;
                        case 7 % change Tr
                            data.controllers(row).ResetTime = event.NewData;                
                    end
                    set(h,'Enable','on');
                else
                    warning('VAC:gui:onCellEdit', 'Invalid input!');   
                end

            end
        end % createCLoops
        
        %controls
        function gui = createControls(gui, parent)
            box = uiextras.HButtonBox('Parent', parent, ...
                'HorizontalAlignment', 'left', ...
                'Padding', 10);
            gui.controls.reset = uicontrol('Parent', box, ...
                'String', 'Reset', ...
                'Callback', @onReset);
            function onReset(~,~)
                choice = questdlg('All settings will be reset. Continue?', ...
                    'Reset', 'Yes', 'No', 'No');
                if strcmp(choice, 'Yes')
                    data.attackcontrollers = initAttackControllers();
                    data.controllers = initControllers();
                    data.settings = initSettings();
                    updateInterface();
                end
            end
            
            panels = uiextras.VBox('Parent', parent, ...
                                 'Spacing', 8, ...
                                 'Padding', 21);
            %General
            gui.controls.general= uiextras.BoxPanel('Parent',panels, ...
                                  'Title','General', ...
                                  'FontSize', 11, ...
                                  'Padding', 8);
            vbox= uiextras.VBox('Parent', gui.controls.general, ...
                                'Spacing', 4);
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Attack mode');
            gui.controls.mode = uicontrol('Parent', vbox, ...
                      'Style', 'popup', ...
                      'FontSize', 9, ...
                      'String', AttackMode.Modes(), ...
                      'Value', 1, ...
                      'Callback', @onModeChanged);
            function onModeChanged(~, ~)
                updateControls(createController( ...
                    extractControlsData(), 'xmv', 1));%which type doesnt matter
            end % onModeChanged
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Attack type');
            gui.controls.type = uicontrol('Parent', vbox, ...
                      'Style', 'popup', ...
                      'FontSize', 9, ...
                      'String', AttackType.Types(), ...
                      'Value', 1, ...
                      'Callback', @onTypeChanged);
            function onTypeChanged(~, ~)
                updateControls(createController( ...
                    extractControlsData(), 'xmv', 1));%which type doesnt matter
            end % onTypeChanged
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Start');
            hbox = uiextras.HBox('Parent', vbox, ...
                'Spacing', 4);
            gui.controls.start = uicontrol('Parent', hbox, ...
                    'Style', 'edit', ...
                    'HorizontalAlignment', 'left', ...
                    'FontSize', 11, ...
                    'String',  1, ...
                    'Callback', @onStartChanged);
            function onStartChanged(src, ~)
                value = str2double(get(src, 'String'));
                if isempty(value)
                    warning('VAC:gui:onStartChanged', 'Value must be a number.');
                    set(src, 'String', 1);
                    return;
                end
                if value < 0
                    warning('VAC:gui:onStartChanged', 'Value must be a positiv integer.');
                    set(src, 'String', 1);
                    return;
                end
            end % onStartChanged
            gui.controls.startunits = uicontrol('Parent', hbox, ...
                'Style', 'popup', ...
                'FontSize', 8, ...
                'String', TimeUnits.Units(), ...
                'Value', 1);
            set(hbox, 'Sizes', [-1 80]);
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Duration');
            hbox = uiextras.HBox('Parent', vbox, ...
                'Spacing', 4);
            gui.controls.duration = uicontrol('Parent', hbox, ...
                    'Style', 'edit', ...
                    'HorizontalAlignment', 'left', ...
                    'FontSize', 11, ...
                    'String',  1, ...
                    'Callback', @onDurationChanged);
            function onDurationChanged(src, ~)
                value = str2double(get(src, 'String'));
                if isempty(value)
                    warning('VAC:gui:onDurationChanged', 'Value must be a number.');
                    set(src, 'String', 1);
                    return;
                end
                if value < 0
                    warning('VAC:gui:onDurationChanged', 'Value must be a positiv integer.');
                    set(src, 'String', 1);
                    return;
                end
            end % onDurationChanged
            gui.controls.durationunits = uicontrol('Parent', hbox, ...
                'Style', 'popup', ...
                'FontSize', 8, ...
                'String', TimeUnits.Units(), ...
                'Value', 1);
            set(hbox, 'Sizes', [-1 80]);
            %Integrity
            gui.controls.integrity = uiextras.BoxPanel('Parent',panels, ...
                'Title', 'Integrity', ...
                'FontSize', 11, ...
                'Padding', 8);
            vbox= uiextras.VBox('Parent', gui.controls.integrity, ...
                                'Spacing', 4);            
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Value');
            gui.controls.value = uicontrol('Parent', vbox, ...
                    'Style', 'edit', ...
                    'HorizontalAlignment', 'left', ...
                    'FontSize', 11, ...
                    'String', 1, ...
                    'Callback', @onIntegrityValueChanged);
            function onIntegrityValueChanged(src, ~)
                value = str2double(get(src, 'String'));
                if isempty(value)
                    warning('VAC:gui:onIntegrityValueChanged', 'Value must be a number.');
                    set(src, 'String', 1);
                    return;
                end
            end % onIntegrityValueChanged
            %Custom
            gui.controls.custom = uiextras.BoxPanel('Parent',panels, ...
                'Title', 'Custom', ...
                'FontSize', 11, ...
                'Padding', 8);
            vbox= uiextras.VBox('Parent', gui.controls.custom, ...
                                'Spacing', 4);            
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Signal variable');
            gui.controls.signal = uicontrol('Parent', vbox, ...
                    'Style', 'edit', ...
                    'HorizontalAlignment', 'left', ...
                    'FontSize', 11, ...
                    'String',  '[]', ...
                    'Callback', @onSignalChanged);
            function onSignalChanged(src, ~)
                value = get(src, 'String');
                if isempty(value)
                    set(src, 'String', '[]');
                    return;
                end
                if isnumeric(value)
                    warning('VAC:gui:onSignalChanged', 'Value must be a character value.');
                    set(src, 'String', '[]');
                    return;
                end
            end % onSignalChanged
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Sample time');
            gui.controls.sampling = uicontrol('Parent', vbox, ...
                    'Style', 'edit', ...
                    'HorizontalAlignment', 'left', ...
                    'FontSize', 11, ...
                    'String',  -1, ...
                    'Callback', @onSamplingChanged);
            function onSamplingChanged(src, ~)
                value = str2double(get(src, 'String'));
                if isempty(value)
                    warning('VAC:gui:onSignalChanged', 'Value must be a numeric value.');
                    set(src, 'String', -1);
                    return;
                end
            end % onSamplingChanged
            uicontrol('Parent', vbox, ...
                      'Style', 'text', ...
                      'FontSize', 11, ...
                      'HorizontalAlignment', 'right', ...
                      'String', 'Output after final');
            gui.controls.output = uicontrol('Parent', vbox, ...
                      'Style', 'popup', ...
                      'FontSize', 9, ...
                      'String', CustomOutput.Outputs(), ...
                      'Value', 1, ...
                      'Callback', @onOutputTypeChanged);
            function onOutputTypeChanged(~, ~)
                
            end % onOutputTypeChanged
            %Right buttons
            box = uiextras.HButtonBox('Parent', parent, ...
                'HorizontalAlignment', 'right', ...
                'Padding', 10);
            gui.controls.profile = uicontrol('Parent', box, ...
                'String', 'Profile', ...
                'Callback', @onProfile);
            function onProfile(~,~)
                %extract controller data
                controller = createController(extractControlsData(), 'xmv', 1);
                
                if controller.Mode == AttackMode.NONE
                   msgbox('Select attack mode first!', 'Attack', 'help');
                   return; 
                end
                
                %example siugnal function
                func = @sin;
                func_sampling = 0.001;
                normalTime = 0:func_sampling:data.settings.simulation.duration;
                normalSignal = func(0:func_sampling:data.settings.simulation.duration);
                
                %create resulting attack signal
                attackStart = controller.Start;
                attackDuration = controller.Duration;
                if controller.Mode == AttackMode.INTERVAL
                    if attackStart + controller.Duration < data.settings.simulation.duration
                        attackEnd = attackStart + controller.Duration;
                    else
                        attackEnd = data.settings.simulation.duration;                        
                    end
                else
                    attackEnd = data.settings.simulation.duration;                 
                end               
                
                if controller.Mode == AttackMode.PERIODIC
                    periodStart = attackStart;
                    periodEnd = attackStart + attackDuration;
                    attackSignal = [];
                    attackTime = [];
                    lineTime = [];
                    lineSignal = [];
                    while 1
                        [periodSignal, periodTime] = getSignalInPeriod(periodStart, periodEnd);
                        attackSignal = [attackSignal periodSignal]; %#ok<AGROW>
                        attackTime = [attackTime periodTime]; %#ok<AGROW>
                        lineTime = [lineTime periodTime]; %#ok<AGROW>
                        lineSignal = [lineSignal periodSignal]; %#ok<AGROW>
                        if attackTime(end) > attackEnd
                            break;
                        end
                        periodStart = periodEnd + attackStart;
                        periodEnd = periodStart + attackDuration;
                        lineTime = [lineTime normalTime(normalTime > attackTime(end) & normalTime < periodStart)]; %#ok<AGROW>
                        lineSignal = [lineSignal normalSignal(normalTime > attackTime(end) & normalTime < periodStart)]; %#ok<AGROW>
                        attackSignal = [attackSignal 0 0]; %#ok<AGROW>
                        attackTime = [attackTime attackTime(end) periodStart]; %#ok<AGROW>                        
                    end
                    if controller.Type == AttackType.CUSTOM
                        lineTime = lineTime(lineTime <= attackEnd);
                        lineSignal = lineSignal(lineTime <= attackEnd);
                        attackSignal = attackSignal(attackTime <= attackEnd);
                        attackTime = attackTime(attackTime <= attackEnd);
                    else
                        %attackSignal(end) = ;
                        attackTime(end) = attackEnd;
                        lineTime(end) = attackEnd;
                    end
                else
                    [attackSignal, attackTime] = getSignalInPeriod(attackStart, attackEnd);
                    lineTime = attackTime;
                    lineSignal = attackSignal;
                end
                
                if attackStart > 0
                   lineTime = [normalTime(normalTime < attackStart) lineTime];
                   lineSignal = [normalSignal(normalTime < attackStart) lineSignal];
                end
                if attackEnd < data.settings.simulation.duration
                   lineTime = [lineTime normalTime(normalTime > attackEnd)];
                   lineSignal = [lineSignal normalSignal(normalTime > attackEnd)];
                end
                
                function [signal, time] = getSignalInPeriod(from, to)
                    if controller.Type == AttackType.DOS
                        time = [from to];
                        signal = ones(1, length(time)) * func(from);
                    elseif controller.Type == AttackType.INTEGRITY
                        time = [from to];
                        signal = ones(1, length(time)) * controller.Value;
                    else % custom signal
                        customSignal = evalin('base', controller.Signal);
                        if controller.Sampling == -1
                            evalin('base', 'initModel');
                            sampling = evalin('base', 'Ts_base');
                        else
                            sampling = controller.Sampling;
                        end
                        signalTime = attackStart:sampling:attackStart+sampling*(length(customSignal)-1);
                        if signalTime(end) < to
                            if controller.OutputType == CustomOutput.HOLDINGVALUE
                                times = signalTime(end)+sampling:sampling:to;
                                signalTime = [signalTime times];
                                customSignal = [customSignal ones(1,length(times))*customSignal(end)];
                            elseif controller.OutputType == CustomOutput.SETTINGZERO
                                times = signalTime(end)+sampling:sampling:to;
                                signalTime = [signalTime times];
                                customSignal = [customSignal zeros(1,length(times))];
                            elseif controller.OutputType == CustomOutput.EXTRAPOLATION
                                times = signalTime(end)+sampling:sampling:to;
                                point = interp1(signalTime,customSignal,to,'linear','extrap');
                                points = (point-customSignal(end))/(to-signalTime(end))*times;
                                customSignal = [customSignal points-points(1)+customSignal(end)];
                                signalTime = [signalTime times];
                            else % cyclic
                                while signalTime(end) < to
                                    customSignal = [customSignal customSignal]; %#ok<AGROW>
                                    signalTime = [signalTime signalTime+(signalTime(end)-signalTime(1))]; %#ok<AGROW>
                                end                                
                            end
                        end                        
                        indexes = signalTime >= from & signalTime <= to;
                        time = signalTime(indexes);
                        signal = customSignal(indexes);
                    end
                end
                
                %setup child window
                profileWindow = setupChildWindow(interface.window);
                profileAxes = axes('Parent', profileWindow);
                area(profileAxes, normalTime, normalSignal, ...
                    'FaceColor', 'blue', 'EdgeColor', 'blue');
                hold(profileAxes, 'on');
                area(profileAxes, attackTime, attackSignal, ...
                    'FaceColor', 'red', 'EdgeColor', 'red');
                plot(profileAxes, lineTime, lineSignal, ...
                    'black', 'LineWidth', 4);
                hold(profileAxes, 'off');
                xlabel(profileAxes, 'Hours');
                title(profileAxes, 'Attack Signal');
                xlim(profileAxes, [0 data.settings.simulation.duration]);
                ylim(profileAxes, [min([attackSignal -1])-1 max([attackSignal 1])+1]);
                legend(profileAxes, 'Normal', 'Attack window', 'Resulting signal');
            end % onProfile
            gui.controls.apply = uicontrol('Parent', box, ...
                'String', 'Apply', ...
                'Callback', @onApply);
            function onApply(~,~)
                if isfield(interface.xmvs, 'selected')% xmvs
                    controller = createController(extractControlsData(), 'xmv', ...
                              interface.xmvs.selected);
                    data.attackcontrollers.xmvs(interface.xmvs.selected) = controller;
                elseif isfield(interface.xmeas, 'selected')
                    controller = createController(extractControlsData(), 'xmeas', ...
                              interface.xmeas.selected);
                    data.attackcontrollers.xmeas(interface.xmeas.selected) = controller;
                else
                    msgbox('First select an controller!', 'Selection');
                    return;
                end
            end % onApply
            gui.controls.Run = uicontrol('Parent', box, ...
                'String', 'Run', ...
                'Callback', @onRun);
            function onRun(~,~)
                 progressbar('Simulation running..', 'Busy');
                 cHandler = onCleanup(@OnClosing);
                 %load model
                 exec(['if ~bdIsLoaded(model)' ...
                            'load_system(model);' ...
                       'end']);
                 %set attack controllers
                 for i = 1:length(data.attackcontrollers.xmvs)
                     data.attackcontrollers.xmvs(i).Set('base');
                 end
                 for i = 1:length(data.attackcontrollers.xmeas)
                     data.attackcontrollers.xmeas(i).Set('base');
                 end
                 %set control loops
                 for i = 1:length(data.controllers)
                     data.controllers(i).Set('base');
                 end
                 %set disturbances
                 exec(['set_disturb(''' mat2str(data.settings.simulation.disturbances) ''');']);
                 %set simulation duration
                 exec(['set_param(model, ''StopTime'', ''' num2str(data.settings.simulation.duration) ''');']);
                 %set callbacks
                 exec(['set_param(model, ''StopFcn'',''' strtrim(reshape(data.settings.simulation.callbacks.afterend.',1,[])) ''');']);
                 %run simulation
                 exec('sim(model);');
                 %% clean up function handler
                 function OnClosing()
                    exec('close_system(model, 0);');
                    progressbar('Simulation running..', 'Close');
                 end                 
            end % onRun
            
            set(panels, 'Sizes', [220 80 185]);
        end % createControls
    end % createInterface

    function updateInterface()
        updateControls();
        updateXmvs();
        updateXmeas();
        updateCLoops();
    end % updateInterface

    function updateControls(controller)
        if nargin > 0 && isa(controller, 'AttackController')
            %do nothing
        elseif isfield(interface.xmvs, 'selected')% xmvs
            controller = data.attackcontrollers.xmvs(interface.xmvs.selected);
        elseif isfield(interface.xmeas, 'selected')% xmeas
            controller = data.attackcontrollers.xmeas(interface.xmeas.selected);
        else
            return;
        end
        
        if controller.Mode == AttackMode.NONE
           set(interface.controls.type, 'Enable', 'off');
           set(interface.controls.start, 'Enable', 'off');
           set(interface.controls.duration, 'Enable', 'off');
           set(interface.controls.integrity, 'Enable', 'off');
           set(interface.controls.custom, 'Enable', 'off');
        else
           set(interface.controls.type, 'Enable', 'on');
           set(interface.controls.start, 'Enable', 'on');
           
           if controller.Mode == AttackMode.STEP
               set(interface.controls.duration, 'Enable', 'off');
           else
               set(interface.controls.duration, 'Enable', 'on');
           end
           
           if controller.Type == AttackType.DOS
               set(interface.controls.integrity, 'Enable', 'off');
               set(interface.controls.custom, 'Enable', 'off');
           elseif controller.Type == AttackType.INTEGRITY
               set(interface.controls.integrity, 'Enable', 'on');
               set(interface.controls.custom, 'Enable', 'off');
           elseif controller.Type == AttackType.CUSTOM
               set(interface.controls.integrity, 'Enable', 'off');
               set(interface.controls.custom, 'Enable', 'on');
           end
        end
        
        set(interface.controls.mode, 'Value', controller.Mode.Value);
        set(interface.controls.type, 'Value', controller.Type.Value);
        set(interface.controls.start, 'String', controller.Start);
        set(interface.controls.duration, 'String', controller.Duration);
        set(interface.controls.value, 'String', controller.Value);
        set(interface.controls.signal, 'String', controller.Signal);
        set(interface.controls.sampling, 'String', controller.Sampling);
        set(interface.controls.output, 'Value', controller.OutputType.Value);
    end % updateControls

    function updateXmvs()
        tableData = [data.xmvs.titles', data.xmvs.steadys', data.xmvs.mins', data.xmvs.maxs', data.xmvs.ylabels'];
        if isfield(interface.xmvs, 'selected')
            tableData(interface.xmvs.selected, :) = colorRow(tableData(interface.xmvs.selected, :), '#00eeee');
        end
        for i = 1:size(tableData, 1)
            if data.attackcontrollers.xmvs(i).Mode ~= AttackMode.NONE
                tableData(i, :) = colorRow(tableData(i, :), '#ff3366');
            end
        end
        set(interface.xmvs.table, 'Data', tableData);
        set(interface.xmvs.table, 'ColumnWidth', ...
                num2cell(repmat((column(get(interface.xmvs.table, 'Position'), 3)-73)/(length(get(interface.xmvs.table, 'ColumnName'))), ...
                              1, length(get(interface.xmvs.table, 'ColumnName')))));
    end % updateXmvs

    function updateXmeas()
        tableData = [data.xmeas.titles', data.xmeas.steadys', data.xmeas.ylabels'];
        if isfield(interface.xmeas, 'selected')
            tableData(interface.xmeas.selected, :) = colorRow(tableData(interface.xmeas.selected, :), '#00eeee');
        end
        for i = 1:size(tableData, 1)
            if data.attackcontrollers.xmeas(i).Mode ~= AttackMode.NONE
                tableData(i, :) = colorRow(tableData(i, :), '#ff3366');
            end
        end
        set(interface.xmeas.table, 'Data', tableData);
        set(interface.xmeas.table, 'ColumnWidth', ...
                num2cell(repmat((column(get(interface.xmeas.table, 'Position'), 3)-77)/(length(get(interface.xmeas.table, 'ColumnName'))), ...
                              1, length(get(interface.xmeas.table, 'ColumnName')))));
    end % updateXmeas

    function updateCLoops()
        count = length(data.controllers);
        cvs = cell(1, count);
        dsps = cell(1, count);
        for i = 1:count
            controller = data.controllers(i);
            if controller.XmeasBlock ~= -1
                cvs{i} = data.xmeas.titles{controller.XmeasBlock};
                dsps{i} = data.xmeas.steadys{controller.XmeasBlock};
            end
        end
        cvs{10} = 'Separator Vapor Flowrate'; dsps{10} = 16.1026;
        cvs{13} = 'Absorber Scrub Flowrate'; dsps{13} = 15.1198;
        cvs{15} = 'Absorber Circulation Flowrate'; dsps{15} = 0.756;
        cvs{26} = 'Liquid Recycle Flowrate'; dsps{26} = 2.1924;
        types = [data.controllers.Type];
        cltabledata = [cvs', {data.controllers.Name}', dsps', ...
            {types.String}', {data.controllers.SetPoint}', ...
            {data.controllers.Gain}', {data.controllers.ResetTime}'];
        set(interface.cloops.table, 'Data', cltabledata);
        set(interface.cloops.table, 'ColumnName', {'CV', 'MV', 'Default SP', 'Controller', ...
            'SP', 'Gain', 'Reset time'});
        set(interface.cloops.table, 'ColumnFormat', {'char','char','numeric',ControllerType.Types()','numeric','numeric'});
        set(interface.cloops.table, 'ColumnEditable',[false, false, false, true, true, true, true]);
        set(interface.cloops.table, 'ColumnWidth', ...
                num2cell(repmat((column(get(interface.cloops.table, 'Position'), 3)-70)/(length(get(interface.cloops.table, 'ColumnName'))), ...
                              1, length(get(interface.cloops.table, 'ColumnName')))));
    end % updateCLoops

%     function tab = getSelectedTab()
%         tab = get(interface.tabs, 'SelectedChild'); 
%     end

    function cell = colorCell(text, color)
       if isnumeric(text)
           text = num2str(text);
           cell = ['<html><table border=0 width=400 bgcolor=' color '><tr><td>' text '</td></tr></table></html>'];
       else
           text = char(text);
           cell = ['<html><table border=0 width=400 bgcolor=' color '><tr><td>' text '</td></tr></table></html>'];
       end
    end

    function row = colorRow(row, color)
       for i = 1:size(row, 2)           
           row{i} = colorCell(row{i}, color);
       end
    end

    function extractedData = extractControlsData()
        extractedData.mode = AttackMode.FromValue(get(interface.controls.mode, 'Value'));
        extractedData.type = AttackType.FromValue(get(interface.controls.type, 'Value'));
        start = str2double(get(interface.controls.start, 'String'));
        if isempty(start)
            warning('VAC:gui:extractControlsData:start', 'Value must be a number.');
            set(interface.controls.start, 'String', 1);
            return;
        end
        switch get(interface.controls.startunits, 'Value')
            case TimeUnits.MINUTES.Value
                extractedData.start = start/60;
            case TimeUnits.SECONDS.Value
                extractedData.start = start/3600;
            otherwise
                extractedData.start = start;
        end
        duration = str2double(get(interface.controls.duration, 'String'));
        if isempty(duration)
            warning('VAC:gui:extractControlsData:duration', 'Value must be a number.');
            set(interface.controls.duration, 'String', 1);
            return;
        end
        switch get(interface.controls.durationunits, 'Value')
            case TimeUnits.MINUTES.Value
                extractedData.duration = duration/60;
            case TimeUnits.SECONDS.Value
                extractedData.duration = duration/3600;
            otherwise
                extractedData.duration = duration;
        end
        extractedData.value = str2double(get(interface.controls.value, 'String'));
        if isempty(extractedData.value)
            set(interface.controls.value, 'String', 1);
            error('VAC:gui:extractControlsData:value', 'Value must be a number.');
        end
        extractedData.signal = get(interface.controls.signal, 'String');
        if isempty(extractedData.signal) || strcmp(extractedData.signal, '[]')
            extractedData.signal = '[]';
            set(interface.controls.signal, 'String', '[]');
        else
            if evalin('base', ['~exist(''' extractedData.signal ''', ''var'')'])
                msg = ['Variable ''' extractedData.signal ''' couldn''t be found.'];
                extractedData.signal = '[]';
                set(interface.controls.signal, 'String', '[]');
                error('VAC:gui:extractControlsData:signal', msg);
            end
            if evalin('base', ['isnumeric(''' extractedData.signal ''')'])
                set(interface.controls.value, 'String', '[]');
                extractedData.signal = '[]';
                error('VAC:gui:extractControlsData:signal', 'Value must be a character value.');
            end
        end
        
        extractedData.sampling = str2double(get(interface.controls.sampling, 'String'));
        if isempty(extractedData.sampling)
            set(interface.controls.sampling, 'String', -1);
            extractedData.sampling = -1;
            error('VAC:gui:extractControlsData:sampling', 'Value must be a number.');
        end
        extractedData.output = CustomOutput.FromValue(get(interface.controls.output, 'Value'));
    end

    function controller = createController(controlsData, type, block)
       controller = AttackController(char(type), block);
       controller.Start = controlsData.start;
       controller.Duration = controlsData.duration;
       controller.Mode = controlsData.mode;
       controller.Type = controlsData.type;
       controller.Value = controlsData.value;
       controller.Signal = controlsData.signal;
       controller.Sampling = controlsData.sampling;
       controller.OutputType = controlsData.output;
    end
end

function exec(command)
    evalin('base', command);
end
