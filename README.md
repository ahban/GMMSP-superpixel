# GMMSP

Implementation for our work of "Superpixel Segmentation Using Gaussian Mixture Model". The paper is under reviewing.

# Citation

Currently, you can cite our work using the following bibtex. 

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
To run the code, you need Windows x64 OS and MATLAB.
If you already have them, just clone this repository to your local machine by
```
git clone https://github.com/ahban/GMMSP.git
```
and dobule click `gmmsp_demo.m` on your own local machine.


I took the picture of `cat.png` in the `images` path by myself. There is no copyright for this image.

## trouble

You may get an error that invlaid modules or some modules can not be found. 
This is because some DLLs are missing on your PC. 
You have to install "Visual C++ Redistributable Packages for Visual Studio 2013" from 
[https://www.microsoft.com/en-us/download/details.aspx?id=40784](https://www.microsoft.com/en-us/download/details.aspx?id=40784),
and make sure the installed DLLs can be found in `%PATH%`.
I have copied three dependences, it should work on most PCs. 
But if it cannot, please install the Redistributable Pacages by yourself.



# Some results

![](https://github.com/ahban/GMMSP/blob/master/results/cat8x8.png)
![](https://github.com/ahban/GMMSP/blob/master/results/cat14x14.png)
![](https://github.com/ahban/GMMSP/blob/master/results/cat20x20.png)
![](https://github.com/ahban/GMMSP/blob/master/results/cat26x26.png)

# Contact

If you need help, please contact me at sawpara[at]126.com


