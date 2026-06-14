import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Number of points
N = 200

# Irrational numbers for the spherical sequence
rho = 1.324717957244746
alpha = 1 / rho
beta = 1 / rho**2

# Set up figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim([-1,1])
ax.set_ylim([-1,1])
ax.set_zlim([0,1])  # hemisphere: z >= 0
ax.set_box_aspect([1,1,1])

# Plot hemisphere wireframe
u = np.linspace(0, 2*np.pi, 50)
v = np.linspace(0, np.pi/2, 25)  # only upper half
xs = np.outer(np.cos(u), np.sin(v))
ys = np.outer(np.sin(u), np.sin(v))
zs = np.outer(np.ones_like(u), np.cos(v))
ax.plot_wireframe(xs, ys, zs, color='lightgray', alpha=0.3)

# Arrays to store points
x_points = []
y_points = []
z_points = []

# Generate points one by one
for n in range(N):
    u_n = (n * alpha) % 1
    v_n = (n * beta) % 1

    # Map to hemisphere using equal-area projection
    z = u_n  # z in [0,1] for upper hemisphere
    phi = 2 * np.pi * v_n
    r = np.sqrt(1 - z**2)

    x = r * np.cos(phi)
    y = r * np.sin(phi)

    x_points.append(x)
    y_points.append(y)
    z_points.append(z)

    ax.scatter(x_points, y_points, z_points, color='blue', s=20)

    plt.pause(0.05)

plt.show()
