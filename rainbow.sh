#!/bin/bash
matrix_array=("breasttissue_10NN" "iris_dataset_30NN" "YaleA_10NN" "Glass_10NN" "Ecoli_10NN" "dermatology_5NN" "Olivetti_norm_10NN" "Spectro_10NN" "micromass_10NN" "umistfacesnorm_10NN" "Vehicle_10NN" "collins_15NN" "YaleB_10NN" "Goodwin_017" "freeFlyingRobot_8" "s2rmt3ml" "freeFlyingRobot_16" "kineticBatchReactor_6" "TSC_OPF_1047" "bips98_1142" "Fashion_MNIST_norm_10NN" "pkustk01" "rgg_n_2_15_s0" "sme3Dc" "c20stif" "delaunay_n16" "nv1" "consph" "ifiss_mat" "smallworld" "CurlCurl_1" "Linux_call_graph" "pa2010" "delaunay_n19" "Fault_639" "Chevron4" "flickr" "Emilia_923" "ecology1" "nd24k" "Si41Ge41H72" "Ga41As41H72" "pwtk" "pdb1HYS" "cant" "crankseg_2" "shipsec1")

echo "$1"
echo "$2"
echo "$3"
str_model=""
if [ "${1}" -eq "1" ]  
then
    model="./CSR_SPMV"
    str_model=$str_model$model
fi
if [ "${1}" -eq "2" ] 
then
    model="./CSR2_C"
    str_model=$str_model$model
fi
if [ "${1}" -eq "3" ]
then
    model="./CSR2_S"
    str_model=$str_model$model
fi
if [ "${1}" -eq "4" ]
then
    model="./CSR2_C_MP"
    str_model=$str_model$model
fi

src_path="../../dataset/SuitSparse/"
echo "${matrix_array[${2}]}"
final_path="$src_path""${matrix_array[${2}]}""/""${matrix_array[${2}]}"".mtx ""$src_path""${matrix_array[${2}]}""/""${matrix_array[${2}]}""_label.mtx"
echo "$final_path"
#echo "$str_model"
#path=$str${2}
#echo "$path"
$str_model $final_path ${3}

