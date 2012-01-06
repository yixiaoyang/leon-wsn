sourcePic=imread('fig3.tif');
%以下为了彩色图像
%[m,n,o]=size(sourcePic);
%grayPic=rgb2gray(sourcePic);
grayPic=sourcePic;
[m,n]=size(grayPic);

%比例系数为0.2-5.0
K = str2double(inputdlg('请输入比例系数(0.2 - 5.0)', '输入比例系数', 1, {'0.5'}));

%验证范围
if (K < 0.2) or (K > 5.0)
    errordlg('比例系数不在0.2 - 5.0范围内', '错误');
    error('请输入比例系数(0.2 - 5.0)');
end

figure;
imshow(grayPic);

%输出图片长宽
width = K * m;                          
height = K * n;
resultPic = uint8(zeros(width,height));

widthScale = n/width;
heightScale = m/height;

for x = 5:width-5                             
   for y = 5:height-5
       xx = x * widthScale;                    
       yy = y * heightScale;
       if (xx/double(uint16(xx)) == 1.0) && (yy/double(uint16(yy)) == 1.0)       
% if xx and yy is integer,then J(x,y) <- I(x,y)
           resultPic(x,y) = grayPic(int16(xx),int16(yy));
       else                                           
			% xx or yy is not integer
           a = double(uint16(xx));                    
% (a,b) is the base-dot
           b = double(uint16(yy));
           x11 = double(grayPic(a,b));                
% x11 <- I(a,b)
           x12 = double(grayPic(a,b+1));              
% x12 <- I(a,b+1)
           x21 = double(grayPic(a+1,b));              
% x21 <- I(a+1,b)
           x22 = double(grayPic(a+1,b+1));            
% x22 <- I(a+1,b+1)          
           resultPic(x,y) = uint8( (b+1-yy) * ((xx-a)*x21 + (a+1-xx)*x11) + (yy-b) * ((xx-a)*x22 +(a+1-xx) * x12) );
       end
    end
end

figure;
resultPic = imrotate(resultPic,-20);
imshow(resultPic);

