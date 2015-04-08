import numpy as np
import cv2
from infrastructure import step1
 

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
	MBR_list, COM_list, labels = step1(image)
	height = image.shape[0]
	width = image.shape[1]
    # Generate all binary spatial relationships for every pair
	rel = [[{} for i in range(len(MBR_list))] for j in range(len(MBR_list))]
	for i in xrange(len(MBR_list)):
		for j in xrange(len(MBR_list)):
			if i != j:
				rel[i][j]['north'] = North(i, j)
				rel[i][j]['south'] = South(i, j)
				rel[i][j]['east'] = East(i, j)
				rel[i][j]['west'] = West(i, j)
				rel[i][j]['near'] = Near(i, j)
    # transitive reduction 
    # to do: near
	for i in xrange(len(MBR_list)):
		for j in xrange(len(MBR_list)):
			try:
				if rel[i][j]['north'] and rel[j][i]['south']:
					rel[j][i]['south'] = False
				if rel[i][j]['west'] and rel[j][i]['east']:
					rel[j][i]['east'] = False
				if rel[i][j]['north'] is True:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['north'] and rel[i][k]['north']:
							rel[i][j]['north'] = False
				if rel[i][j]['south'] is True:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['south'] and rel[i][k]['south']:
							rel[i][j]['south'] = False
				if rel[i][j]['east'] is True:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['east'] and rel[i][k]['east']:
							rel[i][j]['east'] = False
				if rel[i][j]['west'] is True:
					for k in xrange(len(MBR_list)):
						if rel[k][j]['west'] and rel[i][k]['west']:
							rel[i][j]['west'] = False	
			except KeyError:
				pass

	printable(rel,labels)

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
   			except KeyError:
   				pass
   		print "\n"

def North(S, T):
	# return true if S is north of T
	if type(S) is int:
		xs = MBR_list[S-1][0][1][0]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][0][0]
	else:
		xt = T[0]
	if xs < xt:
		return False
	else:
		return True

def South(S, T):
	# return true if S is South of T
	if type(S) is int:
		xs = MBR_list[S-1][0][0][0]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][1][0]
	else:
		xt = T[0]
	if xs > xt:
		return False
	else:
		return True	


def East(S, T):
	# return true if S is east of T
	if type(S) is int:
		xs = MBR_list[S-1][0][0][1]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][1][1]
	else:
		xt = T[0]
	if xs > xt:
		return False
	else:
		return True

def West(S, T):
	# return true if S is west of T
	if type(S) is int:
		xs = MBR_list[S-1][0][1][1]
	else:
		xs = S[0]
	if type(T) is int:
		xt = MBR_list[T-1][0][0][1]
	else:
		xt = T[0]
	if xs < xt:
		return False
	else:
		return True

def Near(S, T):
	# return true if S is near T
	if type(S) is int:
		xs = COM_list[S-1][0][0]
		ys = COM_list[S-1][0][1]
	else:
		xs = S[0]
		ys = S[1]
	if type(T) is int:
		xt = COM_list[T-1][0][0]
		yt = COM_list[T-1][0][1]
	else:
		xt = T[0]
		yt = T[1]

	if abs(xs - xt) < height / 4 and abs(ys - yt) < width / 3 :
		return True
	else:
		return False

if __name__=='__main__':
	main()