#pragma once
#include "../utils/UsefulDefines.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "MatrixN.h"

#include <math.h>
#include <algorithm>
#include <numeric>

#define V2ZERO Vector2(0,0)

//qol defines
#define RANDCOLOR olc::Pixel(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1)
#define RANDVEC(a) Vector3(rand() % a + 1, rand() % a + 1, rand() % a + 1)

//useful math things
//averages vectors over a interval i and returns that average
//pass in vector v and interval i
#define V_AVG(i, v) ([&] { \
		static std::vector<Vector3> vectors; \
		static Vector3 nv; \
		static int iter = 0; \
		if(i == vectors.size()){ \
			vectors.erase(vectors.begin()); \
			vectors.push_back(v); \
			iter++; \
		} \
		else{ \
			vectors.push_back(v); \
			iter++; \
		}\
		if(iter == i){ \
			nv = Math::averageVector3(vectors); \
			iter = 0; \
		} \
		return nv; \
		}())

//averages vectors but consistently returns the value
#define V_AVGCON(i, v) ([&] { \
		static std::vector<Vector3> vectors; \
		static Vector3 nv; \
		if(i == vectors.size()){ \
			vectors.erase(vectors.begin()); \
			vectors.push_back(v); \
		} \
		else{ \
			vectors.push_back(v); \
		} \
		nv = Math::averageVector3(vectors); \
		return nv; \
		}())

#define F_AVG(i, f) ([&] { \
		static std::vector<float> floats; \
		static float nf; \
		static int iter = 0; \
		if(i == floats.size()){ \
			floats.erase(floats.begin()); \
			floats.push_back(f); \
			iter++; \
		} \
		else{ \
			floats.push_back(f); \
			iter++; \
		}\
		if(iter == i){ \
			nf = Math::average(floats, floats.size()); \
			iter = 0; \
		} \
		return nf; \
		}())

namespace Math {
	static Vector4 ProjMult(Vector4 v, Matrix4 m) {
		Vector4 nv = v * m;
		if (nv.w != 0) { nv.x /= nv.w; nv.y /= nv.w; nv.z /= nv.w; }
		return nv;
	}

	static Matrix4 LocalToWorld(Vector3 offsetFromOrigin) {
		return Matrix4::TranslationMatrix(offsetFromOrigin);
	}

	static Matrix4 WorldToLocal(Vector3 offsetFromOrigin) {
		return Matrix4::TranslationMatrix(offsetFromOrigin).Inverse();
	}

}



//// Vector3 vs Vector4 Interactions ////

inline Vector4::Vector4(const Vector3& v, const float& w) {
	this->x = v.x; this->y = v.y; this->z = v.z; this->w = w;
}

inline Vector4 Vector3::ToVector4() const {
	return Vector4(x, y, z, 1);
}

inline Vector3 Vector4::ToVector3() const {
	return Vector3(x, y, z);
}



//// Matrix3 vs Matrix4 Interactions ////

inline Matrix4 Matrix3::To4x4() {
	return Matrix4(
		data[0],	data[1],	data[2],	0,
		data[3],	data[4],	data[5],	0,
		data[6],	data[7],	data[8],	0,
		0,			0,			0,			1
	);
}

inline Matrix3 Matrix4::To3x3() {
	return Matrix3(
		data[0], data[1], data[2],
		data[4], data[5], data[6],
		data[8], data[9], data[10]
	);
}



//// Vector vs Matrix Interactions ////

inline Vector3 Vector3::operator *  (const Matrix3& rhs) const {
	return Vector3(
		x*rhs.data[0] + y*rhs.data[3] + z*rhs.data[6], 
		x*rhs.data[1] + y*rhs.data[4] + z*rhs.data[7], 
		x*rhs.data[2] + y*rhs.data[5] + z*rhs.data[8]);
}

inline void Vector3::operator *= (const Matrix3& rhs) {
	*this = Vector3(
	x * rhs.data[0] + y * rhs.data[3] + z * rhs.data[6],
	x * rhs.data[1] + y * rhs.data[4] + z * rhs.data[7],
	x * rhs.data[2] + y * rhs.data[5] + z * rhs.data[8]);
}

inline Vector3 Vector3::operator *  (const Matrix4& rhs) const {
	return Vector3(
		x*rhs.data[0] + y*rhs.data[4] + z*rhs.data[8] + rhs.data[12],
		x*rhs.data[1] + y*rhs.data[5] + z*rhs.data[9] + rhs.data[13],
		x*rhs.data[2] + y*rhs.data[6] + z*rhs.data[10] + rhs.data[14]);
}

