/*****************************************************************************/
// File         : matrix.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-18
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : matrix for pure C/C++ or host matrix for cuda application
/*****************************************************************************/


#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "util.hpp"
#include "memory.hpp"


#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

namespace su{

	template<class _T = int, size_t _S = _DEFAULT_AIGNED_WIDTH_BYTES, class _MEM = su::Mem>
	class Mat
	{
	public:
		Mat();
		Mat(const size_t _rows, const size_t _cols, const size_t _layers = 1);

		Mat(const su::Mat<_T, _S, _MEM> &_obj);
		Mat<_T, _S, _MEM> operator=(const su::Mat<_T, _S, _MEM> &_obj);
		Mat<_T, _S, _MEM> operator=(const _T &v);

		~Mat();

	public:
		void create(const size_t _rows, const size_t _cols, const size_t _layers = 1);
		void clear(){ memset(data, 0, layers*steps*rows*sizeof(_T)); }

	public:
		inline       _T& operator[](const size_t &i)      { return data[i]; }
		inline const _T& operator[](const size_t &i) const{ return data[i]; }

		inline       _T& operator()(const size_t &r, const size_t &c)      { return data[r*steps + c]; }
		inline const _T& operator()(const size_t &r, const size_t &c) const{ return data[r*steps + c]; }

		inline       _T& operator()(const size_t &r, const size_t &c, const size_t &l)      { return data[r*steps + c + l*rows*steps]; }
		inline const _T& operator()(const size_t &r, const size_t &c, const size_t &l) const{ return data[r*steps + c + l*rows*steps]; }

		inline       _T* row(const size_t &r)       { return this->data + r*steps; }
		inline const _T* row(const size_t &r) const { return this->data + r*steps; }
		inline       _T* row(const size_t &r, const size_t &l)       { return this->data + r*steps + l*rows*steps; }
		inline const _T* row(const size_t &r, const size_t &l) const { return this->data + r*steps + l*rows*steps; }

	public:
		_T max();
		_T min();
		_T sum();

	public:
		template<class _TMM>
		Mat<_T, _S, _MEM> operator-(const su::Mat<_T, _S, _TMM> &_obj);
		template<class _TMM>
		Mat<_T, _S, _MEM> operator+(const su::Mat<_T, _S, _TMM> &_obj);
		Mat<_T, _S, _MEM> abs();

	public:
		static Mat<_T, _S, _MEM> zeros(size_t _rows, size_t _cols, size_t _layers = 1);
		static Mat<_T, _S, _MEM>  ones(size_t _rows, size_t _cols, size_t _layers = 1);
		static Mat<_T, _S, _MEM>   eye(size_t _rows, size_t _cols, size_t _layers = 1);

	public:
		friend std::ostream& operator<<(std::ostream& os, const Mat &_mat){
			for (size_t l = 0; l < _mat.layers; l++){
				for (size_t r = 0; r < _mat.rows; r++){
					for (size_t c = 0; c < _mat.cols - 1; c++){
						os << _mat(r, c, l) << ", ";
					}
					if (r != _mat.rows)
						os << _mat(r, _mat.cols - 1, l) << std::endl;
				}
				if (l != _mat.layers - 1){
					os << std::endl;
				}
			}
			return os;
		}

