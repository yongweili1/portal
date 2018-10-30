from enum import Enum


class Command:
    def __init__(self):
        pass

    SetAction = '_set_action'
    ResetScenes = '_reset_scenes'
    LocateCenter = '_locate_center'
    # Update = '_update'
    Rotate = '_rotate'
    Pan = '_pan'
    Zoom = '_zoom'
    Window = '_window'
    Page = '_page'
    Locate = '_locate'
    Maximize = '_maximize'
    DrawGraphic = '_draw_graphic'
    # level command
    Resize = '_resize'


class CmdLevel(object):
    L1 = 1
    L2 = 2
    L3 = 3


class GraphicType(object):
    Line = 'line'
    ProLines = 'prolines'
    Circle = 'circle'
    Contours = 'contours'


class GraphicState(Enum):
    Initial = 0
    Drawing = 1
    Complete = 2
