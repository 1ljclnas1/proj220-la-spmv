#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace std;
#define INT int
#define DOU double
#define my256d double*
extern "C" inline void my256dmul(my256d src1, my256d src2, my256d dst);
extern "C" inline void my256dadd(my256d src1, my256d src2, my256d dst);
extern "C" inline void my256dload(my256d dst, double* x1, double* x2, double* x3, double* x4);
extern "C" inline void my256dfmadd(my256d mul1, my256d mul2, my256d add_ret);
//申请连续内存空间
void my256dset(my256d *&src, int len){
	double *data = new double[4*len];
    for(int i=0;i<len;i++){
		src[i] = &data[i*4];
    }
}
void ReadFileConvertCsr(FILE *& fp_mtx,FILE *& fp_vec,INT *&row_ptr,INT *&col_idx,double *&mtx_val,double *&vec_val, INT *&par_set)
{
    INT row_num,col_num,nzz_num;
    fscanf(fp_mtx,"%d %d %d",&row_num,&col_num,&nzz_num);
    //-----------------------Read Vector-------------------------//
    INT vec_row;
    par_set = (INT*)malloc(sizeof(INT)*10);
    vec_row = col_num;
    fscanf(fp_vec,"%d",&vec_row);
    vec_val = (double*)aligned_alloc(64,sizeof(double)*vec_row);
    
    for(INT i=1;i<=vec_row;i++)
    {
        fscanf(fp_vec,"%lf",&vec_val[i]);
    }
    //-----------------------Read Matrix-------------------------//
    printf("(%d , %d) , nzz_num = %d\n",row_num,col_num,nzz_num);
    INT mtx_width;
    INT x,cnt_ite;
    x = (INT)nzz_num*1.0/row_num+0.5;
    if(x<=16)
    {
            mtx_width = x;
    }
    else
    {
            if(x>128)
            {
                    cnt_ite=0;
                    while(x>128)
                    {
                            if(x&1)
                            {
                                    x++;
                            }
                            x/=2;
                            if(cnt_ite==4)
                            {
                                    break;
                            }
                            cnt_ite++;
                    }
                    mtx_width = x;
            }
            else
            {
                    if(x>=36)
                    {
                            cnt_ite=0;
                            while(x>=36)
                            {
                                    if(x&1)
                                    {
                                            x++;
                                    }
                                    x/=2;
                                    if(cnt_ite==2)
                                    {
                                            break;
                                    }
                                    cnt_ite++;
                            }
                            mtx_width = x;
                    }
                    else
                    {
                            if(x&1)
                            {
                                    if(x%10>=5)
                                            x=(x/10+1)*10;
                                    else
                                            x=x/10*10;
                            }
                            x/=2;
                            mtx_width = x;
                    }
            }
    }
    printf("mtx_width = %d \n",mtx_width);
    INT mtx_high=4;
    INT str_max;
    str_max = nzz_num + row_num * (mtx_width-1);
	row_ptr = (INT *)aligned_alloc(64,sizeof(INT)*(row_num+1));
	col_idx = (INT *)aligned_alloc(64,sizeof(INT)*str_max);
	mtx_val = (double *)aligned_alloc(64,sizeof(double)*str_max);
	memset(mtx_val,0,sizeof(double)*str_max);
	memset(col_idx,0,sizeof(INT)*str_max);

	INT v=-1,cnt=0,cnt1;
    INT row,col;
    double nzz;
    for(INT i=0;i<nzz_num;i++)
    {
        fscanf(fp_mtx,"%d %d %lf",&col,&row,&nzz);
        row--;
        while(v<row)
        {
            v++;
            if(cnt%mtx_width!=0)
            {
                cnt=(cnt+mtx_width-1)/mtx_width*mtx_width;
            }
            row_ptr[v] = cnt;
        }
        col_idx[cnt] = col;
        mtx_val[cnt] = nzz;
        cnt++;
    }
    while(v<row_num)
    {
        v++;
        if(cnt%mtx_width!=0)
        {
            cnt=(cnt+mtx_width-1)/mtx_width*mtx_width;
        }
        row_ptr[v] = cnt;
    }
    if(cnt%mtx_width!=0)
    {
        cnt=(cnt+mtx_width-1)/mtx_width*mtx_width;
    }
    v++;
    row_ptr[v]=cnt;
    if(cnt%(mtx_width*mtx_high)!=0)
    {
        cnt=(cnt+mtx_width*mtx_high-1)/(mtx_width*mtx_high)*(mtx_width*mtx_high);
    }
    par_set[0] = cnt;
    par_set[1] = mtx_width;
    par_set[2] = mtx_high;
    par_set[3] = row_num+1;
    par_set[5] = par_set[0]/par_set[2];
    par_set[6] = par_set[5]/par_set[1];
    par_set[7] = nzz_num;
}

