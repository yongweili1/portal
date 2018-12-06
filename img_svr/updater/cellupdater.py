from model.imagemodel import VolumeInfo, GraphicModel
from model.workflow import GET_CLASS_NAME
from router.routerargs import GraphicType
from scene.coord import translate_from_world_to_screen
from updater.args import RefreshType
from updater.baseupdater import BaseUpdater
from utilities import convert_rgba_to_base64


class CellUpdater(BaseUpdater):
    def __init__(self, entity):
        super(CellUpdater, self).__init__(entity)
        self._result = {'image': None,
                        'crosshair': (),
                        'boundary_pts': (),
                        'wwwl': (),
                        'slice_index': 0,
                        'graphic': {'prolines': {}, 'circle': {}, 'contours': []}
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
                elif t == RefreshType.Graphic:
                    self.update_graphic(scene, workflow)
                elif t == RefreshType.Text:
                    pass
                elif t == RefreshType.SliceIndex:
                    self.update_slice_index(scene, workflow)
                elif t == RefreshType.WWWL:
                    self.update_wwwl(scene)
                elif t == RefreshType.BoundaryPts:
                    self.update_boundary_pts(scene, workflow)
                elif t == RefreshType.All:
                    self.update(RefreshType.Image, RefreshType.Crosshair, RefreshType.Graphic, RefreshType.WWWL,
                                RefreshType.BoundaryPts, RefreshType.SliceIndex)
        except Exception, e:
            print '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CellUpdater update() ---> {}'.format(e)

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
        self._result[RefreshType.SliceIndex] = pt3d_voxel[2]
        print self._result[RefreshType.SliceIndex]

    def update_graphic(self, scene, workflow):
        model_graphic = workflow.get_model(GET_CLASS_NAME(GraphicModel))
        graphics_dict = self._result.get(RefreshType.Graphic)
        graphics_dict[GraphicType.Contours] = scene.get_contours()
        graphics_dict[GraphicType.ProLines] = {}
        for uid, line in model_graphic.lines.items():
            pt2d_pre = translate_from_world_to_screen(scene, line[0])
            pt2d_cur = translate_from_world_to_screen(scene, line[1])
            graphics_dict[GraphicType.ProLines][uid] = (tuple(pt2d_pre), tuple(pt2d_cur))
        # for circle in enumerate(model_graphic.circles):
        #     pt2d_pre = translate_from_world_to_screen(scene, circle[self._uid][0])
        #     pt2d_cur = translate_from_world_to_screen(scene, circle[self._uid][1])
        #     shift = (pt2d_cur[0] - pt2d_pre[0], pt2d_cur[1] - pt2d_pre[1])
        #     graphics_dict[GraphicType.Circle].append((pt2d_pre[0], pt2d_pre[1]), np.linalg.norm(shift))

    def update_wwwl(self, scene):
        wwwl = scene.get_window_level()
        self._result[RefreshType.WWWL] = wwwl

    def update_boundary_pts(self, scene, workflow):
        model_vol = workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        self._result[RefreshType.BoundaryPts] = []
        pts = scene.get_boundary_pts()
        for pt in pts:
            pt = translate_from_world_to_screen(scene, [pt[0], pt[1], model_vol.cursor3d[2]])
            self._result[RefreshType.BoundaryPts].append(pt.tolist())
