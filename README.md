# CSR2
# 将一篇论文在龙芯3C5000上复现，
[A New Format for SIMD-accelerated SpMV](https://ieeexplore.ieee.org/document/9139720)
```
@INPROCEEDINGS{9139720, 
  author={Bian, Haodong and Huang, Jianqiang and Dong, Runting and Liu, Lingbin and Wang, Xiaoying},  
  booktitle={2020 20th IEEE/ACM International Symposium on Cluster, Cloud and Internet Computing (CCGRID)},   
  title={CSR2: A New Format for SIMD-accelerated SpMV},   
  year={2020},  
  volume={},  
  number={},  
  pages={350-359},  
  doi={10.1109/CCGrid49817.2020.00-58}
}
```
Paper :

H. Bian, J. Huang, R. Dong, L. Liu and X. Wang, "CSR2: A New Format for SIMD-accelerated SpMV," 2020 20th IEEE/ACM International Symposium on Cluster, Cloud and Internet Computing (CCGRID), Melbourne, Australia, 2020, pp. 350-359, doi: 10.1109/CCGrid49817.2020.00-58.

## manual:

1. make
2. CSR_2 mtx_file_name vec_file_name times

对于没有vec的文件先用gl工具生成测试的vec
对于文件中的注释，需要删除，可以使用formatmtx工具进行删除。
