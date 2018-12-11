

#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/track_ball.h"

BEGIN_NAMESPACE

// corresponding relation between 2D and 3D coordinate.
//(suppose that bottom left corner is (0,0), width is 1, and height is also 1.
// it must be emphasized that (0,0,?) does not match with (0.5, 0.5). )
//
//    (0,0)                      (1,0)
//      |-------------|------------|
//      |             |            |
//      |    (-,+,?)  |   (+,+,?)  |
//      |             |            |
//      |-----------(0,0,?)--------|
//      |             |            |
//      |    (-,-,?)  |   (+,-,?)  |
//      |             |            |
//      |-------------|------------|
//    (0,1)                       (1,1)

/// \brief This method is used to transform the 2D screen coordinate to 3D trackball coordinate.
///                    The screen coordinate must be captured by the mouse event.
///
/// \param[in] width   The width of the view port which can be mapped exactly into a trackball.
/// \param[in] height  The height of the view port which can be mapped exactly into a trackball.
/// \param[in] BottomLeftCorner  The bottom-left corner coordinate of the view port which can be mapped exactly into a trackball.
///                              It must be noticed that the view port size may be smaller than the window size.
/// \param[in] point   The 2D screen coordinate captured by the mouse event.
///
/// \return    corresponding 3D trackball coordinate
Point3D VirtualTrackBall::ProjectScreenToTrackball(double width, 
    double height, 
    Point2D BottomLeftCorner, 
    Point2D point)
{
    double x = (point.x - BottomLeftCorner.x) / (width / 2) - 1;
    double y = 1 - (point.y - BottomLeftCorner.y) / (height / 2);

    //double z2 = 1.0 - x * x - y * y;
    //double z = z2>0? sqrt(z2):0;
    //return Point3D(x, y, z);

    /*double radius = std::sqrt(width * width + height * height);
    double x = (point.x - BottomLeftCorner.x) / (radius / 2) - 1;
    double y = 1 - (point.y - BottomLeftCorner.y) / (radius / 2); */

    double z2 = 1.0 - x * x - y * y;
    double z = z2>0.5 ? sqrt(z2): 0.5 / sqrt(x * x + y * y);

    Vector3D pt(x,y,z);
    //pt.Normalize();
    return Point3D(pt.x, pt.y, pt.z);
}

/// \brief This method is used to transform trackball position variation to rotation quaternion.
///                    The trackball position variation is caused by mouse movement on 2D screen.
///                    If you get a previous position A and a current position B on the trackball,
///                    then you can calculate the rotation from A to B.
/// \param[in] prePosition3D   The previous position on the trackball.
/// \param[in] currPosition3D  The current position on the trackball.
/// \param[in] centerPoint3D  The center of the trackball. Its default position is set to the origin.
///
/// \return    rotation quaternion composed by rotate axis and rotate angle.
Quaternion VirtualTrackBall::CalculateTrackBallRotation(const Point3D& prePosition3D, 
    const Point3D& currPosition3D,
    const Point3D& centerPoint3D)
{
    Vector3D currVector3D = currPosition3D - centerPoint3D;
    Vector3D preVector3D = prePosition3D - centerPoint3D;
    Vector3D axis = CrossProduct(currVector3D, preVector3D);
    double angle = AngleBetween(currVector3D,preVector3D);
    return Quaternion(angle, axis);
}

/// \brief This method is used to transform mouse movement to rotation quaternion.
///             Then you can rotate the 3D object which is rendered on the 2D screen.
//              The range of mouse movement in x direction must be from o.x to o.x + width,
//              in y direction must be from o.y to o.y + height
/// \param[in] prePosition2D   The previous position on the 2D screen.
/// \param[in] currPosition2D  The current position on the 2D screen.
/// \param[in] BottomLeftCorner  The bottom-left corner coordinate of the view port which can be mapped exactly into a trackball.
/// \param[in] width   The width of the view port which can be mapped exactly into a trackball.
/// \param[in] height  The height of the view port which can be mapped exactly into a trackball.
///                     It must be noticed that the view port size may be smaller than the window size.
/// 
/// \return    rotation quaternion composed by rotate axis and rotate angle.
Quaternion VirtualTrackBall::MouseMove2TackBallRotation(Point2D prePosition2D,
    Point2D currPosition2D,
    Point2D BottomLeftCorner, 
    double width, 
    double height)
{   
    Quaternion myRotation = Quaternion(1.0,Vector3D(0.0,0.0,0.0));
    if ((prePosition2D.x > BottomLeftCorner.x)&&
        (prePosition2D.x < BottomLeftCorner.x+width)&&
        (prePosition2D.y > BottomLeftCorner.y)&&
        (prePosition2D.y < BottomLeftCorner.y+height)
        )
    {
        Point3D prePosition3D = ProjectScreenToTrackball(width, height, BottomLeftCorner, prePosition2D);
        Point3D currPosition3D = ProjectScreenToTrackball(width, height, BottomLeftCorner, currPosition2D);

        myRotation = CalculateTrackBallRotation(prePosition3D, currPosition3D);
    }
    return myRotation;
}
END_NAMESPACE
