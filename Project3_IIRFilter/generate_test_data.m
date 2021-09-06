function generate_test_data( filename )

if nargin <=2
    filename = 'csv_test_data.txt';
end

t_end = 0.2;  % s, Simulationszeit
t_sample = 0.001;  % s, Abtastzeit
f_nutz = 50;  % Grenze des Durchlassbereichs
n = 4; % Filterordnung

% Nyquist-Frequenz: 1/t_step/2
f_sample = 1/t_sample;
f_nyq = f_sample/2;  % Hz]
f_grenz = f_nutz;  % Grenzfrequenz des Filters 

% erzeugung von Testdaten für den Filter
t = 0:t_sample:t_end; % Zeit
x = zeros(size(t));  % Messsignal

% Sprung auf 1 bei 0 und zurück auf 0 bei 1/2*t_end
x(1,1:floor(size(t,2)/2)) = 1;

% Sprung auf 1 bei 1/3*t_end und zurück auf 0 bei 2/3*t_end
%x(1,floor(1/3*size(t,2)):floor(2/3*size(t,2))) = 1;

% Überlagerung mit hochfrequentem Sinus
f_stoer = f_nyq/4;
x = x + 0.1*sin(f_stoer*2*pi*t);

% als csv-Datei exportieren
csv_data = [t' x'];
csvwrite(filename,csv_data);

% IIR-Filterentwurf mit Ornung n und Grenzfrequenz f_grenz
% Hinweis: das Filter-Entwurfs-Tool verwendet eine auf die Nyquist-Frequnz
% normierte Grenzfrequenz zwischen 0..1, wobei 1 f_nyq entspricht.
% Für die Auslegung von Filtern bietet sich außerdem das fdatool an.
rel_nyq_grenze = f_grenz/f_nyq;
[b,a] = butter(n,rel_nyq_grenze);

% Das fvtool berechnet einige wichtige Filtereigenschaften und stellt 
% diese zB. als Bode-Plot dar. Die Güte der Filter lässt sich so besonders
% einfach vergleichen.
hfvt = fvtool(b,a);

% Signal in Matlab filtern
% Der Nenner der z-Übertragungsfunktion ist bei FIR immer 1
y = filter(b,a,x);

% ergebnis plotten
figure(3);
plot(t,x,t,y);

% Daten für Simulink S-Function-Test in Matfile speichern
sim_filter_data = timeseries([x; y],t);
save('data_iir.mat','b','a','sim_filter_data','t_sample','t_end');

% ausgewählte Koeffizienten als c-Array formatieren und in Datei speichern
file = fopen('coeff_iir.h' ,'wt');
fprintf(file, '/*\n * Filterkoeffizienten aus Matlab exportiert:\n */\n');
fprintf(file, '#define NENNER_COEFF_VECTOR ');
fprintf(file, '%.10f',a(1));
for i=2:size(a,2)
    fprintf(file, ',%.10f',a(i));
end
fprintf(file, '\n\n#define ZAEHLER_COEFF_VECTOR ');
fprintf(file, '%.10f',b(1));
for i=2:size(b,2)
    fprintf(file, ',%.10f',b(i));
end
fclose(file);

% unterbricht die Ausführung, um die Daten im Workspace anzusehen, die
% Plots anzuschauen, im Filter-Tool herumzuspielen und zu verhindern, dass
% im weiteren Fehler auftauchen, wenn die csv_result.txt nicht vorhanden ist
keyboard;

% Das von der C-Version gefilterte Signal im Vergleich:
% (die Datei muss natürlich vorhanden sein)
filename = 'csv_result.txt';
csv_data = csvread(filename);

y_ref = filter(b,a,x);

t_c = csv_data(:,1)';
x_c = csv_data(:,2)';
y_c = csv_data(:,3)';

% Betrachtung der Abweichungen
d_t = t_c - t;
d_x = x_c - x;
d_y = y_c - y_ref;

figure(3);
subplot(3,1,1);
plot(t,d_t);
subplot(3,1,2);
plot(t,d_x);
subplot(3,1,3);
plot(t,d_y);

% direkter Vergleich der Signale
figure(4);
plot(t,x,t,y_ref,t,y_c);

keyboard;

end

