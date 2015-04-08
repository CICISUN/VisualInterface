import numpy as np
import cv2

 

def main():
	filename='ass3-labeled.pgm'
	img = cv2.imread(filename,-1)
	print img.shape
	print img.size
	step1(img)

	# cv2.imshow('image',img)
	# cv2.waitKey(0)
	# cv2.destroyAllWindows()

def step1(image):
	area_list = []
	MBR_list = []
	COM_list = []
	for index in xrange(1,28):
		#area
		area = np.where(image == index)
		area_list.append((len(area[0]),index))
		#MBR
		upper_left = (min(area[0]), min(area[1]))
		bottom_right = (max(area[0]), max(area[1]))
		MBR_list.append(((upper_left,bottom_right), index))
		#COM
		xindex = float(sum(area[0])) / len(area[0])
		yindex = float(sum(area[1])) / len(area[1])
		COM_list.append(((xindex,yindex),index))

	ranked = sorted(area_list, key=lambda x: float(x[0]), reverse=True)
		


	print MBR_list, COM_list
 


def check_size(image):
	area_list = []
	for index in xrange(1,28):
		area = 0
		for (x,y), value in np.ndenumerate(image):
			if value==index:
				area = area + value
		area_list.append(((100 * float(area/index)/image.size),index))
	ranked = sorted(area_list, key=lambda x: float(x[0]), reverse=True)
	print ranked
	print area_list



if __name__=='__main__':
	main()