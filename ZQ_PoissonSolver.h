#ifndef _ZQ_POISSON_SOLVER_H_
#define _ZQ_POISSON_SOLVER_H_
#pragma once

#include "ZQ_PCGSolver.h"
#include "ZQ_taucs.h"
#include "ZQ_SparseMatrix.h"

namespace ZQ
{
	namespace ZQ_PoissonSolver
	{
		template<class T>
		void ComputeDivergence(int width, int height, const T* mac_u, const T* mac_v, T* b);

		template<class T>
		void AdjustOpen(int width, int height, T* mac_u, T* mac_v, const T* p);

		template<class T>
		void AdjustOpen(int width, int height, const bool* occupy, T* mac_u, T* mac_v, const T* p);

		template<class T>
		void AdjustClosed(int width, int height, T* mac_u, T* mac_v, const T* p);

		template<class T>
		void AdjustClosed(int width, int height, const bool* occupy, T* mac_u, T* mac_v, const T* p);

		template<class T>
		void RegularGridtoMAC(int width, int height, const T* u, const T* v, T* mac_u, T* mac_v, bool use_peroid_coord = false);

		template<class T>
		void MACtoRegularGrid(int width, int height, const T* mac_u, const T* mac_v, T* u, T* v);

		template<class T>
		int BuildOpenPoisson(int width, int height, taucs_ccs_matrix** A, bool display = false);

		template<class T>
		int BuildClosedPoisson(int width, int height, taucs_ccs_matrix** A, bool display = false);

		template<class T>
		void SolveOpenPoisson(T* u, T* v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveOpenPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveClosedPoisson(T* u, T* v, int width, int height,const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveClosedPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveOpenPoissonSOR(T* u, T* v, int width, int height, int nSORIterations, bool display = false);

		template<class T>
		void SolveOpenPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, int nSORIterations, bool display = false);

		template<class T>
		void SolveClosedPoissonSOR(T* u, T* v, int width, int height, int nSORIterations, bool display = false);

		template<class T>
		void SolveClosedPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, int nSORIterations, bool display = false);

		template<class T>
		void SolveOpenPoissonRedBlack(T* u, T* v, int width, int height, int nIterations, bool display = false);

		template<class T>
		void SolveOpenPoissonRedBlack_MACGrid(T* mac_u, T* mac_v, int width, int height, int nIterations, bool display = false);

		//for scene with obstacles
		template<class T>
		int BuildOpenPoisson(int width, int height, const bool* occupy, taucs_ccs_matrix** A, bool display = false);

		template<class T>
		int BuildClosedPoisson(int width, int height, const bool* occupy, taucs_ccs_matrix** A, bool display = false);

		template<class T>
		void SolveOpenPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveClosedPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, const taucs_ccs_matrix* A, int maxiter, bool display = false);

