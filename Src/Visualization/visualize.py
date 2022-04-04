#!/usr/bin/env python
# coding: utf-8

from bs4 import BeautifulSoup
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import matplotlib.animation as animation
import numpy as np
import bisect
import sys


class Map:
    def __init__(self, xml_file):
        with open(xml_file, 'r') as xml_file:
            soup = BeautifulSoup(xml_file, "lxml")

        self.width = int(soup.width.text)
        self.height = int(soup.height.text)
        self.start = [int(soup.startx.text) - 1, int(soup.starty.text) - 1]
        self.finish = [int(soup.finishx.text) - 1, int(soup.finishy.text) - 1]
        self.grid = [
            [int(cell) for cell in row.text.split(' ')]
            for row in soup.find_all('row')
        ]

        self.obstacles = [
            [
                [int(point['x']) - 1, int(point['y']) - 1, float(point['time'])]
                for point in obstacle.find_all('point')
            ]
            for obstacle in soup.find_all('obstacle')
        ]
        
        self.path = [
            [int(point['x']) - 1, int(point['y']) - 1, float(point['time'])]
            for point in soup.find('path').find_all('point')
        ]
        self.path_time = self.path[-1][-1]


    def getPosition(self, path, time):
        point_i = bisect.bisect_left(path, time, key=lambda point: point[2])

        if point_i == len(path):
            return path[-1][:2]

        point = np.array(path[point_i])
        if point[2] == time:
            return point[0:2]
        else:
            prev_point = np.array(path[point_i - 1])
            k = (time - prev_point[2]) / (point[2] - prev_point[2])
            return (prev_point + k * (point - prev_point))[:2]



if len(sys.argv) < 2:
    print("Error: input file is not specified")
    sys.exit()

input_file = sys.argv[1]
map = Map(input_file)

"""
Creation of animation
"""
fig, ax = plt.subplots(figsize=(10, 5))
plt.rcParams['animation.ffmpeg_path'] = '/usr/local/bin/ffmpeg'
cmap = mcolors.ListedColormap(['white', 'black'])
ax.imshow(map.grid, cmap)

# Grid lines
ax.tick_params('both', top=True, bottom=False, labeltop=True, labelbottom=False)
ax.set_xticks(np.arange(0, map.width, 2))
ax.set_yticks(np.arange(0, map.height, 2))
ax.set_xticks(np.arange(-0.5, map.width, 1), minor=True)
ax.set_yticks(np.arange(-0.5, map.height, 1), minor=True)
ax.grid(which='minor', color='black', linewidth=1, linestyle='--')

# Start positions of obstacles and agent
start_x = [map.obstacles[i][0][0] for i in range(len(map.obstacles))]
start_y = [map.obstacles[i][0][1] for i in range(len(map.obstacles))]
points, = ax.plot(start_x, start_y, marker="o", linestyle='')
agent, = ax.plot(map.start[0], map.start[1], marker="o", color='red', linestyle=':')
ax.plot(map.start[0], map.start[1], marker="$S$", color='red')
ax.plot(map.finish[0], map.finish[1], marker="$F$", color='red')

# Update positions of obstacles and agents
def update(time):
    x, y = [0] * len(map.obstacles), [0] * len(map.obstacles)
    for i in range(len(map.obstacles)):
        x[i], y[i] = map.getPosition(map.obstacles[i], time)
    points.set_data((x, y))

    agent.set_data(map.getPosition(map.path, time))
    return points, agent, 
    
anim = animation.FuncAnimation(fig, update, interval=5, blit=True, repeat=True,
                               frames=np.linspace(0, map.path_time + 2, int(map.path_time) * 30))

print("Animation is created, ready to save")

plt.show()

# writermp4 = animation.writers['ffmpeg'](fps=60)
# anim.save(input_file[:-4] + '.mp4', writermp4)
