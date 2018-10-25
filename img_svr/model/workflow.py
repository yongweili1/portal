from model.cellmodel import CellsInfo
from model.imagemodel import GraphicModel, VolumeInfo


def GET_CLASS_NAME(class_name):
    return class_name.__name__


class Workflow:

    def __init__(self, uid):
        self.__uid = uid
        self.__cellview = []
        self.__dict_models = {}
        self.add_model(GET_CLASS_NAME(VolumeInfo), VolumeInfo())
        self.add_model(GET_CLASS_NAME(GraphicModel), GraphicModel())
        self.add_model(GET_CLASS_NAME(CellsInfo), CellsInfo())

    def add_cellview(self, *cellview):
        for cell in cellview:
            self.__cellview.append(cell)
            cell.workflow = self

    def add_model(self, model_name, model):
        self.__dict_models[model_name] = model

    def get_model(self, model_name):
        return self.__dict_models[model_name]


class WorkflowController:

    def __init__(self):
        self.workflows = []

    def add_workflow(self, workflow):
        self.workflows.append(workflow)
