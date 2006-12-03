function res = laserazosSM3_analyze(file)

%	data = load('out/laserazosSM3.ICPC.stats.txt');
data = load(file);

total_error = data(:,2)';
nvalid = data(:,3)';
mean_error = data(:,4)';
iterations = data(:,5)';
realtime = data(:,6)';

res.total_error = total_error;
res.nvalid = nvalid;
res.mean_error = mean_error;
res.iterations = iterations;
res.realtime = realtime;
res.data = data;

figure
n = 4;

	subplot(n,1,1); 
	title('Total error and mean error (normalized)');
	total_error_norm = total_error / max(total_error);
	plot(total_error_norm);
	hold on
	mean_error_norm = mean_error / max(mean_error);
	
	plot(mean_error_norm);
%	legend('total','mean');
	hold off;

	subplot(n,1,2);
	title('Valid');
	plot(nvalid);

	subplot(n,1,3);
	title('Iterations');
	plot(iterations);
	
	subplot(n,1,4);
	title('Time (s)');
	plot(realtime);