inline void Vector3::operator *= (const Matrix4& rhs) {
	*this = Vector3(
	x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8]  + rhs.data[12],
	x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9]  + rhs.data[13],
	x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + rhs.data[14]);
}

inline Vector4 Vector4::operator *  (const Matrix4& rhs) const {
	return Vector4(
		x*rhs.data[0] + y*rhs.data[4] + z*rhs.data[8]  + w*rhs.data[12],
		x*rhs.data[1] + y*rhs.data[5] + z*rhs.data[9]  + w*rhs.data[13],
		x*rhs.data[2] + y*rhs.data[6] + z*rhs.data[10] + w*rhs.data[14],
		x*rhs.data[3] + y*rhs.data[7] + z*rhs.data[11] + w*rhs.data[15]);
}

inline void Vector4::operator *= (const Matrix4& rhs) {
	*this = Vector4(
	x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8]  + w * rhs.data[12],
	x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9]  + w * rhs.data[13],
	x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + w * rhs.data[14],
	x * rhs.data[3] + y * rhs.data[7] + z * rhs.data[11] + w * rhs.data[15]);
}

//returns a rotation transformation matrix based on input in degrees
//rotates over the Y, then Z then X, ref: https://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
inline Matrix3 Matrix3::RotationMatrix(Vector3 rotation) {
	rotation *= TO_RADIANS;
	float cosX = cosf(rotation.x);
	float sinX = sinf(rotation.x);
	float cosY = cosf(rotation.y);
	float sinY = sinf(rotation.y);
	float cosZ = cosf(rotation.z);
	float sinZ = sinf(rotation.z);
	float r00 = cosY*cosZ;	float r01 = -cosY*sinZ*cosX + sinY*sinX;	float r02 = cosY*sinZ*sinX + sinY*cosX;
	float r10 = sinZ;		float r11 = cosZ*cosX;						float r12 = -cosZ*sinX;
	float r20 = -sinY*cosZ;	float r21 = sinY*sinZ*cosX + cosY*sinX;		float r22 = -sinY*sinZ*sinX + cosY*cosX;
	return Matrix3(
		r00,	r01,	r02,
		r10,	r11,	r12,
		r20,	r21,	r22);
}

//returns a scale matrix where (0,0) = scale.x, (1,1) = scale.y, (2,2) = scale.z
inline Matrix3 Matrix3::ScaleMatrix(Vector3 scale) {
	Matrix3 newMatrix = Matrix3::IDENTITY;
	newMatrix.data[0] = scale.x;
	newMatrix.data[4] = scale.y;
	newMatrix.data[8] = scale.z;
	return newMatrix;
}

//returns a rotation transformation matrix based on input in degrees
//rotates over the Y, then Z then X
inline Matrix4 Matrix4::RotationMatrix(Vector3 rotation) {
	rotation *= TO_RADIANS;
	float cosX = cosf(rotation.x);
	float sinX = sinf(rotation.x);
	float cosY = cosf(rotation.y);
	float sinY = sinf(rotation.y);
	float cosZ = cosf(rotation.z);
	float sinZ = sinf(rotation.z);
	float r00 = cosY*cosZ;	float r01 = -cosY*sinZ*cosX + sinY*sinX;	float r02 = cosY*sinZ*sinX + sinY*cosX;
	float r10 = sinZ;		float r11 = cosZ*cosX;						float r12 = -cosZ*sinX;
	float r20 = -sinY*cosZ;	float r21 = sinY*sinZ*cosX + cosY*sinX;		float r22 = -sinY*sinZ*sinX + cosY*cosX;
	return Matrix4(
		r00,	r01,	r02,	0,
		r10,	r11,	r12,	0,
		r20,	r21,	r22,	0,
		0,		0,		0,		1);
}

//returns a translation matrix where (0,3) = translation.x, (1,3) = translation.y, (2,3) = translation.z
inline Matrix4 Matrix4::TranslationMatrix(Vector3 translation) {
	return Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translation.x, translation.y, translation.z, 1);
}

//returns a scale matrix where (0,0) = scale.x, (1,1) = scale.y, (2,2) = scale.z
inline Matrix4 Matrix4::ScaleMatrix(Vector3 scale) {
	Matrix4 newMatrix = Matrix4::IDENTITY;
	newMatrix.data[0] = scale.x;
	newMatrix.data[4] = scale.y;
	newMatrix.data[8] = scale.z;
	return newMatrix;
}

