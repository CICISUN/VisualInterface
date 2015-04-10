import numpy as np
import cv2
import sys,os
import Tkinter
from Tkinter import *
from PIL import Image, ImageTk
from ttk import Frame, Style
from spatial import East,North,Near,step2

 
def find_pc_dfs(x,y,pc,mark,oldrel):
	# implement floodfill algorithm
	trel = np.zeros((27, 3), bool)
	#left
	if x-1 > 0 and (x-1, y) not in mark:
		for i in xrange(27):
			trel[i] = [North(i, [x-1, y]), East(i, [x-1, y]), Near(i, [x-1, y])]
		mark.append((x-1,y))
		if np.all(trel == oldrel):
			pc.append((x-1,y))
			pc = find_pc_dfs(x-1,y,pc,mark,trel)

	#right
	if x+1 < image0.shape[1] and (x+1, y) not in mark:
		for i in xrange(27):
			trel[i] =  [North(i, [x+1, y]), East(i, [x+1, y]), Near(i, [x+1, y])]
		mark.append((x+1,y))
		if np.all(trel == oldrel):
			pc.append((x+1,y))
			pc = find_pc_dfs(x+1,y,pc,mark,trel)
	#up
	if y-1 > 0 and (x, y-1) not in mark:
		for i in xrange(27):
			trel[i] =  [North(i, [x, y-1]), East(i, [x, y-1]), Near(i, [x, y-1])]
		mark.append((x, y-1))
		if np.all(trel == oldrel):
			pc.append((x, y-1))
			pc = find_pc_dfs(x,y-1,pc,mark,trel)
	#down
	if y+1 < image0.shape[0] and (x,y+1) not in mark:
		for i in xrange(27):
			trel[i] =  [North(i, [x,y+1]), East(i, [x,y+1]), Near(i, [x,y+1])]
		mark.append((x,y+1))
		if np.all(trel == oldrel):
			pc.append((x,y+1))
			pc = find_pc_dfs(x,y+1,pc,mark,trel)

	return pc

def get_des(x, y):
	# print out description for point/building
	global count
	if count == 0:
		print 'Loading point cloud for starting point'
	else:
		print 'Loading point cloud for target point'
	bnum=0
	try:
		bnum = image0[y][x]
	except IndexError:
		print 'Click again!'

	if bnum==0:
		return 0

	if bnum != 0:
		bnum = bnum - 1
		for i in xrange(27):
			for k,v in rel[i][bnum].iteritems():
				if v:
					print k, ' of ', labels[str(i+1)] ,
	return bnum

def calc_adj(rel):
	res = {}
	for i in xrange(len(rel)):
		tmp_v=[]
		for j in xrange(len(rel)):
			if rel[i][j]['east'] or rel[i][j]['west'] or rel[i][j]['north'] or rel[i][j]['south'] and rel[i][j]['near'] \
			or rel[j][i]['east'] or rel[j][i]['west'] or rel[j][i]['north'] or rel[j][i]['south'] and rel[j][i]['near']:
				tmp_v.append(j)
		res[i] = set(tmp_v)
	print res

	return res


def find_nearest(type):
	mindis = 100000000000
	nnum = 0
	for bnum in COM_list:
		if type==1:
			tmp = (bnum[0][0]-coor[0][1])**2 + (bnum[0][1]-coor[0][0])**2
		elif type==2:
			tmp = (bnum[0][0]-coor[1][1])**2 + (bnum[0][1]-coor[1][0])**2
		if tmp < mindis:
			mindis=tmp
			nnum=bnum[1]
	return nnum

def reset_callback():
	global coor
	global image
	del coor[:]
	image = Image.open("campus.ppm")
	img = ImageTk.PhotoImage(image)
	obj_img = Tkinter.Label(w, image=img)
	obj_img.place(x=0, y=0, width=image.size[0], height=image.size[1])
	w.mainloop()

# http://eddmann.com/posts/depth-first-search-and-breadth-first-search-in-python/
def find_path_bfs(start, end):
	global adj
	queue = [(start, [start])]
	while queue:
		(vertex, path) = queue.pop(0)
		for next in adj[vertex] - set(path):
			if next == end:
				yield path + [next]
			else:
				queue.append((next, path + [next]))

