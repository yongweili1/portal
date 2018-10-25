from model.imagemodel import VolumeInfo, GraphicModel
from model.workflow import GET_CLASS_NAME
from router.routerargs import GraphicType
from scene.coord import translate_from_world_to_screen
from updater.baseupdater import BaseUpdater
from updater.args import RefreshType
from utilities import convert_rgba_to_base64


class CellUpdater(BaseUpdater):
    def __init__(self, entity):
        super(CellUpdater, self).__init__(entity)
        self._result = {'image': None,
                        'crosshair': (),
                        'wwwl': (),
                        'graphic': {'prolines': {}, 'circle': {}}
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
                elif t == RefreshType.All:
                    self.update(RefreshType.Image, RefreshType.Crosshair, RefreshType.Graphic)
        except Exception, e:
            print '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CellUpdater update() ---> {}'.format(e)

    def update_image(self, scene, workflow):
        rgba_data = scene.render()
        self._result[RefreshType.Image] = convert_rgba_to_base64(rgba_data, 'PNG')

    def update_crosshair(self, scene, workflow):
        model_vol = workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        cursor2d = translate_from_world_to_screen(scene, model_vol.cursor3d)
        self._result[RefreshType.Crosshair] = tuple(cursor2d.tolist())

    def update_graphic(self, scene, workflow):
        model_graphic = workflow.get_model(GET_CLASS_NAME(GraphicModel))
        graphics_dict = self._result.get(RefreshType.Graphic)
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

