#ifndef QUAT_H
#define QUAT_H

#include "Math.hpp"
#include "Vector3.hpp"

class Quaternion {
public:
	Quaternion() : x(0), y(0), z(0), s(0) {};
	Quaternion(float _s, float _x, float _y, float _z) : s(_s), x(_x), y(_y), z(_z) {};
	Quaternion(float _s, Vector3 _v) : s(_s), v(_v) {};

	Quaternion operator+(const Quaternion &rhs) const {
		return Quaternion(s+rhs.s, v+rhs.v);
	}

	Quaternion operator -(const Quaternion &rhs) const {
		return Quaternion(s-rhs.s, v-rhs.v);
	}

	Quaternion operator*(const Quaternion &rhs) const {
		Quaternion ret;
		ret.s = (s * rhs.s) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
		ret.x = (s * rhs.x) + (x * rhs.s) + (y * rhs.z) - (z * rhs.y);
		ret.y = (s * rhs.y) - (x * rhs.z) + (y * rhs.s) + (z * rhs.x);
		ret.z = (s * rhs.z) + (x * rhs.y) - (y * rhs.x) + (z * rhs.s);
		return ret;
	}

	void operator*=(const Quaternion& rhs){
		(*this) = (*this) * rhs;
	}

	void operator+=(const Quaternion& rhs){
		(*this) = (*this) + rhs;
	}

	void operator-=(const Quaternion& rhs){
		(*this) = (*this) - rhs;
	}

	float Length() const {
		return (float)sqrt(s*s + x*x + y*y + z*z);
	}

	float LengthSq() const {
		return (float)(s*s + x*x + y*y + z*z);
	}

	void Normalise(){
		float f = Length();
		s /= f;
		x /= f;
		y /= f;
		z /= f;
	}

	Quaternion Conjugate() const {
		return Quaternion(s, -v);
	}

	static inline Quaternion FromAxisAngle(const Vector3 &axis, float angle){
		return Quaternion(cosf(angle/2), axis*sinf(angle/2));
	}

	void ToAxisAngle(Vector3 &axis, float &angle) const{
		angle = acosf(s);

		float sinf_theta_inv = 1.0f/sinf(angle);

		axis.x = v.x*sinf_theta_inv;
		axis.y = v.y*sinf_theta_inv;
		axis.z = v.z*sinf_theta_inv;

		angle*=2;
	}

	static Quaternion FromEuler(const Vector3& angles){
		float cos_z_2 = cosf(0.5f*angles.z);
		float cos_y_2 = cosf(0.5f*angles.y);
		float cos_x_2 = cosf(0.5f*angles.x);

		float sin_z_2 = sinf(0.5f*angles.z);
		float sin_y_2 = sinf(0.5f*angles.y);
		float sin_x_2 = sinf(0.5f*angles.x);

		Quaternion ret;
		ret.s = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		ret.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		ret.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		ret.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;

		return ret;
	} 

	Vector3 ToEuler() const {
		float sqw = s*s;    
		float sqx = v.x*v.x;    
		float sqy = v.y*v.y;    
		float sqz = v.z*v.z;    

		Vector3 euler;
		euler.x = atan2f(2.f * (v.x*v.y + v.z*s), sqx - sqy - sqz + sqw);    		
		euler.y = asinf(-2.f * (v.x*v.z - v.y*s));
		euler.z = atan2f(2.f * (v.y*v.z + v.x*s), -sqx - sqy + sqz + sqw);    
		return euler;
	}

	union {
		struct {
			Vector3 v;
			float s;
		};

		struct {
			float x, y, z;
			float s;
		};
	};
};

#endif