//returns a transformation matrix of the combined translation, rotation, and scale matrices from input vectors
//rotates over the Y, then Z then X
inline Matrix4 Matrix4::TransformationMatrix(Vector3 translation, Vector3 rotation, Vector3 scale) {
	rotation *= TO_RADIANS;
	float cosX = cosf(rotation.x);
	float sinX = sinf(rotation.x);
	float cosY = cosf(rotation.y);
	float sinY = sinf(rotation.y);
	float cosZ = cosf(rotation.z);
	float sinZ = sinf(rotation.z);
	float r00 = cosY*cosZ;	float r01 = -cosY*sinZ*cosX + sinY*sinX;	float r02 = cosY*sinZ*sinX + sinY*cosX;
	float r10 = sinZ;		float r11 = cosZ*cosX;						float r12 = -cosZ*sinX;
	float r20 = -sinY*cosZ;	float r21 = sinY*sinZ*cosX + cosY*sinX;		float r22 = -sinY*sinZ*sinX + cosY*cosX;
	return Matrix4(
		scale.x*r00,	r01,			r02,			0,
		r10,			scale.y*r11,	r12,			0,
		r20,			r21,			scale.z*r22,	0,
		translation.x,	translation.y,	translation.z,	1);
}

//returns a transformation matrix of the combined translation, rotation, and scale matrices from input vectors
//rotates over the Y, then Z then X, ref: https://www.euclideanspace.com/maths/geometry/affine/aroundPoint/index.htm
inline Matrix4 Matrix4::RotationMatrixAroundPoint(Vector3 pivot, Vector3 rotation) {
	pivot = -pivot; //gotta negate this for some reason :)
	rotation *= TO_RADIANS;
	float cosX = cosf(rotation.x);
	float sinX = sinf(rotation.x);
	float cosY = cosf(rotation.y);
	float sinY = sinf(rotation.y);
	float cosZ = cosf(rotation.z);
	float sinZ = sinf(rotation.z);
	float r00 = cosY*cosZ;	float r01 = -cosY*sinZ*cosX + sinY*sinX;	float r02 = cosY*sinZ*sinX + sinY*cosX;
	float r10 = sinZ;		float r11 = cosZ*cosX;						float r12 = -cosZ*sinX;
	float r20 = -sinY*cosZ;	float r21 = sinY*sinZ*cosX + cosY*sinX;		float r22 = -sinY*sinZ*sinX + cosY*cosX;
	return Matrix4(
		r00, r01, r02, 0,
		r10, r11, r12, 0,
		r20, r21, r22, 0,
		pivot.x - r00*pivot.x - r01*pivot.y - r02*pivot.z, pivot.y - r10*pivot.x - r11*pivot.y - r12*pivot.z, pivot.z - r20*pivot.x - r21*pivot.y - r22*pivot.z, 1);
}

//// Non-Vector vs Vector Interactions ////

inline Vector3::Vector3(const Vector2& v) {
	this->x = v.x; this->y = v.y; this->z = 0;
}

inline Vector2 Vector3::ToVector2() const {
	return Vector2(x, y);
}

inline MatrixN Vector3::ToM1x3() const {
	return MatrixN(1, 3, {x, y, z});
}

inline MatrixN Vector3::ToM1x4(float w) const {
	return MatrixN(1, 4, {x, y, z, w});
}



//// Non-MatrixN vs MatrixN Interactions ////

//Creates a 1x3 matrix
inline MatrixN::MatrixN(Vector3 v) {
	this->rows = 1; this->cols = 3; this->elementCount = 3;
	this->data = {v.x, v.y, v.z};
}

//Creates a 1x4 matrix
inline MatrixN::MatrixN(Vector3 v, float w) {
	this->rows = 1; this->cols = 4; this->elementCount = 4;
	this->data = {v.x, v.y, v.z, w};
}



namespace Math {

	inline static float to_radians(float angle) { return angle * (M_PI / 180); }
	inline static float to_degrees(float angle) { return angle * (180 / M_PI); }

	inline static Vector3 to_radians(Vector3& vector) { return vector * (M_PI / 180); }
	inline static Vector3 to_degrees(Vector3& vector) { return vector * (180 / M_PI); }

