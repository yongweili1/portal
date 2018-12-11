
#ifndef UIH_ARITHMETIC_TRACKBALL_H_
#define UIH_ARITHMETIC_TRACKBALL_H_
#include "uai_app_defs.h"

BEGIN_NAMESPACE

class Point2D;
class Point3D;
    /// \class MedViewer3DTrackBall
    /// \brief MedViewer3DTrackBall is used to transform mouse event into rotate axis and rotate angle. 
    ///                                  The principle of the trackball can be find in 
    ///                                  \\10.1.2.13\Public\ÁÙÊ±½»»»Çø\liangliang.pan\data\Trackball.png
class VirtualTrackBall
{
public:
    /// \brief This method is used to transform the 2D screen coordinate to 3D trackball coordinate.
    ///                    The screen coordinate must be captured by the mouse event.
    static Point3D ProjectScreenToTrackball(double width, double height, 
        Point2D BottomLeftCorner, Point2D point);

    /// \brief This method is used to transform mouse movement to rotation quaternion.
    ///        Then you can rotate the 3D object which is rendered on the 2D screen.
    static Quaternion MouseMove2TackBallRotation(Point2D previousPoint2D, Point2D currentPoint2D, 
        Point2D BottomLeftCorner, double width, double height);

protected:
    /// \brief This method is used to transform trackball position variation to rotation quaternion.
    ///                    The trackball position variation is caused by mouse movement on 2D screen.
    ///                    If you get a previous position A and a current position B on the trackball,
    ///                    then you can calculate the rotation from A to B.
    static Quaternion CalculateTrackBallRotation(const Point3D& prePosition3D, 
        const Point3D& currentPosition3D,
        const Point3D& centerPoint3D = Point3D(0.0,0.0,0.0));
};

END_NAMESPACE

#endif