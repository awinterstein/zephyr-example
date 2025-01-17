FROM ubuntu:24.10

WORKDIR /workspaces

ENV DEBIAN_FRONTEND=noninteractive
ENV PIP_NO_CACHE_DIR=1

# install Zepyr development dependencies
RUN apt-get update && \
    apt-get -y --no-install-recommends install \
        git cmake ninja-build gperf \
        ccache dfu-util device-tree-compiler wget \
        python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
        make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1
RUN apt-get -y --no-install-recommends install python3-venv

# create virtual Python environment and enable it
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip3 install -U west

# install Python packages for development
RUN pip3 install \
    cmake_format

# install additional Python dependencies for Zephyr
RUN pip3 install \
        -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/main/scripts/requirements.txt \
        -r https://raw.githubusercontent.com/zephyrproject-rtos/mcuboot/main/scripts/requirements.txt

# download and extract the Zephyr SDK
ENV ZEPHYR_SDK_VERSION=0.16.5-1
RUN wget -q https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz && \
    wget -q -O - https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/sha256.sum | shasum --check --ignore-missing && \
    tar xvf zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz -C /opt/ && \
    rm zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz

# run the setup script of the Zephyr SDK
RUN cd /opt/zephyr-sdk-${ZEPHYR_SDK_VERSION} && ./setup.sh -t arm-zephyr-eabi -h -c

# install additional packages that are needed for the development in Visual Studio Code
RUN apt-get update && apt-get install -y --no-install-recommends jq

# clean-up leftovers from apt package installations
RUN apt-get clean -y && \
    apt-get autoremove --purge -y && \
    rm -rf /var/lib/apt/lists/*
