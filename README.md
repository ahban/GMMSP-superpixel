# GMMSP

Implementation for our work of "Superpixel Segmentation Using Gaussian Mixture Model". 

A parallel implementation on GPU can be found at [gGMMSP](https://github.com/ahban/gGMMSP) which runs very fast (about 1000FPS for 320x240 images on GTX 1080).

# Citation

This method has been publised as a regular paper in IEEE Transactions on Image Processing.
We would appreciate it if you develop your work based on our method and, of course, if you cite our paper.
The new bibtex is as follows.

```
@article{Ban18,
  author   =  {Zhihua Ban and Jianguo Liu and Li Cao},
  journal  =  {IEEE Transactions on Image Processing},
  title    =  {Superpixel Segmentation Using Gaussian Mixture Model},
  year     =  {2018},
  volume   =  {27},
  number   =  {8},
  pages    =  {4105-4117},
  doi      =  {10.1109/TIP.2018.2836306}
}
```

This work was uploaded to arXiv two years ago with the following bibtex.
**please note that the following paper is an early version of this work. If you want to understand the algorithm, you have to find the paper above. Unfortunatly, I cannot upload the pdf of the above paper due to IEEE copyright :(.**
```
@article{Ban16,
  author    = {Zhihua Ban and Jianguo Liu and Li Cao},
  title     = {Superpixel Segmentation Using Gaussian Mixture Model},
  journal   = {{arXiv} preprint},
  volume    = {1612.08792},
  year      = {2016},
  url       = {http://arxiv.org/abs/1612.08792}
}
```


# Run the code
To run the code, you need Windows x64 OS and MATLAB (**at least R2015a**), or Ubuntu 14.04 64-bit and MATLAB (**at least R2015b**).
If you already have them, just clone this repository to your local machine by
```
git clone https://github.com/ahban/GMMSP.git
```
and dobule click `gmmsp_demo.m` on your own local machine.


I took the picture of `cat.png` in the `images` path by myself. There is no copyright for this image.

## trouble

You may get an error that invlaid modules or some modules can not be found. 
This is because your PC misses some DLLs.
You have to install ***Visual C++ Redistributable Packages for Visual Studio 2013*** from 
[https://www.microsoft.com/en-us/download/details.aspx?id=40784](https://www.microsoft.com/en-us/download/details.aspx?id=40784),
and make sure the installed DLLs can be found in `%PATH%`.
I have copied three dependences, it should work on most PCs. 
But if it cannot, please install the Redistributable Packages by yourself.

## compile the code

You are encouraged to use the compiled binaries for both Windows and Linux. 
However, if you prefer, you can compile the code by yourself, as you may want to try some new compilers.

We provide a script for your own compilation in `src`.
Just change directory to `src`, and run `compileme.m`.


# Some results

![](https://raw.githubusercontent.com/ahban/GMMSP/master/results/cat8x8.png)
![](https://raw.githubusercontent.com/ahban/GMMSP/master/results/cat14x14.png)
![](https://raw.githubusercontent.com/ahban/GMMSP/master/results/cat20x20.png)
![](https://raw.githubusercontent.com/ahban/GMMSP/master/results/cat26x26.png)

# Contact

If you need help, please contact me at sawpara[at]126.com

