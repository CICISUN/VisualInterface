import numpy as np
import cv2
import sys,os
import Tkinter
from Tkinter import *
from PIL import Image, ImageTk
from ttk import Frame, Style
from spatial import East,North,Near,step2
from infrastructure import step1
 
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
 
	if color ==(0,255,0):
		txt = 'Loading point cloud for starting point\n'
	else:
		txt = 'Loading point cloud for target point\n'
	bnum=0
	try:
		bnum = image0[y][x]
	except IndexError:
		print 'Click again!'

	if bnum==0:
		for i in range(27):
			if Near(i, [x, y]):
				txt = txt + 'Near ' + labels[str(i+1)].replace('\r\n','') + '\n'
			if Near(i, [x, y]) and East(i, [x, y]):
				txt = txt + 'East of ' + labels[str(i+1)].replace('\r\n','') + '\n'
			if Near(i, [x, y]) and North(i, [x, y]):
				txt = txt + 'North of ' + labels[str(i+1)].replace('\r\n','') + '\n'

	if bnum != 0:
		bnum = bnum - 1
		for i in xrange(27):
			for k,v in rel[i][bnum].iteritems():
				if v:
					if k == 'near':
						txt = txt+ str(k) + ' ' + labels[str(i+1)].replace('\r\n','') + '\n'
					else:
						txt = txt+ str(k) + ' of ' + labels[str(i+1)].replace('\r\n','') + '\n'
				
	return txt

def calc_adj(rel):
	res = {}
	for i in xrange(len(rel)):
		tmp_v=[]
		for j in xrange(len(rel)):
			if rel[i][j]['near'] and rel[j][i]['near'] and (rel[i][j]['east'] or rel[i][j]['west'] or rel[i][j]['north'] or rel[i][j]['south'] or rel[j][i]['east'] or rel[j][i]['west'] or rel[j][i]['north'] or rel[j][i]['south']):
				tmp_v.append(j)
		res[i] = set(tmp_v)
	# print res

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
	text = Text(w, height=image.size[1], width=image.size[0]+50)
	text.place(x=image.size[0]+10, y=0, width=image.size[0]-15, height=image.size[1]-100)
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
	txt = 'You are nearest to ' + labels[str(bnum)].replace('\r\n','')

	start = bnum-1
	tnum = find_nearest(2)
	txt = txt + '\n' + 'You are going to ' + labels[str(tnum)].replace('\r\n','')
	end = tnum-1
	path =  shortest_path(start, end)

	txt = txt + '\n' 

	if image0[coor[0][1]][coor[0][0]] != 0:
		start_from_b = True
	else:
		start_from_b = False

	if image0[coor[1][1]][coor[1][0]] != 0:
		end_in_b = True
	else:
		end_in_b = False

	txt = txt +'\n'+ printable(path, start_from_b, end_in_b)

	text = Text(w, height=image.size[1], width=image.size[0]+50)
	text.place(x=image.size[0]+10, y=0, width=image.size[0]-15, height=image.size[1]-100)
	text.insert(END, txt)
	w.mainloop()

def printable(path, start_from_b, end_in_b):
	global rel
	# print rel
	# if start_from_b:
	# 	start = path.pop(0)
	txt = ''
	for i in xrange(len(path)-1):
		if i==0:
			# txt = txt + 'Go to the building (' +str(labels[str(path[i+1]+1)].replace('\r\n',''))+') that is '
			txt = txt + 'Go to the building that is '
		else:
			# txt = txt + 'Then go to the building (' +str(labels[str(path[i+1]+1)].replace('\r\n',''))+') that is '
			txt = txt + 'Then go to the building that is '

		for k,v in rel[path[i]][path[i+1]].iteritems():
			if v:
				if rel[path[i]][path[i+1]].keys().index(k) == len(rel[path[i]][path[i+1]])-1:
					txt = txt + str(k) 
				else:
					txt = txt + str(k) + ' and '

		txt = txt + '(which is '
		for k,v in dsc[path[i+1]].iteritems():
			if k=='size':
				txt = txt + str(v) + ' sized, '
			if k=='shape':
				txt = txt + str(v) + ', '
			if k=='orientation':
				txt = txt + str(v) + ', '
			if k=='extremum':
				for e in v:
					txt = txt + str(e) + ', '
		txt = txt + ' )\n'

	return txt

def btns_callback():
	global color
	color = (0,255,0)
	print 'Click on where you are now'
def btnt_callback():
	global color
	color = (255,0,0)
	print 'Click on where you are goint'	


def onCanvasClick(event):
	print 'Got click', event.x, event.y, event.widget
	global image
	coor.append((event.x,event.y))

	cur_rel = np.zeros((27, 3), bool)
	x = round(event.x)
	y = round(event.y)

	for i in range(27):
		cur_rel[i] = [North(i, [x, y]), East(i, [x, y]), Near(i, [x, y])]

	txt = get_des(x,y)
	text = Text(w, height=image.size[1], width=image.size[0]+50)
	text.insert(END, txt)
	text.place(x=image.size[0]+10, y=0, width=image.size[0]-15, height=image.size[1]-100)

	
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
	global color
	global image
	global adj
	global w
	global dsc

	count = 0
	coor = []
	filename='ass3-labeled.pgm'
	image0 = cv2.imread(filename,-1)
	
	rel,MBR_list,COM_list,labels, dsc = step2(image0)

	 

	adj = calc_adj(rel)


	w = Tk()
	image = Image.open("campus.ppm")
	w.geometry('%dx%d' %(image.size[0] * 2 +50,image.size[1]))

	# canvas = Canvas(w, width=image.size[0]*2, height=image.size[1])

	img = ImageTk.PhotoImage(image)
	obj_img = Tkinter.Label(w, image=img)
	obj_img.place(x=0, y=0, width=image.size[0], height=image.size[1])
	# obj_img.grid(row=0, column=0)
	btn = Tkinter.Button(w,text='Tell me!',command = btn_callback)
	reset = Tkinter.Button(w,text='Reset!',command = reset_callback)
	btn_s = Tkinter.Button(w,text='Set start!',command = btns_callback)
	btn_t = Tkinter.Button(w,text='Set target!',command = btnt_callback)
	# btn.grid(row=0, column=1)
	text = Text(w, height=image.size[1], width=image.size[0])
	# scroll = Scrollbar(w, command=text.yview)

	text.insert(END,'Welcome to CU Map!\n')
	usage = 'usage: click choose a start Button and click on the map,\nclick choose a target Button and click on the map\nclick choose a tell me Button and see the results below\n'
	text.insert(END, usage)
	# text.pack(side=RIGHT)
	# scroll.pack(side=RIGHT, fill=Y)
	text.place(x=image.size[0]+10, y=0, width=image.size[0]-15, height=image.size[1]-100)
	btn.place(x=image.size[0]+50, y=image.size[1]-50)
	btn_s.place(x=image.size[0]+50, y=image.size[1]-100)
	reset.place(x=image.size[0]+150, y=image.size[1]-50)
	btn_t.place(x=image.size[0]+150, y=image.size[1]-100)
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