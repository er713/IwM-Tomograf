import numpy as np
import ctypes
from skimage.io import imread, imshow, imsave
from math import pi
from matplotlib import pyplot as plt


class Transform:
    def __init__(self):
        self.__cLib = ctypes.CDLL("./transf.so")

    def free_all(self):
        self.__cLib.free_result()
        self.__cLib.free_bitmap()

    def free_bitmap(self):
        self.__cLib.free_bitmap()

    def call_transform(self, bitmap, decoders, rang, step):
        # bitmap = imread("zdjecia/Kropka.jpg", True)
        # imshow(bitmap)
        # plt.show()
        self.__cLib.create_bitmap(len(bitmap), len(bitmap[0]))
        setter = self.__cLib.set_bitmap
        setter.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_float]
        for i, b in enumerate(bitmap):
            for j, val in enumerate(b):
                self.__cLib.set_bitmap(i, j, val)

        transform = self.__cLib.transform_bres
        transform.argtypes = [ctypes.c_float, ctypes.c_int, ctypes.c_float]
        transform(step, decoders, rang)

        get_result = self.__cLib.get_result
        get_result.argtypes = [ctypes.c_int, ctypes.c_int]
        get_result.restype = ctypes.c_float
        res = np.zeros((int(2 * pi // step), decoders))
        for i, r in enumerate(res):
            for j, w in enumerate(r):
                res[i, j] = get_result(i, j)

        # print(res)
        m = np.amax(res)
        print(m)
        res = res / m
        # res = np.array(res,dtype=float).resize((decoders, ))
        # imshow(res.T)
        # plt.show()
        # imsave("wyn.jpg", res.T, format="jpg")
        return res.T

    def reverse_transform(self, decoders, rang, step, size):
        self.__cLib.create_bitmap(size, size)
        get_bitmap = self.__cLib.get_bitmap
        get_bitmap.argtypes = [ctypes.c_int, ctypes.c_int]
        get_bitmap.restype = ctypes.c_float

        reverse = self.__cLib.reverse_bres
        reverse.argtypes = [ctypes.c_float, ctypes.c_int, ctypes.c_float, ctypes.c_int]
        print("za")
        reverse(step, decoders, rang, size) #TODO result w c nie ma
        print("za")

        res = np.zeros((size, size), dtype=float)
        for i, re in enumerate(res):
            for j, r in enumerate(re):
                res[i, j] = get_bitmap(i, j)

        return res


if __name__ == '__main__':
    print("hello")
    test = Transform()
    # test.call_transform()
    print("yep")
