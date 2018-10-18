import numpy as np


class Plane3D:
    def __init__(self, vec_x, vec_y, pt3d):
        self.axis_x = vec_x
        self.axis_y = vec_y
        self.point3d = pt3d

    @property
    def normal(self):
        norm = np.cross(self.axis_x, self.axis_y)
        return norm


def translate_from_world_to_screen(look_right, look_up, look_at, size, spacing, pt3d):
    plane = Plane3D(look_up, look_right, look_at)
    pt3d_plane = project_point3d_on_plane(plane, pt3d)
    origin3d_plane = _get_scene_world_origin(look_right, look_up, look_at, size)
    axes = np.array([np.array(look_up), np.array(look_right)])
    point2d = np.matmul(axes, pt3d_plane - origin3d_plane)/spacing
    return point2d


def translate_from_screen_to_world(scene, pt2d):
    camera = scene.camera
    view_size = scene.get_view_size()
    offset = np.array(view_size) / 2
    axes = np.array([np.array(camera.right), np.array(camera.up)])
    origin3d = camera.look_at - np.matmul(offset, axes)
    point = np.array(pt2d, dtype=np.double)
    return origin3d + np.matmul(point * scene.get_spacing(), axes)


def project_point3d_on_plane(plane, pt3d):
    """ get projected 3d position of 3d point on view2d """
    offset = np.array(pt3d) - np.array(plane.point3d)
    offset -= plane.normal * np.sum(offset * plane.normal)
    return offset + plane.point3d


def _get_scene_world_origin(look_right, look_up, look_at, view_size):
    offset = np.array(view_size) / 2
    axes = np.array([np.array(look_right), np.array(look_up)])
    return look_at - np.matmul(offset, axes)
