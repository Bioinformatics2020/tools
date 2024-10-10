import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import sys
import threading
import time

def generate_noisy_line(direction_vector, num_points, noise_std):
    """
    Generate points along a line in 3D space and add random noise to them.
    
    Parameters:
    direction_vector (array-like): The direction vector of the line.
    num_points (int): Number of points to generate along the line.
    noise_std (float): Standard deviation of the noise to be added to each point.
    
    Returns:
    numpy.ndarray: An array of shape (num_points, 3) with noisy points.
    """
    # Normalize the direction vector
    normalized_direction = direction_vector / np.linalg.norm(direction_vector)
    
    # 在0-10之间生成随机点，注意随机点的平均数不能接近0
    distances = np.linspace(0, 10, num_points)
    
    # Calculate points on the line without noise
    line_points = np.outer(distances, normalized_direction)
    
    # 设定随机数种子以便结果可重复
    np.random.seed(0)

    # Add random noise to the points
    noise = np.random.normal(0, noise_std, line_points.shape)
    noisy_points = line_points + noise
    
    return noisy_points


def project_error(points, line):
    """
    计算平均每一个点到线距离的平方和，作为损失函数
    
    Parameters:
    points (numpy.ndarray): Array of shape (n, 3) representing n points in 3D space.
    line (numpy.ndarray): Direction vector of the line of shape (3,).
    
    Returns:
    numpy.ndarray: Array of squared distances from each point to the line.
    """
    # 方向向量
    line_unit = line / np.linalg.norm(line)

    # 投影长度 t
    t = np.dot(points, line_unit)
    # 直线上的最近点 Q
    closest_point_on_line = np.outer(t, line_unit)
    
    # L2范数
    dist = np.linalg.norm(closest_point_on_line - points)

    # 均方误差
    sum_of_squares = np.mean(dist)
    
    return sum_of_squares


# 准确的高维直线
direction_vector = np.array([1.1, 2, -3, 4, 5, 6])
# 生成带噪声的直线上的点
points_with_noise = generate_noisy_line(direction_vector, 1000, 0.1)

min_mse = project_error(points_with_noise,direction_vector)
print(min_mse)

global quit, iterations, delta


quit = False
iterations = 0
delta = np.full(direction_vector.shape, 0.1)
min_mse =  np.full(direction_vector.shape, sys.float_info.max/direction_vector.shape[0])

# 预测的直线
prediction_direction = np.full(direction_vector.shape, 0.0)

# 更新预测值
def update_predictions():
    global quit, iterations, delta

    delta_mse = 0.0
    for i,value  in enumerate(prediction_direction):
        new_direction = prediction_direction[i] + delta[i]
        mse = np.mean((points_with_noise[:,i] - new_direction) ** 2)
        
        delta_mse += abs(mse - min_mse[i])

        if mse > min_mse[i]:
            delta[i] = delta[i] * (-0.8)
        else:
            min_mse[i] = mse
            prediction_direction[i] = new_direction

    if delta_mse < 1e-16:
        quit = True
    iterations += 1


def run_predictions():
    global quit
    while not quit:
        update_predictions()

def run_log():
    global quit, iterations
    while not quit:
        time.sleep(0.1)
        print(prediction_direction,project_error(points_with_noise,prediction_direction))
    print(iterations,prediction_direction,project_error(points_with_noise,prediction_direction),min_mse)



# 异步高频率执行任务
thread = threading.Thread(target=run_predictions)
thread.start()
# 异步低频率输出log
log_thread = threading.Thread(target=run_log)
log_thread.start()

# ------图形可视化
if 0:
    # 生成直线
    points_with_line = generate_noisy_line(prediction_direction, 10, 0)
    
    # 可视化
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # 绘制带有噪声的点
    ax.scatter(points_with_noise[:, 0], points_with_noise[:, 1], points_with_noise[:, 2], s=2, color='blue')

    # 在3D空间中绘制直线
    ax.plot(points_with_line[:, 0], points_with_line[:, 1], points_with_line[:, 2], linewidth=2, color='red')

    # 设置轴标签
    ax.set_xlabel('X axis')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')

    # 添加图例
    ax.legend()

    plt.show()