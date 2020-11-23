﻿/*
Copyright 2020 LiGuer. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef _MAT_H
#define _MAT_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
template<class T>
class Mat
{
public:
/******************************************************************************
*                    核心数据
******************************************************************************/
	T* data = NULL;
	int rows = 0, cols = 0;
/******************************************************************************
*                    基础函数
******************************************************************************/
	/*---------------- 构造析构函数 ----------------*/
	Mat() { ; }
	Mat(const int _rows, const int _cols) { zero(_rows, _cols); }
	Mat(const Mat& a) { assign(a); }
	~Mat() { free(data); }
	/*---------------- 基础函数 ----------------*/
	void clean() {memset(data, 0, sizeof(T) * rows * cols);}		//清零 
	void error() { exit(-1);}
	void eatMat(Mat& a) {											//吃掉另一个矩阵的数据 (指针操作)
		if (data != NULL)free(data); 
		data = a.data; a.data = NULL;
		rows = a.rows; cols = a.cols; a.rows = a.cols = 0;
	}
/******************************************************************************
*                    基础矩阵
******************************************************************************/
	/*---------------- 零元 ----------------*/
	void zero(const int _rows, const int _cols) {
		if (data != NULL)free(data);
		data = (T*)malloc(sizeof(T) * _rows * _cols);
		memset(data, 0, sizeof(T) * _rows * _cols);
		rows = _rows;	cols = _cols;
	}
	/*---------------- 单位元 ----------------*/
	void E(const int _rows) {
		zero(_rows, _rows);
		for (int i = 0; i < rows; i++) {
			data[i * cols + i] = 1;
		}
	}
	/*---------------- 随机元 ----------------*/
	void rands(const int _rows, const int _cols,T st,T ed) {
		zero(_rows, _cols);
		for (int i = 0; i < rows * cols; i++) {
			data[i] = rand() / double(RAND_MAX) * (ed - st) + st;	//[st,ed)
		}
	}