	//ref: https://en.cppreference.com/w/cpp/algorithm/clamp
	static float clamp(float v, float lo, float hi) {
		ASSERT(lo < hi, "The low must be less than the high clamp");
		return (v < lo) ? lo : (hi < v) ? hi : v;
	}

	//for debugging with floats or doubles
	static std::string append_decimal(std::string s) {
		while (s.back() != '.') {
			s.pop_back();
		}
		s.pop_back();
		return s;
	}

	//append all trailing zeros
	static std::string append_zeroes(std::string s) {
		while (s.back() == '0') {
			s.pop_back();
		}
		s.pop_back();
		return s;
	}

	//append to two decimal places
	static std::string append_two_decimal(std::string s) {
		if (s.length() >= 2) {
			while (s.at(s.length() - 4) != '.') {
				s.pop_back();
			}
			s.pop_back();
		}
		return s;
	}

	static std::string str2f(float s) {
		char buffer[50];
		std::snprintf(buffer, 50, "%-.2f", s);
		return std::string(buffer);
	}

	//round a float to two decimal places
	static float round2f(float f) { return (float)((int)(f * 100 + .5)) / 100; }

	static Vector3 round2v(Vector3 v) {
		return Vector3(
			(float)((int)(v.x * 100 + .5)) / 100,
			(float)((int)(v.y * 100 + .5)) / 100,
			(float)((int)(v.z * 100 + .5)) / 100);
	}

	//average any std container probably
	template<class FWIt>
	static float average(FWIt a, const FWIt b, int size) { return std::accumulate(a, b, 0.0) / size; }

	template<class T>
	static double average(const T& container, int size) { return average(std::begin(container), std::end(container), size); }

	static Vector3 averageVector3(std::vector<Vector3> v) {
		Vector3 n;
		for (Vector3 e : v) { n += e; }
		if (v.size() != 0) return n / v.size();
		else return Vector3::ZERO;
	}

	//conversions
	static Vector3 vi2dToVector3(olc::vi2d vector, float z = 0) { return Vector3((float)vector.x, (float)vector.y, z); }

	//interpolating
	static float lerpf(float p1, float p2, float t) { return (1.f - t) * p1 + t * p2; }

	static Vector3 lerpv(Vector3 v1, Vector3 v2, float t) { return  v1 * (1.f - t) + v2 * t; }
	static Vector2 lerpv(Vector2 v1, Vector2 v2, float t) { return  v1 * (1.f - t) + v2 * t; }

	//this function returns a matrix that tells a vector how to look at a specific point in space.
	static Matrix4 LookAtMatrix(const Vector3& pos, const Vector3& target) {
		//get new forward direction
		Vector3 newFor = (target - pos).normalized();

		//get right direction
		Vector3 newRight; 
		if(newFor == Vector3::UP || newFor == Vector3::DOWN) { 
			newRight = Vector3::RIGHT;
			//newRight = Vector3(0,.999f,0).cross(newFor);
		} else {
			newRight = (Vector3::UP.cross(newFor)).normalized(); 
		}

		//get up direction
		Vector3 newUp = newRight.cross(newFor); 

		//make point at matrix
		return Matrix4(
			newRight.x, newRight.y, newRight.z, 0,
			newUp.x,	newUp.y,	newUp.z,	0,
			newFor.x,	newFor.y,	newFor.z,	0,
			pos.x, pos.y, pos.z, 1
		);
	}

	//this assumes a rectangle whose pivot is the top right corner
	static bool PointInRect(Vector2 size, Vector2 pos, Vector2 point) {
		return pos.x < point.x&& point.x < pos.x + size.x &&
			pos.y < point.y&& point.y < pos.y + size.y;
	}

	static float DistTwoPoints(Vector3 a, Vector3 b) {
		return sqrtf(
			(a.x - b.x) * (a.x - b.x) +
			(a.y - b.y) * (a.y - b.y) +
			(a.z - b.z) * (a.z - b.z)
		);
	}

	static float DistPointToPlane(Vector3 point, Vector3 plane_n, Vector3 plane_p) {
		return (plane_n.x * point.x + plane_n.y * point.y + plane_n.z * point.z - plane_n.dot(plane_p));
	}

