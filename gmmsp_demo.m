clear
clc

addpath ./src

% image and v_x,v_y
img = imread('images/cat.png');
v_x = 22;
v_y = v_x;

% call GMMSP
label = mx_GMMSP(img, v_x, v_y);

% show result
boundaries = abs(imfilter(label, [-1 1],'replicate'))>0 | abs(imfilter(label, [-1 1]', 'replicate'))>0;  
itm1 = img(:,:,1);
itm2 = img(:,:,2);
itm3 = img(:,:,3);
itm1(boundaries) = 255;
itm2(boundaries) = 0;
itm3(boundaries) = 0;
itm(:,:,1) = itm1;
itm(:,:,2) = itm2;
itm(:,:,3) = itm3;
imshow(itm)

imwrite(itm, 'result.png');

  







