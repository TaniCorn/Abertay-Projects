//////////
//////////Vector 2 files
//////////Written by Tanapat Somrid 
/////////Starting 01/12/2021
//////// Most Recent Update 06/01/2022
//////// Most Recent change: Changed Magnitude Function, added multiply function

#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>
#include <iostream>

/// <summary>
/// Used for easy (and soon to be complex) Vector2 Operations
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T> class Vector2 {
public:

	#pragma region CONSTRUCTORS
	/// <summary>
/// Default Constructor
/// </summary>
	Vector2() {};
	/// <summary>
	/// Individual Coordinate Constructor
	/// </summary>
	Vector2(T x, T y) : Vector2() { this->x = x; this->y = y; }
	/// <summary>
	/// Assignment with different typename
	/// </summary>
	template <typename U> explicit Vector2(const Vector2<U>& difPrimitive) {
		x = static_cast<T>(difPrimitive.x); y = static_cast<T>(difPrimitive.y);
	}
	//Copy Constructors
	Vector2(const Vector2& rhs) { x = rhs.x; y = rhs.y; }//Required for assignment and required for below operatorr//	Vector2<int> dif = position;
	Vector2& operator =(const Vector2& copy) { x = copy.x; y = copy.y; return *this; }//So this will copy/assign//		Node* endNode = new Node(endPos);
#pragma endregion

	T x, y;

	#pragma region FUNCTIONS
	//T Magnitude() const{
	//	return (T)(sqrt(abs(x * x) + abs(y * y)));
	//}
	float Magnitude() const {
		return (float)(sqrt(abs(x * x) + abs(y * y)));
	}
	static float Magnitude(const Vector2<T>& vector) {
		return (float)(sqrt(abs(vector.x * vector.x) + abs(vector.y * vector.y)));
	}
	void Normalise() {
		*this = *this / static_cast<int>(this->Magnitude());
	}
	Vector2<float> Normalize() {
		return Vector2<float>(static_cast<Vector2<float>>(*this) / this->Magnitude());
	}
	/// <summary>
	/// The EUCLIDEAN distance from this vector and the given vector
	/// </summary>
	float DistanceFrom(const Vector2<T>& pos) {
		int xl = abs(pos.x - x); int yl = abs(pos.y - y);
		float sq = sqrt((xl*xl) + (yl * yl));
		return sq;
	}
	/// <summary>
	/// The EUCLIDEAN distance between two given vectors
	/// <para> From vector A to vector B</para>
	/// </summary>
	static float DistanceBetween(const Vector2& a, const Vector2& b) {
		int x = abs(b.x - a.x); int y = abs(b.y - a.y);
		float sq = sqrt((x * x) + (y * y));
		return sq;
	}
#pragma endregion
};

//I had a slight understanding of operator overload already but this was the first time trying to do it in a templated class setting. 
//All operators besides the assignment operator seems to need to be outside the class,
//Although maybe when I test the others i'll find out the assigment/increments (+= -+) will need to be inside the class

#pragma region OPERATOR FUNCTIONS
template <typename T> Vector2<T> operator +(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>((lhs.x + rhs.x), (lhs.y + rhs.y));
}
template <typename T> Vector2<T> operator -(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
template <typename T> Vector2<float> operator -(const Vector2<T>& lhs, const Vector2<float>& rhs) {
	return Vector2<float>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
template <typename T, typename U> Vector2<float> operator -(const Vector2<T>& lhs, const Vector2<U>& rhs) {
	return Vector2<float>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
template <typename T> Vector2<T> operator /(const Vector2<T>& lhs, const T& rhs) {
	return Vector2<T>((lhs.x / rhs), (lhs.y / rhs));
}
template <typename T> Vector2<T> operator *(const Vector2<T>& lhs, const T& rhs) {
	return Vector2<T>((lhs.x * rhs), (lhs.y * rhs));
}
template <typename T> void operator +=(Vector2<T>& lhs, const Vector2<T>& rhs) {
	lhs.x = lhs.x + rhs.x;
	lhs.y = lhs.y + rhs.y;
	return;
}
template <typename T> void operator -=(Vector2<T>& lhs, const Vector2<T>& rhs) {
	lhs.x = lhs.x - rhs.x;
	lhs.y = lhs.y - rhs.y;
	return;
}
template <typename T> void operator /=(const Vector2<T>& lhs, const T& rhs) {
	lhs.x = lhs.x / rhs.x;
	lhs.y = lhs.y / rhs.y;
	return;
}
template <typename T> bool operator ==(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}
template <typename T> bool operator !=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return !(lhs.x == rhs.x && lhs.y == rhs.y);
}
template <typename T> bool operator >(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return (lhs.Magnitude() > rhs.Magnitude());
}
template <typename T> bool operator <(const Vector2<T>& lhs, const Vector2<T>& rhs){
	return (lhs.Magnitude() < rhs.Magnitude());
}
template<typename T> std::ostream& operator<<(std::ostream& stream, Vector2<T> const& vector)
{
	return stream << "X:" << vector.x << " Y:" << vector.y;
}
#pragma endregion


#endif // !VECTOR2_H