import numpy as np
import matplotlib.pyplot as plt

# Number of points
N = 500

# Golden angle in radians
golden_angle = np.pi * (3 - np.sqrt(5))

# Set up figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.set_xlim([-1, 1])
ax.set_ylim([-1, 1])
ax.set_zlim([0, 1])
ax.set_box_aspect([1, 1, 1])

# Hemisphere wireframe
u = np.linspace(0, 2*np.pi, 50)
v = np.linspace(0, np.pi/2, 25)

xs = np.outer(np.cos(u), np.sin(v))
ys = np.outer(np.sin(u), np.sin(v))
zs = np.outer(np.ones_like(u), np.cos(v))

ax.plot_wireframe(xs, ys, zs,
                  color='lightgray',
                  alpha=0.25)

x_points = []
y_points = []
z_points = []

for n in range(N):

    # Equal-area progression from pole to equator
    z = 1 - (n + 0.5) / N

    # Golden-angle rotation
    phi = n * golden_angle

    r = np.sqrt(1 - z*z)

    x = r * np.cos(phi)
    y = r * np.sin(phi)

    x_points.append(x)
    y_points.append(y)
    z_points.append(z)

    ax.cla()

    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([0, 1])
    ax.set_box_aspect([1, 1, 1])

    ax.plot_wireframe(xs, ys, zs,
                      color='lightgray',
                      alpha=0.25)

    ax.scatter(
        x_points,
        y_points,
        z_points,
        c=np.arange(len(x_points)),
        cmap='viridis',
        s=20
    )

    plt.pause(0.01)

plt.show()
