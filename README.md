# SSearch

SSearch is a sketch-based image retrieval web application, you can click [here](http://omap.fudan.edu.cn/sketch_based_image_retrieval) to see the live demo. This repository is the basic version of SSearch.

By SSearch, you can draw a sketch on the drawing board，then the system will return similar images.The algorithm used in this application is based on our paper posted on ACM ICMR2015.

![此处输入图片的描述](http://7xjuf4.com1.z0.glb.clouddn.com/sketch_sketch.PNG)


## System Workerflow

- 1) client send image data(Base64)
- 2) thrift RPC transport image data to server
- 3) server compute the image data, then return the result to clinet by thrift RPC 

## Client

Written in nodejs, providing a drawing board for user to draw. So when you deploy this application, nodejs should be installed first.

## Server

Written in C++, computing the similarity between the image data and the images in the image database, then return the most similar images;

## Communication

thrift RPC
