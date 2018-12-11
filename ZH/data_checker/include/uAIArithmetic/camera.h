#ifndef UIH_ARITHMETIC_CAMERA_H_
#define UIH_ARITHMETIC_CAMERA_H_

#include "uai_app_defs.h"
#include "uAIArithmetic/point3d.h"
#include "uAIArithmetic/vector3d.h"
#include "uAIArithmetic/matrix_4x4.h"
#include <memory>

BEGIN_NAMESPACE

/// \class ProjectionCamera
/// \brief ProjectionCamera is the abstraction of 3D Camera which controls the angle 
///        of view, the looking direction of user's eyes, and the frustum which is
///        used to represent either a visible area or a projection area. Can be used
///        for a number of applications.

class Camera
{
    // Specifies perspective (realistic) or orthographic (architectural) projection.
public:
    Camera(void);
    virtual ~Camera(void);
    Camera(const Camera &myCamera);

public:
    enum ProjectionType
    {
        PT_ORTHOGRAPHIC,
        PT_PERSPECTIVE
    };
    /// Sets the position of the camera.
    void                    SetEye(const Point3D& eye);

    /// Retrieves the position of the camera. 
    const Point3D GetEye()const { return m_Eye;}

    /// Sets the looking direction of the camera.
    void                    SetLookAt(const Point3D& At);

    /// Retrieves the looking direction of the camera.
    const Point3D GetLookAt()const { return m_At;}

    /// Sets the up-direction of the camera.
    void                    SetUpDirection(const Vector3D& Updirection);

    /// Retrieves the up-direction of the camera.
    const Vector3D GetUpDirection()const { return m_Up;}

    /// Sets the Y-dimension Field Of View (FOV) of the frustum.
    void SetFovY(double dFovy);

    /// Retrieves the frustums Y-dimension Field Of View (FOV).
    double GetFovY()const{ return m_dFovY;}

    /// Sets the position of the near clipping plane.
    void SetNearClipDistance(double dNearDist);

    /// Retrieves the position of the near clipping plane.
    double GetNearClipDistance()const{ return m_dNearDist;}

    /// Sets the distance to the far clipping plane.
    void SetFarClipDistance(double dFarDist);

    /// Retrieves the distance from the frustum to the far clipping plane.
    double GetFarClipDistance()const{ return m_dFarDist;}

    /// Sets the aspect ratio for the frustum viewport.
    /// The ratio between the x and y dimensions of the rectangular area 
    /// visible through the frustum is known as aspect ratio: aspect = width / height .
    void SetAspectRatio(double dRatio);

    /// Retrieves the current aspect ratio.
    double GetAspectRatio() const{ return m_dAspect;}

    /// Gets the orthographic window height, for use with orthographic rendering only. 
    double GetOrthoWindowHeight() const{ return m_dOrthoHeight;}

    /// Sets the orthographic window height, for use with orthographic rendering only. 
    void SetOrthoWindowHeight(double dOrthoHeight);

    /// Gets the orthographic window width, for use with orthographic rendering only. 
    double GetOrthoWindowWidth() const{return m_dOrthoHeight * m_dAspect;}

    /// Sets the orthographic window width, for use with orthographic rendering only. 
    void SetOrthoWindowWidth(double dOrthoWidth);

    /// Sets the orthographic window settings, for use with orthographic rendering only. 
    void SetOrthoWindow(double w, double h);

    /// \brief move camera along Z axis
    virtual void MoveCameraZ(double dStep);

    /// \brief move camera along Y axis
    virtual void MoveCameraY(double dStep);

    /// \brief move camera along X axis
    virtual void MoveCameraX(double dStep);

    /// \brief rotate camera
    virtual void Rotate(double dAngle,const Vector3D& vAxis);

    /// \brief rotate camera
    virtual void Rotate(const Quaternion& myQuaternion);

    /// \brief rotate camera
    virtual void Rotate(const Matrix4x4& myMatrix);

    /// \brief get the reference of view matrix
    const Matrix4x4 GetViewMatrix();

    /// \brief get the reference of projection matrix
    const Matrix4x4 GetProjectionMatrix();

    /// \brief get the product of view matrix and projection matrix
    const Matrix4x4 GetViewProjectionMatrix();

    /// \brief zoom in or zoom out
    /// return true if execute the zoom operation, else return false
    virtual bool Zoom(double dfactor);

