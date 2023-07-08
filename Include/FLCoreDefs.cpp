#include <reactphysics3d/reactphysics3d.h>

const Vector Vector::Zero;
const Vector Vector::UnitX (1.0f, 0, 0.0f);
const Vector Vector::UnitY (0, 1, 0);
const Vector Vector::UnitZ (0, 0, 1);
const Vector Vector::UnitScale (1, 1, 1);

Vector::Vector()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector::Vector(reactphysics3d::Vector3 vec)
{
    this->x = static_cast<float>(vec.x);
    this->y = static_cast<float>(vec.y);
    this->z = static_cast<float>(vec.z);
}

bool Vector::operator==(const Vector& vector) const
{
	if (this == &vector)
		return true;

	if (x == vector.x && y == vector.y && z == vector.z)
		return true;

	return false;
}

bool Vector::operator!=(const Vector& vector) const
{
	return !(*this == vector);
}

Vector& Vector::operator+=(const Vector& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;

	return *this;
}

Vector Vector::operator+(const Vector& vector) const
{
	return Vector(x, y, z) += vector;
}

Vector& Vector::operator-=(const Vector& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;

	return *this;
}

Vector Vector::operator-(const Vector& vector) const
{
	return Vector(x, y, z) -= vector;
}

float Vector::LengthSquared() const
{
	return x * x + y * y + z * z;
}

