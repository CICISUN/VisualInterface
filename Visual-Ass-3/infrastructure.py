import numpy as np
import cv2
from matplotlib import pyplot as plt
import codecs

def main():
	filename = 'ass3-labeled.pgm'
	img = cv2.imread(filename,-1)
	MBR_list, COM_list, labels = step1(img)

def step1(image):
	area_list = []
	MBR_list = []
	COM_list = []
	dsc = [ {} for i in range(27)]
	labels = name_mapping()
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

	check_size(image, area_list, dsc)
	check_shape(image, MBR_list, dsc)
	check_orientation(image, MBR_list, dsc)
	ranked = sorted(area_list, key=lambda x: float(x[0]), reverse=True)
	check_extremum(image, ranked, MBR_list, dsc)
 	printable(dsc, labels, area_list, MBR_list, COM_list)
 	return MBR_list, COM_list, area_list, labels, dsc
	
def printable(dsc, labels, area_list, MBR_list, COM_list):
	outfile = codecs.open('part1_output.txt', encoding = 'utf-8', mode = 'w')
	for index in xrange(len(dsc)):
   		outfile.write("Building %d: %s" %(index+1,labels[str(index+1)].replace('\r\n','')))
   		outfile.write("\nCenter of Mass: (%f, %f)" %(COM_list[index][0][0],COM_list[index][0][1]))
  		outfile.write("\nArea: " + str(area_list[index][0]))
   		outfile.write("\nMinimum Bounding Rectangle: (%d,%d) to (%d,%d)" %(MBR_list[index][0][0][0],MBR_list[index][0][0][1],MBR_list[index][0][1][0],MBR_list[index][0][1][1]))
   		outfile.write("\nDescription: " + dsc[index]["size"] + 'sized')
   		if dsc[index]["shape"] is not 'Empty':
   			outfile.write(', ' + dsc[index]["shape"])
   		if dsc[index]["orientation"] is not 'Empty':
   			outfile.write(', ' + dsc[index]["orientation"])
   		if len(dsc[index]["extremum"]) is not 0:
   			for item in dsc[index]["extremum"]:
   				outfile.write(', ' + item)
   		outfile.write("\r\n\n")



 
def name_mapping():
	labelfile = open('ass3-table.txt', 'r')
	labels = {}
	for line in labelfile:
		tmp = line.split('=')
		labels[tmp[0]] = tmp[1].replace('\"','')
	return labels


def check_size(image, area_list, dsc):
	for index in xrange(len(dsc)):
		area = area_list[index][0]
		if area > 4000:
			dsc[index]["size"] = 'Large'
		elif area > 2000:
			dsc[index]["size"] = 'Average'
		elif area > 100:
			dsc[index]["size"] = 'Small'
		else:
			dsc[index]["size"] = 'Tiny'

def check_shape(image, MBR_list, dsc):
	for index in xrange(len(dsc)):
		lower_blue = np.array([index])
		upper_blue = np.array([index])
		img_tmp = cv2.inRange(image, lower_blue, upper_blue)
		thresh = cv2.Canny(img_tmp,100,200)
		contours, hierarchy = cv2.findContours(thresh,1,2)
		cv2.drawContours(img_tmp,contours,-1,(128,255,0),3)
		cnt = contours[0]
		epsilon = 0.1*cv2.arcLength(cnt,True)
		approx = cv2.approxPolyDP(cnt,epsilon,True)
		cv2.drawContours(img_tmp,approx,-1,(255,0,0),5)
		if len(approx) == 3:
			dsc[index]["shape"] = 'L-shaped'
		else:
			height = abs(MBR_list[index][0][1][0] - MBR_list[index][0][0][0])
			width = abs(MBR_list[index][0][1][1] - MBR_list[index][0][0][1])
			# if round(float(width / height)) == 1.0 and round(float(height / width)):
			if float(width / height) >= 1/1.5 and float(width / height) <= 1.5 or float(height / width) >= 1/1.5 and float(height / width) <= 1.5:
				dsc[index]["shape"] = 'Squarish'
			else:
				dsc[index]["shape"] = 'Rectangular'
		# cv2.imshow('result',img_tmp)
		# cv2.waitKey(0)