		template<class T>
		void SolveOpenPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nSORIterations, bool display = false);

		template<class T>
		void SolveClosedPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nSORIterations,bool display = false);

		template<class T>
		void SolveOpenPoissonRedBlack_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nIterations, bool display = false);


		/***************************************************************************************************/
		/***************************************************************************************************/
		/**************************             definitions             ************************************/
		/***************************************************************************************************/
		/***************************************************************************************************/

		template<class T>
		void ComputeDivergence(int width, int height, const T* mac_u, const T* mac_v, T* b)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					b[i*width+j] = mac_u[i*(width+1)+(j+1)]-mac_u[i*(width+1)+j] + mac_v[(i+1)*width+j]-mac_v[i*width+j];
				}
			}
		}

		template<class T>
		void AdjustOpen(int width, int height, T* mac_u, T* mac_v, const T* p)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 1;j < width;j++)
				{
					mac_u[i*(width+1)+j] -= p[i*width+j] - p[i*width+(j-1)];
				}
				mac_u[i*(width+1)+0] -= p[i*width+0] - 0;
				mac_u[i*(width+1)+width] -= 0 - p[i*width+width-1];
			}

			for(int j = 0;j < width;j++)
			{
				for(int i = 1;i < height;i++)
				{
					mac_v[i*width+j] -= p[i*width+j] - p[(i-1)*width+j];
				}
				mac_v[0*width+j] -= p[0*width+j] - 0;
				mac_v[height*width+j] -= 0 - p[(height-1)*width+j];
			}
		}

		template<class T>
		void AdjustOpen(int width, int height, const bool* occupy, T* mac_u, T* mac_v, const T* p)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 1;j < width;j++)
				{
					if(!occupy[i*width+j] && !occupy[i*width+j-1])
						mac_u[i*(width+1)+j] -= p[i*width+j] - p[i*width+j-1];
				}

				if(!occupy[i*width+0])
					mac_u[i*(width+1)+0] -= p[i*width+0] - 0;

				if(!occupy[i*width+width-1])
					mac_u[i*(width+1)+width] -= 0 - p[i*width+width-1];
			}

			for(int j = 0;j < width;j++)
			{
				for(int i = 1;i < height;i++)
				{
					if(!occupy[i*width+j] && !occupy[(i-1)*width+j])
						mac_v[i*width+j] -= p[i*width+j] - p[(i-1)*width+j];
				}
				if(!occupy[0*width+j])
					mac_v[0*width+j] -= p[0*width+j] - 0;

				if(!occupy[(height-1)*width+j])
					mac_v[height*width+j] -= 0 - p[(height-1)*width+j];
			}
		}

		template<class T>
		void AdjustClosed(int width, int height, T* mac_u, T* mac_v, const T* p)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 1;j < width;j++)
				{
					mac_u[i*(width+1)+j] -= p[i*width+j] - p[i*width+(j-1)];
				}
			}

			for(int j = 0;j < width;j++)
			{
				for(int i = 1;i < height;i++)
				{

					mac_v[i*width+j] -= p[i*width+j] - p[(i-1)*width+j];
				}
			}
		}

		template<class T>
		void AdjustClosed(int width, int height, const bool* occupy, T* mac_u, T* mac_v, const T* p)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 1;j < width;j++)
				{
					if(!occupy[i*width+j] && !occupy[i*width+j-1])
						mac_u[i*(width+1)+j] -= p[i*width+j] - p[i*width+j-1];
				}
			}

			for(int j = 0;j < width;j++)
			{
				for(int i = 1;i < height;i++)
				{
					if(!occupy[i*width+j] && !occupy[(i-1)*width+j])
						mac_v[i*width+j] -= p[i*width+j] - p[(i-1)*width+j];
				}
			}
		}

		template<class T>
		void RegularGridtoMAC(int width, int height, const T* u, const T* v, T* mac_u, T* mac_v, bool use_peroid_coord /* = false*/)
		{
			if(!use_peroid_coord)
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 1;j < width;j++)
					{
						mac_u[i*(width+1)+j] = 0.5*(u[i*width+(j-1)]+u[i*width+j]);
					}
					mac_u[i*(width+1)+0] = u[i*width+0];
					mac_u[i*(width+1)+width] = u[i*width+width-1];
				}

				for(int j = 0;j < width;j++)
				{
					for(int i = 1;i < height;i++)
					{
						mac_v[i*width+j] = 0.5*(v[(i-1)*width+j]+v[i*width+j]);
					}
					mac_v[0*width+j] = v[0*width+j];
					mac_v[height*width+j] = v[(height-1)*width+j];
				}
			}
			else
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 1;j < width;j++)
					{
						mac_u[i*(width+1)+j] = 0.5*(u[i*width+(j-1)]+u[i*width+j]);
					}
					mac_u[i*(width+1)+0] = 0.5*(u[i*width+0] + u[i*width+width-1]);
					mac_u[i*(width+1)+width] = 0.5*(u[i*width+0] + u[i*width+width-1]);
				}

				for(int j = 0;j < width;j++)
				{
					for(int i = 1;i < height;i++)
					{
						mac_v[i*width+j] = 0.5*(v[(i-1)*width+j]+v[i*width+j]);
					}
					mac_v[0*width+j] = 0.5*(v[0*width+j] + v[(height-1)*width+j]);
					mac_v[height*width+j] = 0.5*(v[0*width+j] + v[(height-1)*width+j]);
				}
			}
		}

		template<class T>
		void MACtoRegularGrid(int width, int height, const T* mac_u, const T* mac_v, T* u, T* v)
		{
			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					u[i*width+j] = 0.5*(mac_u[i*(width+1)+j]+mac_u[i*(width+1)+j+1]);
					v[i*width+j] = 0.5*(mac_v[(i+1)*width+j]+mac_v[i*width+j]);
				}
			}
		}

		template<class T>
		int BuildOpenPoisson(int width, int height, taucs_ccs_matrix** A, bool display)
		{
			int flag;
			if(strcmp(typeid(T).name(),"float") == 0)
				flag = TAUCS_SINGLE;
			else if(strcmp(typeid(T).name(),"double") == 0)
				flag = TAUCS_DOUBLE;
			else
				return 0;

			int dim = width*height;

			ZQ_SparseMatrix<T> mat(dim,dim);

			int ISLICE = width;
			int JSLICE = 1;


			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					int offset = i*width+j;
					int row_id = offset;

					std::vector<int> indices;
					if(i > 0)
						indices.push_back(offset-ISLICE);
					if(i < height-1)
						indices.push_back(offset+ISLICE);
					if(j > 0)
						indices.push_back(offset-JSLICE);
					if(j < width-1)
						indices.push_back(offset+JSLICE);

					float count = 4;
					for(int cc = 0;cc < indices.size();cc++)
					{
						mat.SetValue(row_id,indices[cc], 1);
					}
					mat.SetValue(row_id,row_id,-count);
				}
			}

			*A = mat.ExportCCS(flag);

			if((*A) == 0)
			{
				if(display)
				{
					printf("create taucs_ccs_matrix fail\n");
				}
				return 0;
			}

			if(display)
			{
				int m = (*A)->m;
				int n = (*A)->n;
				int nnz = (*A)->colptr[n];
				printf("dim: %d x %d, nnz: %d\n",m,n,nnz);
			}

			return (dim);
		}

		template<class T>
		int BuildClosedPoisson(int width, int height, taucs_ccs_matrix** A, bool display)
		{
			int flag;
			if(strcmp(typeid(T).name(),"float") == 0)
				flag = TAUCS_SINGLE;
			else if(strcmp(typeid(T).name(),"double") == 0)
				flag = TAUCS_DOUBLE;
			else
				return 0;

			int dim = width*height;

			int row_num = dim+1;
			int col_num = dim;
			ZQ_SparseMatrix<T> mat(row_num,col_num);

			int ISLICE = width;
			int JSLICE = 1;

			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					int offset = i*width+j;
					int row_id = offset;

					std::vector<int> indices;
					if(i > 0)
						indices.push_back(offset-ISLICE);
					if(i < height-1)
						indices.push_back(offset+ISLICE);
					if(j > 0)
						indices.push_back(offset-JSLICE);
					if(j < width-1)
						indices.push_back(offset+JSLICE);

					float count = indices.size();
					for(int cc = 0;cc < indices.size();cc++)
					{
						int cur_index = indices[cc];
						mat.SetValue(row_id,cur_index, 1);
					}
					mat.SetValue(row_id,row_id, -count);
				}
			}
			mat.SetValue(row_num-1,0,1);

			*A = mat.ExportCCS(flag);

			if((*A) == 0)
			{
				if(display)
				{
					printf("create taucs_ccs_matrix fail\n");
				}
				return 0;
			}

			if(display)
			{
				int m = (*A)->m;
				int n = (*A)->n;
				int nnz = (*A)->colptr[n];
				printf("dim: %d x %d, nnz: %d\n",m,n,nnz);
			}

			return (col_num);
		}

		template<class T>
		void ZQ_PoissonSolver::SolveOpenPoisson(T* u, T* v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display)
		{
			int flag;
			if(strcmp(typeid(T).name(),"float") == 0)
				flag = TAUCS_SINGLE;
			else if(strcmp(typeid(T).name(),"double") == 0)
				flag = TAUCS_DOUBLE;
			else
				return 0;

			T* mac_u = new T[(width+1)*height];
			T* mac_v = new T[width*(height+1)];

			RegularGridtoMAC(width,height,u,v,mac_u,mac_v);

			SolveOpenPoisson_MACGrid(mac_u,mac_v,width,height,A,maxiter,display);

			MACtoRegularGrid(width,height,mac_u,mac_v,u,v);

			delete []mac_u;
			delete []mac_v;
		}

		template<class T>
		void SolveOpenPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display)
		{
			int dim = width*height;

			T* b = new T[width*height];

			int it = 0;
			int max_iter = maxiter;
			double tol = 1e-12;
			T* x0 = new T[dim];
			T* x = new T[dim];
			memset(x0,0,sizeof(T)*dim);
			memset(x,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);

			ZQ_PCGSolver::PCG_sparse_unsquare(A,b,x0,max_iter,tol,x,it,display);

			AdjustOpen(width,height,mac_u,mac_v,x);
		}

		template<class T>
		void SolveClosedPoisson(T* u, T* v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display)
		{
			T* mac_u = new T[(width+1)*height];
			T* mac_v = new T[width*(height+1)];

			RegularGridtoMAC(width,height,u,v,mac_u,mac_v);

			SolveClosedPoisson_MACGrid(mac_u,mac_v,width,height,A,maxiter,display);

			MACtoRegularGrid(width,height,mac_u,mac_v,u,v);

			delete []mac_u;
			delete []mac_v;
		}

		template<class T>
		void SolveClosedPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const taucs_ccs_matrix* A, int maxiter, bool display)
		{
			int dim = width*height;
			T* b = new T[width*height+1];

			int it = 0;
			int max_iter = maxiter;
			double tol = 1e-12;
			T* x0 = new T[dim];
			T* x = new T[dim];
			memset(x0,0,sizeof(T)*dim);
			memset(x,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);
			b[dim] = 0;

			ZQ_PCGSolver::PCG_sparse_unsquare(A,b,x0,max_iter,tol,x,it,display);

			AdjustClosed(width,height,mac_u,mac_v,x);

			delete []b;
			delete []x0;
			delete []x;
		}


		template<class T>
		void SolveOpenPoissonSOR(T* u, T* v, int width, int height, int nSORIterations, bool display)
		{
			T* mac_u = new T[(width+1)*height];
			T* mac_v = new T[width*(height+1)];

			RegularGridtoMAC(width,height,u,v,mac_u,mac_v);

			SolveOpenPoissonSOR_MACGrid(mac_u,mac_v,width,height,nSORIterations,display);

			MACtoRegularGrid(width,height,mac_u,mac_v,u,v);

			delete []mac_u;
			delete []mac_v;	
		}

		template<class T>
		void SolveOpenPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, int nSORIterations, bool display)
		{
			int dim = width*height;

			T* b = new T[dim];
			T* x = new T[dim];
			memset(x,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);


			int ISLICE = width;
			int JSLICE = 1;

			// Begin SOR
			for(int sor_it = 0;sor_it < nSORIterations;sor_it++)
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						int offset = i*width+j;
						double coeff = 0,sigma = 0;

						coeff = 4;

						if(i == 0)
						{
							sigma += x[offset+ISLICE];
						}
						else if(i == height-1)
						{
							sigma += x[offset-ISLICE];
						}
						else 
						{
							sigma += x[offset-ISLICE]+x[offset+ISLICE];
						}

						if(j == 0)
						{
							sigma += x[offset+JSLICE];
						}
						else if(j == width-1)
						{
							sigma += x[offset-JSLICE];
						}
						else
						{
							sigma += x[offset+JSLICE]+x[offset-JSLICE];
						}
						sigma -= b[offset];
						x[offset] = sigma/coeff;
					}
				}
			}

			//End SOR

			AdjustOpen(width,height,mac_u,mac_v,x);

			delete []b;
			delete []x;

		}

		template<class T>
		void SolveClosedPoissonSOR(T* u, T* v, int width, int height, int nSORIterations, bool display)
		{
			T* mac_u = new T[(width+1)*height];
			T* mac_v = new T[width*(height+1)];

			RegularGridtoMAC(width,height,u,v,mac_u,mac_v);

			SolveClosedPoissonSOR_MACGrid(mac_u,mac_v,width,height,nSORIterations,display);

			MACtoRegularGrid(width,height,mac_u,mac_v,u,v);

			delete []mac_u;
			delete []mac_v;	
		}


		template<class T>
		void SolveClosedPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, int nSORIterations, bool display)
		{
			int dim = width*height;
			T* b = new T[dim];
			T* x = new T[dim];
			memset(x,0,sizeof(T)*dim);

			double div_per_volume = 0;

			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					b[i*width+j] = mac_u[i*(width+1)+(j+1)]-mac_u[i*(width+1)+j] + mac_v[(i+1)*width+j]-mac_v[i*width+j];
					div_per_volume += b[i*width+j];
				}
			}

			div_per_volume /= width*height;
			for(int i = 0;i < width*height;i++)
				b[i] -= div_per_volume;


			int ISLICE = width;
			int JSLICE = 1;

			// Begin SOR
			for(int sor_it = 0;sor_it < nSORIterations;sor_it++)
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						int offset = i*width+j;
						double coeff = 0,sigma = 0;

						coeff = 4;

						if(i == 0)
						{
							sigma += x[offset+ISLICE];
						}
						else if(i == height-1)
						{
							sigma += x[offset-ISLICE];
						}
						else 
						{
							sigma += x[offset-ISLICE]+x[offset+ISLICE];
						}

						if(j == 0)
						{
							sigma += x[offset+JSLICE];
						}
						else if(j == width-1)
						{
							sigma += x[offset-JSLICE];
						}
						else
						{
							sigma += x[offset+JSLICE]+x[offset-JSLICE];
						}
						sigma -= b[offset];
						x[offset] = sigma/coeff;
					}
				}
			}

			//End SOR

			AdjustClosed(width,height,mac_u,mac_v,x);



			delete []b;
			delete []x;

		}

		template<class T>
		void SolveOpenPoissonRedBlack(T* u, T* v, int width, int height, int nIterations, bool display)
		{
			T* mac_u = new T[(width+1)*height];
			T* mac_v = new T[width*(height+1)];

			RegularGridtoMAC(width,height,u,v,mac_u,mac_v);

			SolveOpenPoissonRedBlack_MACGrid(mac_u,mac_v,width,height,nIterations,display);

			MACtoRegularGrid(width,height,mac_u,mac_v,u,v);

			delete []mac_u;
			delete []mac_v;	
		}


		template<class T>
		void SolveOpenPoissonRedBlack_MACGrid(T* mac_u, T* mac_v, int width, int height, int nIterations, bool display)
		{
			int dim = width*height;

			T* b = new T[dim];
			T* x = new T[dim];
			memset(x,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);

			int ISLICE = width;
			int JSLICE = 1;

			// Begin Red-Black
			for(int rb_it = 0;rb_it < nIterations;rb_it++)
			{

				// handle red
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						if((i+j)%2 == 1)
							continue;

						int offset = i*width+j;
						double coeff = 0,sigma = 0;

						coeff = 4;

						if(i == 0)
						{
							sigma += x[offset+ISLICE];
						}
						else if(i == height-1)
						{
							sigma += x[offset-ISLICE];
						}
						else 
						{
							sigma += x[offset-ISLICE]+x[offset+ISLICE];
						}

						if(j == 0)
						{
							sigma += x[offset+JSLICE];
						}
						else if(j == width-1)
						{
							sigma += x[offset-JSLICE];
						}
						else
						{
							sigma += x[offset+JSLICE]+x[offset-JSLICE];
						}
						sigma -= b[offset];
						x[offset] = sigma/coeff;
					}
				}

				//handle black

				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						if((i+j)%2 == 0)
							continue;

						int offset = i*width+j;
						double coeff = 0,sigma = 0;

						coeff = 4;

						if(i == 0)
						{
							sigma += x[offset+ISLICE];
						}
						else if(i == height-1)
						{
							sigma += x[offset-ISLICE];
						}
						else 
						{
							sigma += x[offset-ISLICE]+x[offset+ISLICE];
						}

						if(j == 0)
						{
							sigma += x[offset+JSLICE];
						}
						else if(j == width-1)
						{
							sigma += x[offset-JSLICE];
						}
						else
						{
							sigma += x[offset+JSLICE]+x[offset-JSLICE];
						}
						sigma -= b[offset];
						x[offset] = sigma/coeff;
					}
				}
			}

			//End Red-Black

			AdjustOpen(width,height,mac_u,mac_v,x);

			delete []b;
			delete []x;
		}


		/**********************************     for scene with obstacle      *************************************/

		template<class T>
		int BuildOpenPoisson(int width, int height, const bool* occupy, taucs_ccs_matrix** A, bool display)
		{	
			int flag;
			if(strcmp(typeid(T).name(),"float") == 0)
				flag = TAUCS_SINGLE;
			else if(strcmp(typeid(T).name(),"double") == 0)
				flag = TAUCS_DOUBLE;
			else
				return 0;

			int dim = width*height;

			ZQ_SparseMatrix<T> mat(dim,dim);

			int ISLICE = width;
			int JSLICE = 1;

			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					int offset = i*width+j;
					int row_id = offset;

					if(occupy[offset])
					{
						mat.SetValue(row_id,row_id,1);
						continue;
					}

					std::vector<int> indices;
					if(i > 0)
						indices.push_back(offset-ISLICE);
					if(i < height-1)
						indices.push_back(offset+ISLICE);
					if(j > 0)
						indices.push_back(offset-JSLICE);
					if(j < width-1)
						indices.push_back(offset+JSLICE);

					float count = 4 - indices.size();
					for(int cc = 0;cc < indices.size();cc++)
					{
						int cur_index = indices[cc];
						if(!occupy[cur_index])
						{
							count += 1;
							mat.SetValue(row_id,cur_index, 1);
						}
					}
					mat.SetValue(row_id,row_id, -count);
				}
			}


			*A = mat.ExportCCS(flag);
			if((*A) == 0)
			{
				if(display)
				{
					printf("create taucs_ccs_matrix fail\n");
				}
				return 0;
			}

			if(display)
			{
				int m = (*A)->m;
				int n = (*A)->n;
				int nnz = (*A)->colptr[n];
				printf("dim: %d x %d, nnz: %d\n",m,n,nnz);
			}

			return dim;
		}

		template<class T>
		int BuildClosedPoisson(int width, int height, const bool* occupy, taucs_ccs_matrix** A, bool display)
		{
			int flag;
			if(strcmp(typeid(T).name(),"float") == 0)
				flag = TAUCS_SINGLE;
			else if(strcmp(typeid(T).name(),"double") == 0)
				flag = TAUCS_DOUBLE;
			else
				return 0;

			bool haveFirst = false;
			int first = -1;
			for(int i = 0;i < height*width;i++)
			{
				if(!occupy[i])
				{
					haveFirst = true;
					first = i;
					break;
				}
			}

			if(!haveFirst)
			{
				return 0;
			}

			int dim = width*height;
			int col_num = dim;
			int row_num = dim+1;

			ZQ_SparseMatrix<T> mat(row_num,col_num);

			int ISLICE = width;
			int JSLICE = 1;

			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					int offset = i*width+j;
					int row_id = offset;

					if(occupy[offset])
					{
						mat.SetValue(row_id,row_id,1);
						continue;
					}

					std::vector<int> indices;
					if(i > 0)
						indices.push_back(offset-ISLICE);
					if(i < height-1)
						indices.push_back(offset+ISLICE);
					if(j > 0)
						indices.push_back(offset-JSLICE);
					if(j < width-1)
						indices.push_back(offset+JSLICE);

					float count = 0.0f;
					for(int cc = 0;cc < indices.size();cc++)
					{
						int cur_index = indices[cc];
						if(!occupy[cur_index])
						{
							count += 1.0f;
							mat.SetValue(row_id,cur_index, 1);
						}
					}
					mat.SetValue(row_id,row_id, -count);
				}
			}
			mat.SetValue(row_num-1,first,1);


			*A = mat.ExportCCS(flag);

			if((*A) == 0)
			{
				if(display)
				{
					printf("create taucs_ccs_matrix fail\n");
				}
				return 0;
			}

			if(display)
			{
				int m = (*A)->m;
				int n = (*A)->n;
				int nnz = (*A)->colptr[n];
				printf("dim: %d x %d, nnz: %d\n",m,n,nnz);
			}

			return dim;
		}

		template<class T>
		void SolveOpenPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, const taucs_ccs_matrix* A, int maxiter, bool display)
		{

			int dim = width*height;

			int max_iter = maxiter;
			double tol = 1e-12;
			int it = 0;

			T* b = new T[dim];
			memset(b,0,sizeof(T)*dim);
			T* x = new T[dim];
			memset(x,0,sizeof(T)*dim);
			T* x0 = new T[dim];
			memset(x0,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);

			ZQ_PCGSolver::PCG_sparse_unsquare(A,b,x0,max_iter,tol,x,it,display);

			AdjustOpen(width,height,occupy,mac_u,mac_v,x);
			delete []x0;
			delete []x;
			delete []b;

		}

		template<class T>
		void SolveClosedPoisson_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, const taucs_ccs_matrix* A, int maxiter, bool display)
		{
			int dim = width*height;
			int max_iter = maxiter;
			double tol = 1e-12;
			int it = 0;

			T* b = new T[dim+1];
			memset(b,0,sizeof(T)*(dim+1));
			T* x = new T[dim];
			memset(x,0,sizeof(T)*dim);
			T* x0 = new T[dim];
			memset(x0,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);
			b[dim] = 0;

			ZQ_PCGSolver::PCG_sparse_unsquare(A,b,x0,max_iter,tol,x,it,display);

			AdjustClosed(width,height,occupy,mac_u,mac_v,x);

			delete []x0;
			delete []x;
			delete []b;
		}

		template<class T>
		void SolveOpenPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nSORIterations, bool display)
		{
			int dim = width*height;

			T* b = new T[dim];
			T* p = new T[dim];
			memset(p,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);

			int ISLICE = width;
			int JSLICE = 1;

			// Begin SOR
			for(int sor_it = 0;sor_it < nSORIterations;sor_it++)
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						int offset = i*width+j;
						if(occupy[offset])
							continue;

						double coeff = 0,sigma = 0;

						if(i == 0)
						{
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(i == height-1)
						{
							if(occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else 
						{
							if(!occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
						}

						if(j == 0)
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(j == width-1)
						{
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
						}

						sigma -= b[offset];
						if(coeff == 0)
							p[offset] = 0;
						else
							p[offset] = sigma/coeff;
					}
				}
			}

			//End SOR

			AdjustOpen(width,height,occupy,mac_u,mac_v,p);

			delete []b;
			delete []p;
		}


		template<class T>
		void SolveClosedPoissonSOR_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nSORIterations, bool display)
		{
			int dim = width*height;

			T* b = new T[dim];
			T* p = new T[dim];
			memset(p,0,sizeof(T)*dim);

			double div_per_volume = 0;
			int volume = 0;

			for(int i = 0;i < height;i++)
			{
				for(int j = 0;j < width;j++)
				{
					b[i*width+j] = mac_u[i*(width+1)+(j+1)]-mac_u[i*(width+1)+j] + mac_v[(i+1)*width+j]-mac_v[i*width+j];
					div_per_volume += b[i*width+j];
					volume += occupy[i*width+j] ? 0 : 1;

				}
			}
			if(volume != 0)
				div_per_volume /= volume;

			for(int i = 0;i < width*height;i++)
			{
				if(!occupy[i])
					b[i] -= div_per_volume;
			}

			int ISLICE = width;
			int JSLICE = 1;

			// Begin SOR
			for(int sor_it = 0;sor_it < nSORIterations;sor_it++)
			{
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						int offset = i*width+j;
						if(occupy[offset])
							continue;

						double coeff = 0,sigma = 0;

						if(i == 0)
						{
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(i == height-1)
						{
							if(occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else 
						{
							if(!occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
						}

						if(j == 0)
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(j == width-1)
						{
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
						}

						sigma -= b[offset];
						if(coeff == 0)
							p[offset] = 0;
						else
							p[offset] = sigma/coeff;
					}
				}
			}

			//End SOR

			AdjustClosed(width,height,occupy,mac_u,mac_v,p);


			delete []b;
			delete []p;

		}

		template<class T>
		void SolveOpenPoissonRedBlack_MACGrid(T* mac_u, T* mac_v, int width, int height, const bool* occupy, int nIterations, bool display)
		{
			int dim = width*height;

			T* b = new T[dim];
			T* p = new T[dim];
			memset(p,0,sizeof(T)*dim);

			ComputeDivergence(width,height,mac_u,mac_v,b);

			int ISLICE = width;
			int JSLICE = 1;

			// Begin Red-Black
			for(int rb_it = 0;rb_it < nIterations;rb_it++)
			{

				// handle red
				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						if((i+j)%2 == 1)
							continue;

						int offset = i*width+j;
						if(occupy[offset])
							continue;

						double coeff = 0,sigma = 0;

						if(i == 0)
						{
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(i == height-1)
						{
							if(occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else 
						{
							if(!occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
						}

						if(j == 0)
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(j == width-1)
						{
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
						}

						sigma -= b[offset];
						if(coeff == 0)
							p[offset] = 0;
						else
							p[offset] = sigma/coeff;
					}
				}

				//handle black

				for(int i = 0;i < height;i++)
				{
					for(int j = 0;j < width;j++)
					{
						if((i+j)%2 == 0)
							continue;

						int offset = i*width+j;
						if(occupy[offset])
							continue;

						double coeff = 0,sigma = 0;

						if(i == 0)
						{
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(i == height-1)
						{
							if(occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else 
						{
							if(!occupy[offset-ISLICE])
							{
								sigma += p[offset-ISLICE];
								coeff += 1;
							}
							if(!occupy[offset+ISLICE])
							{
								sigma += p[offset+ISLICE];
								coeff += 1;
							}
						}

						if(j == 0)
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else if(j == width-1)
						{
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
							coeff += 1;
						}
						else
						{
							if(!occupy[offset+JSLICE])
							{
								sigma += p[offset+JSLICE];
								coeff += 1;
							}
							if(!occupy[offset-JSLICE])
							{
								sigma += p[offset-JSLICE];
								coeff += 1;
							}
						}

						sigma -= b[offset];
						if(coeff == 0)
							p[offset] = 0;
						else
							p[offset] = sigma/coeff;
					}
				}
			}

			//End Red-Black

			AdjustOpen(width,height,occupy,mac_u,mac_v,p);

			delete []b;
			delete []p;

		}
	}

}


#endif
