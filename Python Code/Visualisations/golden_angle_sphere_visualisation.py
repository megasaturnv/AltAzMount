import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import time

# Number of points to plot
#N = 200
N = 50

# Irrational numbers for generating the spherical sequence
# Using the plastic constant as 3D golden ratio analogue
rho = 1.324717957244746
alpha = 1 / rho
beta = 1 / rho**2

# Prepare figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim([-1,1])
ax.set_ylim([-1,1])
ax.set_zlim([-1,1])
ax.set_box_aspect([1,1,1])

# Plot the unit sphere as a wireframe
u = np.linspace(0, 2*np.pi, 50)
v = np.linspace(0, np.pi, 50)
xs = np.outer(np.cos(u), np.sin(v))
ys = np.outer(np.sin(u), np.sin(v))
zs = np.outer(np.ones_like(u), np.cos(v))
ax.plot_wireframe(xs, ys, zs, color='lightgray', alpha=0.3)

# Arrays to store points
x_points = []
y_points = []
z_points = []

# Generate points one by one and plot in real time
for n in range(N):
    u_n = (n * alpha) % 1   # fractional part
    v_n = (n * beta) % 1

    # Map to sphere using equal-area projection
    z = 1 - 2*u_n             # z coordinate [-1,1]
    phi = 2 * np.pi * v_n     # azimuth angle [0,2pi]
    r = np.sqrt(1 - z**2)     # radius at that z

    x = r * np.cos(phi)
    y = r * np.sin(phi)

    x_points.append(x)
    y_points.append(y)
    z_points.append(z)

    ax.scatter(x_points, y_points, z_points, color='blue', s=20)

    plt.pause(0.05)  # small pause to see animation

plt.show()
