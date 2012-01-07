%	彩色图像的双线性插值程序 (2007-05-03 17:35)
%	分类： MATLAB
%	文件: 	bilinear_rgb.rar
%	大小: 	56KB
%	简单说明：
%	1.文件：
%		bilinear.m  matlab源程序
%		FreeBSD.jpg  插值之前的RGB图像，大小为200x211
%		FreeBSD2.jpg  插值之后的RGB图像，大小为400x422（即放大2倍）
%	2、程序运行说明
%		我初步将允许放大的比例设置为1-9之间，当然也可以修改程序使之可以放大到更大比例，但是这样计算量将会很大，慎用。程序启动后将会询问要放大的倍数，请输入1到9之间的数（可以是小数，比如3.14）。如果输入的放大比例不是1-9之间的数，程序不会执行，而是直接退出。
%	完整源程序如下：

% THIS PROGRAMME IS WRITTEN BY Rockins
% THE FEATURE IS BILINEAR-INTERPOLATE THE SOUCE-IMAGE TO GET A DESTINATE-IMAGE
% THE MAXIMUM SCALOR == 9.0, THE MINIMUM SCALOR == 1.0
% Copyright 2006-2007,All Copyrights(C) Reserved by Rockins
% You can redistibute this programme under the GNU Less GPL license
% If you have any question about this programme,please contact me via
% ybc2084@163.com

% read source image into memory,and get the primitive rows and cols
RGB = imread('test_in.jpg');
R = RGB(:,:,1);
G = RGB(:,:,2);
B = RGB(:,:,3);
[nrows,ncols,ncoms]=size(RGB);

% Next line is the scale-factor,the range is 1.0-9.0
K = str2double(inputdlg('please input scale factor (must between 1.0 - 9.0)', 'INPUT scale factor', 1, {'5.0'}));

% Validating
if (K < 1.0) | (K > 9.0)
    errordlg('scale factor beyond permitted range(1.0 - 9.0)', 'ERROR');
    error('please input scale factor (must between 1.0 - 9.0)');
end

% display source image
imshow(RGB);

% output image width and height are both scaled by factor K
width = K * ncols;
height = K * nrows;
RR = uint8(zeros(height,width));
GG = uint8(zeros(height,width));
BB = uint8(zeros(height,width));
OUT = uint8(zeros(height,width,ncoms));

% width scalor and height scalor
widthScale = 1/K;
heightScale = 1/K;

% bilinear interpolate
for x = K:width-K % this index range is to avoid exceeding the permitted matrix index
   for y = K:height-K
       xx = x * widthScale; % xx and yy are the source ordinate,while x and y are the destinate ordinate
       yy = y * heightScale;
       if (xx <= 1.0e-8)
          xx = 1;
       end
       if (xx > ncols - 1)
          xx = ncols - 1;
       end
       if (yy <= 1.0e-8)
          yy = 1;
       end
       if (yy > nrows - 1)
          yy = nrows - 1;
       end
       
       if (xx/double(uint16(xx)) == 1.0) && (yy/double(uint16(yy)) == 1.0) % if a and b is integer,then J(x,y) <- I(x,y)
           RR(y,x) = R(int16(yy),int16(xx));
           GG(y,x) = G(int16(yy),int16(xx));
           BB(y,x) = B(int16(yy),int16(xx));
       else % a or b is not integer
           a = double(fix(yy)); % (a,b) is the base-dot
           b = double(fix(xx));
           
           r11 = double(R(a,b));
           r12 = double(R(a,b+1));
           r21 = double(R(a+1,b));
           r22 = double(R(a+1,b+1));
           RR(y,x) = uint8( (b+1-xx) * ((yy-a)*r21 + (a+1-yy)*r11) + (xx-b) * ((yy-a)*r22 +(a+1-yy) * r12) );
           
           g11 = double(G(a,b));
           g12 = double(G(a,b+1));
           g21 = double(G(a+1,b));
           g22 = double(G(a+1,b+1));
           GG(y,x) = uint8( (b+1-xx) * ((yy-a)*g21 + (a+1-yy)*g11) + (xx-b) * ((yy-a)*g22 +(a+1-yy) * g12) );
           
           b11 = double(B(a,b));
           b12 = double(B(a,b+1));
           b21 = double(B(a+1,b));
           b22 = double(B(a+1,b+1));
           BB(y,x) = uint8( (b+1-xx) * ((yy-a)*b21 + (a+1-yy)*b11) + (xx-b) * ((yy-a)*b22 +(a+1-yy) * b12) );
       end
    end
end

OUT(:,:,1) = RR;
OUT(:,:,2) = GG;
OUT(:,:,3) = BB;
% show the interplated image
imwrite(OUT, 'test_out.jpg', 'jpg');
figure;
imshow(OUT);