	public:
		_T *data;
		size_t layers, steps;
		union {
			size_t cols;
			size_t width;
		};
		union {
			size_t rows;
			size_t height;
		};
	};
	

	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM>::~Mat(){
		if (data){
			_MEM::release(data); data = NULL;
		}
		rows = layers = steps = cols = 0;
	}

	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM>::Mat(){
		this->cols = this->rows = this->steps = this->layers = 0;
		this->data = NULL;
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM>::Mat(const size_t _rows, const size_t _cols, const size_t _layers){
		this->cols = this->rows = this->steps = this->layers = 0;
		this->data = NULL;
		this->create(_rows, _cols, _layers);
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM>::Mat(const su::Mat<_T, _S, _MEM> &_obj){
		this->cols = this->rows = this->steps = this->layers = 0;
		this->data = NULL;
		this->create(_obj.rows, _obj.cols, _obj.layers);
		memcpy(data, _obj.data, layers*rows*steps*sizeof(_T));
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::operator=(const su::Mat<_T, _S, _MEM> &obj){
		this->create(obj.rows, obj.cols, obj.layers);
		memcpy(data, obj.data, layers*rows*steps*sizeof(_T));
		return *this;
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S,_MEM> su::Mat<_T, _S, _MEM>::operator=(const _T &v){
		SU_ASSERT(data != NULL, "null data");
		for (size_t t = 0; t < layers*steps*rows; t++){
			data[t] = v;
		}
		return *this;
	}
	
	template<class _T, size_t _S, class _MEM>
	void su::Mat<_T, _S, _MEM>::create(const size_t _rows, const size_t _cols, const size_t _layers){
		if (this->cols == _cols && this->rows == _rows && _layers == this->layers){
			return;
		}
		this->rows = _rows;
		this->cols = _cols;
		this->layers = _layers;
	
		// align x axis
		//steps = (((cols*sizeof(_T)+_S - 1) / _S)*_S) / sizeof(_T);
		steps = su::aligned_words<_T, _S>(cols);
		
		// re-creating 
		if (data){ // data not null
			_MEM::release(data); data = NULL;
		}
		if (layers*rows*steps == 0){
			data = NULL; return;
		}
		data = (_T*)_MEM::malloc(layers*rows*steps*sizeof(_T), _S);
		SU_ASSERT(NULL != data, " null pointer ");
	}
	
	
	template<class _T, size_t _S, class _MEM>
	_T su::Mat<_T, _S, _MEM>::max(){
		SU_ASSERT(NULL != data, "");
		_T maxv = data[0];
		for (size_t l = 0; l < layers; l++){
			for (size_t r = 0; r < rows; r++){
				_T *pr = row(r, l);
				for (size_t c = 0; c < cols; c++){
					if (maxv < pr[c]){
						maxv = pr[c];
					}
				}
			}
		}
		return maxv;
	}
	
	template<class _T, size_t _S, class _MEM>
	_T su::Mat<_T, _S, _MEM>::min(){
		SU_ASSERT(NULL != data, "");
		_T minv = data[0];
		for (size_t l = 0; l < layers; l++){
			for (size_t r = 0; r < rows; r++){
				_T *pr = row(r, l);
				for (size_t c = 0; c < cols; c++){
					if (minv > pr[c]){
						minv = pr[c];
					}
				}
			}
		}
		return minv;
	}
	
	template<class _T, size_t _S, class _MEM>
	_T su::Mat<_T, _S, _MEM>::sum(){
		SU_ASSERT(NULL != data, "");
		_T sumv = 0;
		for (size_t l = 0; l < layers; l++){
			for (size_t r = 0; r < rows; r++){
				_T *pr = row(r, l);
				for (size_t c = 0; c < cols; c++){
					sumv += pr[c];
				}
			}
		}
		return sumv;
	}
	

	template<class _T, size_t _S, class _MEM>
	template<class _TMM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::operator-(const su::Mat<_T, _S, _TMM> &_obj){
		SU_ASSERT(NULL != data, "");
		SU_ASSERT(_obj.rows == this->rows && _obj.cols == this->cols && _obj.layers == this->layers, "");
		su::Mat<_T, _S, _MEM> res(*this);
		for (size_t k = 0; k < layers*rows*steps; k++){
			res[k] = res[k] - _obj[k];
		}
		return res;
	}
	
	template<class _T, size_t _S, class _MEM>
	template<class _TMM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::operator+(const su::Mat<_T, _S, _TMM> &_obj){
		SU_ASSERT(NULL != data, "");
		SU_ASSERT(_obj.rows == this->rows && _obj.cols == this->cols && _obj.layers == this->layers, "");
		su::Mat<_T, _S, _MEM> res(*this);
		for (size_t k = 0; k < layers*rows*steps; k++){
			res[k] = res[k] + _obj[k];
		}
		return res;
	}
	
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::abs(){
		su::Mat<_T, _S, _MEM> res(*this);
		for (size_t k = 0; k < layers*rows*steps; k++){
			res[k] = std::abs(res[k]);
		}
		return res;
	}
	
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::zeros(size_t _rows, size_t _cols, size_t _layers){
		su::Mat<_T, _S, _MEM> res(_rows, _cols, _layers);
		res.clear();
		return res;
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S, _MEM>::ones(size_t _rows, size_t _cols, size_t _layers){
		su::Mat<_T, _S, _MEM> res(_rows, _cols, _layers);
		for (size_t i = 0; i < res.rows*res.steps*res.layers; i++){
			res[i] = _T(1);
		}
		return res;
	}
	
	template<class _T, size_t _S, class _MEM>
	su::Mat<_T, _S, _MEM> su::Mat<_T, _S,_MEM>::eye(size_t _rows, size_t _cols, size_t _layers){
		su::Mat<_T, _S, _MEM> res(_rows, _cols, _layers);
		res.clear();
		size_t md = _cols > _rows ? _rows : _cols;
		for (size_t l = 0; l < _layers; l++){
			for (size_t x = 0; x < md; x++){
				res(x, x, l) = _T(1);
			}
		}
		return res;
	}

}; // end namespace suan


#endif