
#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/camera.h"

BEGIN_NAMESPACE

const double Camera::INFINITE_FAR_PLANE_ADJUST = 0.00001;

/// \brief        default constructor of the class ProjectionCamera
///
/// \return       none
Camera::Camera(void):
m_At(0.0,0.0,-1.0),
m_Up(0.0,1.0,0.0),
m_Eye(0.0,0.0,0.0),
m_dFovY(1.0),
m_dAspect(1.0),
m_dNearDist(0.2),
m_dFarDist(10),
m_dOrthoHeight(1.0),
m_bViewMatrixDirty(true),
m_bPerspectiveMatrixDirty(true),
m_ProjType(PT_ORTHOGRAPHIC)
{
    m_ViewMatrix.SetIdintity();
    m_ProjectionMatrix.SetIdintity();
}

Camera::Camera(const Camera &myCamera)
{
    *this = myCamera;
}

/// \brief        destructor of the class ProjectionCamera
///
/// \return       none
Camera::~Camera(void)
{
}

/// \brief Sets the position of the camera
///
/// \param[in] eye The position of the camera
///
/// \return none
void Camera::SetEye(const Point3D& eye)
{
    if(m_Eye == eye)
        return;
    m_Eye = eye;
    SetViewMatrixDirty();
}

/// \brief Sets the looking direction of the camera
///
/// \param[in] At The looking direction of the camera
///
/// \return none
void Camera::SetLookAt(const Point3D& At)
{
    if(m_At == At)
        return;
    m_At = At;
    SetViewMatrixDirty();
}

/// \brief Sets the up direction of the camera
///
/// \param[in] Updirection The up direction of the camera
///
/// \return none
void Camera::SetUpDirection(const Vector3D& Updirection)
{
    if (m_Up == Updirection)
        return;
    m_Up = Updirection;
    SetViewMatrixDirty();
}

/// \brief Sets the aspect ratio for the frustum viewport
///
/// \param[in] dRatio The ratio between the x and y dimensions
///
/// \return none
void Camera::SetAspectRatio(double dRatio)
{
    if(abs(m_dAspect - dRatio) < UIH_ARITHMETIC_DOUBLE_ZERO)
        return;
    m_dAspect = dRatio;
    SetPerspectiveMatrixDirty();
}

/// \brief Sets the Y-dimension Field Of View (FOV) of the frustum.
///
/// \param[in] dFovy The VERTICAL field-of-view
///
/// \return none
void Camera::SetFovY(double dFovy)
{
    if(abs(m_dFovY - dFovy) < UIH_ARITHMETIC_DOUBLE_ZERO)
        return;
    m_dFovY = dFovy;
    SetPerspectiveMatrixDirty();
}

/// \brief Sets the position of the near clipping plane.
///
/// \param[in] dNearDist The position of the near clipping plane.
///
/// \return none
void Camera::SetNearClipDistance(double dNearDist)
{
    if(abs(m_dNearDist - dNearDist) < UIH_ARITHMETIC_DOUBLE_ZERO)
        return;
    m_dNearDist = dNearDist;
    SetPerspectiveMatrixDirty();
}

/// Sets the position of the far clipping plane.
///
/// \param[in] dFarDist The position of the far clipping plane.
///
/// \return none
void Camera::SetFarClipDistance(double dFarDist)
{
    if(abs(m_dFarDist - dFarDist) < UIH_ARITHMETIC_DOUBLE_ZERO)
        return;
    m_dFarDist = dFarDist;
    SetPerspectiveMatrixDirty();
}

/// Sets the orthographic window height, for use with orthographic rendering only. 
///
/// \param[in] dOrthoHeight The height of the view window in world units.
///
/// \return none
void Camera::SetOrthoWindowHeight(double dOrthoHeight)
{
    m_dOrthoHeight = dOrthoHeight;
    SetPerspectiveMatrixDirty();
}

/// Sets the orthographic window width, for use with orthographic rendering only. 
///
/// \param[in] dOrthoWidth The width of the view window in world units.
///
/// \return none
void Camera::SetOrthoWindowWidth(double dOrthoWidth)
{
    m_dOrthoHeight = dOrthoWidth / m_dAspect;
    SetPerspectiveMatrixDirty();
}

/// Sets the orthographic window settings, for use with orthographic rendering only. 
///
/// \param[in] w The width of the view window in world units.
/// \param[in] h The height of the view window in world units.
///
/// \return none
void Camera::SetOrthoWindow(double w, double h)
{
    m_dOrthoHeight = h;
    m_dAspect = w / h;
    SetPerspectiveMatrixDirty();
}

