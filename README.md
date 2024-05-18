# FHEtool
## 简介
为解决市面上全同态加密算法库繁多、参数设置复杂、使用门槛高的问题，提供全同态加密算法库工具，辅助使用者选取合适的算法库与参数。支持输入加密数据数值范围与乘法次数等要求，提供常用全同态算法库在多组参数设置下的精度与性能。
## 使用说明
### 直接使用已封装的服务
1. 环境配置
服务使用python运行，请首先确保已配置python的运行环境，接下来安装以下python库：
```
pip install --upgrade pip
pip install requests
pip install tkinter
pip install matplotlib
```
2. 运行函数
```
python fhe_start.py
```
### 修改服务实现定制化要求
