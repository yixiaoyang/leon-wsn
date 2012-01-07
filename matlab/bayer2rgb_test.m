in_filename = 'test.bmp';
out_filename = 'bayer2rgb.jpg';
RGB=imread(in_filename);
sizeim=[240 320];
imageout = bayer2rgb(RGB, 's',sizeim);
imwrite(imageout,out_filename,'jpg');
figure;
imshow(imageout);