/// \brief rotate camera
///
/// \param[in] angle Rotating angle.
/// \param[in] vAxis rotating axis.
///
/// \return none
void Camera::Rotate(double dAngle,const Vector3D& vAxis)
{
    //Vector3D v3Temp = m_At - m_Eye;
    //Quaternion quaternion(dAngle,vAxis);
    //v3Temp = quaternion.ToMatrix().Transform(v3Temp);
    //m_At = m_Eye + v3Temp;
    Rotate(Quaternion(dAngle,vAxis));
    SetViewMatrixDirty();
}

/// \brief rotate camera
///
/// \param[in] myQuaternion Rotating component.
///
/// \return none
void Camera::Rotate(const Quaternion& myQuaternion)
{
    //SetViewMatrixDirty();
    Rotate(myQuaternion.ToMatrix());
}

void Camera::Rotate(const Matrix4x4& myMatrix)
{
    Matrix4x4 matrix = GetViewMatrix();
    if (!matrix.HasInverse())
    {
        return;
    }
    //Matrix4x4 matrix;
    //matrix.SetIdintity();

    //Vector3D vecTrans = m_At - Point3D(0.0,0.0,0.0);
    //matrix.Prepend(MakeTranslate(-vecTrans));
    //matrix.Prepend(myQuaternion);
    //matrix.Prepend(MakeTranslate(vecTrans));
    matrix.Prepend(MakeTranslate(Point3D(0.0,0.0,0.0) - matrix.Transform(m_At)));
    matrix = matrix.Inverse() * myMatrix * matrix;
    m_Eye = matrix.Transform(m_Eye);
    m_Up = matrix.Transform(m_Up);

    SetViewMatrixDirty();
}

/// \brief Move camera along Z axis
///
/// \param[in] dStep Step of moving
///
/// \return none
void Camera::MoveCameraZ(double dStep)
{
    //Vector3D v3Temp = m_At - m_Eye;
    //v3Temp.Normalize();
    //v3Temp *= dStep;
    //m_Eye += v3Temp;
    //m_At  += v3Temp;

    Matrix4x4 viewMatrix = GetViewMatrix();
    if (!viewMatrix.HasInverse())
    {
        return;
    }
    Matrix4x4 matrix = viewMatrix;
    matrix.Prepend(MakeTranslate(Point3D(0.0,0.0,0.0) - Point3D(0.0,0.0,dStep)));
    matrix.Prepend(viewMatrix.Inverse());
    m_Eye = matrix.Transform(m_Eye);
    m_At  = matrix.Transform(m_At);

    SetViewMatrixDirty();
}

/// \brief Move camera along X axis
///
/// \param[in] dStep Step of moving
///
/// \return none
void Camera::MoveCameraX(double dStep)
{
    //Vector3D cross = m_At - m_Eye;
    //cross = cross.CrossProduct(m_Up);
    //cross.Normalize();
    //cross *= dStep;
    //m_Eye += cross;
    //m_At  += cross;

    Matrix4x4 viewMatrix = GetViewMatrix();
    if (!viewMatrix.HasInverse())
    {
        return;
    }
    Matrix4x4 matrix = viewMatrix;
    matrix.Prepend(MakeTranslate(Point3D(0.0,0.0,0.0) - Point3D(dStep,0.0,0.0)));
    matrix.Prepend(viewMatrix.Inverse());
    m_Eye = matrix.Transform(m_Eye);
    m_At = matrix.Transform(m_At);

    SetViewMatrixDirty();
}

/// \brief Move camera along Y axis
///
/// \param[in] dStep Step of moving
///
/// \return none
void Camera::MoveCameraY(double dStep)
{
    //Vector3D v3Temp = m_Up;
    //v3Temp.Normalize();
    //v3Temp *= dStep;
    //m_Eye += v3Temp;
    //m_At  += v3Temp;

    Matrix4x4 viewMatrix = GetViewMatrix();
    if (!viewMatrix.HasInverse())
    {
        return;
    }
    Matrix4x4 matrix = viewMatrix;
    matrix.Prepend(MakeTranslate(Point3D(0.0,0.0,0.0) - Point3D(0.0,dStep,0.0)));
    matrix.Prepend(viewMatrix.Inverse());
    m_Eye = matrix.Transform(m_Eye);
    m_At = matrix.Transform(m_At);

    SetViewMatrixDirty();
}

