
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
  itm = display_superpixels(label, img);
  figure; imshow(itm);
  title(['$$v_x=', num2str(v_x), ', v_y=', num2str(v_y), '$$'], 'interpreter','latex');  

  imwrite(itm, ['results/', image_stem, num2str(v_x), 'x', num2str(v_y), '.png']);

end