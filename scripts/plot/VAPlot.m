%	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
%	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
%	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
%	All rights reserved.
%	License: http://opensource.org/licenses/BSD-3-Clause
%	----------------------------------------------------------------------

simout_labels=['   %O2  '; '  Pres  ';'  HAc-L ';'  Vap-L ';'  Vap-P ';'  Pre-T ';'  RCT-T '; ...
          '  Sep-L ';'  Sep-T ';'  Sep-V ';'  Com-T ';'  Abs-L ';'  Cir-F ';'  Cir-T '; ...
          '  Scr-F ';'  Scr-T ';'  %CO2  ';'  %C2H6 ';' FEHE-T ';'  %H2O  ';'  Col-T '; ...
          '  Org-L ';'  Aqu-L ';'  Col-L ';' Vap-In ';' Dect-T ';'%VAc E-3'];
      
xmv_labels=[' F-O2 ';'F-C2H4';' F-HAc';' Q-Vap';' F-Vap';'Q-Heat';'ShellT';'F-SepL';' T-Sep'; ...
          'F-SepV';'Q-Comp';'F-AbsL';'F-Circ';'Q-Circ';'F-Scru';'Q-Scru';' F-CO2';' Purge'; ...
          'bypass';'Reflux';'Q-Rebo';'F-Orga';'F-Aque';' F-Bot';'Q_Cond';'F-Tank'];

setpoints = zeros(1, 26);
if isequal(gcr, model)
    for i=1:26
        setpoints(i) = str2double(get_param([bdroot '/xmv' num2str(i)], 'Value'));
    end
end

lowlimits=[0.07,118,0,0.65,118,100,150,0,30,10,50,0,0,0,0,0,0,0.2,120,0.05,108,0,0,0,0];
highlimits=[0.08,132,1,0.75,132,200,170,1,50,20,100,1,20,50,1,50,0.02,0.35,140,0.30,112,1,1,1,4];

figure;
maximize;
for i=1:8
    subplot(4,2,i);
    plot(tout, simout(:,i));
    hold on
    plot([0 tout(end)], [setpoints(i) setpoints(i)],':g');
    plot([0 tout(end)], [lowlimits(i) lowlimits(i)],':r');
    plot([0 tout(end)], [highlimits(i) highlimits(i)],':r');
    hold off
    ylabel(simout_labels(i,:));
    xlabel('hours')
    %axis tight
    %axis([0, tout(end), lowlimits(i), highlimits(i)]);
    xlim([0, tout(end)]);
end

figure;
maximize;
for i=9:16
    subplot(4,2,i-8);
    plot(tout, simout(:,i));
    if i<=size(setpoints,1)
        hold on
        plot([0 tout(end)], [setpoints(i) setpoints(i)],':g');
        hold off
    end
    ylabel(simout_labels(i,:));
    xlabel('hours')
    %axis tight
    %axis([0, tout(end), lowlimits(i), highlimits(i)]);
    xlim([0, tout(end)]);
end

figure;
maximize;
for i=17:24
    subplot(4,2,i-16);
    plot(tout, simout(:,i));
    if i<=size(setpoints,1)
        hold on
        plot([0 tout(end)], [setpoints(i) setpoints(i)],':g');
        hold off
    end
    ylabel(simout_labels(i,:));
    xlabel('hours')
    %axis tight
    %axis([0, tout(end), lowlimits(i), highlimits(i)]);
    xlim([0, tout(end)]);
end

figure;
maximize;
for i=25:25
    subplot(4,2,i-24);
    plot(tout, simout(:,i));
    if i<=size(setpoints,1)
        hold on
        plot([0 tout(end)], [setpoints(i) setpoints(i)],':g');
        hold off
    end
    ylabel(simout_labels(i,:));
    xlabel('hours')
    %axis tight
    %axis([0, tout(end), lowlimits(i), highlimits(i)]);
    xlim([0, tout(end)]);
end

%-----------------------------------------------------------------------------
figure;
maximize;
for i=1:8
    subplot(4,2,i);
    plot(tout, xmv(:,i));
    ylabel(xmv_labels(i,:));
    xlabel('hours')
    axis tight
    xlim([0, tout(end)]);
end

figure;
maximize;
for i=9:16
    subplot(4,2,i-8);
    plot(tout, xmv(:,i));
    ylabel(xmv_labels(i,:));
    xlabel('hours')
    axis tight
    xlim([0, tout(end)]);
end

figure;
maximize;
for i=17:24
    subplot(4,2,i-16);
    plot(tout, xmv(:,i));
    ylabel(xmv_labels(i,:));
    xlabel('hours')
    axis tight
    xlim([0, tout(end)]);
end

%-----------------------------------------------------------------------------
figure;
maximize;
subplot(4,2,1);
plot(tout, simout(:,1));
hold on
plot([0 tout(end)], [setpoints(1) setpoints(1)],':g');
hold off
ylabel(simout_labels(1,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(1), highlimits(1)]);
xlim([0, tout(end)]);

subplot(4,2,2);
plot(tout, simout(:,2));
hold on
plot([0 tout(end)], [setpoints(2) setpoints(2)],':g');
hold off
ylabel(simout_labels(2,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(2), highlimits(2)]);
xlim([0, tout(end)]);

subplot(4,2,3);
plot(tout, simout(:,7));
hold on
plot([0 tout(end)], [setpoints(7) setpoints(7)],':g');
hold off
ylabel(simout_labels(7,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(7), highlimits(7)]);
xlim([0, tout(end)]);

subplot(4,2,4);
plot(tout, simout(:,9));
hold on
plot([0 tout(end)], [setpoints(9) setpoints(9)],':g');
hold off
ylabel(simout_labels(9,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(9), highlimits(9)]);
xlim([0, tout(end)]);

subplot(4,2,5);
plot(tout, simout(:,17));
hold on
plot([0 tout(end)], [setpoints(17) setpoints(17)],':g');
hold off
ylabel(simout_labels(17,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(17), highlimits(17)]);
xlim([0, tout(end)]);

subplot(4,2,6);
plot(tout, simout(:,18));
hold on
plot([0 tout(end)], [setpoints(18) setpoints(18)],':g');
hold off
ylabel(simout_labels(18,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(18), highlimits(18)]);
xlim([0, tout(end)]);

subplot(4,2,7);
plot(tout, simout(:,20));
hold on
plot([0 tout(end)], [setpoints(20) setpoints(20)],':g');
hold off
ylabel(simout_labels(20,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(20), highlimits(20)]);
xlim([0, tout(end)]);

subplot(4,2,8);
plot(tout, simout(:,21));
hold on
plot([0 tout(end)], [setpoints(21) setpoints(21)],':g');
hold off
ylabel(simout_labels(21,:));
xlabel('hours')
%axis([0, tout(end), lowlimits(21), highlimits(21)]);
xlim([0, tout(end)]);