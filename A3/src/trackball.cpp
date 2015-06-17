#include <math.h>
#include "trackball.hpp"

// I have modified the given "trackball.c" file to use Qt classes

QVector3D calcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter) {
   QVector3D newVec;
   QVector3D oldVec;
   
   float fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   newVec.setX(fNewX * 2.0 / fDiameter);
   newVec.setY(fNewY * 2.0 / fDiameter);
   newVec.setZ(1.0 - newVec.x() * newVec.x() - newVec.y() * newVec.y());

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (newVec.z() < 0.0) {
        fLength = sqrt(1.0 - newVec.z());
        newVec.setZ(0.0);
        newVec.setX(newVec.x() / fLength);
        newVec.setY(newVec.y() / fLength);
   } else {
        newVec.setZ(sqrt(newVec.z()));
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   oldVec.setX(fOldX * 2.0 / fDiameter);
   oldVec.setY(fOldY * 2.0 / fDiameter);
   oldVec.setZ(1.0 - oldVec.x() * oldVec.x() - oldVec.y() * oldVec.y());
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
    if (oldVec.z() < 0.0) {
        fLength = sqrt(1.0 - oldVec.z());
        oldVec.setZ(0.0);
        oldVec.setX(oldVec.x() / fLength);
        oldVec.setY(oldVec.y() / fLength);
    } else {
        oldVec.setZ(sqrt(oldVec.z()));
    }

   /* Generate rotation vector by calculating cross product:
    * 
    * oldVec x newVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between oldVec and newVec which we need
    * in order to perform the rotation.
    */
   
   QVector3D rotVec;
   
   rotVec.setX(oldVec.y() * newVec.z() - newVec.y() * oldVec.z());
   rotVec.setY(oldVec.z() * newVec.x() - newVec.z() * oldVec.x());
   rotVec.setZ(oldVec.x() * newVec.y() - newVec.x() * oldVec.y());
   
   return rotVec;
}

QMatrix4x4 axisRotMatrix(QVector3D rotVec) {
    float fRadians;
    QMatrix4x4 rotMat;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = rotVec.length();

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return rotMat;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    rotVec.normalize();

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    rotMat(0, 0) = dCosAlpha + rotVec.x()*rotVec.x()*dT;
    rotMat(0, 1) = rotVec.x()*rotVec.y()*dT + rotVec.z()*dSinAlpha;
    rotMat(0, 2) = rotVec.x()*rotVec.z()*dT - rotVec.y()*dSinAlpha;
    rotMat(0, 3) = 0;

    rotMat(1, 0) = rotVec.x()*rotVec.y()*dT - dSinAlpha*rotVec.z();
    rotMat(1, 1) = dCosAlpha + rotVec.y()*rotVec.y()*dT;
    rotMat(1, 2) = rotVec.y()*rotVec.z()*dT + dSinAlpha*rotVec.x();
    rotMat(1, 3) = 0;

    rotMat(2, 0) = rotVec.z()*rotVec.x()*dT + dSinAlpha*rotVec.y();
    rotMat(2, 1) = rotVec.z()*rotVec.y()*dT - dSinAlpha*rotVec.x();
    rotMat(2, 2) = dCosAlpha + rotVec.z()*rotVec.z()*dT;
    rotMat(2, 3) = 0;

    rotMat(3, 0) = 0;
    rotMat(3, 1) = 0;
    rotMat(3, 2) = 0;
    rotMat(3, 3) = 1;

    return rotMat;
}
