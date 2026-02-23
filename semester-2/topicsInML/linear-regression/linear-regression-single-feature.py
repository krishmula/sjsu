# Linear Regression: California Housing Price

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.datasets import fetch_california_housing

housing = fetch_california_housing()
x = housing.data
y = housing.target

print("Dataset shape:", x.shape)
print("Feature names:", housing.feature_names)
print("First 5 values of median income: ", x[:5, 0])
print("First 5 target values:", y[:5])

# Now, we're going to implement linear regression on a single feature first: Median Income

x_simple = x[:, 0:1] 
plt.scatter(x_simple, y, alpha=0.5)
plt.xlabel(housing.feature_names[0])
plt.ylabel("House Value")
plt.title("House Value vs " + housing.feature_names[0])
plt.show()

# Since this is linear regression on a single feature, we only have 1 weight (w), and 1 bias (b). 
# Since this is essentially a single neuron, in the grand scale of a neural net.
# Forward Pass: y = w . x + b

w = 0.5
b = 0.1

for i, income in enumerate(x[:10, 0]):
    y_pred = w * income + b
    print(f"Predicted value for {income}: {y_pred} , and target value: {y[i]}")
    print(f"Basic loss for {income}: {(y_pred - y[i])}")

# Implementing L2-Loss
# One thing is still bugging me: Is L2-Loss per sample, or is it all of the losses of all the samples combined?

for i, income in enumerate(x[:10, 0]):
    y_pred = w * income + b
    loss = (y_pred - y[i])**2
    print(f"Predicted value for {income}: {y_pred} , and target value: {y[i]}")
    print(f"L2 Loss for {income}: {loss}")

# Implementing Mean Squared Error

total_L2_loss = 0

for i, income in enumerate(x[:10, 0]):
    y_pred = w * income + b
    loss = ((y_pred - y[i])**2)
    total_L2_loss += loss
    print(f"Predicted value for {income}: {y_pred} , and target value: {y[i]}")
    print(f"MSE Loss for {income}: {loss}")

# Divided by 10, becuase we are only calculating the loss for the first 10 samples
mse_loss = total_L2_loss / 10
print(f"Mean Squared Error Loss: {mse_loss}")

# Implementing Mean Absolute Error (MAE) Loss

total_L1_loss = 0

for i, income in enumerate(x[:10, 0]):
    y_pred = w * income + b
    loss = abs(y_pred - y[i])
    total_L1_loss += loss
    print(f"Predicted value for {income}: {y_pred} , and target value: {y[i]}")
    print(f"L1 Loss for {income}: {loss}")

# Divided by 10, becuase we are only calculating the loss for the first 10 samples
mse_loss = total_L1_loss / 10
print(f"Mean Absolute Error Loss: {mse_loss}")
