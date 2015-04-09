import numpy as np
import cv2
import sys,os
import Tkinter
from Tkinter import *

def main():
	filename='ass3-campus.pgm'
	img = cv2.imread(filename,-1)
	print img.shape
	print img.size
	# # step1(img)
	# app = QtGui.QApplication(sys.argv)
	# window = QtGui.QMainWindow()
	# window.setGeometry(0, 0, 500, 500)
	# pic = QtGui.QLabel(window)
	# pic.setGeometry(0, 0, 500, 500)
	# #use full ABSOLUTE path to the image, not relative
	# pic.setPixmap(QtGui.QPixmap(os.getcwd() + "/edge.png"))
	# window.show()
	# sys.exit(app.exec_())

	# # cv2.imshow('image',img)
	# # cv2.waitKey(0)
	# # cv2.destroyAllWindows()
	cv2.imwrite('campus.ppm', img)




if __name__=='__main__':
	main()