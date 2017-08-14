%% Connection / Setup
clear all

readChannelID = 244820;
readAPIKey = '7PKO12RVT5C40PE8';
samplesTested = 3;
numPoints = (samplesTested*6)+samplesTested-1; % Number of data points to read from ThingSpeak

%% Data Collection
[data, time] = thingSpeakRead(readChannelID, 'NumPoints', numPoints, 'ReadKey', readAPIKey);

%% Parse data
dataEndA = 1;
dataEndB = 4;
dataEndVolt = 1; 
for sample = 1:samplesTested
%     R(1:3,sample) = data(dataEndA:dataEndA+2, 1);
%     G(1:3,sample) = data(dataEndA:dataEndA+2, 2);
%     B(1:3,sample) = data(dataEndA:dataEndA+2, 3);
    Lux(1:3,sample) = data(dataEndA:dataEndA+2, 4);
    
%     R2(1:3,sample) = data(dataEndB:dataEndB+2, 5);
%     G2(1:3,sample) = data(dataEndB:dataEndB+2, 6);
%     B2(1:3,sample) = data(dataEndB:dataEndB+2, 7);
    
    Volt(1:6,sample) = data(dataEndVolt:dataEndVolt+5, 8);
    
    dataEndA = dataEndA+7;
    dataEndB = dataEndB+7;
    dataEndVolt = dataEndVolt+7;
end
   
%% Analyze
% meanR = mean(R);
% meanG = mean(G);
% meanB = mean(B);
meanLux = mean(Lux);
% meanR2 = mean(R2);
% meanG2 = mean(G2);
% meanB2 = mean(B2);
meanVolt = mean(Volt);

% stdR = std(R);
% stdG = std(G);
% stdB = std(B);
stdLux = std(Lux);
% stdR2 = std(R2);
% stdG2 = std(G2);
% stdB2 = std(B2);
stdVolt = std(Volt);

%% Plotting
x = 1:samplesTested;

figure(1)
clf
hold on
% p1 = plot(x,meanR,'r');
% errorbar(x,meanR,stdR,'vertical', 'r');
% 
% p2 = plot(x,meanG,'g');
% errorbar(x,meanG,stdG,'vertical', 'g');
% 
% p3 = plot(x,meanB,'b');
% errorbar(x,meanB,stdB,'vertical', 'b');

p4 = plot(x,meanLux,'k');
errorbar(x,meanLux,stdLux,'vertical', 'k');

ylabel 'Amplitudes'
xlabel 'Sample #'
title 'Color Intensity Means and Std. Deviations for 3 Points (Throughput Sensor)'

% legend([p1 p2 p3 p4],{'Red','Green','Blue','Lux'})
hold off
    
% figure(2)
% clf
% hold on
% p1 = plot(x,meanR2,'r');
% errorbar(x,meanR2,stdR2,'vertical', 'r');
% 
% p2 = plot(x,meanG2,'g');
% errorbar(x,meanG2,stdG2,'vertical', 'g');
% 
% p3 = plot(x,meanB2,'b');
% errorbar(x,meanB2,stdB2,'vertical', 'b');
% 
% ylabel 'Amplitudes'
% xlabel 'Sample #'
% title 'Color Intensity Means and Std. Deviations for 3 Points (Turbidity Sensor)'

% legend([p1 p2 p3],{'Red','Green','Blue'})
% hold off

figure(3)
clf
hold on
p1 = plot(x,meanVolt,'k');
errorbar(x,meanVolt,stdVolt,'vertical', 'k');

ylabel 'Potential Difference (mV)'
xlabel 'Sample #'
title 'Potential Difference Means and Std. Deviations for 6 points'

hold off

%% Sample Sorting
for i = 1:length(meanLux)
    total(i) = -(meanLux(i));
end

[sorted, Index] = sort(total);

disp('Samples in order from lowest ppm to highest:')
for k = 1:length(meanLux)
    fprintf('Sample %d \n',Index(k)) 
end
fprintf('\n') 

%% pH Estimation
pH_estimate1 = ((meanVolt(1)) - 801.37)./-68.247
pH_estimate2 = ((meanVolt(2)) - 801.37)./-68.247
pH_estimate3 = ((meanVolt(3)) - 801.37)./-68.247