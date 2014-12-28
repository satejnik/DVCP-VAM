%This routine is used to plot measurements for the VAc plant
%Copyright: Rong Chen and Kedar David, June 2002
function Transient_Plot(y_history,u_history,setpoint,my_label,MV_label,sampling)
warning off
k_y=(size(y_history,1)-1)*sampling; %ending minute
k_u=(size(u_history,1)-1)*sampling; %ending minute
y_lowlimit=[0.07,118,0,0.65,118,100,150,0,30,10,50,0,0,0,0,0,0,0.2,120,0.05,108,0,0,0,0];
y_highlimit=[0.08,132,1,0.75,132,200,170,1,50,20,100,1,20,50,1,50,0.02,0.35,140,0.30,112,1,1,1,4];
figure(1);
for i=1:8
    subplot(4,2,i);
    plot(0:sampling:k_y,y_history(:,i));
    if i<=size(setpoint,1)
        hold on
        plot(0:k_y,setpoint(i)*[ones(1,k_y),1],':r');
        hold off
    end
    ylabel(my_label(i,:));
    xlabel('minute')
    %axis tight
    axis([0,k_y,y_lowlimit(i),y_highlimit(i)]);
end
figure(2)
for i=9:16
    subplot(4,2,i-8);
    plot(0:sampling:k_y,y_history(:,i));
    if i<=size(setpoint,1)
        hold on
        plot(0:k_y,setpoint(i)*[ones(1,k_y),1],':r');
        hold off
    end
    ylabel(my_label(i,:));
    xlabel('minute')
    %axis tight
    axis([0,k_y,y_lowlimit(i),y_highlimit(i)]);
end
figure(3)
for i=17:24
    subplot(4,2,i-16);
    plot(0:sampling:k_y,y_history(:,i));
    if i<=size(setpoint,1)
        hold on
        plot(0:k_y,setpoint(i)*[ones(1,k_y),1],':r');
        hold off
    end
    ylabel(my_label(i,:));
    xlabel('minute')
    %axis tight
    axis([0,k_y,y_lowlimit(i),y_highlimit(i)]);
end
figure(4)
for i=25:25
    subplot(4,2,i-24);
    plot(0:sampling:k_y,y_history(:,i));
    if i<=size(setpoint,1)
        hold on
        plot(0:k_y,setpoint(i)*[ones(1,k_y),1],':r');
        hold off
    end
    ylabel(my_label(i,:));
    xlabel('minute')
    %axis tight
    axis([0,k_y,y_lowlimit(i),y_highlimit(i)]);
end
%-----------------------------------------------------------------------------
figure(5);
for i=1:8
    subplot(4,2,i);
    plot(0:sampling:k_u,u_history(:,i));
    ylabel(MV_label(i,:));
    xlabel('minute')
    axis tight
end
figure(6);
for i=9:16
    subplot(4,2,i-8);
    plot(0:sampling:k_u,u_history(:,i));
    ylabel(MV_label(i,:));
    xlabel('minute')
    axis tight
end
figure(7);
for i=17:24
    subplot(4,2,i-16);
    plot(0:sampling:k_u,u_history(:,i));
    ylabel(MV_label(i,:));
    xlabel('minute')
    axis tight
end
%-----------------------------------------------------------------------------
figure(8);
subplot(4,2,1);
plot(0:sampling:k_y,y_history(:,1));
hold on
plot(0:k_y,setpoint(1)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(1,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(1),y_highlimit(1)]);
subplot(4,2,2);
plot(0:sampling:k_y,y_history(:,2));
hold on
plot(0:k_y,setpoint(2)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(2,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(2),y_highlimit(2)]);
subplot(4,2,3);
plot(0:sampling:k_y,y_history(:,7));
hold on
plot(0:k_y,setpoint(7)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(7,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(7),y_highlimit(7)]);
subplot(4,2,4);
plot(0:sampling:k_y,y_history(:,9));
hold on
plot(0:k_y,setpoint(9)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(9,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(9),y_highlimit(9)]);
subplot(4,2,5);
plot(0:sampling:k_y,y_history(:,17));
hold on
plot(0:k_y,setpoint(17)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(17,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(17),y_highlimit(17)]);
subplot(4,2,6);
plot(0:sampling:k_y,y_history(:,18));
hold on
plot(0:k_y,setpoint(18)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(18,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(18),y_highlimit(18)]);
subplot(4,2,7);
plot(0:sampling:k_y,y_history(:,20));
hold on
plot(0:k_y,setpoint(20)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(20,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(20),y_highlimit(20)]);
subplot(4,2,8);
plot(0:sampling:k_y,y_history(:,21));
hold on
plot(0:k_y,setpoint(21)*[ones(1,k_y),1],':r');
hold off
ylabel(my_label(21,:));
xlabel('minute')
axis([0,k_y,y_lowlimit(21),y_highlimit(21)]);
warning on