float Vector::DotProduct(const Vector& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector Vector::CrossProduct(const Vector& v) const
{
	return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector Vector::MidPoint(const Vector& v) const
{
	return Vector((x + v.x) * 0.5f, (y + v.y) * 0.5f, (z + v.z) * 0.5f);
}

Vector Vector::operator*(float mult) const
{
    return Vector(this->x * mult, this->y * mult, this->z * mult);
}

Vector Vector::operator/(float mult) const
{
    return Vector(this->x / mult, this->y / mult, this->z / mult);
}

Vector Vector::operator*(Matrix& mat) const
{
    Vector kProd;
    kProd.x = x * mat.data[0][0] + 
        y * mat.data[1][0] +
        z * mat.data[2][0];

    kProd.y = x * mat.data[0][1] +
        y * mat.data[1][1] +
        z * mat.data[2][1];

    kProd.z = x * mat.data[0][2] +
        y * mat.data[1][2] +
        z * mat.data[2][2];
    return kProd;
}

Vector& Vector::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

bool Vector::operator<(const Vector& v) const
{
	return (x < v.x && y < v.y && z < v.z);
}

bool Vector::operator>(const Vector& v) const
{
	return (x > v.x && y > v.y && z > v.z);
}

Vector Vector::Lerp(const Vector& right, float time)
{
	return *this * time + right * time * (1.0f - time);
}

Vector Vector::Perpendicular() const
{
	static float fSquareZero = 1e-06f * 1e-06f;

	Vector perp = this->CrossProduct(UnitX);

	// Check length
	if (perp.LengthSquared() < fSquareZero)
	{
		/* This vector is the Y axis multiplied by a scalar, so we have to use another axis. */
		perp = this->CrossProduct(UnitY);
	}

	return perp;
}

void Vector::Normalise()
{
    float fMag = Length();
    float fInvMag = (std::abs(fMag) > 0.000f) ? 1.0f / fMag : 1.00e+12f;
    x *= fInvMag;
    y *= fInvMag;
    z *= fInvMag;
	
	/*auto f = static_cast<float>(sqrt(x * x + y * y + z * z));

	// Will also work for zero-sized vectors, but will change nothing
	if (f > 1e-06f)
	{
		f = 1.0f / f;
		x *= f;
		y *= f;
		z *= f;
	}*/
}

Vector Vector::operator*(Quaternion& rotation) const
{
    float x2 = rotation.x + rotation.x;
    float y2 = rotation.y + rotation.y;
    float z2 = rotation.z + rotation.z;

    float wx2 = rotation.w * x2;
    float wy2 = rotation.w * y2;
    float wz2 = rotation.w * z2;
    float xx2 = rotation.x * x2;
    float xy2 = rotation.x * y2;
    float xz2 = rotation.x * z2;
    float yy2 = rotation.y * y2;
    float yz2 = rotation.y * z2;
    float zz2 = rotation.z * z2;

    return Vector(
        this->x * (1.0f - yy2 - zz2) + this->y * (xy2 - wz2) + this->z * (xz2 + wy2),
        this->x * (xy2 + wz2) + this->y * (1.0f - xx2 - zz2) + this->z * (yz2 - wx2),
        this->x * (xz2 - wy2) + this->y * (yz2 + wx2) + this->z * (1.0f - xx2 - yy2));
}

float Vector::Length() const
{
	return sqrt(LengthSquared());
}

Matrix::Matrix()
{
	data[0][0] = 1;
	data[0][1] = 0;
	data[0][2] = 0;

	data[1][0] = 0;
	data[1][1] = 1;
	data[1][2] = 0;

	data[2][0] = 0;
	data[2][1] = 0;
	data[2][2] = 1;
}

Matrix::Matrix(float x, float y, float z)
{
	float cosY = cosf(y); // Yaw
	float sinY = sinf(y);

	float cosP = cosf(x); // Pitch
	float sinP = sinf(x);

	float cosR = cosf(z); // Roll
	float sinR = sinf(z);

	this->data[0][0] = cosY * cosR + sinY * sinP * sinR;
	this->data[1][0] = cosR * sinY * sinP - sinR * cosY;
	this->data[2][0] = cosP * sinY;

	this->data[0][1] = cosP * sinR;
	this->data[1][1] = cosR * cosP;
	this->data[2][1] = -sinP;

	this->data[0][2] = sinR * cosY * sinP - sinY * cosR;
	this->data[1][2] = sinY * sinR + cosR * cosY * sinP;
	this->data[2][2] = cosP * cosY;
}

Matrix::Matrix(float y1, float y2, float y3, float x1, float x2, float x3, float z1, float z2, float z3)
{
	this->y1 = y1;
	this->y2 = y2;
	this->y3 = y3;
	this->x1 = x1;
	this->x2 = x2;
	this->x3 = x3;
	this->z1 = z1;
	this->z2 = z2;
	this->z3 = z3;
}

Matrix::Matrix(Vector vec)
{
	float cosY = cos(vec.y); // Yaw
	float sinY = sin(vec.y);

	float cosP = cos(vec.x); // Pitch
	float sinP = sin(vec.x);

	float cosR = cos(vec.z); // Roll
	float sinR = sin(vec.z);

	this->y1 = cosY * cosR + sinY * sinP * sinR;
	this->y2 = cosR * sinY * sinP - sinR * cosY;
	this->y3 = cosP * sinY;

	this->x1 = cosP * sinR;
	this->x2 = cosR * cosP;
	this->x3 = -sinP;

	this->z1 = sinR * cosY * sinP - sinY * cosR;
	this->z2 = sinY * sinR + cosR * cosY * sinP;
	this->z3 = cosP * cosY;
}

Vector Matrix::MatrixVectorMultiplication(Vector vec)
{
	Vector retVector;
	retVector.x = this->data[0][0] * vec.x + this->data[0][1] * vec.y + this->data[0][2] * vec.z;
	retVector.y = this->data[1][0] * vec.x + this->data[1][1] * vec.y + this->data[1][2] * vec.z;
	retVector.z = this->data[2][0] * vec.x + this->data[2][1] * vec.y + this->data[2][2] * vec.z;
	return retVector;
}

Matrix Matrix::operator*(const Matrix& mat)
{
	Matrix matrix;

	const float a00 = this->data[0][0], a01 = this->data[0][1], a02 = this->data[0][2],
	            a10 = this->data[1][0], a11 = this->data[1][1], a12 = this->data[1][2],
	            a20 = this->data[2][0], a21 = this->data[2][1], a22 = this->data[2][2],

	            b00 = mat.data[0][0], b01 = mat.data[0][1], b02 = mat.data[0][2],
	            b10 = mat.data[1][0], b11 = mat.data[1][1], b12 = mat.data[1][2],
	            b20 = mat.data[2][0], b21 = mat.data[2][1], b22 = mat.data[2][2];

	matrix.data[0][0] = b00 * a00 + b01 * a10 + b02 * a20;
	matrix.data[0][1] = b00 * a01 + b01 * a11 + b02 * a21;
	matrix.data[0][2] = b00 * a02 + b01 * a12 + b02 * a22;

	matrix.data[1][0] = b10 * a00 + b11 * a10 + b12 * a20;
	matrix.data[1][1] = b10 * a01 + b11 * a11 + b12 * a21;
	matrix.data[1][2] = b10 * a02 + b11 * a12 + b12 * a22;

	matrix.data[2][0] = b20 * a00 + b21 * a10 + b22 * a20;
	matrix.data[2][1] = b20 * a01 + b21 * a11 + b22 * a21;
	matrix.data[2][2] = b20 * a02 + b21 * a12 + b22 * a22;

	return matrix;
}

void Matrix::operator*=(const Matrix& target)
{
	Matrix matrix = *this * target;
	this->data[0][0] = matrix.data[0][0];
	this->data[0][1] = matrix.data[0][1];
	this->data[0][2] = matrix.data[0][2];
	this->data[1][0] = matrix.data[1][0];
	this->data[1][1] = matrix.data[1][1];
	this->data[1][2] = matrix.data[1][2];
	this->data[2][0] = matrix.data[2][0];
	this->data[2][1] = matrix.data[2][1];
	this->data[2][2] = matrix.data[2][2];
}



Matrix Matrix::RotateMatrix(const Vector& euler)
{
    Matrix matrix_out = Matrix();

    if (std::abs(euler.y) > 0.00001)
    {
        float y_rad = euler.y * (float)(M_PI / 180.);
        float cos_y = cos(y_rad);
        float sin_y = sin(y_rad);
        Matrix matrix_local;
        matrix_local.data[0][0] = cos_y;
        matrix_local.data[0][1] = 0;
        matrix_local.data[0][2] = sin_y;
        matrix_local.data[1][0] = 0;
        matrix_local.data[1][1] = 1;
        matrix_local.data[1][2] = 0;
        matrix_local.data[2][0] = -sin_y;
        matrix_local.data[2][1] = 0;
        matrix_local.data[2][2] = cos_y;
        matrix_out *= matrix_local;
    }
    if (std::abs(euler.x) > 0.00001)
    {
        float x_rad = euler.x * (float)(M_PI / 180.);
        float cos_x = cos(x_rad);
        float sin_x = sin(x_rad);
        Matrix matrix_local;
        matrix_local.data[0][0] = 1;
        matrix_local.data[0][1] = 0;
        matrix_local.data[0][2] = 0;
        matrix_local.data[1][0] = 0;
        matrix_local.data[1][1] = cos_x;
        matrix_local.data[1][2] = -sin_x;
        matrix_local.data[2][0] = 0;
        matrix_local.data[2][1] = sin_x;
        matrix_local.data[2][2] = cos_x;
        matrix_out *= matrix_local;
    }
    if (std::abs(euler.z) > 0.00001)
    {
        float z_rad = euler.z * (float)(M_PI / 180.);
        float cos_z = cos(z_rad);
        float sin_z = sin(z_rad);
        Matrix matrix_local;
        matrix_local.data[0][0] = cos_z;
        matrix_local.data[0][1] = -sin_z;
        matrix_local.data[0][2] = 0;
        matrix_local.data[1][0] = sin_z;
        matrix_local.data[1][1] = cos_z;
        matrix_local.data[1][2] = 0;
        matrix_local.data[2][0] = 0;
        matrix_local.data[2][1] = 0;
        matrix_local.data[2][2] = 1;
        matrix_out *= matrix_local;
    }

    return matrix_out;
}

float Matrix::Determinant()
{
    float fCofactor00 = data[1][1] * data[2][2] - data[1][2] * data[2][1];
    float fCofactor10 = data[1][2] * data[2][0] - data[1][0] * data[2][2];
    float fCofactor20 = data[1][0] * data[2][1] - data[1][1] * data[2][0];

    return data[0][0] * fCofactor00 + data[0][1] * fCofactor10 + data[0][2] * fCofactor20;
}

Quaternion::Quaternion(float x, float y, float z)
{
    float c1 = cos(y / 2);
    float s1 = sin(y / 2);
    float c2 = cos(x / 2);
    float s2 = sin(x / 2);
    float c3 = cos(z / 2);
    float s3 = sin(z / 2);
    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;

    this->w = c1c2 * c3 - s1s2 * s3;
    this->x = c1c2 * s3 + s1s2 * c3;
    this->y = s1 * c2 * c3 + c1 * s2 * s3;
    this->z = c1 * s2 * c3 - s1 * c2 * s3;
}

Quaternion::Quaternion(Matrix m)
{
    w = sqrt(std::max(0.0f, 1 + m.data[0][0] + m.data[1][1] + m.data[2][2])) / 2;
    x = sqrt(std::max(0.0f, 1 + m.data[0][0] - m.data[1][1] - m.data[2][2])) / 2;
    y = sqrt(std::max(0.0f, 1 - m.data[0][0] + m.data[1][1] - m.data[2][2])) / 2;
    z = sqrt(std::max(0.0f, 1 - m.data[0][0] - m.data[1][1] + m.data[2][2])) / 2;
    x = (float)_copysign(x, m.data[2][1] - m.data[1][2]);
    y = (float)_copysign(y, m.data[0][2] - m.data[2][0]);
    z = (float)_copysign(z, m.data[1][0] - m.data[0][1]);
}

Quaternion::Quaternion(ConversionOrder order, float pitch, float yaw, float roll)
{
    pitch *= .5f;
    yaw *= .5f;
    roll *= .5f;

    const float sp = sin(pitch), cp = cos(pitch),
        sy = sin(yaw), cy = cos(yaw),
        sr = sin(roll), cr = cos(roll),

        a0 = sp * cy * cr, b0 = cp * sy * sr,
        a1 = cp * sy * cr, b1 = sp * cy * sr,
        a2 = cp * cy * sr, b2 = sp * sy * cr,
        a3 = cp * cy * cr, b3 = sp * sy * sr;

    switch (order) {
    case XYZ:
        this->x = a0 + b0;
        this->y = a1 - b1;
        this->z = a2 + b2;
        this->w = a3 - b3;

        return;
    case YXZ:
        this->x = a0 + b0;
        this->y = a1 - b1;
        this->z = a2 - b2;
        this->w = a3 + b3;

        return;
    case ZXY:
        this->x = a0 - b0;
        this->y = a1 + b1;
        this->z = a2 + b2;
        this->w = a3 - b3;

        return;
    case ZYX:
        this->x = a0 - b0;
        this->y = a1 + b1;
        this->z = a2 - b2;
        this->w = a3 + b3;

        return;
    case YZX:
        this->x = a0 + b0;
        this->y = a1 + b1;
        this->z = a2 - b2;
        this->w = a3 - b3;
        return;
    }
}

Vector Quaternion::EulerAngles()
{
    Vector angles;

    // roll (z-axis rotation)
    const float sinr_cosp = 2 * (this->w * this->x + this->y * this->z);
    const float cosr_cosp = 1 - 2 * (this->x * this->x + this->y * this->y);
    angles.z = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    const float sinp = 2 * (this->w * this->y - this->z * this->x);
    if (std::abs(sinp) >= 1)
        angles.y = std::copysign(3.14159265358979323846f / 2.0f, sinp); // use 90 degrees if out of range
    else
        angles.y = std::asin(sinp);

    // yaw (x-axis rotation)
    const float siny_cosp = 2 * (this->w * this->z + this->x * this->y);
    const float cosy_cosp = 1 - 2 * (this->y * this->y + this->z * this->z);
    angles.x = std::atan2(siny_cosp, cosy_cosp);
    return angles;
}

float Quaternion::DotProduct(Quaternion right) const
{
	return this->x * right.x + this->y * right.y + this->z * right.z + this->w * right.w;
}

float Quaternion::AngleDifference(Quaternion right) const
{
	const float dot = this->DotProduct(right);
	return acos(std::min(abs(dot), 0.1f)) * 0.02f * 57.29578f;
}

Quaternion Quaternion::operator*(Quaternion quat2)
{
    Quaternion product;
    product.x = this->w * quat2.x + this->x * quat2.w + this->y * quat2.z - this->z * quat2.y;
    product.y = this->w * quat2.y - this->x * quat2.z + this->y * quat2.w + this->z * quat2.x;
    product.z = this->w * quat2.z + this->x * quat2.y - this->y * quat2.x + this->z * quat2.w;
    product.w = this->w * quat2.w - this->x * quat2.x - this->y * quat2.y - this->z * quat2.z;
    return product;
}

Quaternion Quaternion::operator*(float& f)
{
	return Quaternion(w * f, x * f, y * f, z * f);
}

Vector Quaternion::operator*(Vector vec)
{
    Vector uv, uuv;
    Vector qvec(x, y, z);
    uv = qvec.CrossProduct(vec);
    uuv = qvec.CrossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return vec + uv + uuv;
}

Quaternion Quaternion::Inverse() const
{
    float fNorm = w * w + x * x + y * y + z * z;
    if (fNorm > 0.0)
    {
        float fInvNorm = 1.0f / fNorm;
        return Quaternion(w * fInvNorm, -x * fInvNorm, -y * fInvNorm, -z * fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return Quaternion(0, 0, 0, 0);
    }
}

const Quaternion Quaternion::operator*(const float& f)
{
	return Quaternion(w * f, x * f, y * f, z * f);
}

Quaternion Quaternion::operator+(Quaternion& rhs)
{
	return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}

const Quaternion Quaternion::operator+(const Quaternion& rhs)
{
	return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}

void Quaternion::Slerp(Quaternion& quat, float time)
{
    // Calculate angle between them.
    const float cosHalfTheta = this->w * quat.w + this->x * quat.x + this->y * quat.y + this->z * quat.z;
	
    // if qa=qb or qa=-qb then theta = 0 and we can return qa
    if (abs(cosHalfTheta) >= 1.0) 
    {
        return;
    }
    // Calculate temporary values.
    float halfTheta = acos(cosHalfTheta);
    float sinHalfTheta = sqrt(1.0f - cosHalfTheta * cosHalfTheta);
	
    // if theta = 180 degrees then result is not fully defined
    // we could rotate around any axis normal to qa or qb
    if (std::fabs(sinHalfTheta) < 0.001f)
    {
    	// fabs is floating point absolute
        w = (this->w * 0.5f + quat.w * 0.5f);
        x = (this->x * 0.5f + quat.x * 0.5f);
        y = (this->y * 0.5f + quat.y * 0.5f);
        z = (this->z * 0.5f + quat.z * 0.5f);
        return;
    }
    const float ratioA = sin((1 - time) * halfTheta) / sinHalfTheta;
    const float ratioB = sin(time * halfTheta) / sinHalfTheta;
    //calculate Quaternion.
    w = (this->w * ratioA + quat.w * ratioB);
    x = (this->x * ratioA + quat.x * ratioB);
    y = (this->y * ratioA + quat.y * ratioB);
    z = (this->z * ratioA + quat.z * ratioB);
}

Matrix Quaternion::ToMatrix()
{
    Matrix mat;
    float q0 = w;
    float q1 = x;
    float q2 = y;
    float q3 = z;

    mat.x1 = 2 * (q1 * q2 + q0 * q3);
    mat.x2 = 2 * (q0 * q0 + q2 * q2) - 1;
    mat.x3 = 2 * (q2 * q3 - q0 * q1);

    mat.y1 = 2 * (q0 * q0 + q1 * q1) - 1;
    mat.y2 = 2 * (q1 * q2 - q0 * q3);
    mat.y3 = 2 * (q1 * q3 + q0 * q2);

    mat.z1 = 2 * (q1 * q3 - q0 * q2);
    mat.z2 = 2 * (q2 * q3 + q0 * q1);
    mat.z3 = 2 * (q0 * q0 + q3 * q3) - 1;

    return mat;
}

Vector Quaternion::ToAxisAngle(float& angle)
{
    Vector axis;
    const float fSqrLength = x * x + y * y + z * z;
    if (fSqrLength > 0.0f)
    {
        angle = 2.0f * std::acos(w);
        const float fInvLength = 1 / std::sqrt(fSqrLength);
        axis.x = x * fInvLength;
        axis.y = y * fInvLength;
        axis.z = z * fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        angle = 0.0f;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    }
    return axis;
}

Quaternion Quaternion::FromAxisAngle(float& angle, Vector& axis)
{
    Quaternion quat;
	
    float fHalfAngle = (0.5f * angle);
    float fSin = std::sin(fHalfAngle);
    quat.w = std::cos(fHalfAngle);
    quat.x = fSin * axis.x;
    quat.y = fSin * axis.y;
    quat.z = fSin * axis.z;
    return quat;
}

Vector Matrix::GetEuler()
{
   
    Vector vector3;
    vector3.z = 0.0f;
    float num = this->data[1][2];
    if ((float)num < 1.0)
    {
        if ((float)num > -1.0)
        {
            vector3.x = std::asin(-num);
            vector3.y = std::atan2(this->data[0][2], this->data[2][2]);
            vector3.z = std::atan2(this->data[1][0], this->data[1][1]);
        }
        else
        {
            vector3.x = 1.570796f;
            vector3.y = -std::atan2(-this->data[0][1], this->data[0][0]);
        }
    }
    else
    {
        vector3.x = -1.570796f;
        vector3.y = -std::atan2(-this->data[0][1], this->data[0][0]);
    }
    return vector3;
}


