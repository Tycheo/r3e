#ifndef VECTOR3_H
#define VECTOR3_H

#include "Math.hpp"

class Vector3 {
public:
	Vector3(): x(0), y(0), z(0) {}
	Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vector3(float s) : x(s), y(s), z(s){}

	const Vector3 operator -(void) const {
		return Vector3(-x, -y, -z);
	}

	Vector3 operator+(const Vector3& rhs) const {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z );
	}

	Vector3 operator-(const Vector3& rhs) const {
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z );
	}

	Vector3 operator*(const float& rhs) const {
		return Vector3(x * rhs, y * rhs, z * rhs);
	}

	Vector3 operator*(const Vector3& rhs) const {
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	Vector3 operator/(const Vector3& rhs) const {
		return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	void operator+=(const Vector3& rhs){
		(*this) = (*this) + rhs;
	}

	void operator-=(const Vector3& rhs){
		(*this) = (*this) - rhs;
	}

	void operator*=(const Vector3& rhs){
		(*this) = (*this) * rhs;
	}

	void operator/=(const Vector3& rhs){
		(*this) = (*this) / rhs;
	}

	float Length() const {
		return sqrt(x * x + y * y + z * z);
	}

	float LengthSq() const {
		return x * x + y * y + z * z;
	}

	bool IsZero() const {
		return (x + y + z) < 0.001f;
	}

	float Normalize(){
		float norm  = Length();

		if(!norm){
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}else{
			x = x / norm;
			y = y / norm;
			z = z / norm;
		}

		return norm;
	}

	static Vector3 CreateCross(const Vector3& v1, const Vector3& v2){
		Vector3 v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	static Vector3 CreateNormalized(const Vector3& in){
		Vector3 vec = in;
		vec.Normalize();
		return vec;
	}

	static float CreateDotProduct(const Vector3& v1, const Vector3& v2){
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	float x, y, z;
};

#endif
