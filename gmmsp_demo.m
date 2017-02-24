
clear
clc

addpath ./src

mkdir('results')

image_name = 'images/cat.png';
[~, image_stem] = fileparts(image_name);

close all
% image and v_x,v_y
img = imread(image_name);

for v_x = 8:6:26
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
  figure; imshow(itm);
  title(['$$v_x=', num2str(v_x), ', v_y=', num2str(v_y), '$$'], 'interpreter','latex');  

  imwrite(itm, ['results/', image_stem, num2str(v_x), 'x', num2str(v_y), '.png']);

end