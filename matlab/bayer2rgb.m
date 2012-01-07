% bayer2rgb   conversion from bayer pattern to rgb
%             patt: 	bayer pattern
%             mode: 	one of both of this
%                      		's' spline
%		       				'1' inverted image
%	         sizeim: desired sizxe for the image (if spline mode)
%            imageout: rgb image			  

% Dana Cobzas

function  imageout = bayer2rgb(patt, mode, sizeim)

if(nargin<2)mode='';end;
if(nargin<3)sizeim=[320 240];end;

[m,n]=size(patt);
w_r = 1.2;%1.4;
w_b = 1.2;%1.4;
w_g = 1.2;

r_rows = [1:2:m];
r_cols = [1:2:n];
g1_rows = [1:2:m];
g1_cols = [2:2:n];
g2_rows = [2:2:m];
g2_cols = [1:2:n];
b_rows =  [2:2:m];
b_cols =  [2:2:n];

   
if length(findstr(mode,'s'))>0 |  nargin == 3 
 
  % spline interpolation
  imageout = zeros(sizeim(2),sizeim(1),3);
  [y1,y2] = meshgrid(1:sizeim(1),1:sizeim(2));
  y1 = double(y1(:));
  y2 = double(y2(:));
  v1 = size(patt,1)*y1/sizeim(1);
  v2 = size(patt,2)*y2/sizeim(2);

  rout = imageout(:,:,1)';
  r = double(patt(r_rows,r_cols));
  grd = {r_rows,r_cols};
  rout((y2-1)*sizeim(1)+y1) = splncore(grd,r,{v1,v2});

  gout = imageout(:,:,2)';
  g = double(patt(g1_rows,g1_cols)); 
  grd = {g1_rows,g1_cols};
  gout((y2-1)*sizeim(1)+y1) = splncore(grd,g,{v1,v2});
  g = double(patt(g2_rows,g2_cols));
  grd = {g2_rows,g2_cols};
  gout((y2-1)*sizeim(1)+y1) =(gout((y2-1)*sizeim(1)+y1) +splncore(grd,g,{v1,v2}))/2;
  
  bout = imageout(:,:,3)';
  b = double(patt(b_rows,b_cols));
  grd = {b_rows,b_cols};
  bout((y2-1)*sizeim(1)+y1) = splncore(grd,b,{v1,v2});

  imageout(:,:,1)=(w_r*rout)';
  imageout(:,:,2)=(w_g*gout)';
  imageout(:,:,3)=(w_b*bout)';
  
  imageout = max(min(imageout,255),0);
  imageout = imageout/255;
  
else % half resolution
  imageout(:,:,1)=(min(w_r*double(patt(r_rows,r_cols)),255))';
  imageout(:,:,3)=(min(w_b*double(patt(b_rows, b_cols)),255))';
  imageout(:,:,2)=(min(w_g*(double(patt(g1_rows,g1_cols))+double(patt(g2_rows,g2_cols)))/2,255))';
  imageout = imageout/255;
 
end

