#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <QMatrix4x4>
#include <QVector3D>

/* Function prototypes */
QVector3D calcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter);
QMatrix4x4 axisRotMatrix(QVector3D rotVec);

#endif
