#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>
#define INT int
#define DOU double
#define P 1000
int main(int argc,char ** argv)
{
        FILE * fp_mtx;
        FILE * fp_vec;
        FILE * fp_ans;
        DOU  * vec_val;
        INT  * row_ptr;
        INT  * col_idx;
        DOU  * mtx_val;
        INT  * par_set;
        DOU  * mtx_ans;
        char * filename_mtx;
        char * filename_vec;
	char * Iterations;
        if(argc>=2)
        {
                filename_mtx = argv[1];
                filename_vec = argv[2];
		Iterations = argv[3];
        }
        else
        {
                printf("error! please input right filename\n");
        }
	INT ite = 0;
	for(INT i=0;i<strlen(Iterations);i++)
	{
		ite = ite * 10 + Iterations[i]-'0';
	}
        fp_mtx = fopen(filename_mtx,"rb+");
        fp_vec = fopen(filename_vec,"rb+");
        fp_ans = fopen("answer_serial.mtx","wb+");
        INT vec_row;
        fscanf(fp_vec,"%d",&vec_row);
        vec_val = (DOU *)malloc(sizeof(DOU)*vec_row);
        memset(vec_val,0,sizeof(DOU)*vec_row);
        for(INT i=0;i<vec_row;i++)
        {
                fscanf(fp_vec,"%lf",&vec_val[i]);
        }
        //-----------------------Read Matrix-------------------------//
        par_set = (INT *)malloc(sizeof(INT)*10);
        INT row_num,col_num,nzz_num;
        fscanf(fp_mtx,"%d %d %d",&row_num,&col_num,&nzz_num);
        INT str_max = nzz_num;
        row_ptr = (INT *)malloc(sizeof(INT)*(row_num+1));
        col_idx = (INT *)malloc(sizeof(INT)*str_max);
        mtx_val = (DOU *)malloc(sizeof(DOU)*str_max);
        mtx_ans = (DOU *)malloc(sizeof(DOU)*row_num);
        memset(mtx_val,0,sizeof(DOU)*str_max);
        memset(col_idx,0,sizeof(INT)*str_max);
        INT v=-1,cnt=0,cnt1;
        INT row,col;
        DOU nzz;
        for(INT i=0;i<nzz_num;i++)
        {
                fscanf(fp_mtx,"%d %d %lf",&col,&row,&nzz);
                row--;
                col--;
                while(v<row)
                {
                        v++;
                        row_ptr[v] = cnt;
                }
                col_idx[cnt] = col;
                mtx_val[cnt] = nzz;
                cnt++;
        }
        while(v<row_num)
        {
                v++;
                row_ptr[v] = cnt;
        }
        row_ptr[row_num]=cnt;
        fprintf(fp_ans,"%d\n",row_num);
	struct timeval start,end;
	double timeuse;
	gettimeofday(&start,NULL);
	for(int p=0;p<ite;p++){
        for(INT i=0;i<row_num;i++)
        {
                mtx_ans[i]=0;
                for(INT j=row_ptr[i];j<row_ptr[i+1];j++)
                {
                        mtx_ans[i]+=mtx_val[j]*vec_val[col_idx[j]];
                }
                fprintf(fp_ans,"%.6lf\n",mtx_ans[i]);
        }
	}
	gettimeofday(&end,NULL);
        timeuse = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
        printf("CSR COMPUTE Time : %.6lf ms\n",timeuse*1000/ite);
        return 0;
}
