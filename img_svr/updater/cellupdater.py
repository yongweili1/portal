from model.imagemodel import VolumeInfo, GraphicModel
from model.workflow import GET_CLASS_NAME
from router.routerargs import GraphicType
from scene.coord import translate_from_world_to_screen
from scene.scene import CameraPos
from updater.args import RefreshType
from updater.baseupdater import BaseUpdater
from utilities import convert_rgba_to_base64, execute_time
from netbase.c_log import log


class CellUpdater(BaseUpdater):
    def __init__(self, entity):
        super(CellUpdater, self).__init__(entity)
        self._result = {'image': None,
                        'crosshair': (),
                        'boundary_pts': (),
                        'wwwl': (),
                        'slice_index': 0,
                        }

    def get_result(self):
        return self._result

    def update(self, *render_type):
        try:
            scene = self._entity.get_scene()
            workflow = self._entity.workflow
            for t in render_type:
                if t == RefreshType.Image:
                    self.update_image(scene, workflow)
                elif t == RefreshType.Crosshair:
                    self.update_crosshair(scene, workflow)
                elif t == RefreshType.SliceIndex:
                    self.update_slice_index(scene, workflow)
                elif t == RefreshType.WWWL:
                    self.update_wwwl(scene)
                elif t == RefreshType.BoundaryPts:
                    self.update_boundary_pts(scene, workflow)
                elif t == RefreshType.All:
                    self.update(RefreshType.Image, RefreshType.Crosshair, RefreshType.WWWL,
                                RefreshType.BoundaryPts, RefreshType.SliceIndex)
        except Exception, e:
            log.dev_error('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CellUpdater update() ---> {}'.format(e))

    @execute_time
    def update_image(self, scene, workflow):
        rgba_data = scene.render()
        self._result[RefreshType.Image] = convert_rgba_to_base64(rgba_data, 'jpeg')

    def update_crosshair(self, scene, workflow):
        model_vol = workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        cursor2d = translate_from_world_to_screen(scene, model_vol.cursor3d)
        self._result[RefreshType.Crosshair] = tuple(cursor2d)

    def update_slice_index(self, scene, workflow):
        model_vol = workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        pt3d_voxel = scene.volume.world_to_voxel(model_vol.cursor3d)
        self._result[RefreshType.SliceIndex] = int(round(pt3d_voxel[2]))
        log.dev_info(self._result[RefreshType.SliceIndex])

    def update_wwwl(self, scene):
        wwwl = scene.get_window_level()
        self._result[RefreshType.WWWL] = wwwl

    def update_boundary_pts(self, scene, workflow):
        model_vol = workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        self._result[RefreshType.BoundaryPts] = []
        volume = scene.volume
        center_x, center_y, center_z = volume.center()
        columns, rows, height = volume.size()
        spacing_x, spacing_y, spacing_z = volume.spacing()
        pos = scene.get_default_pos()
        if pos == CameraPos.Coronal:
            left_upper = [center_x - columns * spacing_x / 2, center_z - height * spacing_z / 2]
            right_upper = [center_x + columns * spacing_x / 2, center_z - height * spacing_z / 2]
            right_bottom = [center_x + columns * spacing_x / 2, center_z + height * spacing_z / 2]
            left_bottom = [center_x - columns * spacing_x / 2, center_z + height * spacing_z / 2]
            pts = [left_upper, right_upper, right_bottom, left_bottom]
            for pt in pts:
                pt = translate_from_world_to_screen(scene, [pt[0], model_vol.cursor3d[1], pt[1]])
                self._result[RefreshType.BoundaryPts].append(pt.tolist())
        elif pos == CameraPos.Sagittal:
            left_upper = [center_y - rows * spacing_y / 2, center_z - height * spacing_z / 2]
            right_upper = [center_y + rows * spacing_y / 2, center_z - height * spacing_z / 2]
            right_bottom = [center_y + rows * spacing_y / 2, center_z + height * spacing_z / 2]
            left_bottom = [center_y - rows * spacing_y / 2, center_z + height * spacing_z / 2]
            pts = [left_upper, right_upper, right_bottom, left_bottom]
            for pt in pts:
                pt = translate_from_world_to_screen(scene, [model_vol.cursor3d[0], pt[0], pt[1]])
                self._result[RefreshType.BoundaryPts].append(pt.tolist())
        elif pos == CameraPos.Transverse:
            left_upper = [center_x - columns * spacing_x / 2, center_y - rows * spacing_y / 2]
            right_upper = [center_x + columns * spacing_x / 2, center_y - rows * spacing_y / 2]
            right_bottom = [center_x + columns * spacing_x / 2, center_y + rows * spacing_y / 2]
            left_bottom = [center_x - columns * spacing_x / 2, center_y + rows * spacing_y / 2]
            pts = [left_upper, right_upper, right_bottom, left_bottom]
            for pt in pts:
                pt = translate_from_world_to_screen(scene, [pt[0], pt[1], model_vol.cursor3d[2]])
                self._result[RefreshType.BoundaryPts].append(pt.tolist())
        else:
            raise StandardError