void CsrConvertCsr2(INT *&row_ptr,double *&mtx_val,INT *&Csr2_row_ptr,my256d *&Csr2_mtx_val_list,INT*& par_set){
    INT i,x,y,z,i1,t;

    const INT i_end  = par_set[3]; // row_num + 1
    const INT i_end1 = par_set[0]; // cnt == nzz_size/mtx_high*mtx_wight
    const INT mtx_width = par_set[1]; // mtx_width
    Csr2_mtx_val_list = new my256d[i_end1>>2];
    Csr2_row_ptr = (INT*)aligned_alloc(64,sizeof(INT)*i_end);
    my256dset(Csr2_mtx_val_list, i_end1>>2);

    for(i=0;i<i_end;i++)
    {
        Csr2_row_ptr[i]=row_ptr[i]/mtx_width;
    }
    for(i=0;i<i_end1;i++)
    {
        t = i/mtx_width;
        z = t>>2;
        z = z*mtx_width;
        x = z+i-t*mtx_width;
        y = (i-(z<<2))/mtx_width;
	Csr2_mtx_val_list[x][y] = mtx_val[i];
    }
}
/// @brief  记录每一行每个元素与vec相乘后的值并存入Csr2_mid_val
/// @param Csr2_mid_val 
/// @param Csr2_mtx_val 
/// @param col_idx 
/// @param vec_val 
/// @param par_set 
void Madd(my256d *&Csr2_mid_val,my256d *&Csr2_mtx_val,INT *&col_idx,double *&vec_val,INT *& par_set){

    const INT i_end     = par_set[6];
    const INT mtx_width = par_set[1];
    const INT mtx_high  = par_set[2];
    INT   i,j,xx,yy,zz;
    const INT m1 = mtx_width<<1;
    const INT m2 = m1 + mtx_width;
    my256d* Csr2_mid_val1=new my256d[1];
    my256d* Csr2_col_val=new my256d[1];
    my256dset(Csr2_mid_val1,1);
    my256dset(Csr2_col_val,1);
    my256d* tmp=new my256d[1];
    my256dset(tmp,1);
    for(i=0;i<i_end;i++)
    {
        xx=i*mtx_width;
        yy=xx<<2;
        my256dload(*Csr2_col_val, &vec_val[col_idx[yy]], &vec_val[col_idx[yy+mtx_width]],
			&vec_val[col_idx[yy+m1]], &vec_val[col_idx[yy+m2]]);
        (*Csr2_col_val)[3] = vec_val[col_idx[yy+m2]];
        (*Csr2_col_val)[2] = vec_val[col_idx[yy+m1]];
        (*Csr2_col_val)[1] = vec_val[col_idx[yy+mtx_width]];
        (*Csr2_col_val)[0] = vec_val[col_idx[yy]];
        my256dmul(Csr2_mtx_val[xx], *Csr2_col_val, *Csr2_mid_val1);

        xx=(xx<<1)+xx;
        zz=yy+mtx_width;
        for(j=yy+1;j<zz;j++)
        {
            //my256dload(*Csr2_col_val, &vec_val[col_idx[j]], &vec_val[col_idx[j+mtx_width]], &vec_val[col_idx[j+m1]], &vec_val[col_idx[j+m2]]);
            (*Csr2_col_val)[3] = vec_val[col_idx[j+m2]];
            (*Csr2_col_val)[2] = vec_val[col_idx[j+m1]];
            (*Csr2_col_val)[1] = vec_val[col_idx[j+mtx_width]];
            (*Csr2_col_val)[0] = vec_val[col_idx[j]];
            my256dfmadd(Csr2_mtx_val[j-xx],*Csr2_col_val,*Csr2_mid_val1);
            //my256dmul(Csr2_mtx_val[j-xx],*Csr2_col_val,*tmp);
            //my256dadd(*Csr2_mid_val1,*tmp,*Csr2_mid_val1);
        }
        
        //Csr2_mid_val[i]=*Csr2_mid_val1;
        for(int p=0;p<4;p++){
            Csr2_mid_val[i][p]=(*Csr2_mid_val1)[p];
        }
    }
    delete Csr2_mid_val1;
    Csr2_mid_val1=0;
    delete Csr2_col_val;
    Csr2_col_val=0;
}

