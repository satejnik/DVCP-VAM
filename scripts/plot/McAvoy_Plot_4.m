%This routine is used to plot measurements for the VAc plant
%Copyright: Rong Chen and Kedar David, June 2002
function McAvoy_Plot_4(y_history,u_history,setpoint,my_label,MV_label,sampling)
warning off
k_y=(size(y_history,1)-1)*sampling; %ending minute
k_u=(size(u_history,1)-1)*sampling; %ending minute
y_lowlimit=[0.0748*100 157.5 109.8 0.000005*100000 0.09*100 0.48*1000 0.78*1000 0*1000];
y_highlimit=[0.0752*100 159.6 110.2 0.00002*100000 0.11*100 0.54*1000 0.86*1000 1.5*1000];
figure(1);
subplot(4,2,1);
plot(0:sampling:k_y,y_history(:,1)*100);
hold on
plot(0:k_y,setpoint(1)*[ones(1,k_y),1]*100,':r');
hold off
ylabel(my_label(1,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(1),y_highlimit(1)]);
subplot(4,2,2);
plot(0:sampling:k_y,y_history(:,7));
hold on
plot(0:k_y,setpoint(7)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(7,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(2),y_highlimit(2)]);
subplot(4,2,3);
plot(0:sampling:k_y,y_history(:,21));
hold on
plot(0:k_y,setpoint(21)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(21,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(3),y_highlimit(3)]);
subplot(4,2,4);
plot(0:sampling:k_y,y_history(:,27)*100000);
ylabel(my_label(27,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(4),y_highlimit(4)]);
subplot(4,2,5);
plot(0:sampling:k_y,y_history(:,20)*100);
hold on
plot(0:k_y,setpoint(20)*[ones(1,k_y),1]*100,':r');
hold off
ylabel(my_label(20,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(5),y_highlimit(5)]);
subplot(4,2,6);
plot(0:sampling:k_u,u_history(:,1)*1000);
ylabel('Fresh O2');
xlabel('minute')
axis([0,k_u,y_lowlimit(6),y_highlimit(6)]);
subplot(4,2,7);
plot(0:sampling:k_u,u_history(:,2)*1000);
ylabel('Fresh C2H4');
xlabel('minute')
axis([0,k_u,y_lowlimit(7),y_highlimit(7)]);
subplot(4,2,8);
plot(0:sampling:k_u,u_history(:,3)*1000);
ylabel('Fresh HAc');
xlabel('minute')
axis([0,k_u,y_lowlimit(8),y_highlimit(8)]);
%title('Liquid Recycle Flow Increases 20% To 2.64 kmol/min')
figure(2)
close(2);
warning on

