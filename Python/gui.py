import math
import streamlit as st
import numpy as np
import io
from os import remove
from skimage.io import imread

from main import Transform

st.write("""
# Tomograf
""")
tomograf = Transform()

image_bytes = st.file_uploader("Choose a image", type="jpg")

if image_bytes is not None:
    tomograf.free_all()
    with open("tem.jpg", 'wb') as f:
        f.write(image_bytes.read())
    image = imread("tem.jpg", True)
    remove("tem.jpg")
    st.image(image)

    decoders = st.slider("Liczba dekoderów", 50, 1000, 800, 10)
    rang = st.slider("Rozpiętość (w stopniach)", 100, 330, 250, 1)
    step = st.slider("Krok (w stopniach)", 0.1, 5.0, 0.5, 0.1)

    if st.button("Włącz tomograf"):
        sinogram = tomograf.call_transform(image, decoders, rang * math.pi / 180, step * math.pi / 180)
        tomograf.free_bitmap()
        sinogram = sinogram
        st.image(sinogram)

    if st.button("Odwróc"):
        print("przed")
        odw = tomograf.reverse_transform(decoders, rang * math.pi / 180, step * math.pi / 180,
                                         int(math.floor(math.sqrt(decoders * 360 / step))))
        print("po")
        st.image(odw)
        tomograf.free_all()