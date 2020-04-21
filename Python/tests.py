from main import Transform
from skimage.io import imread, imsave

if __name__ == "__main__":
    image = imread("zdjecia/Shepp_logan.jpg", True)
    tomograf = Transform()
    with open("wynik/default.txt", 'w') as res:
        res.write("Przypadek domyślny:\n")
        rev, rmse = tomograf.do_all(image, 180, 180, 180, True)
        res.write(str(rmse) + "\n\n")
        imsave("wynik/default.jpg", rev)
    #
    with open("wynik/decoders.txt", 'w') as res:
        res.write("Dekodery:\n")
        for decoders in range(90, 721, 90):
            tomograf = Transform()
            rev, rmse = tomograf.do_all(image, decoders, 180, 180, True)
            res.write("( " + str(decoders) + ", " + str(rmse) + ")\n")
            imsave("wynik/dec" + str(decoders) + ".jpg", rev)
    #
    with open("wynik/step.txt", 'w') as res:
        res.write("\nScany:\n")
        for steps in range(90, 721, 90):
            tomograf = Transform()
            rev, rmse = tomograf.do_all(image, 180, 180, steps, True)
            res.write("( " + str(steps) + ", " + str(rmse) + ")\n")
            imsave("wynik/step" + str(steps) + ".jpg", rev)

    with open("wynik/rang.txt", 'w') as res:
        res.write("\nRozpietosc:\n")
        for rang in range(45, 271, 45):
            tomograf = Transform()
            rev, rmse = tomograf.do_all(image, 180, rang, 180, True)
            res.write("( " + str(rang) + ", " + str(rmse) + ")\n")
            imsave("wynik/rang" + str(rang) + ".jpg", rev)
    #
    with open("wynik/pic1.txt", 'w') as res:
        res.write("\nObraz 1:\n")
        image = imread("zdjecia/SADDLE_PE.JPG", True)
        rev, rmse = Transform().do_all(image, 360, 270, 360, False)
        res.write(str(rmse) + "\t")
        imsave("wynik/o1_bf.jpg", rev)
        rev, rmse = Transform().do_all(image, 360, 270, 360, True)
        res.write(str(rmse)+"\n")
        imsave("wynik/o1_zf.jpg", rev)
    #
    with open("wynik/pic2.txt", 'w') as res:
        res.write("\nObraz 2:\n")
        image = imread("zdjecia/CT_ScoutView.jpg", True)
        rev, rmse = Transform().do_all(image, 360, 270, 360, False)
        res.write(str(rmse) + "\t")
        imsave("wynik/o2_bf.jpg", rev)
        rev, rmse = Transform().do_all(image, 360, 270, 360, True)
        res.write(str(rmse) + "\n")
        imsave("wynik/o2_zf.jpg", rev)
    print("!!koniec!!")
