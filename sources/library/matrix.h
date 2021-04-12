#pragma once
#include "hlsdk.h"
#include <vector>
#include <string>
#include <assert.h>

template<class T> class Matrix 
{
public:
	Matrix(int rows, int cols)
	{
		Initialize(rows, cols);
	}

	Matrix(int rows, int cols, const std::vector<int> &data)
	{
		Initialize(rows, cols);
		m_Values.assign(&data[0], &data[0] + rows * cols);
	}

	inline int GetRows() const				{ return m_iRows; }
	inline int GetColumns() const			{ return m_iColumns; }
	inline void Resize(int rows, int cols)	{ Initialize(rows, cols); }
	inline const T &GetElementAt(int row, int col) const {
		return m_Values[GetIndex(row, col)];
	}
	inline T &ElementAt(int row, int col) {
		return m_Values[GetIndex(row, col)];
	}

	void Assign(const Matrix &src) 
	{
		Initialize(src.GetRows(), src.GetColumns());
		for (int i = 1; i <= src.GetRows(); ++i)
		{
			for (int j = 1; j <= src.GetColumns(); ++j)
				ElementAt(i, j) = src.GetElementAt(i, j);
		}
	}

	Matrix Add(const Matrix &operand) const
	{
		assert(m_iRows == operand.GetRows() && m_iColumns == operand.GetColumns());
		Matrix result(m_iRows, m_iColumns);
		for (int i = 1; i <= m_iRows; ++i)
		{
			for (int j = 1; j <= m_iColumns; ++j)
				result.ElementAt(i, j) = GetElementAt(i, j) + operand.GetElementAt(i, j);
		}
		return result;
	}

	Matrix Sub(const Matrix &operand) const
	{
		assert(m_iRows == operand.GetRows() && m_iColumns == operand.GetColumns());
		Matrix result(m_iRows, m_iColumns);
		for (int i = 1; i <= m_iRows; ++i)
		{
			for (int j = 1; j <= m_iColumns; ++j)
				result.ElementAt(i, j) = GetElementAt(i, j) - operand.GetElementAt(i, j);
		}
		return result;
	}

	Matrix Multiply(const Matrix &operand) const
	{
		/*
		* result matrix height (rows count) = this matrix height
		* and result matrix width (columns count) = operand matrix width
		*/
		assert(m_iColumns == operand.GetRows());
		Matrix result(m_iRows, operand.GetColumns());
		for (int i = 1; i <= result.GetRows(); ++i)
		{
			for (int j = 1; j <= result.GetColumns(); ++j)
			{
				for (int k = 1; k <= operand.GetRows(); ++k)
					result.ElementAt(i, j) += GetElementAt(i, k) * operand.GetElementAt(k, j);
			}
		}
		return result;
	}

	Matrix Product(T x) const
	{
		Matrix result(m_iRows, m_iColumns);
		for (int i = 1; i <= m_iRows; ++i)
		{
			for (int j = 1; j <= m_iColumns; ++j)
				result.ElementAt(i, j) *= x;
		}
		return result;
	}

	Matrix Power(int power) const
	{
		assert(power > 1);
		Matrix thisMatrix = *this;
		Matrix result = thisMatrix;
		for (int i = 1; i < power; ++i) {
			result.Assign(result.Multiply(thisMatrix));
		}
		return result;
	}

	static Matrix CreateIdentity(int size = 2)
	{
		Matrix result(size, size);
		for (int i = 1; i <= size; ++i)
		{
			for (int j = 1; j <= size; ++j)
			{
				if (i == j)
					result.ElementAt(i, j) = 1;
				else
					result.ElementAt(i, j) = 0;
			}
		}
		return result;
	}

	static Matrix CreateTranslate(float x, float y, float z)
	{
		Matrix result = CreateIdentity(4);
		result.ElementAt(1, 4) = x;
		result.ElementAt(2, 4) = y;
		result.ElementAt(3, 4) = z;
		return result;
	}

	static Matrix CreateRotateX(float angle)
	{
		Matrix result = CreateIdentity(4);
		const float pi = 3.141592f;
		const float radAngle = angle * (pi * 2 / 360);
		result.ElementAt(2, 2) = cosf(radAngle);
		result.ElementAt(3, 2) = sinf(radAngle);
		result.ElementAt(2, 3) = -sinf(radAngle);
		result.ElementAt(3, 3) = cosf(radAngle);
		return result;
	}

	static Matrix CreateRotateY(float angle)
	{
		Matrix result = CreateIdentity(4);
		const float pi = 3.141592f;
		const float radAngle = angle * (pi * 2 / 360);
		result.ElementAt(1, 1) = cosf(radAngle);
		result.ElementAt(1, 3) = sinf(radAngle);
		result.ElementAt(3, 1) = -sinf(radAngle);
		result.ElementAt(3, 3) = cosf(radAngle);
		return result;
	}

	static Matrix CreateRotateZ(float angle)
	{
		Matrix result = CreateIdentity(4);
		const float pi = 3.141592f;
		const float radAngle = angle * (pi * 2 / 360);
		result.ElementAt(1, 1) = cosf(radAngle);
		result.ElementAt(1, 2) = -sinf(radAngle);
		result.ElementAt(2, 1) = sinf(radAngle);
		result.ElementAt(2, 2) = cosf(radAngle);
		return result;
	}

	// HLSDK stuff
	vec3_t MultiplyVector(const vec3_t &operand)
	{
		Matrix vecMat(4, 1);
		vecMat.ElementAt(1, 1) = operand.x;
		vecMat.ElementAt(2, 1) = operand.y;
		vecMat.ElementAt(3, 1) = operand.z;
		vecMat.ElementAt(4, 1) = 1;
		Matrix multResult = Multiply(vecMat);
		return vec3_t(
			multResult.GetElementAt(1, 1),
			multResult.GetElementAt(2, 1),
			multResult.GetElementAt(3, 1)
		);
	}

	// operators
	Matrix operator+(const Matrix &operand) const { return Add(operand); }
	Matrix operator-(const Matrix &operand) const { return Sub(operand); }
	Matrix operator*(const Matrix &operand) const { return Multiply(operand); }
	Matrix operator*(const T &operand) const { return Product(operand); }
	void operator=(const Matrix &operand) { Assign(operand); }

private:
	int GetIndex(int row, int col) const
	{ 
		return (col - 1) + (row - 1) * m_iColumns; 
	}

	void Initialize(int rows, int cols)
	{
		m_iRows = rows;
		m_iColumns = cols;
		m_Values.resize(m_iRows * m_iColumns, 0);
	}

	int m_iRows;
	int m_iColumns;
	std::vector<T> m_Values;
};
