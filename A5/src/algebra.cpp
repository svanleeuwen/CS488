//---------------------------------------------------------------------------
//
// CS488 -- Introduction to Computer Graphics
//
// algebra.hpp/algebra.cpp
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#include "algebra.hpp"

Point3D::Point3D(const Vector4D& vec) {
    if(vec[3] == 0 || vec[3] == 1) {
        v_[0] = vec[0];
        v_[1] = vec[1];
        v_[2] = vec[2];
    } else {
        v_[0] = vec[0] / vec[3];
        v_[1] = vec[1] / vec[3];
        v_[2] = vec[2] / vec[3];
    }
}



Vector3D::Vector3D(const Vector4D& vec) {
    if(vec[3] == 0 || vec[3] == 1) {
        v_[0] = vec[0];
        v_[1] = vec[1];
        v_[2] = vec[2];
    } else {
        v_[0] = vec[0] / vec[3];
        v_[1] = vec[1] / vec[3];
        v_[2] = vec[2] / vec[3];
    }
}

double Vector3D::normalize()
{
  double denom = 1.0;
  double x = (v_[0] > 0.0) ? v_[0] : -v_[0];
  double y = (v_[1] > 0.0) ? v_[1] : -v_[1];
  double z = (v_[2] > 0.0) ? v_[2] : -v_[2];

  if(x > y) {
    if(x > z) {
      if(1.0 + x > 1.0) {
        y = y / x;
        z = z / x;
        denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
      }
    } else { /* z > x > y */ 
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  } else {
    if(y > z) {
      if(1.0 + y > 1.0) {
        z = z / y;
        x = x / y;
        denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
      }
    } else { /* x < y < z */
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  }

  if(1.0 + x + y + z > 1.0) {
    v_[0] *= denom;
    v_[1] *= denom;
    v_[2] *= denom;
    return 1.0 / denom;
  }

  return 0.0;
}

Point3D Point3D::min(const Point3D& p1, const Point3D& p2) {
    Point3D minPoint;

    if(p1[0] < p2[0]) {
        minPoint[0] = p1[0];
    } else {
        minPoint[0] = p2[0];
    }
    
    if(p1[1] < p2[1]) {
        minPoint[1] = p1[1];
    } else {
        minPoint[1] = p2[1];
    }

    if(p1[2] < p2[2]) {
        minPoint[2] = p1[2];
    } else {
        minPoint[2] = p2[2];
    }

    return minPoint;
}

Point3D Point3D::max(const Point3D& p1, const Point3D& p2) {
    Point3D maxPoint;

    if(p1[0] > p2[0]) {
        maxPoint[0] = p1[0];
    } else {
        maxPoint[0] = p2[0];
    }
    
    if(p1[1] > p2[1]) {
        maxPoint[1] = p1[1];
    } else {
        maxPoint[1] = p2[1];
    }

    if(p1[2] > p2[2]) {
        maxPoint[2] = p1[2];
    } else {
        maxPoint[2] = p2[2];
    }

    return maxPoint;
}

/*
 * Define some helper functions for matrix inversion.
 */

static void swaprows(Matrix4x4& a, size_t r1, size_t r2)
{
  std::swap(a[r1][0], a[r2][0]);
  std::swap(a[r1][1], a[r2][1]);
  std::swap(a[r1][2], a[r2][2]);
  std::swap(a[r1][3], a[r2][3]);
}

static void dividerow(Matrix4x4& a, size_t r, double fac)
{
  a[r][0] /= fac;
  a[r][1] /= fac;
  a[r][2] /= fac;
  a[r][3] /= fac;
}

static void submultrow(Matrix4x4& a, size_t dest, size_t src, double fac)
{
  a[dest][0] -= fac * a[src][0];
  a[dest][1] -= fac * a[src][1];
  a[dest][2] -= fac * a[src][2];
  a[dest][3] -= fac * a[src][3];
}

/*
 * invertMatrix
 *
 * I lifted this code from the skeleton code of a raytracer assignment
 * from a different school.  I taught that course too, so I figured it
 * would be okay.
 */
Matrix4x4 Matrix4x4::invert() const
{
  /* The algorithm is plain old Gauss-Jordan elimination 
     with partial pivoting. */

  Matrix4x4 a(*this);
  Matrix4x4 ret;

  /* Loop over cols of a from left to right, 
     eliminating above and below diag */

  /* Find largest pivot in column j among rows j..3 */
  for(size_t j = 0; j < 4; ++j) { 
    size_t i1 = j; /* Row with largest pivot candidate */
    for(size_t i = j + 1; i < 4; ++i) {
      if(fabs(a[i][j]) > fabs(a[i1][j])) {
        i1 = i;
      }
    }

    /* Swap rows i1 and j in a and ret to put pivot on diagonal */
    swaprows(a, i1, j);
    swaprows(ret, i1, j);

    /* Scale row j to have a unit diagonal */
    if(a[j][j] == 0.0) {
      // Theoretically throw an exception.
      return ret;
    }

    dividerow(ret, j, a[j][j]);
    dividerow(a, j, a[j][j]);

    /* Eliminate off-diagonal elems in col j of a, doing identical 
       ops to b */
    for(size_t i = 0; i < 4; ++i) {
      if(i != j) {
        submultrow(ret, i, j, a[i][j]);
        submultrow(a, i, j, a[i][j]);
      }
    }
  }

  return ret;
}

Matrix4x4 Matrix4x4::getRotMat(char axis, double angle) {
    Matrix4x4 rotMat;
    
    if(axis == 'x') {
        rotMat[1][1] = cos(angle * M_PI / 180.0);
        rotMat[1][2] = -sin(angle * M_PI / 180.0);
        rotMat[2][1] = sin(angle * M_PI / 180.0);
        rotMat[2][2] = cos(angle * M_PI / 180.0);
    } else if(axis == 'y') {
        rotMat[0][0] = cos(angle * M_PI / 180.0);
        rotMat[0][2] = sin(angle * M_PI / 180.0);
        rotMat[2][0] = -sin(angle * M_PI / 180.0);
        rotMat[2][2] = cos(angle * M_PI / 180.0);
    } else {
        rotMat[0][0] = cos(angle * M_PI / 180.0);
        rotMat[0][1] = -sin(angle * M_PI / 180.0);
        rotMat[1][0] = sin(angle * M_PI / 180.0);
        rotMat[1][1] = cos(angle * M_PI / 180.0);
    }

    return rotMat;
}

Matrix4x4 Matrix4x4::getScaleMat(const Vector3D& amount) {
    Matrix4x4 scaleMat;

    scaleMat[0][0] = amount[0];
    scaleMat[1][1] = amount[1];
    scaleMat[2][2] = amount[2];

    return scaleMat;
}

Matrix4x4 Matrix4x4::getTransMat(const Vector3D& amount) {
    Matrix4x4 transMat;

    transMat[0][3] = amount[0];
    transMat[1][3] = amount[1];
    transMat[2][3] = amount[2];

    return transMat;
}

Colour& Colour::operator +=(const Colour& other) {
    (*this) = (*this) + other;
    return *this;
}
