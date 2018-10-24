#!/usr/bin/env python
import sys

import vtk
from vtk.util.numpy_support import vtk_to_numpy
from PyQt5.QtWidgets import QApplication
from datetime import datetime

from viewer3.BE.scene.scene import SceneBase


class VRScene(SceneBase):
    """
    volume rendering

    """
    def __init__(self):
        super(VRScene, self).__init__()

        self._vtk_render = vtk.vtkRenderer()
        self._vtk_window = vtk.vtkRenderWindow()
        self._vtk_window.SetOffScreenRendering(1)
        self._vtk_window.AddRenderer(self._vtk_render)

        reader = vtk.vtkNIFTIImageReader()
        reader.SetFileName(r'../data/05-02-1995.nii')
        reader.Update()

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInputConnection(reader.GetOutputPort())
        shiftScale.SetOutputScalarTypeToUnsignedChar()

        volumeMapper = vtk.vtkVolumeRayCastMapper()
        volumeMapper.SetVolumeRayCastFunction(vtk.vtkVolumeRayCastCompositeFunction())
        volumeMapper.SetInputConnection(shiftScale.GetOutputPort())

        opacityTransferFunction = vtk.vtkPiecewiseFunction()
        opacityTransferFunction.AddPoint(20, 0.0)
        opacityTransferFunction.AddPoint(255, 0.2)

        colorTransferFunction = vtk.vtkColorTransferFunction()
        colorTransferFunction.AddRGBPoint(0.0, 0.0, 0.0, 0.0)
        colorTransferFunction.AddRGBPoint(64.0, 1.0, 0.0, 0.0)
        colorTransferFunction.AddRGBPoint(128.0, 0.0, 0.0, 1.0)
        colorTransferFunction.AddRGBPoint(192.0, 0.0, 1.0, 0.0)
        colorTransferFunction.AddRGBPoint(255.0, 0.0, 0.2, 0.0)

        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetColor(colorTransferFunction)
        volumeProperty.SetScalarOpacity(opacityTransferFunction)
        volumeProperty.ShadeOn()
        volumeProperty.SetInterpolationTypeToLinear()

        self._volume = vtk.vtkVolume()
        self._volume.SetMapper(volumeMapper)
        self._volume.SetProperty(volumeProperty)

    def render(self):
        print '------------------ vr render start'
        begin = datetime.now()

        camera_vtk = vtk.vtkCamera()
        camera_vtk.SetViewUp(0, 0, 1)
        camera_vtk.SetPosition(0, 1, 0)
        camera_vtk.SetFocalPoint(0, 0, 0)
        camera_vtk.ComputeViewPlaneNormal()
        camera_vtk.Dolly(1.5)

        self._vtk_render.AddVolume(self._volume)
        self._vtk_render.SetActiveCamera(camera_vtk)
        self._vtk_render.ResetCamera()
        self._vtk_render.SetBackground(0, 0, 0)
        self._vtk_render.ResetCameraClippingRange()

        self._vtk_window.SetSize(self._width, self._height)
        self._vtk_window.Render()

        win2img = vtk.vtkWindowToImageFilter()
        win2img.SetInput(self._vtk_window)
        win2img.Update()

        image_data = win2img.GetOutput()
        width, height, _ = image_data.GetDimensions()
        vtk_arr = image_data.GetPointData().GetScalars()
        components = vtk_arr.GetNumberOfComponents()
        arr = vtk_to_numpy(vtk_arr).reshape(width, height, components)

        end = datetime.now()
        print 'vr render time: {}'.format(end - begin)

        return arr


class MeshScene(SceneBase):
    """
    mesh surface rendering

    """
    def __init__(self):
        super(MeshScene, self).__init__()

        self._vtk_render = vtk.vtkRenderer()
        self._vtk_window = vtk.vtkRenderWindow()
        self._vtk_window.SetOffScreenRendering(1)
        self._vtk_window.AddRenderer(self._vtk_render)

        # reader = vtk.vtkDICOMImageReader()
        # reader.SetDataByteOrderToLittleEndian()
        # reader.SetDirectoryName(r"E:\data\1.2.156.112605.161340571453.20140402034128.3.30864.1")
        # reader.SetDataSpacing(3.2, 3.2, 1.5)
        # reader.Update()

        reader = vtk.vtkNIFTIImageReader()
        reader.SetFileName(r'../data/05-02-1995.nii')
        reader.Update()

        skinExtractor = vtk.vtkContourFilter()
        skinExtractor.SetInputConnection(reader.GetOutputPort())
        skinExtractor.SetValue(0, 500)

        skinNormals = vtk.vtkPolyDataNormals()
        skinNormals.SetInputConnection(skinExtractor.GetOutputPort())
        skinNormals.SetFeatureAngle(60.0)

        skinMapper = vtk.vtkPolyDataMapper()
        skinMapper.SetInputConnection(skinNormals.GetOutputPort())
        skinMapper.ScalarVisibilityOff()

        self._skin = vtk.vtkActor()
        self._skin.SetMapper(skinMapper)
        self._camera_vtk = vtk.vtkCamera()

    def render(self):
        print '------------------ vr render start'
        begin = datetime.now()

        self._camera_vtk.SetViewUp(0, 0, 1)
        self._camera_vtk.SetPosition(0, 1, 0)
        self._camera_vtk.SetFocalPoint(0, 0, 0)
        self._camera_vtk.ComputeViewPlaneNormal()
        self._camera_vtk.Dolly(1)

        self._vtk_render.AddActor(self._skin)
        self._vtk_render.SetActiveCamera(self._camera_vtk)
        self._vtk_render.ResetCamera()
        self._vtk_render.SetBackground(0, 0, 0)
        self._vtk_render.ResetCameraClippingRange()

        self._vtk_window.SetSize(self._width, self._height)
        self._vtk_window.Render()

        win2img = vtk.vtkWindowToImageFilter()
        win2img.SetInput(self._vtk_window)
        win2img.Update()

        image_data = win2img.GetOutput()
        width, height, _ = image_data.GetDimensions()
        vtk_arr = image_data.GetPointData().GetScalars()
        components = vtk_arr.GetNumberOfComponents()
        arr = vtk_to_numpy(vtk_arr).reshape(width, height, components)

        end = datetime.now()
        print 'vr render time: {}'.format(end - begin)

        return arr


if __name__ == '__main__':
    app = QApplication(sys.argv)

    scene = MeshScene()
    scene.set_display_size(800, 800)
    data = scene.render()

    from viewer3.FE.overlay.overlay import OverlayImage
    overlay = OverlayImage()
    overlay.set_rgb_data(data, 800, 800)
    overlay.show()

    sys.exit(app.exec_())