/// \brief Internal functions for calculating projection matrix.
///
/// \return none
void Camera::CalcProjMatrix()
{

    if (m_ProjType == PT_PERSPECTIVE)
    {
        double tanThetaY = tan(m_dFovY * 0.5);
        double tanThetaX = tanThetaY * m_dAspect;
        double half_w = tanThetaX * m_dNearDist;
        double half_h = tanThetaY * m_dFarDist;
        double left   = - half_w;
        double right  = + half_w;
        double bottom = - half_h;
        double top    = + half_h;

        double inv_w = 1 / (right - left);
        double inv_h = 1 / (top - bottom);
        double inv_d = 1 / (m_dFarDist - m_dNearDist);
        // Calculate matrix elements
        double A = 2 * m_dNearDist * inv_w;
        double B = 2 * m_dFarDist * inv_h;
        double C = (right + left) * inv_w;
        double D = (top + bottom) * inv_h;
        double q, qn;
        if (abs(m_dFarDist) < UIH_ARITHMETIC_DOUBLE_ZERO)
        {
            // Infinite far plane
            q = INFINITE_FAR_PLANE_ADJUST - 1;
            qn = m_dNearDist * (INFINITE_FAR_PLANE_ADJUST - 2);
        }
        else
        {
            q = - (m_dFarDist + m_dNearDist) * inv_d;
            qn = -2 * (m_dFarDist * m_dNearDist) * inv_d;
        }
        // NB: This creates 'uniform' perspective projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   C   0  ]
        // [ 0   B   D   0  ]
        // [ 0   0   q   qn ]
        // [ 0   0   -1  0  ]
        //
        // A = 2 * near / (right - left)
        // B = 2 * near / (top - bottom)
        // C = (right + left) / (right - left)
        // D = (top + bottom) / (top - bottom)
        // q = - (far + near) / (far - near)
        // qn = - 2 * (far * near) / (far - near)
        m_ProjectionMatrix.SetIdintity();
        m_ProjectionMatrix.m[0][0] = A;
        m_ProjectionMatrix.m[2][0] = C;
        m_ProjectionMatrix.m[1][1] = B;
        m_ProjectionMatrix.m[2][1] = D;
        m_ProjectionMatrix.m[2][2] = q;
        m_ProjectionMatrix.m[3][2] = qn;
        m_ProjectionMatrix.m[2][3] = -1;
        m_ProjectionMatrix.m[3][3] = 0;
    }
    else if (m_ProjType == PT_ORTHOGRAPHIC)
    {
        double half_w = GetOrthoWindowWidth() * 0.5f;
        double half_h = GetOrthoWindowHeight() * 0.5f;
        double left   = - half_w;
        double right  = + half_w;
        double bottom = - half_h;
        double top    = + half_h;

        double inv_w = 1 / (right - left);
        double inv_h = 1 / (top - bottom);
        double inv_d = 1 / (m_dFarDist - m_dNearDist);
        double A = 2 * inv_w;
        double B = 2 * inv_h;
        double C = - (right + left) * inv_w;
        double D = - (top + bottom) * inv_h;
        double q, qn;
        if (abs(m_dFarDist) < UIH_ARITHMETIC_DOUBLE_ZERO)
        {
            // Can not do infinite far plane here, avoid divided zero only
            q = - INFINITE_FAR_PLANE_ADJUST / m_dNearDist;
            qn = - INFINITE_FAR_PLANE_ADJUST - 1;
        }
        else
        {
            q = - 2 * inv_d;
            qn = - (m_dFarDist + m_dNearDist)  * inv_d;
        }
        // NB: This creates 'uniform' orthographic projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   0   C  ]
        // [ 0   B   0   D  ]
        // [ 0   0   q   qn ]
        // [ 0   0   0   1  ]
        //
        // A = 2 * / (right - left)
        // B = 2 * / (top - bottom)
        // C = - (right + left) / (right - left)
        // D = - (top + bottom) / (top - bottom)
        // q = - 2 / (far - near)
        // qn = - (far + near) / (far - near)
        m_ProjectionMatrix.SetIdintity();
        m_ProjectionMatrix.m[0][0] = A;
        m_ProjectionMatrix.m[3][0] = C;
        m_ProjectionMatrix.m[1][1] = B;
        m_ProjectionMatrix.m[3][1] = D;
        m_ProjectionMatrix.m[2][2] = q;
        m_ProjectionMatrix.m[3][2] = qn;
        m_ProjectionMatrix.m[3][3] = 1;
    } // ortho     	
}

