import math

import numpy as np
import ctypes
from skimage.filters import gaussian
from math import pi
from streamlit import cache


class Transform:
    decoders = 0
    rang = 0
    step = 0.0
    fil = False
    iter = (0, 180)

    def __init__(self):
        self.__cLib = ctypes.CDLL("./transf.so")

    def free_all(self):
        self.__cLib.free_result()
        self.__cLib.free_bitmap()

    def free_bitmap(self):
        self.__cLib.free_bitmap()

    @cache
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
        return res

    def reverse_transform(self, decoders, rang, step, size, fil):
        # sresult = self.__cLib.set_result
        # sresult.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_float]
        # for i, rr in enumerate(sinog.T):
        #     for j, r in enumerate(rr):
        #         sresult(i, j, r)

        self.__cLib.create_bitmap(size[0], size[1])
        get_bitmap = self.__cLib.get_bitmap
        get_bitmap.argtypes = [ctypes.c_int, ctypes.c_int]
        get_bitmap.restype = ctypes.c_float

        reverse = self.__cLib.reverse_bres
        reverse.argtypes = [ctypes.c_float, ctypes.c_int, ctypes.c_float, ctypes.c_int]
        # print("za")
        if fil:
            reverse(step, decoders, rang, 1)
        else:
            reverse(step, decoders, rang, 0)
        # print("za")

        res = np.zeros(size, dtype=float)
        for i, re in enumerate(res):
            for j, r in enumerate(re):
                res[i, j] = get_bitmap(i, j)

        res = gaussian(res, sigma=1)

        if fil:
            ma = np.amin(res)
            print(ma)
            res = res - ma
            ma = np.amax(res)
            if ma == np.inf:
                ma = np.ma.masked_invalid(res).max()
                for i, l in enumerate(res):
                    for j, v in enumerate(l):
                        if v == np.inf:
                            res[i, j] = ma
            print(ma)
            res = res / ma
            # print(res)
        else:
            ma = np.amax(res)
            print(ma)
            res = res / ma

        # mi = 1.0
        # ma = 0.0
        # radius = min(size[0], size[1]) / 2 - 1
        # radius = radius * 0.9
        # x, y = size[0] / 2 - 1, size[1] / 2 - 1
        # for i, rr in enumerate(res):
        #     for j, r in enumerate(rr):
        #         if mi > r:
        #             mi = r
        #         if ma < r:
        #             ma = r
        # print(x, y, radius)
        # print(mi, ma)
        #
        # for i, rr in enumerate(res):
        #     for j, r in enumerate(rr):
        #         if (x - i) ** 2 + (y - j) ** 2 <= radius ** 2:
        #             # print(i, j)
        #             res[i, j] = self.__cut((r - 1.1 * mi) / (ma - 1.1 * mi))

        return res

    def __cut(self, x):
        if x <= 0:
            return 0.0
        elif x >= 1:
            return 1.0
        return x

    def cut_all(self, image):
        m = np.amax(image)
        for i, l in enumerate(image):
            for j, v in enumerate(l):
                image[i, j] = v / m
        return image

    def norm(self, image, alpha, beta):
        i = np.amin(image)
        a = np.amax(image)
        i += alpha
        a -= beta
        for ri, rr in enumerate(image):
            for rj, r in enumerate(rr):
                image[ri, rj] = self.__cut((r - i) / (a - i))

        return image

    def reverse_transform_it(self, decoders, rang, step, size, fil, beg_s, end_s):

        self.__cLib.create_bitmap(size[0], size[1])
        get_bitmap = self.__cLib.get_bitmap
        get_bitmap.argtypes = [ctypes.c_int, ctypes.c_int]
        get_bitmap.restype = ctypes.c_float

        reverse = self.__cLib.reverse_bres_it
        reverse.argtypes = [ctypes.c_float, ctypes.c_int, ctypes.c_float, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        # print("za")
        if fil:
            reverse(step, decoders, rang, 1, beg_s, end_s)
        else:
            reverse(step, decoders, rang, 0, beg_s, end_s)
        # print("za")

        res = np.zeros(size, dtype=float)
        for i, re in enumerate(res):
            for j, r in enumerate(re):
                res[i, j] = get_bitmap(i, j)

        res = gaussian(res, sigma=1)

        if fil:
            ma = np.amin(res)
            print(ma)
            res = res - ma
            ma = np.amax(res)
            print(ma)
            res = res / ma
            # print(res)
        else:
            ma = np.amax(res)
            print(ma)
            res = res / ma

        return res

    def do_all(self, image, decoders, rang, steps, filter: bool):
        self.call_transform(image, decoders, rang * math.pi / 180, 2 * math.pi / steps)
        self.free_bitmap()
        rev = self.reverse_transform(decoders, rang * math.pi / 180, 2 * math.pi / steps, image.shape, filter)
        rev = self.norm(rev, 0.38, 0.0)
        return rev, math.sqrt(((self.cut_all(image) - rev) ** 2).mean())


if __name__ == '__main__':
    print("hello")
    test = Transform()
    # test.call_transform()
    print("yep")
