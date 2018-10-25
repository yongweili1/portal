from PyQt5.QtCore import QObject, pyqtSignal

from args import RefreshType


class SendRouter(QObject):
    """ a router for send command or signal """

    # :param (CmdType, RouterPath, CmdArgs)
    sRouterCommand = pyqtSignal(str, list, tuple)

    # :param (CmdType, RouterPath, Level, CmdArgs)
    sRouterLevelCommand = pyqtSignal(str, list, int, tuple)

    def __init__(self, uid):
        super(SendRouter, self).__init__()
        self._uid = uid

    def send_cmd(self, cmd, *args):
        self.sRouterCommand.emit(cmd, [self._uid], args)

    def send_level_cmd(self, cmd, level, *args):
        self.sRouterLevelCommand.emit(cmd, [self._uid], level, args)


class ReceiveRouter(QObject):
    """ a router for receive command or signal"""

    def __init__(self, accept=False):
        super(ReceiveRouter, self).__init__()
        self._sender = None
        self._is_accept = accept

    def _on_cmd_handler(self, cmd, path, *args):
        if self._is_accept is True:
            print 'receive {} by {} with {}'.format(cmd, path, args)

    def _on_level_cmd_handler(self, cmd, path, level, *args):
        pass

    def accept(self):
        self._is_accept = True

    def ignore(self):
        self._is_accept = False

    def set_sender(self, sender):
        self._sender = sender
        if sender is not None:
            sender.sRouterCommand.connect(self._on_cmd_handler)
            sender.sRouterLevelCommand.connect(self._on_level_cmd_handler)


class DuplexRouter(SendRouter, ReceiveRouter):
    """
    a router supports send and receive command or signal,
    which can also handle command to call entity function

    """

    def __init__(self, uid, entity=None, accept=False):
        SendRouter.__init__(self, uid)
        ReceiveRouter.__init__(self, accept)
        self.__entity = entity

    def set_entity(self, entity):
        self.__entity = entity

    def _on_cmd_handler(self, cmd, path, args):
        try:
            path.append(self._uid)
            print "------> receive cmd '{}' by {} with args {}".format(cmd, path, args)
            en = self.__entity
            if en is not None and self._is_accept is True:
                op_fun = getattr(en, cmd)
                print '---> operation begin <---'
                op_fun(path, *args)
                print '---> operation end <---'
                print '---> update begin <---'
                en.updater().update(RefreshType.All)
                print '---> update end <---'
                print '---> refresh begin <---'
                en.refresh(en.updater().get_result(), RefreshType.All)
                print '---> refresh end <---'
            else:
                self.sRouterCommand.emit(cmd, path, args)
        except Exception, e:
            print '_on_cmd_handler except: {}'.format(e)

    def _on_level_cmd_handler(self, cmd, path, level, args):
        try:
            path.append(self._uid)
            print "------> receive cmd '{}' by {} with args {}".format(cmd, path, args)
            en = self.__entity
            if self.__entity is not None and len(path) == level + 1:
                op_fun = getattr(en, cmd)
                print '---> operation begin <---'
                op_fun(path, *args)
                print '---> operation end <---'
                print '---> update begin <---'
                en.updater().update(RefreshType.All)
                print '---> update end <---'
                print '---> refresh begin <---'
                en.refresh(en.updater().get_result(), RefreshType.All)
                print '---> refresh end <---'
            else:
                self.sRouterLevelCommand.emit(cmd, path, level, args)
        except Exception, e:
            print '_on_level_cmd_handler except: {}'.format(e)


def link_routers(router_lst):
    """
    link all routers by sequence

    """
    for i, router in enumerate(router_lst):
        assert (isinstance(router, ReceiveRouter))
        if i > 0:
            router.set_sender(router_lst[i - 1])


if __name__ == '__main__':
    from viewer3.BE.router import Command

    router1 = DuplexRouter(1, None, True)
    router2 = DuplexRouter(2, None, False)
    router3 = DuplexRouter(3, None, True)

    link_routers([router1, router2, router3])
    router1.send_cmd(Command.Rotate, 90)
    router1.send_cmd(Command.Pan, (100, 100), (200, 200))
    router1.send_cmd(Command.Zoom, 1.2)
    router1.send_cmd(Command.Window, 1.2, 1.2)
    router1.send_cmd(Command.Page, 2)
    router1.send_cmd(Command.Locate, (100, 200))
    router1.send_cmd('XXXXX', (100, 100), (200, 200), False)
