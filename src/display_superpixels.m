function itm = display_superpixels(label, img)
  % part of boundaries are ploted
  boundaries = abs(imfilter(label, [0 -1 1],'replicate'))>0 | abs(imfilter(label, [0 -1 1]', 'replicate'))>0;
  %boundaries = boundaries | abs(imfilter(label, [1 -1 0],'replicate'))>0 | abs(imfilter(label, [1 -1 0]', 'replicate'))>0;
  itm1 = img(:,:,1);
  itm2 = img(:,:,2);
  itm3 = img(:,:,3);
  itm1(boundaries) = 255;
  itm2(boundaries) = 0;
  itm3(boundaries) = 0;
  itm(:,:,1) = itm1;
  itm(:,:,2) = itm2;
  itm(:,:,3) = itm3;
end