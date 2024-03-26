
from PIL import Image


file = r'C:\Users\Administrator\Downloads\7b3ceed85dd728f2c2ab62419ce76980.png'
img=Image.open(r'C:\Users\Administrator\Downloads\7b3ceed85dd728f2c2ab62419ce76980.png')

print(len(img.split()))



from PIL import Image
import cv2
from skimage import io
 
def shape_size():
    img = cv2.imread(file)
    print(img.shape)  # h, w, c
    img1 = Image.open(file)
    print(img1.size)  # w, h
    img2 = io.imread(file)
    print(img2.shape)  # h, w, c
 
 
if __name__ == '__main__':
    shape_size()