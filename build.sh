mkdir -p build
cd build

FLAGS='-DPLATFORM_LINUX=1'

#g++ -m32 -g ../hot_code_reload_launcher.cpp -o hot_code_reload_launcher ${FLAGS}  
g++ -std=c++11 -m64 -g /media/karan/9AA41D2CA41D0BFF/workspace/hot_code_reload/hot_code_reload_launcher.cpp -o hot_code_reload_launcher ${FLAGS} -w

cd ..


