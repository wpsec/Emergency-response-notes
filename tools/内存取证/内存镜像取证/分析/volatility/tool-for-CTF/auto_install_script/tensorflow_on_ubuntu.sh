# /bin/bash
cp ./.condarc ~
conda create -n tensorflow python=3.7 -y
conda activate tensorflow
pip install tensorflow==1.15.3 -i https://pypi.tuna.tsinghua.edu.cn/simple/ --timeout=1000
pip install pandas -i https://pypi.tuna.tsinghua.edu.cn/simple/ --timeout=1000
pip install matplotlib -i https://pypi.tuna.tsinghua.edu.cn/simple/ --timeout=1000
pip install opencv-python -i https://pypi.tuna.tsinghua.edu.cn/simple/ --timeout=1000
pip install librosa -i https://pypi.tuna.tsinghua.edu.cn/simple/ --timeout=1000
