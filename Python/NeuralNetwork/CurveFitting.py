import numpy as np
from tensorflow import keras
from tensorflow.keras import layers

import matplotlib.pyplot as plt

# 设定随机数种子以便结果可重复
np.random.seed(0)

# 创建数据集
# 假设我们要拟合的多项式为 y = x^3 - 2x^2 + x + 5
x = np.random.uniform(low=-10, high=10, size=500)
y = x**3 - 2*x**2 + x + 5 + np.random.randn(500) * 10  # 加入一些噪声

# 创建模型
model = keras.Sequential([
    layers.Dense(64, activation='relu', input_shape=[1]),
    layers.Dense(64, activation='relu'),
    layers.Dense(1)
])

# 编译模型
optimizer = keras.optimizers.Adam(0.001)
model.compile(loss='mse',
              optimizer=optimizer,
              metrics=['mae'])



# 训练模型
history = model.fit(
x, y,
epochs=200,
validation_split = 0.2,
verbose=0)

# 预测
sort_x = x.copy()
sort_x.sort()
predictions = model.predict(sort_x)

# 可视化结果
plt.figure()
plt.scatter(x, y, s=1, label='Data')
plt.plot(sort_x, predictions, color='red', label='Predictions')
plt.legend()
plt.show()