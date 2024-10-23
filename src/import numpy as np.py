import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Constants (adjust to match your gripper's actual dimensions)
RADIUS = 20.0         # Crank length (mm)
LINK_LENGTH = 50.0    # Connecting rod length (mm)
PI = np.pi

# Function to calculate the crank angle from closure percentage
def get_angle_from_percentage(percentage):
    ratio = percentage / 100.0
    x_max = RADIUS + LINK_LENGTH      # Slider position at θ = 0°
    x_min = np.sqrt(LINK_LENGTH**2 - RADIUS**2)  # Slider position at θ = 180°
    delta_x = x_max - x_min
    x = x_max - ratio * delta_x

    # Calculate the crank angle θ for a given slider position x
    cos_theta = (x**2 + RADIUS**2 - LINK_LENGTH**2) / (2 * x * RADIUS)
    cos_theta = np.clip(cos_theta, -1.0, 1.0)
    theta = np.arccos(cos_theta)
    angle_deg = np.degrees(theta)
    return angle_deg

# Function to calculate positions of mechanism components
def calculate_positions(angle_deg):
    theta = np.radians(angle_deg)
    # Crank end point (rotating arm)
    crank_end = np.array([RADIUS * np.cos(theta), RADIUS * np.sin(theta)])
    # Slider position along x-axis (gripper position)
    x = crank_end[0] + np.sqrt(LINK_LENGTH**2 - crank_end[1]**2)
    slider_pos = np.array([x, 0])
    # Connecting rod positions
    connecting_rod_x = [crank_end[0], slider_pos[0]]
    connecting_rod_y = [crank_end[1], slider_pos[1]]
    return crank_end, slider_pos, connecting_rod_x, connecting_rod_y

# Generate data for animation
percentages = np.linspace(0, 100, 100)  # Closure percentages from 0% to 100%
angles = [get_angle_from_percentage(p) for p in percentages]

# Setup the plot
fig, ax = plt.subplots()
ax.set_xlim(-RADIUS - 10, RADIUS + LINK_LENGTH + 50)
ax.set_ylim(-RADIUS - 10, RADIUS + 50)
ax.set_aspect('equal')
ax.grid(True)
ax.set_title('Crank-Slider Gripper Mechanism Visualization')
ax.set_xlabel('X (mm)')
ax.set_ylabel('Y (mm)')

# Plot elements
crank_line, = ax.plot([], [], 'b-', linewidth=2, label='Crank (Motor Arm)')
connecting_rod_line, = ax.plot([], [], 'r-', linewidth=2, label='Connecting Rod')
slider_dot, = ax.plot([], [], 'go', markersize=8, label='Gripper (Slider)')
angle_text = ax.text(0.05, 0.95, '', transform=ax.transAxes, fontsize=12, verticalalignment='top')

# Fixed points
motor_pivot = np.array([0, 0])
ax.plot(motor_pivot[0], motor_pivot[1], 'ko', markersize=5, label='Motor Pivot')

# Animation update function
def update(frame):
    angle_deg = angles[frame]
    crank_end, slider_pos, connecting_rod_x, connecting_rod_y = calculate_positions(angle_deg)

    # Update crank line
    crank_line.set_data([motor_pivot[0], crank_end[0]], [motor_pivot[1], crank_end[1]])

    # Update connecting rod line
    connecting_rod_line.set_data(connecting_rod_x, connecting_rod_y)

    # Update slider position
    slider_dot.set_data(slider_pos[0], slider_pos[1])

    # Update angle text
    angle_text.set_text(f'Closure: {percentages[frame]:.1f}%\nCrank Angle: {angle_deg:.1f}°')

    return crank_line, connecting_rod_line, slider_dot, angle_text

# Create animation
ani = FuncAnimation(fig, update, frames=len(percentages), blit=True, interval=50)

# Add legend
ax.legend()

# Show the plot
plt.show()