def check_orientation(image, MBR_list, dsc):
	for index in xrange(len(dsc)):
		height = abs(MBR_list[index][0][1][0] - MBR_list[index][0][0][0])
		width = abs(MBR_list[index][0][1][1] - MBR_list[index][0][0][1])
		if width / float(height) > 1.5:
			dsc[index]["orientation"] = 'East-to-west oriented'
		if height / float(width) > 1.5:
			dsc[index]["orientation"] = 'North-to-south oriented'
		else:
			dsc[index]["orientation"] = 'Symmetric'

def check_extremum(image, ranked, MBR_list, dsc):
	longest = 0
	thinest = 1000000
	northernmost = 1000000
	southernmost = 0
	easternmost = 0
	westernmost = 1000000
	minne = 1000000
	minnw = 1000000
	minse = 1000000
	minsw = 1000000
	for index in xrange(len(dsc)):
		dsc[index]["extremum"] = []
		# smallest / largest
		if index+1 == ranked[0][1]:
			dsc[index]["extremum"].append('Largest')
		elif index+1 == ranked[len(dsc)-1][1]:
			dsc[index]["extremum"].append('Smallest')
		height = abs(MBR_list[index][0][1][0] - MBR_list[index][0][0][0])
		width = abs(MBR_list[index][0][1][1] - MBR_list[index][0][0][1])
		# longest / thinest
		if min(width,height) < thinest:
			thinest_index = index
			thinest = min(width,height)
		if max(width,height) > longest:
			longest_index = index
			longest = max(width,height)
		# most  NE, NW, SE, SW,
		# Todo :change to distance between COM and four vertices, more accurate
		if MBR_list[index][0][0][0] + image.shape[1] - MBR_list[index][0][1][1] < minne:
			minne = MBR_list[index][0][0][0] + image.shape[1] - MBR_list[index][0][1][1]
			ne_index = index
		if MBR_list[index][0][0][0] + MBR_list[index][0][0][1] < minnw:
			minnw = MBR_list[index][0][0][0] + MBR_list[index][0][0][1]
			nw_index = index
		if image.shape[1] + image.shape[0] - MBR_list[index][0][1][0] + MBR_list[index][0][1][1] < minse:
			minse = image.shape[1] + image.shape[0] - MBR_list[index][0][1][0] + MBR_list[index][0][1][1]
			se_index = index
		if image.shape[1] + image.shape[0] - MBR_list[index][0][1][0] + MBR_list[index][0][0][1] < minsw:
			minsw = image.shape[1] + image.shape[0] - MBR_list[index][0][1][0] + MBR_list[index][0][0][1]
			sw_index = index
		# northernmost / southernmost / easternmost / westernmost
		if MBR_list[index][0][0][0] <= northernmost:
			northernmost = MBR_list[index][0][0][0]
			northernmost_index = index
		if MBR_list[index][0][1][0] >= southernmost:
			southernmost = MBR_list[index][0][1][0]
			southernmost_index = index
		if MBR_list[index][0][1][1] >= easternmost:
			easternmost = MBR_list[index][0][1][1]
			easternmost_index = index
		if MBR_list[index][0][0][1] <= westernmost:
			westernmost = MBR_list[index][0][0][1]
			westernmost_index = index

	dsc[thinest_index]["extremum"].append('Thinest')
	dsc[longest_index]["extremum"].append('Longest')
	dsc[northernmost_index]["extremum"].append('Northernmost')
	dsc[southernmost_index]["extremum"].append('Southernmost')
	dsc[easternmost_index]["extremum"].append('Easternmost')
	dsc[westernmost_index]["extremum"].append('Westernmost')
	dsc[ne_index]["extremum"].append('Northern-easternmost')
	dsc[nw_index]["extremum"].append('Northern-westernmost')
	dsc[se_index]["extremum"].append('Southern-easternmost')
	dsc[sw_index]["extremum"].append('Southern-westernmost')

if __name__=='__main__':
	main()