        /// Apply transform
    void ApplyTransform( const Quaternion myQuaternion);

    void ApplyTransform( const Matrix4x4 &myMatrix);

    /// \brief set projection type, either PT_ORTHOGRAPHIC or PT_PERSPECTIVE.
    void SetProjectionType(ProjectionType type);

    // \brief Get Projection type
    ProjectionType GetProjectionType() const;

    /// \brief Get the projection vector of camera's view direction on a given plane
    Vector3D GetViewDirProjectionOnPlane(const Plane& plane)const;

    /// \brief Get the projection vector of camera's view direction on a given plane
    /// and calculate the projection vector's length on the X axis and Y axis 
    /// of the given plane
    Vector3D GetViewDirProjectionOnPlane(const Plane& plane,double& oxLength,double& oyLength)const;

    /// \brief calculate camera's view direction by a given plane and the projection
    /// vector's length on the X axis and Y axis 
    Vector3D CalculateViewDirByPlane(const Plane& plane,double oxLength,double oyLength);

    /// \brief Set camera's view direction by a given plane and the projection
    /// vector's length on the X axis and Y axis 
    void SetViewDirByPlane(const Plane& plane,double oxLength,double oyLength);

    /// \brief Set camera's view direction by a given nomal and the projection
    /// vector's length on the X axis and Y axis, the nomal must be (1,0,0), 
    /// (0,1,0) or (0,0,1), according to x plane, y plane, z plane
    void SetViewDirByPlane(const Vector3D& nomal,double oxLength,double oyLength);

    /// \brief Get camera's view direction's length on a given plane, the nomal must be (1,0,0), 
    /// (0,1,0) or (0,0,1), according to x plane, y plane, z plane
    void GetViewDirProjectionOnPlane(const Vector3D& nomal,double& oxLength,double& oyLength);

    /// \brief Get view direction
    Vector3D GetViewDirection()const;

    /// \brief move camera to a new position, keeping view direction and up direction
    void MoveToNewPosition(const Point3D& position);

    Camera& operator =(const Camera &myCamera);

	/// \brief Set view matrix to input matrix
	///            attentation : call this interface will force replace old one
	void SetViewMatrix(const Matrix4x4& mat);

	/// \brief Set projection matrix to input matrix
	///            attentation : call this interface will force replace old one
	void SetProjectionMatrix(const Matrix4x4& mat);

protected:
    /// Tests whether the view matrix is dirty.
    bool IsViewMatrixDirty() const { return m_bViewMatrixDirty;}

    /// Sets the view matrix dirty.
    void SetViewMatrixDirty(){m_bViewMatrixDirty = true;}

private:
    /// Sets the view matrix not dirty.
    void RemoveViewMatrixDirty() { m_bViewMatrixDirty = false;}

    /// Tests whether the projection matrix is dirty.
    bool IsPerspectiveMatrixDirty() const { return m_bPerspectiveMatrixDirty;}

    /// Sets the projection matrix dirty.
    void SetPerspectiveMatrixDirty(){m_bPerspectiveMatrixDirty = true;}

    /// Sets the projection matrix not dirty.
    void RemovePerspectiveMatrixDirty() { m_bPerspectiveMatrixDirty = false;}

     // Internal functions for calculating view matrix.
    void CalcViewMatrix();

     // Internal functions for calculating projection matrix.
    void CalcProjMatrix();

private:
    /// The eye position of the camera
    Point3D m_Eye;

    /// The looking direction of the camera
    Point3D m_At;

    /// The up direction of the camera.
    Vector3D m_Up;

    /// A flag to indicate whether view matrix is dirty
    bool m_bViewMatrixDirty;

    /// View matrix
    Matrix4x4 m_ViewMatrix;

    /// y-direction field-of-view 
    double m_dFovY;

     /// x/y viewport ratio
    double m_dAspect;

    /// Near clip distance
    double m_dNearDist;

     /// Far clip distance
    double m_dFarDist;

    /// Ortho height size
    double m_dOrthoHeight;

    /// A flag to indicate whether perspective matrix is dirty
    bool m_bPerspectiveMatrixDirty;

    /// Orthographic or perspective?
    ProjectionType m_ProjType;

    /// projection matrix
   Matrix4x4 m_ProjectionMatrix;

    /// Small constant used to reduce far plane projection to avoid inaccuracies
   static const double INFINITE_FAR_PLANE_ADJUST;
};

END_NAMESPACE
#endif
