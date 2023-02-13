import sys
import time
import cv2
import numpy as np
import os

img = cv2.imread('TestImage.png', cv2.IMREAD_COLOR)

#Convert to Grayscale
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

#Blur image to reduce noise
blurred = cv2.GaussianBlur(gray, (9, 9), 0)

#Perform canny edge-detection
edged = cv2.Canny(blurred, 50, 150)

#Perform hough lines probalistic transform
lines = cv2.HoughLinesP(edged,1,np.pi/180,10,80,1)

#Draw lines on input image
if(lines.any() != None):
    for line in lines:
        for x1,y1,x2,y2 in line:
            cv2.line(img,(x1,y1),(x2,y2),(0,255,0),2)

cv2.imshow('image', img)
cv2.waitKey(0)

cv2.imwrite('C:/Users/Oben/Desktop/Test_img.jpg', img)
cv2.imwrite('C:/Users/Oben/Desktop/Test_blurred.jpg', blurred)
cv2.imwrite('C:/Users/Oben/Desktop/Test_edged.jpg', edged)