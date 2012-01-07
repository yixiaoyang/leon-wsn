xx = 2.5;
disp(int16(xx));
disp(xx-int16(xx));
disp(double(xx-(int16(xx)))) ;
if ( xx-(int16(xx)) == 0 )	
	disp('2.5 == integer!');
end

xx = 2.0;
if ( xx-(int16(xx)) == 0 )
	disp('2.0 == integer!');
end

inname = 'hello';
disp(inname);
