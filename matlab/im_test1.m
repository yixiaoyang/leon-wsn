%------------------------------------------------------------------
% xiaoyang @2011-12-14
% CFA interpolation testing
%------------------------------------------------------------------
in_filename = 'test_in.jpg';
out_filename = 'test_out2.jpg';

% image scale
scale = 1.0;
wscale = 1/scale;
hscale = 1/scale;
%------------------------------------------------------------------
RGB=imread(in_filename);
R = RGB(:,:,1);
G = RGB(:,:,2);
B = RGB(:,:,3);
[nrows,ncols,ncoms]=size(RGB);

% display source image
imshow(RGB);

% interpolation
width = scale * ncols;
height = scale * nrows;
RR = uint8(zeros(height,width));
GG = uint8(zeros(height,width));
BB = uint8(zeros(height,width));
OUT = uint8(zeros(height,width,ncoms));

% bilinear interpolation
for x = scale : width-scale
	for y = scale : height-scale
		xx = x*wscale;
		yy = y*hscale;
		
		% reach boundry
		if(xx <= 10e-8)
			xx = 1;
		end
		if(xx > ncols-1)
			xx = ncols-1;
		end
		
		if(yy <= 10e-8)
			yy = 1;
		end
		if(yy > nrows-1 )
			yy = nrows;
		end
		
		% interpolation
		if( ( xx-(int16(xx)) == 0 ) && ( yy-(int16(yy)) == 0 ) )
			
		else
				
		end
	end
end

% end of bilinear interpolation

OUT(:, :, 1) = RR;
OUT(:, :, 2) = GG;
OUT(:, :, 3) = BB;
imwrite(OUT,out_filename,'jpg');
figure;
imshow(OUT);

% end of script
