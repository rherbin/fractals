import pygame as pg
import sys
import numpy as np
from math import *

winsize = 800
display = pg.display.set_mode((winsize,winsize))
clock = pg.time.Clock()
win = np.array([float(winsize/2),float(winsize/2)])

display.fill((0,0,0))

iter = 50

scale = 1

def mandel():
    for x in range(800):
        for y in range(800):
            c = complex((x-600)/(300*scale),(y-400)/(300*scale))
            i = 0
            z = 0
            while (i<iter and abs(z) < 2):
                #z = complex(abs(z.real),abs(z.imag))
                z = z*z + c
                i+=1
            pg.draw.rect(display, (255*i/iter,0,0), pg.Rect(x,y,1,1))

mandel()

while True:

    for event in pg.event.get():
        if event.type == pg.QUIT:
            pg.quit()
            sys.exit()

    keys = pg.key.get_pressed()

    if keys[pg.K_UP]:
        scale *= 1.5
        display.fill((0,0,0))
        mandel()
    if keys[pg.K_DOWN]:
        scale /= 1.5
        display.fill((0,0,0))
        mandel()

    clock.tick(60)
    pg.display.update()