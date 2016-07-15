#ifndef _DPZ_QUICKSORT_
#define _DPZ_QUICKSORT_
//#include <algorithm>
namespace dpz{

	template<typename T>
	void swap(T& arg1,T& arg2){
		T tmp;
		tmp=arg1;
		arg1=arg2;
		arg2=tmp;
	}
	template<typename T,typename COMP>
	unsigned int partition(T ctx,int p,int q,COMP comp)
	{
		int i = p;
		for(int j = p + 1 ;j < q; j++ )
		{
			if(comp(ctx[j],ctx[p]))
			{
				i++;
				swap(ctx[i], ctx[j]);
			}
		}
		swap(ctx[i],ctx[p]);
		return i;
	}
	template<typename T,typename COMP>
	void sort(T ctx,int p,int q,COMP comp)
	{
		if( p < q )
		{
			int r = partition(ctx,p,q,comp);
			sort(ctx,p,r-1,comp);
			sort(ctx,r+1,q,comp);
		}
	}
	template<typename T,typename COMP>
	void sort(T arg1,T arg2,COMP comp){
		sort(arg1,0,arg2-arg1,comp);
	}
}
#endif