/// \brief Internal functions for calculating view matrix.
///
/// \return none
void  Camera::CalcViewMatrix()
{
    Vector3D zaxis = m_Eye - m_At;
    zaxis.Normalize();
    Vector3D xaxis = m_Up.CrossProduct(zaxis);
    xaxis.Normalize();
    Vector3D yaxis = zaxis.CrossProduct(xaxis);
    m_ViewMatrix.SetIdintity();
    m_ViewMatrix.m[0][0] = xaxis.x;
    m_ViewMatrix.m[1][0] = xaxis.y;
    m_ViewMatrix.m[2][0] = xaxis.z;
    m_ViewMatrix.m[3][0] = -(xaxis.DotProduct(m_Eye - Point3D(0,0,0)));
    //m_ViewMatrix.m[3][0] = -(m_Eye.x);
    m_ViewMatrix.m[0][1] = yaxis.x;
    m_ViewMatrix.m[1][1] = yaxis.y;
    m_ViewMatrix.m[2][1] = yaxis.z;
    m_ViewMatrix.m[3][1] = -(yaxis.DotProduct(m_Eye - Point3D(0,0,0)));
    //m_ViewMatrix.m[3][1] = -(m_Eye.y);
    m_ViewMatrix.m[0][2] = zaxis.x;
    m_ViewMatrix.m[1][2] = zaxis.y;
    m_ViewMatrix.m[2][2] = zaxis.z;
    m_ViewMatrix.m[3][2] = -(zaxis.DotProduct(m_Eye - Point3D(0,0,0)));
    //m_ViewMatrix.m[3][2] = -(m_Eye.z);
    m_ViewMatrix.m[0][3] = 0;
    m_ViewMatrix.m[1][3] = 0;
    m_ViewMatrix.m[2][3] = 0;
    m_ViewMatrix.m[3][3] = 1;
}

/// \brief Get the view matrix
///
/// \return The view matrix
const Matrix4x4 Camera::GetViewMatrix()
{
    if (IsViewMatrixDirty())
    {
        CalcViewMatrix();
        RemoveViewMatrixDirty();	
    }
    return m_ViewMatrix;	
}

/// \brief Get the projection matrix
///
/// \return The projection matrix
const Matrix4x4 Camera::GetProjectionMatrix()
{
    if (IsPerspectiveMatrixDirty())
    {
        CalcProjMatrix();
        RemovePerspectiveMatrixDirty();
    }
    return m_ProjectionMatrix;
}

/// \brief Get the product of view matrix and projection matrix
///
/// \return The product of view matrix and projection matrix
const Matrix4x4 Camera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}

/// \brief zoom in or zoom out
///
/// \param[in] dfactor zoom factor
///
/// \return none
bool Camera::Zoom(double dfactor)
{
    if (UIH_ARITHMETIC_DOUBLE_ZERO > dfactor)
        return false;
	if (m_ProjType == PT_PERSPECTIVE)
	{
		double dFovY = GetFovY();
		double dNewFovY = 2.0*std::atan(
			std::tan(dFovY/2.0)/dfactor);
		SetFovY(dNewFovY);
	}
	else if (m_ProjType == PT_ORTHOGRAPHIC)
	{
		double dOrthoWidth = GetOrthoWindowWidth();
		double dOrthoHeight = GetOrthoWindowHeight();
		SetOrthoWindow(dOrthoWidth/dfactor, dOrthoHeight/dfactor);
	}
	return true;
}

void  Camera::ApplyTransform( const Quaternion myQuaternion)
{
    Matrix4x4 myMatrix = myQuaternion.ToMatrix();
    m_Eye = myMatrix.Transform(m_Eye);
    m_Up = myMatrix.Transform(m_Up);
    m_At = myMatrix.Transform(m_At);
    SetViewMatrixDirty();
}

void  Camera::ApplyTransform( const Matrix4x4 &myMatrix)
{
    m_Eye = myMatrix.Transform(m_Eye);
    m_Up = myMatrix.Transform(m_Up);
    m_At = myMatrix.Transform(m_At);
    SetViewMatrixDirty();
}

/// \brief set projection type
///
/// \param[in] type projection type
///
/// \return none
void Camera::SetProjectionType(ProjectionType type)
{
    if (m_ProjType == type)
    {
        return;
    }
    m_ProjType = type;
    SetPerspectiveMatrixDirty();
}

