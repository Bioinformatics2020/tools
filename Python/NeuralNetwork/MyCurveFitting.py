import numpy as np
import sys
from functools import partial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
import time

'''
迭代多项式每一项权重，预测多项式图形

每一项均匀迭代，前一项迭代完成之后迭代第二项
'''

def polynomial(x, coefficients):
    """
    计算给定x值的多项式值。

    参数:
    x (float): 输入的x值。
    coefficients (list): 多项式各项的系数列表，从最低次开始，例如 [a_0, a_1, ..., a_n]。

    返回:
    float: 多项式f(x)的值。
    """
    result = 0
    for i, coeff in enumerate(coefficients):
        result += coeff * (x ** i)
    return result


def polynomial_predictions(x,params,polynomial_func):
    '''
    根据多项式参数完成拟合计算
    '''

    # 使用 functools.partial 预先绑定参数
    polynomial_with_params = partial(polynomial_func, coefficients=params)
    # 将自定义函数转换为向量化函数
    vectorized_function = np.vectorize(polynomial_with_params, otypes=[np.float64])

    # 预测
    return vectorized_function(x)


# 设定随机数种子以便结果可重复
np.random.seed(0)

# 点的数量
point_number = 500

# 创建数据集
# 假设我们要拟合的多项式为 y = x^3 - 2x^2 + x + 5
# 创建一个从 -10 到 10，共 500 个元素的数组
x = np.linspace(start=-10, stop=10, num=point_number)

# 模拟采样获取到的真实值
true_y = x**3 - 2*x**2 + x + 5
true_y = true_y + np.random.randn(point_number) * 10 # 加入一些噪声

global min_mse, params, min_pred_y, update_index, delta
global quit, iterations

# params = [5,1,-2,1]
params = [0,0,0,0]
update_index = 0
min_mse = sys.float_info.max
min_pred_y = x
delta = 0.1

quit = False
iterations = 0

# 更新预测值
def update_predictions():
    global min_mse, params, min_pred_y, update_index, delta
    global quit, iterations

    new_params = params.copy()
    new_params[update_index] = new_params[update_index]+delta

    pred_y = polynomial_predictions(x,new_params,polynomial)
    
    # 计算均方误差
    # 50元素时 polynomial函数的误差为126.69, 迭代法误差为107.11
    # 500元素时 polynomial函数的误差为99.7, 迭代法误差为98.7
    # 5000元素时 polynomial函数的误差为97.11, 迭代法误差为96.99
    # 无噪声时近似无误差
    mse = np.mean((true_y - pred_y) ** 2)

    iterations+=1

    if(mse < min_mse):
        min_mse = mse
        params = new_params

        min_pred_y = pred_y.copy()
    elif (update_index+1<len(params)):
        update_index += 1
    else:
        update_index = 0
        delta *= -0.8
        if abs(delta) < 1e-6:
            quit = True




def run_predictions():
    global quit
    while not quit:
        update_predictions()

def run_log():
    global quit, iterations
    while not quit:
        time.sleep(1)
        print(delta,params,min_mse)
    print(iterations,delta,params,min_mse)



# 异步高频率执行任务
thread = threading.Thread(target=run_predictions)
thread.start()
# 异步低频率输出log
log_thread = threading.Thread(target=run_log)
log_thread.start()


# ------图形可视化
if 0:
    # 初始化图形
    fig, ax = plt.subplots()
    line, = ax.plot([], [], 'r-', lw=2)
    ax.scatter(x, true_y,s=1, label='Data', color='blue')


    # 初始化函数
    def init():
        line.set_data([], [])
        return line,

    # 更新函数
    def animate(i):
        global min_pred_y
        line.set_data(x, min_pred_y)
        return line,

    # 设置动画
    ani = animation.FuncAnimation(fig, animate, frames=200, init_func=init, blit=True)

    # 显示图形
    plt.show()