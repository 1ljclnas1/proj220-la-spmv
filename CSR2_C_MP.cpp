#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <stdint.h>
#include <iostream>
#define INT int
#define DOU double

void ReadFileConvertCsr(FILE * fp_mtx,FILE * fp_vec,INT *&row_ptr,INT *&col_idx,DOU *&mtx_val,DOU *&vec_val,INT *&par_set)
{
        INT row_num,col_num,nzz_num;
        fscanf(fp_mtx,"%d %d %d",&row_num,&col_num,&nzz_num);
        //-----------------------Read Vector-------------------------//
        INT vec_row;
        vec_row = col_num;
        //srand(time(NULL));
        fscanf(fp_vec,"%d",&vec_row);
	vec_val = (DOU *)aligned_alloc(64,sizeof(DOU)*vec_row);
        //vec_val = (DOU *)malloc(sizeof(DOU)*vec_row);
        memset(vec_val,0,sizeof(DOU)*vec_row);
        for(INT i=1;i<=vec_row;i++)
        {
                fscanf(fp_vec,"%lf",&vec_val[i]);
                //vec_val[i] = ((rand()%10)+1)*1.0;
        }
        //-----------------------Read Matrix-------------------------//
        par_set = (INT *)malloc(sizeof(INT)*10);
        //INT row_num,col_num,nzz_num;
        //fscanf(fp_mtx,"%d %d %d",&row_num,&col_num,&nzz_num);
        printf("(%d , %d) , nzz_num = %d\n",row_num,col_num,nzz_num);
        INT mtx_width;
        INT x,cnt_ite;
        x = INT(nzz_num*1.0/row_num+0.5);
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
                                //printf("%d\n",x);
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
	//mtx_width = 8;
        printf("mtx_width = %d \n",mtx_width);
        INT mtx_high=4;
        INT str_max;
        str_max = nzz_num + row_num * (mtx_width-1);
	row_ptr = (INT *)aligned_alloc(64,sizeof(INT)*(row_num+1));
        //row_ptr = (INT *)malloc(sizeof(INT)*(row_num+1));
        col_idx = (INT *)aligned_alloc(64,sizeof(INT)*str_max);
	//col_idx = (INT *)malloc(sizeof(INT)*str_max);
        mtx_val = (DOU *)aligned_alloc(64,sizeof(DOU)*str_max);
	//mtx_val = (DOU *)malloc(sizeof(DOU)*str_max);
        memset(mtx_val,0,sizeof(DOU)*str_max);
        memset(col_idx,0,sizeof(INT)*str_max);
        INT v=-1,cnt=0,cnt1;
        INT row,col;
        DOU nzz;
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
                // printf("%d, %f ",cnt,nzz);
                cnt++;
        }
        // printf("\n");
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

struct my256d{
    double a; //0
    double b; //1
    double c; //2
    double d; //3
    my256d operator*(my256d other){
        a *= other.a;
        b *= other.b;
        c *= other.c;
        d *= other.d;
        return *this;
    }
    my256d operator+(my256d other){
        a += other.a;
        b += other.b;
        c += other.c;
        d += other.d;
        return *this;
    }

};

void CsrConvertCsr2(INT *&row_ptr,DOU *&mtx_val,INT *&Csr2_row_ptr,my256d *&Csr2_mtx_val_list,INT * par_set){
    INT i,x,y,z,i1,t;

    const INT i_end  = par_set[3]; // row_num + 1
	const INT i_end1 = par_set[0]; // cnt == nzz_size/mtx_high*mtx_wight
    const INT mtx_width = par_set[1]; // mtx_width

    // printf("i_end1: %d\n", i_end1);

    Csr2_row_ptr = (INT *)aligned_alloc(64,sizeof(INT)*i_end);
    Csr2_mtx_val_list = (my256d *)aligned_alloc(64,sizeof(my256d)*(i_end1>>2));

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
        if(y==0) Csr2_mtx_val_list[x].a = mtx_val[i];
        else if(y==1) Csr2_mtx_val_list[x].b = mtx_val[i];
        else if(y==2) Csr2_mtx_val_list[x].c = mtx_val[i];
        else if(y==3) Csr2_mtx_val_list[x].d = mtx_val[i];
    }
}

