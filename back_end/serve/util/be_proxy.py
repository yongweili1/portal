import sys
import threading

sys.path.append('..')
from netbase import comproxy


class BEProxyInstance(comproxy.PyCommProxy):
    _instance_lock = threading.Lock()

    def __init__(self):
        comproxy.PyCommProxy.__init__(self, "", "10.9.19.150:10000")

    def __new__(cls, *args, **kwargs):
        if not hasattr(BEProxyInstance, "_instance"):
            with BEProxyInstance._instance_lock:
                if not hasattr(BEProxyInstance, "_instance"):
                    BEProxyInstance._instance = object.__new__(cls)
        return BEProxyInstance._instance


proxy = BEProxyInstance()