def shortest_path(start, end):
	try:
		return next(find_path_bfs(start, end))
	except StopIteration:
		return None

def btn_callback():
	bnum = find_nearest(1)
	print 'You are nearest to', labels[str(bnum)].replace('\r\n','')
	start = bnum-1
	tnum = find_nearest(2)
	print 'You are going to', labels[str(tnum)].replace('\r\n','')
	end = tnum-1
	path =  shortest_path(start, end)
	print path
	if image0[coor[0][1]][coor[0][0]] != 0:
		start_from_b = True
	if image0[coor[1][1]][coor[1][0]] != 0:
		end_in_b = True

	printable(path, start_from_b, end_in_b)

def printable(path, start_from_b, end_in_b):
	global rel
	if start_from_b:
		start = path.pop(0)
	print 'Go ',
	for bnum in path:
		for k,v in rel[start][bnum].iteritems():
			if v:
				print k, ' and '



def onCanvasClick(event):
	print 'Got click', event.x, event.y, event.widget
	global count
	global image
	coor.append((event.x,event.y))

	if count > 1:
		count = 0
		color = (255,0,0)
		print 'please reset!'
	elif count == 1:
		txt = get_des(event.x,event.y)
		color = (255,0,0)	
		count = count + 1
	elif count == 0:
		txt = get_des(event.x,event.y)
		color = (0,255,0)
		count = count + 1

	cur_rel = np.zeros((27, 3), bool)
	x = round(event.x)
	y = round(event.y)

	for i in range(27):
		cur_rel[i] = [North(i, [x, y]), East(i, [x, y]), Near(i, [x, y])]
	
	sys.setrecursionlimit(10000)
	point_cloud = find_pc_dfs(x,y,[(x,y)],[(x,y)],cur_rel)
	# change display
	for pix in point_cloud:
		# round to int and convert to int    
		xx = int(pix[0]) 
		yy = int(pix[1])
		tmp =  tuple(map(int, [xx,yy]))
		image.putpixel(tmp, color)
	

	img = ImageTk.PhotoImage(image)
	obj_img = Tkinter.Label(w, image=img)
	obj_img.place(x=0, y=0, width=image.size[0], height=image.size[1])
	w.mainloop()

# def onObjectClick(event):
# 	print 'Got object click', event.x, event.y, event.widget,
# 	print event.widget.find_closest(event.x, event.y) 

def step3():
	global coor
	global rel
	global MBR_list
	global COM_list
	global image0
	global labels
	global count
	global image
	global adj
	global w
	count = 0
	coor = []
	filename='ass3-labeled.pgm'
	image0 = cv2.imread(filename,-1)
	
	rel,MBR_list,COM_list,labels = step2(image0)
	adj = calc_adj(rel)


	w = Tk()
	image = Image.open("campus.ppm")
	w.geometry('%dx%d' %(image.size[0] * 2,image.size[1]))

	# canvas = Canvas(w, width=image.size[0]*2, height=image.size[1])

	img = ImageTk.PhotoImage(image)
	obj_img = Tkinter.Label(w, image=img)
	obj_img.place(x=0, y=0, width=image.size[0], height=image.size[1])
	# obj_img.grid(row=0, column=0)
	btn = Tkinter.Button(w,text='Tell me!',command = btn_callback)
	reset = Tkinter.Button(w,text='Reset!',command = reset_callback)
	# btn.grid(row=0, column=1)
	btn.place(x=image.size[0]+50, y=image.size[1]-50)
	reset.place(x=image.size[0]+150, y=image.size[1]-50)
	# obj_img = canvas.create_image(0,0, anchor=NW, image=img)
	# obj_text = canvas.create_text(image.size[0]+3, 10, anchor=W, font="Purisa", text="Welcome to CU Map")

	w.bind('<Double-1>', onCanvasClick)
	# canvas.tag_bind(obj_img, '<Double-1>', onObjectClick)
	# canvas.pack()
	w.mainloop()

def main():
	step3()


if __name__=='__main__':
	main()