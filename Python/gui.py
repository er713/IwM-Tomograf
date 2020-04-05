import math
import streamlit as st
from os import remove
from skimage.io import imread
from copy import copy

from main import Transform

st.write("""
# Tomograf
""")

image_bytes = st.file_uploader("Choose a image", type="jpg")

if image_bytes is not None:
    tomograf = Transform()
    m = 1.0
    # tomograf.free_all()
    with open("tem.jpg", 'wb') as f:
        f.write(image_bytes.read())
    image = imread("tem.jpg", True)
    remove("tem.jpg")
    st.image(image)

    decoders = st.slider("Liczba dekoderów", 50, 1000, 800, 10)
    rang = st.slider("Rozpiętość (w stopniach)", 100, 330, 250, 1)
    step = st.slider("Krok (w stopniach)", 0.1, 5.0, 0.5, 0.1)

    if not st.checkbox("Przetwarzanie iteracyjne"):

        if st.checkbox("Włącz tomograf"):
            sinogram = tomograf.call_transform(image, decoders, rang * math.pi / 180, step * math.pi / 180)
            tomograf.free_bitmap()
            st.image(sinogram)
            fil = st.checkbox("Filtruj sinogram")
            if st.checkbox("Odwrotna transformacja"):
                odw = tomograf.reverse_transform(decoders, rang * math.pi / 180, step * math.pi / 180,
                                                 image.shape, fil)
                st.image(odw)
                al = st.slider("Zwiększenie czerni", 0.0, 1.0, 0.38, 0.01)
                bt = st.slider("Zwiększenie bieli", 0.0, 1.0 - al, 0.0, 0.01)
                nodw = tomograf.norm(copy(odw), al, bt)
                # print(nodw)
                st.image(nodw)
    else:
        beg_s, end_s = st.slider("Obrót od, do", 0, 360, (90, 180), 1)
        print(beg_s, end_s)
        if st.checkbox("Włącz tomograf"):
            sinogram = tomograf.call_transform(image, decoders, rang * math.pi / 180, step * math.pi / 180)
            tomograf.free_bitmap()
            cutted = sinogram[int(beg_s / 360 * sinogram.shape[0]):int(end_s / 360 * sinogram.shape[0]), ]
            st.image(cutted)
            fil = st.checkbox("Filtruj sinogram")
            if st.checkbox("Odwrotna transformacja"):
                odw = tomograf.reverse_transform_it(decoders, rang * math.pi / 180, step * math.pi / 180,
                                                 image.shape, fil, beg_s, end_s)
                st.image(odw)
                al = st.slider("Zwiększenie czerni", 0.0, 1.0, 0.38, 0.01)
                bt = st.slider("Zwiększenie bieli", 0.0, 1.0 - al, 0.0, 0.01)
                nodw = tomograf.norm(copy(odw), al, bt)
                # print(nodw)
                st.image(nodw)
