class VolumeInfo:
    def __init__(self):
        self.cursor3d = None
        self.default_cursor3d = None


class GraphicModel:
    def __init__(self):
        self.lines = {}  # {id:(pt1, pt2), id:(pt1, pt2)}
        self.circles = {}  # {id:[(pt3d), (pt3d), (pt3d)]}
