%% Read image, Convert to RGB format
RGB = imread('test.bmp');
%imshow(RGB);
[w,h,a]=size(RGB);
%w=16;
%h=16;
Bayer=zeros(w,h,'uint8');
%R 
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        if(bitand(i0,1) && bitand(j0,1))   %都奇数,B、R分量
            Bayer(i,j)=RGB(i,j,1);
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))%其中一个为奇数，G分量
            Bayer(i,j)=RGB(i,j,2);
        elseif (bitxor(i0,1)&&bitxor(j0,1)) %偶数，B、R分量
         Bayer(i,j)=RGB(i,j,3);  
        end
    end
end


%% Demosaic，block size:2*2
newimg=zeros(w,h,3,'uint8');
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        %% Process Red Part
        if (bitand(i0,1) && bitand(j0,1))
            newimg(i,j,1)=Bayer(i,j);
            if((i>1&& j>1)&&(i<w&&j<h))
            	newimg(i,j,2)=(Bayer(i+1,j)+Bayer(i,j+1))/2; 	%Green分量
            	newimg(i,j,3)=Bayer(i+1,j+1);	%Blue分量
            end
        
        %% Process Green Part
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))
            
            newimg(i,j,2)=Bayer(i,j);
            if( bitand(i0,1) && bitxor(j0,1) )
                        if((i>1&& j>1)&&(i<w&&j<h))
                            newimg(i,j,1)=Bayer(i,j-1); %% R
                            newimg(i,j,3)=Bayer(i+1,j); %% B
                        end
            end
            if (bitxor(i0,1)&&bitand(j0,1))
                        if((i>1&& j>1)&&(i<w&&j<h))
                            newimg(i,j,1)=Bayer(i-1,j);
                            newimg(i,j,3)=Bayer(i,j+1);
                        end
            end
       
        %% Process Blue Part
        elseif(bitxor(i0,1)&&bitxor(j0,1))
             newimg(i,j,3)=Bayer(i,j);  
                     if((i>1&& j>1)&&(i<w&&j<h))
                         newimg(i,j,2)=(Bayer(i,j-1)+Bayer(i-1,2))/2;
                         newimg(i,j,1)=Bayer(i-1,j-1);
                     end
        end
    end
end

disp('write img1');
imwrite(newimg, 'new1.jpg', 'jpg');

%% Demosiac 2，block size:
newimg2=zeros(w,h,3,'uint8');
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        %% Process Red Part
        if (bitand(i0,1) && bitand(j0,1))
            newimg2(i,j,1)=Bayer(i,j);
            if((i>2&& j>2)&&(i<w-1&&j<h-1))
            	newimg2(i,j,2)=(Bayer(i+1,j)+Bayer(i,j+1))/2;
                newimg2(i,j,3)=(Bayer(i+1,j+1)+Bayer(i-1,j+1))/2;
            end
        
        %% Process Green Part
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))
            
            newimg2(i,j,2)=Bayer(i,j);
            if( bitand(i0,1) && bitxor(j0,1) )
                        if((i>1&& j>2) && (i<w&&j<h-1))
                            newimg2(i,j,1)=(Bayer(i,j-1)+Bayer(i,j+1))/2; %% R
                            newimg2(i,j,3)=(Bayer(i+1,j)+Bayer(i+1,j+2))/2; %% B
                        end
            end
            if (bitxor(i0,1)&&bitand(j0,1))
                        if((i>1&& j>2) && (i<w-1&&j<h-1))
                            newimg2(i,j,1)=(Bayer(i-1,j)+Bayer(i-1,j+2))/2;
                            newimg2(i,j,3)=(Bayer(i,j+1)+Bayer(i,j-1))/2;
                        end
            end
       
        %% Process Blue Part
        elseif(bitxor(i0,1)&&bitxor(j0,1))
             newimg2(i,j,3)=Bayer(i,j);  
                     if((i>2&& j>2)&&(i<w-1&&j<h-1))
                         newimg2(i,j,1)=(Bayer(i-1,j-1)+Bayer(i+1,j+1))/2;
                         newimg2(i,j,2)=(Bayer(i,j-1)+Bayer(i-1,2))/2;

                     end
        end
    end
end

disp('write img2');
imwrite(newimg2, 'new2.jpg', 'jpg');


%% Demosaic 3 reduce size
%newimg3=zeros(w/2,h/2,3,'uint8');
%i0=1;j0=1;
%for i=1:2:w
%    for j=1:2:h
%            newimg3(i0,j0,1)=Bayer(i,j);
%            newimg3(i0,j0,2)=(Bayer(i+1,j)+Bayer(i,j+1))/2;
%            newimg3(i0,j0,3)=Bayer(i+1,j+1);
%                      j0=j0+1;     
%            
%    end

%          i0=i0+1;
%end

%disp('write img3');
%imwrite(newimg3, 'new3.jpg', 'jpg');

