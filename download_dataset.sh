# 工作目录
data_name="dataset"
if [ ! -d "$data_name" ]; then # 判断目录是否存在
  mkdir "$data_name" # 创建目录
  echo "目录 $data_name 创建成功！"
fi
cd "$data_dir"
# 下载paper中的测试矩阵
wget https://suitesparse-collection-website.herokuapp.com/MM/ND/nd24k.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/PARSEC/Si41Ge41H72.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/PARSEC/Ga41As41H72.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Boeing/pwtk.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Williams/cant.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Williams/pdb1HYS.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/GHS_psdef/crankseg_2.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DNVS/shipsec1.tar.gz
# 下载1000以下的测试矩阵
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/breasttissue_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/iris_dataset_30NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/YaleA_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Glass_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Ecoli_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/dermatology_5NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Olivetti_norm_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Spectro_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/micromass_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/umistfacesnorm_10NN.tar.gz 
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Vehicle_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/collins_15NN.tar.gz
# 下载1w以下的测试矩阵
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/YaleB_10NN.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Goodwin/Goodwin_017.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/VDOL/freeFlyingRobot_8.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Cylshell/s2rmt3m1.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/VDOL/freeFlyingRobot_16.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/VDOL/kineticBatchReactor_6.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/IPSO/TSC_OPF_1047.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Rommes/bips98_1142.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/ML_Graph/Fashion_MNIST_norm_10NN.tar.gz
# 下载10w以下的测试矩阵
wget https://suitesparse-collection-website.herokuapp.com/MM/Chen/pkustk01.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/rgg_n_2_15_s0.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/FEMLAB/sme3Dc.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Boeing/ct20stif.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/delaunay_n16.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/VLSI/nv1.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Williams/consph.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Embree/ifiss_mat.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/smallworld.tar.gz
# 下载100w以下的测试矩阵
wget https://suitesparse-collection-website.herokuapp.com/MM/Bodendiek/CurlCurl_1.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Sorensen/Linux_call_graph.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/pa2010.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/delaunay_n19.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Janna/Fault_639.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Chevron/Chevron4.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Gleich/flickr.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/Janna/Emilia_923.tar.gz
wget https://suitesparse-collection-website.herokuapp.com/MM/McRae/ecology1.tar.gz

# 循环解压
for file in *.gz; do
    tar -xvzf "$file"
    rm "$file"
done

# 规范数据集文件名,删除除matrix以外的所有文件
for dir in */; do
    # 进入目录
    cd "$dir"
    # 删除非 .mtx 结尾的文件
    for file in *; do
        if [[ ! "$file" =~ "${dir%/}.mtx$" ]]; then
            echo "Deleting file $dir$file"
            rm -rf "$file"
        fi
    done
    # 回到上级目录
    cd ..
done
# 生成规范命名的乘向量
# 遍历一级目录
for dir in */; do
    # 获取目录名
    dir_name=$(basename "$dir")
    # 获取匹配的文件名
    file_name="$dir_name.mtx"
    # 读取第一行并存入临时变量
    if [[ -f "$dir$file_name" ]]; then
        line=$(head -n1 "$dir$file_name")
        #gl $dir$file_name+"_label.mtx" "$result"
        echo "gl" $dir$file_name+"_label.mtx" "$line"
    fi
done


# 数据预处理删除注释
formatmtx ./