/// @brief  记录每一行每个元素与vec相乘后的值并存入Csr2_mid_val
/// @param Csr2_mid_val 
/// @param Csr2_mtx_val 
/// @param col_idx 
/// @param vec_val 
/// @param par_set 
void Madd(my256d *&Csr2_mid_val,my256d *&Csr2_mtx_val,INT *col_idx,DOU *&vec_val,INT * par_set){
    const INT i_end     = par_set[6];
    const INT mtx_width = par_set[1];
    const INT mtx_high  = par_set[2];
    INT   i,j,xx,yy,zz;
    const INT m1 = mtx_width<<1;
    const INT m2 = m1 + mtx_width;
    my256d Csr2_mid_val1;
    my256d Csr2_col_val;
    #pragma omp parallel private(i,j,xx,yy,zz,Csr2_mid_val1,Csr2_col_val)
        {
                #pragma omp for schedule(static) nowait
    for(i=0;i<i_end;i++)
    {
        xx=i*mtx_width;
        yy=xx<<2;
        Csr2_col_val.d = vec_val[col_idx[yy+m2]];
        Csr2_col_val.c = vec_val[col_idx[yy+m1]];
        Csr2_col_val.b = vec_val[col_idx[yy+mtx_width]];
        Csr2_col_val.a = vec_val[col_idx[yy]];
        Csr2_mid_val1=Csr2_mtx_val[xx]*Csr2_col_val;
        xx=(xx<<1)+xx;
        zz=yy+mtx_width;
        for(j=yy+1;j<zz;j++)
        {
            Csr2_col_val.d = vec_val[col_idx[j+m2]];
            Csr2_col_val.c = vec_val[col_idx[j+m1]];
            Csr2_col_val.b = vec_val[col_idx[j+mtx_width]];
            Csr2_col_val.a = vec_val[col_idx[j]];
            Csr2_mid_val1 =Csr2_mtx_val[j-xx]*Csr2_col_val+Csr2_mid_val1;
        }
        Csr2_mid_val[i] = Csr2_mid_val1;
    }
	}
}

/// @brief 将同一行的元素值加起来得到最终的值。
/// @param Csr2_row_ptr 
/// @param Csr2_mid_val 
/// @param mtx_ans 
/// @param par_set 
void SegSum(INT *&Csr2_row_ptr,my256d *&Csr2_mid_val,DOU *&mtx_ans,INT *&par_set){
    INT i,j;
    const INT num=par_set[3]-1;

    //每一行求最终值每一行
    for(i=0;i<num;i++)
    {
        DOU s=0;
        int t1,t2;
        t1=Csr2_row_ptr[i];
        t2=Csr2_row_ptr[i+1];
        for(j=t1;j<t2;j++)
        {
            int t=j>>2;
            int p = j-(t<<2);
            switch(p){
                case 0:
                    s+=Csr2_mid_val[t].a;
                    break;
                case 1:
                    s+=Csr2_mid_val[t].b;
                    break;
                case 2:
                    s+=Csr2_mid_val[t].c;
                    break;
                case 3:
                    s+=Csr2_mid_val[t].d;
                    break;
            }
        }
        mtx_ans[i] = s;
    }
}
int main(int argc,char ** argv){
    FILE * fp_mtx;
    FILE * fp_vec;
    FILE * fp_ans;
    DOU  * vec_val;
    INT  * row_ptr;
    INT  * col_idx;
    DOU  * mtx_val;
    INT  * par_set;
    DOU  * mtx_ans;
    INT  * Csr2_row_ptr;
    my256d  * Csr2_mtx_val;
    my256d  * Csr2_mid_val;
    fp_ans = fopen("answer_CSR2_c.mtx","wb+");
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

    struct timeval start,end;
    double timeuse;
    gettimeofday(&start,NULL);
    //---------------------------------------------------------------//
    CsrConvertCsr2(row_ptr,mtx_val,Csr2_row_ptr,Csr2_mtx_val,par_set);
    Csr2_mid_val  = (my256d *)malloc(sizeof(my256d)*par_set[6]);
    mtx_ans = (DOU *)malloc(sizeof(DOU)*(par_set[3]-1));
    //---------------------------------------------------------------//
    gettimeofday(&end,NULL);
    timeuse = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
    printf("CsrConvertCsr2 Time : %.6lf ms\n",timeuse*1000);
    //---------------------------CSR2 SpMV---------------------------//
    gettimeofday(&start,NULL);
    for(INT i=0;i<ite;i++)
    {
        Madd(Csr2_mid_val,Csr2_mtx_val,col_idx,vec_val,par_set);
        SegSum(Csr2_row_ptr,Csr2_mid_val,mtx_ans,par_set);
    }
    gettimeofday(&end,NULL);
    timeuse = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
    //------------------------Important Information Print--------------------------------------//
    printf("CSR2_Spmv Calculate Time : %.6lf ms\n",(timeuse/ite)*1000.0);
    printf("CSR2_Spmv Gflops : %.6lf Gflops \n",((2*par_set[7])*1.0)/1000000000/(timeuse*1.0/ite));
    double bandwidth;
    bandwidth = ((par_set[3]+par_set[7])*sizeof(int)+(2*par_set[7]+par_set[3]-1)*sizeof(double))/(timeuse*1.0/ite)/(1000*1000*1000);
    printf("bandwidth = %lf GB/s\n",bandwidth);
    printf("------------------------END----------------------\n");
    fprintf(fp_ans,"%d\n",par_set[3]-1);
    for(INT i=0;i<par_set[3]-1;i++)
    {
	fprintf(fp_ans,"%.6lf\n",mtx_ans[i]);
    }
    //--------------------------------------------------------------//
    return 0;
}