/// @brief 将同一行的元素值加起来得到最终的值。
/// @param Csr2_row_ptr 
/// @param Csr2_mid_val 
/// @param mtx_ans 
/// @param par_set 
void SegSum(INT *&Csr2_row_ptr,my256d *&Csr2_mid_val,double *&mtx_ans,INT *&par_set){
    INT i,j;
    const INT num=par_set[3]-1;
    
    //每一行求最终值每一行
    for(i=0;i<num;i++)
    {
        double s=0;
        INT t1,t2;
        t1=Csr2_row_ptr[i];
        t2=Csr2_row_ptr[i+1];
        for(j=t1;j<t2;j++)
        {
            INT t=j>>2;
            INT p = j-(t<<2);
	    for(j=t1;j<t2;j++)
	    {
	        INT t=j>>2;
		s+=Csr2_mid_val[t][j-(t<<2)];
	    }
	    mtx_ans[i] = s;
        }
    }
    
    //释放内存
    //for(int i=0;i<par_set[6];i++){
    //	delete Csr2_mid_val[i];
    //	Csr2_mid_val[i]=0;
    //}
    //delete Csr2_mid_val;
    //Csr2_mid_val=0;
}
int main(int argc,char ** argv){
    cout << sizeof(double) <<endl;
    FILE * fp_mtx;
    FILE * fp_vec;
    FILE * fp_ans;
    double  * vec_val;
    INT  * row_ptr;
    INT  * col_idx;
    double  * mtx_val;
    INT  * par_set;
    double  * mtx_ans;
    INT  * Csr2_row_ptr;
    my256d  * Csr2_mtx_val;
    my256d  * Csr2_mid_val;
    fp_ans = fopen("answer_CSR2_S.mtx","wb+");
    char * Matrixname;
    char * Vectorname;
    char * Iterations; 
    if(argc>3)
    {
	Matrixname = argv[1];
	Vectorname = argv[2];
	Iterations = argv[3];
    }
	printf("----------------%s----------------\n",Matrixname);
	fp_mtx = fopen(Matrixname,"rb+");			// Read matrix file
	fp_vec = fopen(Vectorname,"rb+");			// Read vector file
	//----------------------Counting iterations----------------------//
	INT ite=0;
	for(INT i=0;i<strlen(Iterations);i++)
	{
		ite = ite * 10 + Iterations[i] - '0';
	}
	printf("Number of iterations are %d times\n",ite);
	//---------------------------------------------------------------//
    ReadFileConvertCsr(fp_mtx,fp_vec,row_ptr,col_idx,mtx_val,vec_val,par_set);
    printf("Read2Csr over");
    struct timeval start,end;
    double timeuse;
    gettimeofday(&start,NULL);
    //---------------------------------------------------------------//
    CsrConvertCsr2(row_ptr,mtx_val,Csr2_row_ptr,Csr2_mtx_val,par_set);

        //---------------------------------------------------------------//
    gettimeofday(&end,NULL);
    timeuse = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
    printf("CsrConvertCsr2 Time : %.6lf ms\n",timeuse*1000);
    printf("Csr2Csr2 over");
    Csr2_mid_val = new my256d[par_set[6]];
    my256dset(Csr2_mid_val,par_set[6]);
    mtx_ans = (double *)malloc(sizeof(double)*(par_set[3]-1));

    //---------------------------CSR2 SpMV---------------------------//
    gettimeofday(&start,NULL);
    for(INT i=0;i<ite;i++)
    {
        Madd(Csr2_mid_val,Csr2_mtx_val,col_idx,vec_val,par_set);
        SegSum(Csr2_row_ptr,Csr2_mid_val,mtx_ans,par_set);
    }
    gettimeofday(&end,NULL);
    timeuse = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
    //------------------------Important Information PrINT--------------------------------------//
    printf("CSR2_Spmv Calculate Time : %.6lf ms\n",(timeuse/ite)*1000.0);
    printf("CSR2_Spmv Gflops : %.6lf Gflops \n",((2*par_set[7])*1.0)/1000000000/(timeuse*1.0/ite));
    double bandwidth;
    bandwidth = ((par_set[3]+par_set[7])*sizeof(INT)+(2*par_set[7]+par_set[3]-1)*sizeof(double))/(timeuse*1.0/ite)/(1000*1000*1000);
    printf("bandwidth = %lf GB/s\n",bandwidth);
    printf("------------------------END----------------------\n");
    fprintf(fp_ans,"%d\n",par_set[3]-1);
    for(INT i=0;i<par_set[3]-1;i++)
    {
	if(i==par_set[3]-1){
	    break;
	}
	fprintf(fp_ans,"%.6lf\n",mtx_ans[i]);
    }
    //fclose(fp_ans);
}

