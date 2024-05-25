# 第一阶段：构建环境
FROM ubuntu:20.04 

# 避免交互式提示
ARG DEBIAN_FRONTEND=noninteractive

# 更新apt包管理器并安装构建环境和依赖
RUN apt-get update && \
    apt-get install -y \
    g++ \
    libjsoncpp1 \
    libboost-system1.71.0 \
    libboost-filesystem1.71.0 \
    libboost-thread1.71.0 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /home/work

# 假设已经有编译好的可执行文件
COPY code_process_server /home/work/code_process_server

# 创建/home/code文件夹
RUN mkdir -p /home/work/code