	//where a line intersects with a plane
	static Vector3 VectorPlaneIntersect(Vector3 plane_p, Vector3 plane_n, Vector3 line_start, Vector3 line_end, float& t) {
		//plane_n.normalize();
		float plane_d = -plane_n.dot(plane_p);
		float ad = line_start.dot(plane_n);
		float bd = line_end.dot(plane_n);
		t = (-plane_d - ad) / (bd - ad);
		Vector3 line_start_to_end = line_end - line_start;
		Vector3 line_to_intersect = line_start_to_end * t;
		return line_start + line_to_intersect;
	}

	//return where two lines intersect on the x axis with slope and the y-intercept
	static Vector3 LineIntersect2(float slope1, float ycross1, float slope2, float ycross2) {
		MatrixN lhs(2,2,{ slope1, ycross1, slope2, ycross2 });
		MatrixN rhs(2,1,{ 1, 1 });
		MatrixN det = lhs.Inverse() * rhs;
		float x = 1 / det(0,1) * det(0,0);
		float y = slope1 * x + ycross1;
		return Vector3(x, y);
	}

	//returns where two lines intersect in 3D space //TODO(,sushi) implement this
	static Vector3 LineIntersect3(Vector3 adir, Vector3 ap, Vector3 bdir, Vector3 bp) {
	}

	//returns area of a triangle of sides a and b
	static float TriangleArea(Vector3 a, Vector3 b) { return a.cross(b).mag() / 2; }

	static Vector4 WorldToCamera(Vector3 vertex, Matrix4 viewMatrix) {
		return Math::ProjMult(vertex.ToVector4(), viewMatrix);
	}

	static Vector3 CameraToScreen(Vector3 csVertex, Matrix4 projectionMatrix, Vector2 screenDimensions) {
		Vector3 vm = Math::ProjMult(csVertex.ToVector4(), projectionMatrix).ToVector3();
		vm.x += 1.0f; vm.y += 1.0f;
		vm.x *= 0.5f * (float)screenDimensions.x;
		vm.y *= 0.5f * (float)screenDimensions.y;
		return vm;
	}

	static Vector3 CameraToScreen(Vector3 csVertex, Matrix4 projectionMatrix, Vector2 screenDimensions, float& w) {
		Vector4 bleh = csVertex.ToVector4() * projectionMatrix;
		w = bleh.w;
		Vector3 vm = bleh.normalized().ToVector3();
		vm.x += 1.0f; vm.y += 1.0f;
		vm.x *= 0.5f * (float)screenDimensions.x;
		vm.y *= 0.5f * (float)screenDimensions.y;
		return vm;
	}

	static Vector3 CameraToScreen(Vector4 csVertex, Matrix4 projectionMatrix, Vector2 screenDimensions) {
		Vector3 vm = Math::ProjMult(csVertex, projectionMatrix).ToVector3();
		vm.x += 1.0f; vm.y += 1.0f;
		vm.x *= 0.5f * (float)screenDimensions.x;
		vm.y *= 0.5f * (float)screenDimensions.y;
		return vm;
	}

	static Vector4 CameraToScreenV4(Vector4 csVertex, Matrix4 projectionMatrix, Vector2 screenDimensions) {
		Vector4 vm = (csVertex * projectionMatrix).normalized();
		vm.x += 1.0f; vm.y += 1.0f;
		vm.x *= 0.5f * (float)screenDimensions.x;
		vm.y *= 0.5f * (float)screenDimensions.y;
		return vm;
	}

	static Vector3 WorldToScreen(Vector3 point, Matrix4 ProjMat, Matrix4 ViewMat, Vector2 screenDimensions) {
		return CameraToScreen(WorldToCamera(point, ViewMat), ProjMat, screenDimensions);
	}

	static Vector2 WorldToScreen2D(Vector3 point, Matrix4 ProjMat, Matrix4 ViewMat, Vector2 screenDimensions) {
		Vector3 v = CameraToScreen(WorldToCamera(point, ViewMat), ProjMat, screenDimensions);
		return Vector2(v.x, v.y);
	}

	static Vector3 ScreenToWorld(Vector2 pos, Matrix4 ProjMat, Matrix4 view, Vector2 screenDimensions) {
		Vector3 out(pos);
		out.x /= .5f * (float)screenDimensions.x;
		out.y /= .5f * (float)screenDimensions.y;
		out.x -= 1.f; out.y -= 1.f; out.z = -1.f;
		out = Math::ProjMult(out.ToVector4(), ProjMat.Inverse()).ToVector3();
		out = Math::ProjMult(out.ToVector4(), view.Inverse()).ToVector3();
		return out;
	}
};