// \brief Get Projection type
Camera::ProjectionType Camera::GetProjectionType() const
{
    return m_ProjType;
}

Camera& Camera::operator =(const Camera &myCamera)
{
    m_Eye = myCamera.m_Eye;

    m_At = myCamera.m_At;

    m_Up = myCamera.m_Up;

    m_bViewMatrixDirty  = myCamera.m_bViewMatrixDirty;

    m_dFovY = myCamera.m_dFovY;

    /// x/y viewport ratio
    m_dAspect = myCamera.m_dAspect;

    /// Near clip distance
    m_dNearDist = myCamera.m_dNearDist;

    /// Far clip distance
    m_dFarDist = myCamera.m_dFarDist;

    /// Ortho height size
    m_dOrthoHeight = myCamera.m_dOrthoHeight;

    m_bPerspectiveMatrixDirty = myCamera.m_bPerspectiveMatrixDirty;;

    /// Orthographic or perspective?
    m_ProjType = myCamera.m_ProjType;

    m_ViewMatrix = myCamera.m_ViewMatrix;

    m_ProjectionMatrix = myCamera.m_ProjectionMatrix;

    return *this;
}

/// \brief Get the projection vector of camera's eye direction on a given plane
///
/// \param[in] plane The given plane
///
/// \return The projection vector of camera's eye direction
Vector3D Camera::GetViewDirProjectionOnPlane(const Plane& plane)const
{
    Vector3D planeNomal = plane.GetNormal();
    Vector3D viewDir = (m_At - m_Eye);
    viewDir.Normalize();
    Vector3D myVector3D = (viewDir.DotProduct(planeNomal)) * planeNomal;
    Vector3D projectionVector = viewDir - myVector3D;
    return projectionVector;
}

/// \brief Get the projection vector of camera's view direction on a given plane
/// and calculate the projection vector's length on the X axis and Y axis 
/// of the given plane
///
/// \param[in] plane The given plane
/// \param[out] oxLength projection vector's projection length on the X axis
/// \param[out] oxLength projection vector's projection length on the Y axis
///
/// \return The projection vector of camera's eye direction
Vector3D Camera::GetViewDirProjectionOnPlane(const Plane& plane,double& oxLength,double& oyLength)const
{
    Vector3D projectionVector = GetViewDirProjectionOnPlane(plane);
    Point3D points[4];
    plane.GetVertices(points);
    Vector3D ox = points[1] - points[0];
    Vector3D oy = points[3] - points[0];
    ox.Normalize();
    oy.Normalize();
    oxLength = projectionVector.DotProduct(ox);
    oyLength = projectionVector.DotProduct(oy);
    return projectionVector;
}

/// \brief calculate camera's view direction by a given plane and the projection
/// vector's length on the X axis and Y axis 
///
/// \param[in] plane The given plane
/// \param[in] oxLength projection vector's projection length on the X axis
/// \param[in] oxLength projection vector's projection length on the Y axis
///
/// \return the view direction
Vector3D Camera::CalculateViewDirByPlane(const Plane& plane,double oxLength,double oyLength)
{
    Point3D points[4];
    plane.GetVertices(points);
    Vector3D ox = points[1] - points[0];
    Vector3D oy = points[3] - points[0];
    ox.Normalize();
    oy.Normalize();
    Vector3D projectionVector = oxLength * ox + oyLength * oy;
    double projectionVectorMag = projectionVector.Magnitude();
    if (projectionVectorMag > 1.0)
    {
        return Vector3D(0,0,0);
    }

    Vector3D planeNomal = plane.GetNormal();
    Vector3D viewDir = (m_At - m_Eye);
    viewDir.Normalize();
    Vector3D myVector3D = sqrt(1 - projectionVectorMag * projectionVectorMag) * planeNomal;
    Vector3D newViewDir = myVector3D + projectionVector;
    //m_At = m_Eye + newViewDir * ((m_At - m_Eye).Magnitude());
    return newViewDir;
}

