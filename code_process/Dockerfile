# 使用Ubuntu基础镜像
FROM ubuntu:20.04

# 避免交互式提示
ARG DEBIAN_FRONTEND=noninteractive

# 更新apt包管理器并安装jsoncpp和boost环境
RUN apt-get update && \
    apt-get install -y \
    libjsoncpp-dev \
    libboost-all-dev \
    g++

# 创建/home/code文件夹
RUN mkdir -p /home/work/code

# 设置工作目录
WORKDIR /home/work

# 将当前目录下的code_process_server可执行文件复制到容器的/home/work目录下
COPY code_process_server /home/work/code_process_server