%% Demosaic，block size:3*3 
newimg4=zeros(w,h,3,'uint8');
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        %% Process Red Part
        if (bitand(i0,1) && bitand(j0,1))
            newimg4(i,j,1)=Bayer(i,j);
            if((i>1&& j>1)&&(i<w&&j<h))
            	if(icompare( Bayer(i,j-1),Bayer(i,j+1), Bayer(i+1,j),Bayer(i-1,j) ) == 1)
            		newimg4(i,j,2)=( Bayer(i+1,j)+Bayer(i-1,j) ) / 2;
            	elseif(icompare( Bayer(i,j-1),Bayer(i,j+1), Bayer(i+1,j),Bayer(i-1,j) ) == -1)
            		newimg4(i,j,2)=( Bayer(i,j+1) + Bayer(i,j-1) ) /2; 	%Green分量
            	else 
            		newimg4(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4; 	%Green分量
            	end
	        end
	     %% Process Green Part,有两种分布方式
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))
            newimg5(i,j,2)=Bayer(i,j);
            if( bitand(i0,1) && bitxor(j0,1) )
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i,j-1) + Bayer(i,j+1))/2; %Red
                    newimg5(i,j,3)=(Bayer(i+1,j) + Bayer(i-1,j))/2; %Blue
                end
            end
            if (bitxor(i0,1)&&bitand(j0,1))
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i-1,j) + Bayer(i+1,j))/2; %Red
                    newimg5(i,j,3)=(Bayer(i,j+1) + Bayer(i,j-1))/2; %Blue
                end
            end
        %% Process Blue Part
        elseif(bitxor(i0,1)&&bitxor(j0,1))
             newimg4(i,j,3)=Bayer(i,j);  
             if((i>1&& j>1)&&(i<w&&j<h))
            	if(icompare( Bayer(i,j-1),Bayer(i,j+1), Bayer(i+1,j),Bayer(i-1,j) ) == 1)
            		newimg4(i,j,2)=( Bayer(i+1,j)+Bayer(i-1,j) ) / 2;
            	elseif(icompare( Bayer(i,j-1),Bayer(i,j+1), Bayer(i+1,j),Bayer(i-1,j) ) == -1)
            		newimg4(i,j,2)=( Bayer(i,j+1) + Bayer(i,j-1) ) /2; 	%Green分量
            	else 
            		newimg4(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4; 	%Green分量
            	end
	        end
        end
    end
end

disp('write img4');
imwrite(newimg4, 'new4.jpg', 'jpg');

%-------------------------------------------------------------------------------------------
% 
%-------------------------------------------------------------------------------------------
%% Demosaic，block size:3*3,梯度插值，第一遍扫描，只计算
newimg5=zeros(w,h,3,'uint8');
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        %% Process Red Part
        if (bitand(i0,1) && bitand(j0,1))
            newimg5(i,j,1)=Bayer(i,j);
            if((i>1&& j>1)&&(i<w&&j<h))
            	newimg5(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4; 	%Green分量
            	newimg5(i,j,3)=(Bayer(i+1,j+1) + Bayer(i-1,j-1) + Bayer(i-1,j+1) + Bayer(i+1,j-1))/4;	%Blue分量
            end
        
        %% Process Green Part,有两种分布方式
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))
            newimg5(i,j,2)=Bayer(i,j);
            if( bitand(i0,1) && bitxor(j0,1) )
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i,j-1) + Bayer(i,j+1))/2; %Red
                    newimg5(i,j,3)=(Bayer(i+1,j) + Bayer(i-1,j))/2; %Blue
                end
            end
            if (bitxor(i0,1)&&bitand(j0,1))
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i-1,j) + Bayer(i+1,j))/2; %Red
                    newimg5(i,j,3)=(Bayer(i,j+1) + Bayer(i,j-1))/2; %Blue
                end
            end
       
        %% Process Blue Part
        elseif(bitxor(i0,1)&&bitxor(j0,1))
             newimg5(i,j,3)=Bayer(i,j);  
             if((i>1&& j>1)&&(i<w&&j<h))
                 newimg5(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4;
                 newimg5(i,j,1)=(Bayer(i+1,j+1) + Bayer(i-1,j-1) + Bayer(i-1,j+1) + Bayer(i+1,j-1))/4;%Red
             end
        end
    end
end

% 第二遍扫描
for i=1:w
    for j=1:h
        i0=bitget(i,1);j0=bitget(j,1);
        %% Process Red Part
        if (bitand(i0,1) && bitand(j0,1))
            newimg5(i,j,1)=Bayer(i,j);
            if((i>1&& j>1)&&(i<w&&j<h))
            	newimg5(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4; 	%Green分量
            	newimg5(i,j,3)=(Bayer(i+1,j+1) + Bayer(i-1,j-1) + Bayer(i-1,j+1) + Bayer(i+1,j-1))/4;	%Blue分量
            end
        
        %% Process Green Part,有两种分布方式
        elseif ( (bitand(i0,1) && bitxor(j0,1) )||(bitxor(i0,1)&&bitand(j0,1)))
            newimg5(i,j,2)=Bayer(i,j);
            if( bitand(i0,1) && bitxor(j0,1) )
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i,j-1) + Bayer(i,j+1))/2; %Red
                    newimg5(i,j,3)=(Bayer(i+1,j) + Bayer(i-1,j))/2; %Blue
                end
            end
            if (bitxor(i0,1)&&bitand(j0,1))
                if((i>1&& j>1)&&(i<w&&j<h))
                    newimg5(i,j,1)=(Bayer(i-1,j) + Bayer(i+1,j))/2; %Red
                    newimg5(i,j,3)=(Bayer(i,j+1) + Bayer(i,j-1))/2; %Blue
                end
            end
       
        %% Process Blue Part
        elseif(bitxor(i0,1)&&bitxor(j0,1))
             newimg5(i,j,3)=Bayer(i,j);  
             if((i>1&& j>1)&&(i<w&&j<h))
                 newimg5(i,j,2)=(Bayer(i+1,j) + Bayer(i,j+1) + Bayer(i-1,j) + Bayer(i,j-1))/4;
                 newimg5(i,j,1)=(Bayer(i+1,j+1) + Bayer(i-1,j-1) + Bayer(i-1,j+1) + Bayer(i+1,j-1))/4;%Red
             end
        end
    end
end

disp('write img5');
imwrite(newimg5, 'new5.jpg', 'jpg');

disp('test over!');