/// \brief set camera's view direction by a given plane and the projection
/// vector's length on the X axis and Y axis 
///
/// \param[in] plane The given plane
/// \param[in] oxLength projection vector's projection length on the X axis
/// \param[in] oxLength projection vector's projection length on the Y axis
///
/// \return none
void Camera::SetViewDirByPlane(const Plane& plane,double oxLength,double oyLength)
{
    Vector3D newViewDir = CalculateViewDirByPlane(plane,oxLength,oyLength);
    if (newViewDir != Vector3D(0,0,0))
    {
        Vector3D oldViewDir = m_At - m_Eye;
        double angle = AngleBetween(oldViewDir, newViewDir);
        Vector3D rotateAxis = oldViewDir.CrossProduct(newViewDir);
        rotateAxis.Normalize();
        Quaternion myQuaternion(angle,rotateAxis);
        Matrix4x4 mat = MakeTranslate(Point3D(0,0,0)-m_Eye);
        mat = myQuaternion.ToMatrix() * mat;
        mat = MakeTranslate(m_Eye - Point3D(0,0,0)) * mat;
        m_Up = mat.Transform(m_Up);
        m_At = m_Eye + newViewDir * ((m_At - m_Eye).Magnitude());
        SetViewMatrixDirty();
    }
}

/// \brief Set camera's view direction by a given nomal and the projection
/// vector's length on the X axis and Y axis, the nomal must be (1,0,0), 
/// (0,1,0) or (0,0,1), according to x plane, y plane, z plane
///
/// \param[in] nomal The given plane's nomal
/// \param[in] oxLength projection vector's projection length on the X axis
/// \param[in] oxLength projection vector's projection length on the Y axis
///
/// \return none
void Camera::SetViewDirByPlane(const Vector3D& nomal,double oxLength,double oyLength)
{
    if (nomal == Vector3D(1,0,0))
    {
        Plane plane(Point3D(0,0,0),Point3D(0,1,0),Point3D(0,0,1));
        SetViewDirByPlane(plane,oxLength,oyLength);
    }
    else if(nomal == Vector3D(0,1,0))
    {
        Plane plane(Point3D(0,0,0),Point3D(1,0,0),Point3D(0,0,-1));
        SetViewDirByPlane(plane,oxLength,oyLength);
    }
    else if (nomal == Vector3D(0,0,1))
    {
        Plane plane(Point3D(0,0,0),Point3D(1,0,0),Point3D(0,1,0));
        SetViewDirByPlane(plane,oxLength,oyLength);
    }
}

/// \brief Get camera's view direction's length on a given plane, the nomal must be (1,0,0), 
/// (0,1,0) or (0,0,1), according to x plane, y plane, z plane
///
/// \param[in] nomal The given plane's nomal
/// \param[out] oxLength projection vector's projection length on the X axis
/// \param[out] oxLength projection vector's projection length on the Y axis
///
/// \return none
void Camera::GetViewDirProjectionOnPlane(const Vector3D& nomal,double& oxLength,double& oyLength)
{
    double x = 0.0;
    double y = 0.0;
    if (nomal == Vector3D(1,0,0))
    {
        Plane plane(Point3D(0,0,0),Point3D(0,1,0),Point3D(0,0,1));
        GetViewDirProjectionOnPlane(plane,x,y);
    }
    else if(nomal == Vector3D(0,1,0))
    {
        Plane plane(Point3D(0,0,0),Point3D(1,0,0),Point3D(0,0,-1));
        GetViewDirProjectionOnPlane(plane,x,y);
    }
    else if (nomal == Vector3D(0,0,1))
    {
        Plane plane(Point3D(0,0,0),Point3D(1,0,0),Point3D(0,1,0));
        GetViewDirProjectionOnPlane(plane,x,y);
    }
    oxLength = x;
    oyLength = y;
}

/// \brief Get view direction
/// \return view direction
Vector3D Camera::GetViewDirection()const
{
    Vector3D dir = m_At - m_Eye;
    dir.Normalize();
    return dir;
}

/// \brief move camera to a new position, keeping view direction and up direction
///
/// \param[in] position The new position of camera
///
/// \return none
void Camera::MoveToNewPosition(const Point3D& position)
{
    if (m_Eye == position)
    {
        return;
    }
    Vector3D dir = m_At - m_Eye;
    m_Eye = position;
    m_At = position + dir;
    SetViewMatrixDirty();
}

void Camera::SetViewMatrix( const Matrix4x4& mat )
{
    m_ViewMatrix = mat;
    m_bViewMatrixDirty = false;
}

void Camera::SetProjectionMatrix( const Matrix4x4& mat )
{
    m_ProjectionMatrix = mat;
    m_bPerspectiveMatrixDirty = false;
}

END_NAMESPACE
