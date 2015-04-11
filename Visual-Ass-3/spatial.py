import numpy as np
import cv2
from infrastructure import step1
import math

def main():
	filename='ass3-labeled.pgm'
	image = cv2.imread(filename,-1)
	step2(image)


	# cv2.imshow('image',img)
	# cv2.waitKey(0)
	# cv2.destroyAllWindows()

def step2(image):
	global MBR_list
	global COM_list
	global height 
	global width
	global area_list
	MBR_list, COM_list, area_list,labels,dsc = step1(image)
	height = image.shape[0]
	width = image.shape[1]

    # Generate all binary spatial relationships for every pair
	rel = [[{} for i in range(len(MBR_list))] for j in range(len(MBR_list))]
	for i in xrange(len(MBR_list)):
		for j in xrange(len(MBR_list)):
			rel[i][j]['north'] = North(i+1, j+1)
			rel[i][j]['south'] = South(i+1, j+1)
			rel[i][j]['east'] = East(i+1, j+1)
			rel[i][j]['west'] = West(i+1, j+1)
			rel[i][j]['near'] = Near(i+1, j+1)
    # transitive reduction 
    # to do: near?
	# for i in xrange(len(MBR_list)):
	# 		for j in xrange(len(MBR_list)):
	# 			try:
	# 				if rel[i][j]['north']:
	# 					rel[j][i]['south'] = False
	# 				if rel[i][j]['east']:
	# 					rel[j][i]['west'] = False
	# 			except KeyError:
	# 				pass	

	for i in xrange(len(MBR_list)):
		for j in xrange(len(MBR_list)):
			try:
				# if rel[i][j]['north']:
				# 	rel[j][i]['south'] = False
				# if rel[i][j]['east']:
				# 	rel[j][i]['west'] = False

				if rel[i][j]['north']:
					for k in xrange(len(MBR_list)):
						# print i,j,k, rel[k][j]['north'] , rel[i][k]['north']
						# if i==26 and j==0:
						# 	print rel[k][j]['north'] , rel[i][k]['north']
						if rel[k][j]['north'] and rel[i][k]['north']:
							rel[i][j]['north'] = False
							break
				if rel[i][j]['south']:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['south'] and rel[i][k]['south']:
							rel[i][j]['south'] = False
							break
				if rel[i][j]['east']:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['east'] and rel[i][k]['east']:
							rel[i][j]['east'] = False
							break
				if rel[i][j]['west']:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['west'] and rel[i][k]['west']:
							rel[i][j]['west'] = False	
							break
				# if rel[i][j]['near']:
				# 	for k in xrange(len(MBR_list)):
				# 		if rel[k][j]['near'] and rel[i][k]['near']:
				# 			rel[i][j]['near'] = False	
				# 			break
			except KeyError:
				pass


	c=0
	for i in xrange(27):
		for j in xrange(27):
			try:
				for (k, v) in rel[i][j].iteritems():
					if v:
						c = c + 1

			except KeyError:
				pass
	print c
	# printable(rel,labels)
	# print 'here ======',area_list[0],area_list[3], Near(1,4), Near(2,5), Near(4,7), Near(1,2), Near(2,3), Near(3,6), Near(1,6)
	return rel,MBR_list,COM_list,labels, dsc


def printable(rel,labels):
	for index in xrange(len(rel)):
		print "Building #%d: %s" %(index+1,labels[str(index+1)].replace('\r\n',''))
		print "Description: ", 
		for j in xrange(len(rel)):
   			try:
   				if rel[index][j]['north']:
   					print 'North of %s is %s , ' %(labels[str(index+1)].replace('\r\n',''),labels[str(j+1)].replace('\r\n','')),
   				if rel[index][j]['south']:
   					print 'South of %s is %s , ' %(labels[str(index+1)].replace('\r\n',''),labels[str(j+1)].replace('\r\n','')),
   				if rel[index][j]['east']:
   					print 'East of %s is %s , ' %(labels[str(index+1)].replace('\r\n',''),labels[str(j+1)].replace('\r\n','')),
   				if rel[index][j]['west']:
   					print 'West of %s is %s , ' %(labels[str(index+1)].replace('\r\n',''),labels[str(j+1)].replace('\r\n','')),
   				if rel[index][j]['near']:
   					print 'Near %s is %s , ' %(labels[str(index+1)].replace('\r\n',''),labels[str(j+1)].replace('\r\n','')),
   			except KeyError:
   				pass
   		print "\n"

def North(S, T):
	# return true if S is north of T
	if type(S) is int:
		xs = MBR_list[S-1][0][0][0]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][1][0]
	else:
		xt = T[0]
	if xs < xt:
		return False
	else:
		return True

def South(S, T):
	# return true if S is South of T
	if type(S) is int:
		xs = MBR_list[S-1][0][1][0]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][0][0]
	else:
		xt = T[0]
	if xs > xt:
		return False
	else:
		return True	

def East(S, T):
	# return true if S is east of T
	if type(S) is int:
		xs = MBR_list[S-1][0][1][1]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][0][1]
	else:
		xt = T[0]
	if xs > xt:
		return False
	else:
		return True

def West(S, T):
	# return true if S is west of T
	if type(S) is int:
		xs = MBR_list[S-1][0][0][1]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][1][1]
	else:
		xt = T[0]
	if xs < xt:
		return False
	else:
		return True

def Near(S, T):
	# return true if S is near T
	# Todo, add size to be more accurate
	# small size get stricter threshold while larger size get lower 
	if type(S) is int:
		xs = COM_list[S-1][0][0]
		ys = COM_list[S-1][0][1]
		size_s = area_list[S-1]
	else:
		xs = S[0]
		ys = S[1]
		size_s = 0
	if type(T) is int:
		xt = COM_list[T-1][0][0]
		yt = COM_list[T-1][0][1]
		size_t = area_list[T-1]
	else:
		xt = T[0]
		yt = T[1]
		size_t = 0

	dis = math.sqrt((xs-xt)**2 + (ys-yt)**2)
 
	if size_s>4000 or size_t>4000:
		thresh = 120
	elif size_s>2000 or size_t>2000:
		thresh = 80
	else:
		thresh=50

	if dis<thresh and S != T:
		return True
	else:
		return False

if __name__=='__main__':
	main()