/******************************************************************************
*                    基础运算
*******************************************************************************
*	运算嵌套,暂不稳定, 最好别用
		Eg: b.add(b.mult(a, b), a.mult(-1, a)); 不管括号第一二项顺序,都是数乘,乘法,加法, 问题原因暂不了解。
******************************************************************************/
	/*---------------- "[]"取元素 ----------------*/
	T& operator[](int i) { return data[i]; }
	T& operator()(int i, int j) { return data[i * cols + j]; }
	T& operator()(int i) { return data[i]; }
	/*---------------- max/min ----------------*/
	T max() const {
		T maxdata = *data;
		for (int i = 0; i < rows * cols; i++)maxdata = maxdata >= data[i] ? maxdata : data[i];
		return maxdata;
	}
	T max(int& index) {
		T maxdata = *data; index = 0;
		for (int i = 0; i < rows * cols; i++)
			if (maxdata < data[i]) { maxdata = data[i]; index = i; }
		return maxdata;
	}
	T min() const {
		T mindata = *data;
		for (int i = 0; i < rows * cols; i++)mindata = mindata <= data[i] ? mindata : data[i];
		return mindata;
	}
	T min(int& index) {
		T mindata = *data; index = 0;
		for (int i = 0; i < rows * cols; i++)
			if (mindata > data[i]) { mindata = data[i]; index = i; }
		return mindata;
	}
	/*----------------赋矩阵 [ = ]----------------*/ //不能赋值自己
	void assign(const Mat& a) {
		if (a.data == NULL)error();
		zero(a.rows, a.cols);
		memcpy(data, a.data, sizeof(T) * a.rows * a.cols);
	}
	Mat& operator=(const Mat& a) {
		if (a.data == NULL)error();
		zero(a.rows, a.cols);
		memcpy(data, a.data, sizeof(T) * a.rows * a.cols);
		return *this;
	}
	/*----------------加法 [ + ]----------------*/
	Mat& add(Mat& a, Mat& b) {
		if (a.rows != b.rows || a.cols != b.cols)error();
		Mat ansTemp(a);
		for (int i = 0; i < a.rows * a.cols; i++)ansTemp[i] += b[i];
		eatMat(ansTemp);
		return *this;
	}
	/*----------------乘法 [ * ]----------------*/
	Mat& mult(const Mat& a, const Mat& b) {
		if (a.cols != b.rows) error();
		Mat ansTemp(a.rows, b.cols);
		for (int i = 0; i < a.rows; i++) {
			for (int j = 0; j < b.cols; j++) {
				T sum;
				memset(&sum, 0, sizeof(sum));
				for (int k = 0; k < a.cols; k++) {
					T aV = a.data[i * a.cols + k];
					T bV = b.data[k * b.cols + j];
					sum += aV * bV;
				}
				ansTemp.data[i * ansTemp.cols + j] = sum;
			}
		}
		eatMat(ansTemp);
		return *this;
	}
	/*----------------数乘 [ * ]----------------*/
	Mat& mult(const double a, const Mat& b) {
		Mat ansTemp(b.rows, b.cols);
		for (int i = 0; i < b.rows * b.cols; i++)
			ansTemp.data[i] = a * b.data[i];
		eatMat(ansTemp);
		return *this;
	}
	/*----------------点乘 [ · ]----------------*/	//a·b= aT * b
	Mat& dot(const Mat& a, const Mat& b) {
		Mat ansTemp;
		a.transposi(ansTemp);
		ansTemp.mult(ansTemp, b);
		eatMat(ansTemp);
		return *this;
	}
	/*----------------元素求和 [ sum() ]----------------*/
	void sum(int dim, Mat& ans) {
		int _col = 1, _row = 1;
		if (dim == 0)_row = rows;
		else if (dim == 1)_col = cols;
		ans.zero(_row, _col);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				ans[i] += data[i * cols + j];
			}
		}
	}
	/*----------------转置 [ trans() ]----------------*/
	void transposi(Mat& ans) {
		Mat ansTemp(cols, rows);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				ansTemp.data[j * rows + i] = data[i * cols + j];
			}
		}
		ans.eatMat(ansTemp);
	}
	/*----------------余子式 [ comi ]----------------*/
	/*----------------取逆 [ inv ]----------------*/
	/*----------------行列式 [ abs() ]----------------*/
	/*--------------伴随矩阵 [ adj() ]----------------*/
	/*----------------特征值特征向量 [ eig() ]----------------
	*	特征方程: AX = λX
	*		A: 目标矩阵		X: 特征向量		λ: 特征值
	*	性质:
	*		若 R 为正交矩阵 (R'R = E),有B = R`¹A R , 使得 BY = λY, 特征值不变.
	*				又有 X = R Y.
	*	[算法]雅可比迭代:
	*	* 原理:
	*		对于目标实矩阵A, 构造正交矩阵序列 R1, R2, ... , Rn，
	*			D0 = A
	*			Dj = RjT Dj-1 Rj
	*			=> limj->∞ Dj = D = diag(λ1, λ2, ... , λn)
	*		当非对角元素接近0时，算法即可停止。
		*
			AR, 右乘只改变 pth col and qth col
				djp = c ajp - s ajq
				djq = s ajp + c ajq
			RA, 左乘只改变 pth row and qth row
			R'AR:
				djp = c ajp - s ajq
				djq = s ajp + c ajq
				dpp = c² app + s² aqq - 2 c s apq
				dqq = s² app + c² aqq + 2 c s apq
				dpq = ( c² - s² ) apq + c s ( app - aqq )
				其他元素对称性可得
		*	每一步使得非对角线 dpq dqp 为零
			对dpq: (c² - s²)/(cs) = (aqq - app)/apq
			令 s = sinΦ	c = cosΦ	t = tanΦ = s / c
			θ = cot(2Φ) = (aqq - qpp) / (2 * apq)
			tan2Φ = (aqq - qpp) / apq = 2 * tanΦ / (1 - tan²Φ)
			t² + 2tθ - 1 = 0
	*------------------------------------------------*/
	void eig(T esp, Mat& eigvec, Mat& eigvalue) {
		if (rows != cols)return;
		//[1] init
		eigvalue.assign(*this);
		eigvec.E(rows);
		int n = rows;
		Mat<double> R, RT;
		//[2] begin iteration
		while (true) {
			//[3] Calculate row p and col q
			int p, q;
			T maxelement = eigvalue[1];
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (i != j && fabs(eigvalue[i * n + j]) >= maxelement) {
						maxelement = fabs(eigvalue[i * n + j]); p = i; q = j;
					}
				}
			}if (maxelement < esp)return;			// [2]
			//[4] eigvalue eigvec
			T theta = 0.5 * atan2(2 * eigvalue[p * n + q], eigvalue[q * n + q] - eigvalue[p * n + p]);
			T c = cos(theta), s = sin(theta);		// c,s
			R.E(n);
			R[p * n + p] = c; R[p * n + q] = s;		// R
			R[q * n + p] = -s; R[q * n + q] = c;
			R.transposi(RT);
			eigvalue.mult(RT, eigvalue);			// Dj = RjT Dj-1 Rj
			eigvalue.mult(eigvalue, R);
			eigvec.mult(eigvec, R);					// X = R Y
		}
	}
/******************************************************************************
*                    基础运算
******************************************************************************/
	/*----------------水平向拼接 [ eig() ]----------------*/
	void horizStack(Mat& a, Mat& b) {
		if (a.rows != b.rows)error();
		Mat ansTemp(a.rows, a.cols + b.cols);
		for (int i = 0; i < ansTemp.row; i++) {
			for (int j = 0; j < ansTemp.cols; j++) {
				ansTemp.data[i * cols + j] = j < a.cols ? a(i, j) : b(i, j - a.cols);
			}
		}
		eatMat(ansTemp);
		return *this;
	}
	void swap(Mat& a) {
		T* tptr = a.data;a.data = data;data = tptr;
		int t = a.rows; a.rows = rows; rows = t;
		t = a.cols; a.cols = cols; cols = t;
	}
};
#endif