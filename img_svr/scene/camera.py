# -*- coding: utf-8 -*-
from md.mdmath.python.rotation3d import axis_angle_to_rotation_matrix

from scene.coord import Plane3D, project_point3d_on_plane


class SceneCamera(object):
    """
    a abstract camera for scene to help generate an slice image,
    this locates which slice to see and show

    """

    def __init__(self, look_at=None, fov=None, right=None, up=None):
        """
        generally, locate a camera with eye, lookAt, up and fov, but there
        we simplify parameters with right, up, lookAt and fov

        """
        self.look_at = look_at
        self.fov = fov
        self.right = right
        self.up = up

    def rotate(self, axis, angle):
        rot_mat = axis_angle_to_rotation_matrix(axis, angle)
        self.up = rot_mat.dot(self.up)
        self.right = rot_mat.dot(self.right)

    def pan(self, shift):
        lookat_plane = Plane3D(self.up, self.right, self.look_at)
        new_lookat = project_point3d_on_plane(lookat_plane, self.look_at + shift)
        self.look_at = new_lookat

    def zoom(self, factor):
        self.fov = [factor * self.fov[0], factor * self.fov[1]]

    def look(self, pt3d):
        lookat_plane = Plane3D(self.up, self.right, pt3d)
        new_lookat = project_point3d_on_plane(lookat_plane, self.look_at)
        self.look_at = new_lookat
